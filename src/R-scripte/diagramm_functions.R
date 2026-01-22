#install####
#install.packages("car")
library(package = "car")

op <- options()   #; utils::str(op) # op is a named list
options(scipen = 3)  # keine scientific notation fuer Zahlen
options(digits = 1)

# Funktion: Ausgabedatei von wq_stat einlesen 
file_wq_stat <- function(filename) #####
{
  stat_data <- read.table(filename,header=TRUE, sep='\t', skip=9)

  #####################################
  # Spalten umbenennen
  names(stat_data)[3]<-"date"                       # Datum
  names(stat_data)[5]<-"length(km)"                 # x-Achse f?r alle Diagramme
  names(stat_data)[8]<-"measured"                   # Reihe 1 Diadramme 1
  names(stat_data)[6]<-"concentration"              # Reihe 2 Diadramme 1
  names(stat_data)[14]<-"river discharge"           # Reihe 1 Diadramme 2 [m3/s]
  names(stat_data)[25]<-"measured grdc"             # Reihe 2 Diadramme 2 [m3/s]
  names(stat_data)[28]<-"measured others"           # Reihe 3 Diadramme 2 [m3/s]
  names(stat_data)[15]<-"montly grid cell loading"  # Reihe 4 Diadramme 2 [t/month] - Andere y-Achse!!!
  names(stat_data)[19]<-"monthly loading accumulated"  # Reihe 1 Diadramme 3 [t/month]
  names(stat_data)[26]<-"measured loading"             # Reihe 2 Diadramme 3 [t/month]
  names(stat_data)[30]<-"rout_area"                 # 
  
  #####################################
  # Kopfzeilen einlesen
  x<-readLines(filename, n = 9)
  y<-strsplit(x, split="[\t:]")
  IDVersion=as.numeric(y[[3]][3])
  IDReg=as.numeric(y[[3]][6])
  IDrun=as.numeric(y[[4]][3])
  Name=y[[4]][5]
  parameter_id_input=as.numeric(y[[5]][3])
  parameter_id=as.numeric(y[[5]][5])
  ParameterName=y[[5]][7]
  
  parameter_id_load=as.numeric(y[[6]][3])
  LoadParameterName=y[[6]][5]

  parameter_id_conductivity=as.numeric(y[[7]][3])
  conductivityParameterName=y[[7]][5]

  year_start=as.numeric(y[[8]][3])
  year_end=as.numeric(y[[8]][5])
  
  river_name=y[[9]][1]
  arcid_begin=as.numeric(y[[9]][4])
  arcid_begin_global=as.numeric(y[[9]][6])
  arcid_end=as.numeric(y[[9]][10])
  arcid_end_global=as.numeric(y[[9]][12])
  
  rm(x, y)

  head_data <- data.frame(IDVersion=IDVersion, IDReg=IDReg, IDrun=IDrun,Name=Name
         , parameter_id_input=parameter_id_input, parameter_id=parameter_id
         , ParameterName=ParameterName
         , parameter_id_load=parameter_id_load, LoadParameterName=LoadParameterName
         , parameter_id_conductivity=parameter_id_conductivity, conductivityParameterName=conductivityParameterName
         , year_start=year_start, year_end=year_end
         , river_name=river_name
         , arcid_begin=arcid_begin, arcid_begin_global=arcid_begin_global
         , arcid_end=arcid_end, arcid_end_global=arcid_end_global)
  
  ## factoren zu character
  i<-sapply(head_data, is.factor)
  head_data[i]<-lapply(head_data[i], as.character)
  
  # runden, Falls noetig
  if (round_digits>=0) {
    stat_data[,c(6,8)] <- round(stat_data[,c(6,8)], digits = round_digits)
  }
  
  # wenn noetig rout_area begrenzen ####
  stat_data <- grenze.rout.area(stat_data) 
  stat_data <- grenze.measured(stat_data) 

  return(list(head_data, stat_data))
} # file_wq_stat()
                          
# Funktion: Ausgabedatei von wq_stat_stations einlesen
file_wq_stat_stations <- function(filename) #### 
{
  #print(filename)
  stat_data <- read.table(filename,header=TRUE, sep='\t', skip=9)
  
  #####################################
  # Kopfzeilen einlesen
  x<-readLines(filename, n = 9)
  y<-strsplit(x, split="[\t:]")
  IDVersion=as.numeric(y[[3]][3])
  IDReg=as.numeric(y[[3]][6])
  bas0_id=as.numeric(y[[3]][9])
  IDrun=as.numeric(y[[4]][3])
  Name=y[[4]][5]
  parameter_id_input=as.numeric(y[[5]][3])
  parameter_id=as.numeric(y[[5]][5])
  ParameterName=y[[5]][7]
  
  parameter_id_load=as.numeric(y[[6]][3])
  LoadParameterName=y[[6]][5]

  parameter_id_conductivity=as.numeric(y[[7]][3])
  conductivityParameterName=y[[7]][5]

  year_start=as.numeric(y[[8]][3])
  year_end=as.numeric(y[[8]][5])
  
  river_name=y[[9]][1]
  arcid_begin=as.numeric(y[[9]][4])
  arcid_begin_global=as.numeric(y[[9]][6])
  arcid_end=as.numeric(y[[9]][10])
  arcid_end_global=as.numeric(y[[9]][12])
  
  rm(x, y)
  
  head_data <- data.frame(IDVersion=IDVersion, IDReg=IDReg, bas0_id=bas0_id, IDrun=IDrun,Name=Name
    , parameter_id_input=parameter_id_input, parameter_id=parameter_id
    , ParameterName=ParameterName
    , parameter_id_load=parameter_id_load, LoadParameterName=LoadParameterName
    , parameter_id_conductivity=parameter_id_conductivity, conductivityParameterName=conductivityParameterName
    , year_start=year_start, year_end=year_end
    , river_name=river_name
    , arcid_begin=arcid_begin, arcid_begin_global=arcid_begin_global
    , arcid_end=arcid_end, arcid_end_global=arcid_end_global)
  
  ## factoren zu character
  i<-sapply(head_data, is.factor)
  head_data[i]<-lapply(head_data[i], as.character)

  # runden, Falls noetig
  if (round_digits>=0) {
    stat_data[,c(4, 6, 8)] <- round(stat_data[,c(4, 6, 8)], digits = round_digits)
  }
  
  # wenn noetig rout_area begrenzen ####
  stat_data <- grenze.rout.area(stat_data)
  stat_data <- grenze.measured(stat_data)        # für measured  
  stat_data <- grenze.measured(stat_data, TRUE)  # für measured_conductivity 
  
  return(list(head_data, stat_data))
} # file_wq_stat_stations()

# Funktion: Diagramme erstellen####
diag_generate <- function(x_col, y_matrix, # Matrix f?r x- und y-Achse. Wenn Reihen gleiche x-Werte haben,
                          # x_col darf nur eine Spalte haben, sonst f?r jeden Reihe eine Spalte
                          xlab_value, ylab_value,                # Achsenbeschriftung
                          type_col,         # z.B. c("p","l") : erste Reihe Punkte, Zweite Reihne Linie
                          color_matrix, 
                          lty_matrix,       # 1, 2, 3, 4, 5, 6 bzw. Schl?sselworte: "solid", "dashed", "dotted", "dotdash", "longdash", "twodash"
                          lwd_matrix,       # Linienst?rke
                          pch_matrix,       # Art der Datenpunkt-Symbole Zahl 1-25
                          head1,            # erste Zeile vom Titel
                          head2,            # zweite Zeile vom Titel
                          legend_matrix,    # vector, Wert fuer jede Reihe; wenn==NULL, keine Legende
                          log_scale="",     # "y", "x", "xy" oder ""
                          #x_col_format=NULL,  # formatierte x-Axis Werte fuer Beschriftung
                          is.X.date=FALSE,  # ist x-Axis Werte Datum?
                          plot_diag=FALSE,  # 1:1-Linie Zeichnen (in scatterplot)
                          las=0,            # Orientierung der Achsenbeschriftung (0=parallel,1=waagrecht,3=senkrecht)
                          points.size=1,    # cex
                          cex.axis = 1.5, 
                          cex.lab=2)
{
  if (length(y_matrix[!is.na(y_matrix)])>0) { # nur wenn Daten gibt weiter machen
    # Bei der Datum in x-Achse Achsenbeschriftung spaeter machen, wegen xlabs.by 
    if (!is.X.date) {
      xaxt="s"
      #    xlimit<-range(x_col, na.rm=T)
    }  else {
      xaxt="n"    # keine x-Achse Beschriftung
      #    xlimit<-range(x_col_format, na.rm=T)
      
      # Umrechnen xlabs.by: wenn Anzahl der Labels (years*12/xlabs.by) zu groß ist (hier: >40)
      # soll xlabs vergrößert werden
      # xlabs.by darf Werte c(2,3,4,6,12,24) annehmen
      # so kann xlabs.by in start.R immer = 1 setzen
      x<-strtoi(format(min(x_col_format)[1], form="%Y"))
      y<-strtoi(format(max(x_col_format)[1], form="%Y"))
      years=y-x+1
      #Funktion recode ist aus package "car". Falls nötig, bitte ausführen:  install.packages("car")
      while((years*12/xlabs.by>40 && xlabs.by<24)) {xlabs.by<-recode(xlabs.by, "1=2; 2=3; 3=4; 4:5=6; 6:11=12; else=24")}
    }
    par.las.orig<-par()$las
    #  ylimit<-range(y_matrix, na.rm=T)
    #  if(log_scale=="y") {
    #    ylimit[1]=10
    #    y_matrix$'concentration'[which(y_matrix$'concentration'<10)]<-NA
    #  }
    
    if (plot_diag) {
      xlimit <- range(x_col, na.rm=T)
      ylimit <- range(y_matrix, na.rm=T)
      combined.limit <- c(min(xlimit[1], ylimit[1]), max(xlimit[2], ylimit[2]))
      #combined.limit<-c(0,5000)
      
      par(las=las)
      plot(x_col, y_matrix, type=type_col, xlab=xlab_value, ylab=ylab_value
           , col = color_matrix, lty=lty_matrix, lwd = lwd_matrix, pch=pch_matrix 
           , log=log_scale, xaxt=xaxt, xlim=combined.limit, ylim = combined.limit
           , cex=points.size, cex.axis = cex.axis, cex.lab = cex.lab)
      #, xpd=TRUE)
      par(las=par.las.orig)
      lines(combined.limit, combined.limit, col = "black") # 1:1 Linie
    } else {
      par(las=las)
      matplot(x_col[order(x_col)], y_matrix[order(x_col),], type=type_col, xlab=xlab_value, ylab=ylab_value, 
              col = color_matrix, lty=lty_matrix, lwd = lwd_matrix, pch=pch_matrix, 
              log=log_scale, xaxt=xaxt
              , xpd=TRUE, cex=points.size)
      par(las=par.las.orig)
    }
    
    # Achsenbeschriftung fuer x_col_format, nur jeden xlabs.by schreiben (z.B. xlabs.by=2 - jeden 2. Mal)
    if (is.X.date) {
      par(las=las)
      # axis(1, NULL, x_col_format)
      #xlabs<-format(seq(x_col_format[1], rev(x_col_format)[1], by = "months"),format=datum.format)
      xlabs<-format(seq(min(x_col_format)[1], max(x_col_format)[1], by = "months"),format=datum.format)
      xlabs[!(1:length(xlabs) %in% seq(1,length(xlabs),by=xlabs.by))]<-NA
      axis.POSIXct(1, at = seq(min(x_col_format)[1], max(x_col_format)[1], by = "months"),labels=xlabs)
      
      #axis(2)
      par(las=par.las.orig)
    }
    
    mtext(head1, side = 3, line=2.5, col=1, cex=1)
    mtext(head2, side = 3, line=1, col=1, cex=1)
    
    if (!is.null(legend_matrix)) {
      const.legend=3.5
      if (las==3) const.legend = 2.5 # wenn Achsenbeschriftung senkrecht ist, braucht man mehr Platz, Legende soll niedriger sein
      if(log_scale == "y" || log_scale == "xy") {
        y_legend = 10^(par("usr")[3]-(par("usr")[4] -par("usr")[3])/const.legend)
      } else {
        y_legend = par("usr")[3]-(par("usr")[4] -par("usr")[3])/const.legend
      }
      
      # horiz = TRUE  - Legende wird horizontal sein
      # wenn es nicht gewuenscht ist: horiz= False - vertical
      # mann kann auch legende in Spalten machen: horiz = FALSE, ncol={Anzahl der Spalten}
      legend(x=min(x_col), y=y_legend, legend=legend_matrix, horiz=TRUE
             , lty=lty_matrix, pch=pch_matrix
             , col=color_matrix, bty = "n", cex=1.2)
    }
  }

} # diag_generate()

# Datei für Graphik oeffnen/erstellen
open_file <- function(filename, width=30, height=25, units="cm",res=500,
                      rows=3, columns=1, mar=c(7.1, 4.1, 4.1, 5.1), xpd=TRUE, mgp=c(1.5,0.5,0))
# mgp=c(1.5,0.5,0) The margin line (in mex units) for the axis title, axis labels and axis line.
# mar=c(7.1, 4.1, 4.1, 5.1) c(bottom, left, top, right)
{
  # neue Datei(oder Fenster) f?r Diagrammen ?ffnen
  png(filename,width=width, height=height, units=units,res=res)
  #windows(width=width, height=height)
  
  # Graphik leer mit breitem rechten Rand ?ffnen mar(default: 5.1 4.1 4.1 2.1) (unten, link, oben, recht)
  # mfrow: Diagrammen in 3 Reihen, eine Spalte
  # mgp= (default: c(3,1,0))  The margin line (in mex units) for the axis title, axis labels and axis line
  # number indicating the amount by which plotting text and symbols should be scaled relative to the default. 1=default, 1.5 is 50% larger, 0.5 is 50% smaller, etc. 
  # cex.axis magnification of axis annotation relative to cex  
  if (diag.order==1) {
    par(mfrow=c(rows, columns), mar=mar, xpd=xpd, mgp=mgp, cex.axis=1.2, cex.lab = 1.5)
  } else {
    par(mfcol=c(rows, columns), mar=mar, xpd=xpd, mgp=mgp, cex.axis=1.2, cex.lab = 1.5)
  }
  
}

# Kontrolle, ob Datei mit Graphiken schon voll ist
next_diag <- function(counter, counter_files
    , device.off = FALSE # wenn TRUE, Datei umschalten auch wenn maximale Anzahl der Diagrammen in der Datei nicht erreicht
    , rows = diag.rows, columns = diag.columns) {
  if (device.off) {
    counter=1
    dev.off()
    counter_files <- counter_files + 1
  } else if (counter == rows * columns) {
    counter=1
    dev.off()
    counter_files <- counter_files + 1
  } else {
    counter <- counter + 1
  }
  return(c(counter, counter_files))
}

# 
grenze.rout.area <- function(stat_data) {
  if(min.rout.area>0) {
    stat_data<-stat_data[stat_data$rout_area>=min.rout.area,]
  }
  if(max.rout.area>0) {
    stat_data<-stat_data[stat_data$rout_area<=max.rout.area,]
  }
  return(stat_data)
}

grenze.measured <- function(stat_data
  , conductivity = FALSE # wenn TRUE, dann Feld measured_conductivity; FALSE: measured
  ) {
  if (conductivity) {
    idx1 <- which((!is.na(stat_data$measured_conductivity)) & stat_data$measured_conductivity<measured.grenze)
    idx2 <- which((!is.na(stat_data$measured_conductivity)) & stat_data$measured_conductivity>=measured.grenze)
    stat_data[idx1,"measured_conductivity"] <- stat_data[idx1,"measured_conductivity"]*measured.factor.1
    stat_data[idx2,"measured_conductivity"] <- stat_data[idx2,"measured_conductivity"]*measured.factor.2
  } else {
    idx1 <- which((!is.na(stat_data$measured)) & stat_data$measured<measured.grenze)
    idx2 <- which((!is.na(stat_data$measured)) & stat_data$measured>=measured.grenze)
    stat_data[idx1,"measured"] <- stat_data[idx1,"measured"]*measured.factor.1
    stat_data[idx2,"measured"] <- stat_data[idx2,"measured"]*measured.factor.2
  }
  return(stat_data)
}

# Funktion wird in station_years und stations_scatterplot benutzt
# Stationen sollten keine mehrfache Diagrammen kriegen Falls sie in mehreren Dateien vorhanden sind. 
# Deswegen wird Liste gefuehrt von allen Stationen die schon Diagramme haben
stations.list.check <- function(stations.list, stat_data) { 
  # # # # # # # # # # # # # # # # # # # # # # 
  # Liste der Stationen ueberpruefen
  # Falls Station schon in anderen Datei verwendet wurde, loeschen aus stat_data
  # Falls nicht - einfuegen in den vector stations.list
  
  stations.list.lokal<-stations.list
  if (length(stations.list.lokal) == 0) {
    stations.list.lokal<-c(stations.list.lokal,unique(stat_data$ArcId)) # alle Stationen aus der Datei in die Liste einfuegen
  } else {
    for (station in unique(stat_data$ArcId)) {
      if (length(which(stations.list.lokal %in% station))>0) {
        stat_data<-stat_data[stat_data$ArcId!=station,] # Station aus der data frame loeschen
      } else {
        stations.list.lokal<-c(stations.list.lokal, station)        # neue Station in die Liste einfuegen
      }
    }
  }
  # stations.list soll als globale Variable behandelt werden: 
  # Normale Zuweisungen (<-) haben nur lokalen Gueltigkeitsbereich und sind temporaer. 
  # Ausnahme: assign() und Operator <<-
  stations.list<<-stations.list.lokal
  return(stat_data)
}

# Funktion wird in station_years und stations_scatterplot benutzt
# Dateien mit den Diagrammen haben in den Namen bas0_id
# Falls in verschiedenen Inputdateien gleicher bas0_id ist, darf man counter_files nicht für jede Iputdatei neu ab 1 nummerieren
# in data.frame counter.files wird für jeden Bas0_id erste nicht belegte Nummer gespeichert
counter.files.check <- function(counter.files) {
  # # # # # # # # # # # # # # # # # # # # # # 
  # Zaehler fuer Outputdateien
  # counter_files=1
  idx_bas0_id<-which(counter.files$bas0_id==head_data$bas0_id) # gab es schon bas0_id?
  if (length(idx_bas0_id)>0) { # falls ja, counter_files aus dem data.frame nehmen
    counter_files<-counter.files[idx_bas0_id,]$counter_files[1]
  } else { # falls nein, einfuegen neu mit dem Wert 1
    # counter.files soll als globale Variable behandelt werden: 
    # Normale Zuweisungen (<-) haben nur lokalen Gueltigkeitsbereich und sind temporaer. 
    # Ausnahme: assign() und Operator <<-
    counter.files<<-rbind(counter.files, data.frame(bas0_id=head_data$bas0_id,counter_files=1))
    counter_files=1
  }
  return(counter_files)
}