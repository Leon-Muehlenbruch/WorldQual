# Liste von Stationen wird gesammelt. 
# Falls in verschiedenen Dateien gleiche Stationen gibt, wird nur ein Mal Diagramm erstellt
# Variable anlegen, Falls sie noch nicht gibt
if (length(which(ls() %in% "stations.list"))==0) {
  stations.list<-integer()
}

# counter_files fuer jeden bas0_id
# data frame anlegen, Falls ihn noch nicht gibt
if (length(which(ls() %in% "counter.files"))==0) {
  counter.files<-data.frame(bas0_id=numeric(), counter_files=numeric())
}

#Verzeichnisse bestimmen Funktionen und Daten einlesen####
# Unser Verzeichniss, hier werden Grafikdateien geschrieben
setwd(current_dir)
getwd()

source(paste(r_script_dir,"/","diagramm_functions.R", sep=""))

# 
if (filename_prefix=="") {
  filename_begin <- "station_years"
} else {
  filename_begin=paste("station_years_",filename_prefix, sep="")
}

x<-file_wq_stat_stations(filename)

if(dim(x[[2]])[1]>0) { # nur wenn Datensatz nicht leer ist

head_data<-x[[1]]
stat_data<-x[[2]]
rm(x)

# # # # # # # # # # # # # # # # # # # # # # 
# Liste der Stationen ueberpruefen
# Falls Station schon in anderen Datei verwendet wurde, loeschen aus stat_data
# Falls nicht - einfuegen in den vector stations.list
stat_data <- stations.list.check(stations.list, stat_data)

if (dim(stat_data)[1]==0) { # Wenn keine Daten nach dem stations.list.check vorhanden sind
  #message("in der Datei ", filename, " keine neue Daten vorhanden!")
} else {
  
# # # # # # # # # # # # # # # # # # # # # # 
# Zaehler fuer Diagrammen im Fenster
counter=1

counter_files <- counter.files.check(counter.files)

# Diagrammen fuer alle Zellen
#for (cell in unique(stat_data$ArcId)) 

# Diagrammen nur fuer die Zellen, die gemessene Werte haben
# 2: ArcID, 6: measured, 8: measured_conductivity
for (cell in unique(subset(stat_data[, c(2,6,8)], (!is.na(measured) | !is.na(measured_conductivity)))[, 1]))
{
  if (counter==1) {
    # neue Datei(oder Fenster) fuer Diagrammen ?ffnen
    #open_file(paste(filename_begin,head_data$river_name,"_",head_data$IDrun,"_",counter_files,".png", sep="")
    open_file(paste(filename_begin,"_",head_data$bas0_id,"_",head_data$IDrun,"_",counter_files,".png", sep="")
              , rows=diag.rows, columns=diag.columns)
    counter_files <- counter_files + 1
    idx_bas0_id<-which(counter.files$bas0_id==head_data$bas0_id)
    counter.files[idx_bas0_id,]$counter_files <- counter.files[idx_bas0_id,]$counter_files+1
  }
  
  #Diagramm 1 ####################################
  # 2: ArcID, 3: date
  x_col=subset(stat_data[, c(2,3)], ArcId == cell)[, 2]
  x_col_format<-ISOdate(floor(x_col/100),x_col%%100,1)
  
  # 2: ArcID; 6-7: measured concentration & flag uncert;
  # 8-9: measured conductivity concentration & flag uncert conductivity; 4: calculated concentration
  y_matrix=subset(stat_data[, c(2, 6, 7, 8, 9, 4)], ArcId == cell)[, 2:6]   # y-Achs
  y_matrix[,1][y_matrix[,1]==0] <- NA # measured concentration
  y_matrix[,3][y_matrix[,3]==0] <- NA # measured conductivity concentration
  y_matrix[,5][y_matrix[,5]==0] <- NA # calculated concentration
  
  # für logarithmische Scale Werte unter min_log_scale vernachlässigen
  #if(log_scale != "" && min_log_scale>0) {
  #  y_matrix[,1][y_matrix[,1]<min_log_scale] <- NA    # measured concentration
  #  y_matrix[,3][y_matrix[,3]<min_log_scale] <- NA    # calculated concentration
  #}
  # Werte unter min_axis_y vernachlässigen
  if(min_axis_y>0) {
    y_matrix[,1][y_matrix[,1]<min_axis_y] <- NA    # measured concentration
    y_matrix[,3][y_matrix[,3]<min_axis_y] <- NA    # measured conductivity concentration
    y_matrix[,5][y_matrix[,5]<min_axis_y] <- NA    # calculated concentration
  }
  
  # Werte über max_axis_y vernachlässigen
  if(max_axis_y>0) {
    y_matrix[,1][y_matrix[,1]>max_axis_y] <- NA    # measured concentration
    y_matrix[,3][y_matrix[,3]>max_axis_y] <- NA    # measured conductivity concentration
    y_matrix[,5][y_matrix[,5]>max_axis_y] <- NA    # calculated concentration
  }
  
  y_matrix$sicher <- NA
  y_matrix$sicher[which(y_matrix$flag_uncert==0)] <- y_matrix$measured[which(y_matrix$flag_uncert==0)]
  y_matrix$unsicher <- NA
  y_matrix$unsicher[which(y_matrix$flag_uncert!=0)] <- y_matrix$measured[which(y_matrix$flag_uncert!=0)]

  y_matrix$sicher.conductivity <- NA
  y_matrix$sicher.conductivity[which(y_matrix$flag_uncert_conductivity==0)] <- y_matrix$measured_conductivity[which(y_matrix$flag_uncert_conductivity==0)]
  y_matrix$unsicher.conductivity <- NA
  y_matrix$unsicher.conductivity[which(y_matrix$flag_uncert_conductivity!=0)] <- y_matrix$measured_conductivity[which(y_matrix$flag_uncert_conductivity!=0)]

  # maximal 5 Reihen in der Diagramm: sicher, sicher_measured, unsicher, unsicher_measured, calculated
  # measured könnten nicht alle da sein, dann sie nicht abbilden
  diagramme_rows<-c(0,0,0,0,1)
  if (length(which(is.na(y_matrix$sicher))) != length(y_matrix$sicher))
    diagramme_rows[1]<-1 
  if (length(which(is.na(y_matrix$sicher.conductivity))) != length(y_matrix$sicher.conductivity))
    diagramme_rows[2]<-1 
  if (length(which(is.na(y_matrix$unsicher))) != length(y_matrix$unsicher))
    diagramme_rows[3]<-1 
  if (length(which(is.na(y_matrix$unsicher.conductivity))) != length(y_matrix$unsicher.conductivity))
    diagramme_rows[4]<-1 
  if(sum(diagramme_rows[1:4])==0) diagramme_rows[1]<-1 # mindestens eine Reihe measured soll da sein, auch wenn keine measured Daten da sind
  idx.reihen=which(diagramme_rows==1)
  
  color_matrix= c("red", "red", "brown4", "brown4", "blue")[idx.reihen]
  # lty:   The line type. 0=blank, 1=solid (default), 2=dashed, 3=dotted, 4=dotdash, 5=longdash, 6=twodash) 
  #        or as one of the character strings 
  #       "blank", "solid", "dashed", "dotted", "dotdash", "longdash", or "twodash", where "blank" uses 'invisible lines' (i.e., does not draw them).
  # pch:   ?points gibt es Werte fuer pch_matrix
  lty_matrix = c("blank", "blank", "blank", "blank", "solid")[idx.reihen]
  lwd_matrix=c(8, 8, 8, 8, 1)[idx.reihen]; pch_matrix = c(15, 15, 15, 15, 15)[idx.reihen]
  type_col = c("p","p","p","p","l")[idx.reihen]
  legend_matrix = c("measured", "measured", "measured uncert", "measured uncert", "calculated")[idx.reihen]
  y.columns <- c(6, 8, 7, 9, 5)[idx.reihen] #c(4,5,3)

  diag_generate(x_col_format, y_matrix[,y.columns]
                , xlab_value = "", ylab_value = paste("concentration",unit, sep=" "), type_col = type_col, color_matrix
                , lty_matrix, lwd_matrix, pch_matrix
                , head1 = paste(head_data$bas0_id, ':', cell) , head2 = head_data$ParameterName
                , legend_matrix = legend_matrix, log_scale=log_scale 
                , is.X.date = T, las=labs.las)
  
  if (counter == diag.rows * diag.columns) {
    counter=1
    dev.off()
  } else {
    counter <- counter + 1
  }
  
} # for(cell)
} # Wenn Daten nach dem stations.list.check noch vorhanden sind

if (!is.null(dev.list())) dev.off()

} else {
  message("in der Datei ", filename, " keine Daten vorhanden!")
}