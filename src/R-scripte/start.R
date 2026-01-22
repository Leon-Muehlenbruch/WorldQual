# wq_stat: RiverSection.R,  RiverSection_scen.R, station.R 
# wq_stat_stations: station_years.R, stations_scatterplot.R
# starten:
# source("[pfad]/start.R")
#start##################
rm(list=ls())

# so kriegen wir englische Beschriftung in Datum
Sys.setlocale("LC_TIME","English") 

######### fuer alle scripts:
# Verzeichnisse #####
# Verzeichniss mir Scripten
r_script_dir="Z:/worldqual_workspace/WorldQual/R-scripte"
script="stations_scatterplot.R"
# Verzeichniss, hier werden Grafikdateien geschrieben
current_dir="C:/Users/kynast/Documents/GridWasser/worldqual/macro_test/r_versuch/tst/5"

# log_scale ####
# log_scale: "" - keine logarithmische Skalierung; "y" - logarithmische Skalierung y-Achse; "x" - logarithmische Skalierung x-Achse
log_scale=""

# minimum for log_scale ####
# nur fuer log_scale != ""; falls <= 0 wird automatische range fuer Achse verwendet
#min_log_scale = 1

# min & max for y ####
# falls <= 0 wird automatische range fuer Achse verwendet
min_axis_y = -1
max_axis_y = -1

# Einheit für Konzentration ####
unit="cfu/100ml"

# min.rout.area - max.rout.area ####
# wenn >0, Zeilen mit rou_area < min.rout.area und/oder rout_area> max.rout.area werden aussortiert
min.rout.area <- -1
max.rout.area <- -1

# measured umrechnen####
# wenn measured < measured.grenze, dann measured = measured * measured.factor.1
# sonst (measured >= measured.grenze) measured = measured * measured.factor.2
measured.grenze = 1000
measured.factor.1 <- 1
measured.factor.2 <- 1

# Dateiname(n) ####
# wenn path=="" Datei filename wird in [current_dir] gesucht
# fuer RiverSection_scen.R:  path, filename, path2, filename2, path3, filename3, path4, filename4
# andere Scripte brauchen nur eine Datei path/filename
# Script stations_scatterplot.R bei diag.type == 3 filename wird ignoriert
#   es werden alle Dateien *.txt aus dem Ordner bearbeitet

input.path="V:/USF/reder/run/wq_stat_stations/FC/wwqa_valid_threshold_0_run11_sa"
input.path="V:/USF/reder/run/wq_stat_stations/FC/wwqa_valid_run20_la"
input.path="V:/USF/reder/run/wq_stat_stations/FC/wwqa_valid_run21_af"
#input.path="C:/Users/kynast/Documents/GridWasser/worldqual/macro_test/r_versuch/tst/3/5"
input.filename=""
#input.filename="statistic_station_param_203100009_Arrebita159362_1990_2010.txt"
#input.path2=input.path; input.filename2=input.filename
#input.path3=input.path; input.filename3=input.filename
#input.path4=input.path; input.filename4=input.filename
input.path="Z:/worldqual_workspace/run/wq_stat_stations/test3"
#input.filename="statistic_station_param_202600009_NULL41954_1990_2010.txt"
#input.path="Z:/worldqual_workspace/WorldQual/wq_stat"
#input.filename="statistic_param_202600009_Vaal_2010_2010.txt"

# Dateinamenzusatz ####
filename_prefix=1
# z.B. Datei mit den Diagrammen "stations_scatterplot.png" wird zu "stations_scatterplot_wwqa_valid_threshold_0_run11_sa.png"
# man darf entweder string eingeben oder als filename_prefix Zahl 1 oder 2 verwenden, 
# Mit der Zahl als prefix wird letzte (oder 2 letzten) Ordnername aus input.path verwendet
# ACHTUNG! Wenn man Zahlen verwenden moechte, keine "\\" in input.path verwenden sondern "/"
if (filename_prefix ==1 || filename_prefix == 2) {
  folders <- strsplit(input.path,"/")[[1]]
  ll <- length(folders)
  if (ll>= filename_prefix) {
    if (filename_prefix == 1) filename_prefix <- folders[ll]
    else filename_prefix <- paste(folders[ll-1], folders[ll], sep="_")
  }
  rm (ll,folders)
}

# wenn TRUE, Script starten für alle Dateien im Ordner input.path nach einander
# dann wird input.filename ignoriert!
# fuer scatterplot und diag.type == 3: soll FALSE sein, hier soll ein Scriptaufruf fuer alle Dateien sein!
for.all.files.start = TRUE

# Szenarien fuer RiverSection_scen.R ####
scen <- c("baseline","IPCM", "MIMR", "MPEH5")

# Diagrammenanordnung ####
# diag.rows, diag.columns: Diagrammenanordnung (z.B diag.rows=3, diag.columns=1 drei Diagrammen in einer Spalte)
diag.rows=3
diag.columns=1
# bei mehreren Zeilen / Spalten in einer Datei Reihenfolge der Diagrammen: 
# 1: zuerst Zeilen, dann Spalten; 2: zuerst Spalten dann Zeilen
# 1:  1 2   2: 1 4
#     3 4      2 5
#     5 6      3 6
diag.order=2

#Diagrammenauswahl (RiverSection.R, RiverSection_scen.R) ####
# fuer RiverSection.R (3 Diagrammen), RiverSection_scen.R (zwei Diagrammen): welche Diagrammen? 
# "1" - Diagramm generieren, "0" keine Diagramm
# "111" : alle Diagramme
# "100" : nur erste Diagramm: concentration
# "010" : nur zweite Diagramm: river discharge/ monthly grid cell loading
# "001" : nur dritte Diagramm: monthly loading accumulated
# Bitte wenn mehr als eine Diagramm eingeschaltet ist, aufpassen, dass diag.rows, diag.columns passt,
RiverSection.diag="111"


# diag.type nur fuer stations_scatterplot.R ####
# diag.type 0: eine Diagramm mit allen Stationen
#           1: Diagramm fuer jede Station
#           2: 0 + 1  Aufpassen!!!  wiederholte Stationen werden rausgenommen auch bei den Diagrammen mit allen Stationen
#             besser keine 2 nutzen, sondern getrennt 0 und 1
#           3: eine Diagramm mit allen Stationen aus mehreren Dateien
# bei 3 wird input.filename ignoriert, input.path ist ein Pfad zu Inputdateien; es werden alle Dateien *.txt aus dem Ordner bearbeitet
diag.type<-3

# colorierung nach bas0_id (0 - default), nach Inputdateien (1) oder einfarbig (3)
scatter.color.type = 0

# Punktengröße (default: 1) nur für scatterplot ####
scatter.points.size = 0.5

# Art der Datenpunkt-Symbole Zahl 1-25 nur für scatterplot ####
# default: unsichere Werte Dreieck (17) sonst Rechteck (15); bei einfarbigen kleinpunkten: 20
scatter.pch=15 #20 #15
scatter.pch.uncert=17 #20 #17

# 
scatter.cex.axis = 1.5 # Achsenbeschriftung, Zahlen
scatter.cex.lab=2      # Achsenbeschriftung, labels

# Datumformat ####
# Datumformat auf x-Achse (station.R, station_years.R)
# %d day as a number (01-31); 
# %a abbreviated weekday (Mon); %A unabbreviated weekday (Monday)
# %m month (00-12)
# %b abbreviated month (Jan); %B unabbreviated month (January)
# %y 2-digit-year  (07)
# %Y 4-digit-year (2007)
#
datum.format="%m/%Y"
datum.format="%b"
datum.format="%b %Y"

# x-Achse Labels bei Datumformat nur jede x. ausgeben (station.R)
xlabs.by = 1
# Orientierung der Achsenbeschriftung bei Datumformat (station.R)
#  (0=parallel,1=waagrecht,3=senkrecht)
labs.las = 3

#measures und concentration Werte runden####
# wenn -1 : nicht runden; Wert>=0 : Runden auf round_digits Stellen nach der Komma
round_digits=-1

#
if (script=="stations_scatterplot.R" && diag.type==3) {
  for.all.files.start=FALSE
}

#script starten ##################
if (!for.all.files.start) {  # einmaliger Start
  if(input.path == "") {
    filename <- input.filename
  } else {
    filename <- paste(input.path, input.filename, sep="/")
  }
  
  source(paste(r_script_dir,"/",script, sep=""))
} else { #fuer jede Datei Script starten
  if (input.path=="") {
    input.path <- current_dir
  }
  
  liste <- dir(path=input.path,pattern='.*\\.txt') 
  #if(length(liste)>15) liste<-liste[1:15]  # zum Testzwecken reichen mir 2 Dateien aus
  if(length(liste)==0) {
    stop("Keine Inputdateien im Ordner!")
  }
  
  for (input.number in c(1:length(liste))) {
    filename <- paste(input.path, liste[input.number], sep="/")
    source(paste(r_script_dir,"/",script, sep=""))
  } # for(input.number)
}

