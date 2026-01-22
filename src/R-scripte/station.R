#Verzeichnisse bestimmen Funktionen und Daten einlesen####
# Unser Verzeichniss, hier werden Grafikdateien geschrieben
setwd(current_dir)
getwd()

source(paste(r_script_dir,"/","diagramm_functions.R", sep=""))
# 
if (filename_prefix=="") {
  filename_begin <- "station"
} else {
  filename_begin=paste("station_",filename_prefix, sep="")
}

x<-file_wq_stat(filename)

head_data<-x[[1]]
stat_data<-x[[2]]
rm(x)

# # # # # # # # # # # # # # # # # # # # # # 
# Zaehler fuer Diagrammen im Fenster
counter=1

# Zaehler fuer Outputdateien
counter_files=1

if (length(row.names(subset(stat_data[, c(2,8)], !is.na(measured))))==0) {
  stop("Keine Messdaten vorhanden!")
}

# Diagrammen fuer alle Zellen
#for (cell in unique(stat_data$ArcId)) 

# Diagrammen nur fuer die Zellen, die gemessene Werte haben
for (cell in unique(subset(stat_data[, c(2,8)], !is.na(measured))[, 1])) # 2: ArcID, 8: measured
{
  # Diagramm 1####################################
  #
  x_col=subset(stat_data[, c(2,3)], ArcId == cell)[, 2] # 2: ArcID, 3: date
  #Beschriftung x-Achse im Format "Jan 2000"
  #x_col_format<-format(ISOdate(floor(x_col/100),x_col%%100,1), datum.format)
  x_col_format<-ISOdate(floor(x_col/100),x_col%%100,1)
  
  # 2: ArcID; 8-9: measured concentration & flag uncert; 
  # 10-11: measured conductivity & flag uncert conductivity; 6: calculated concentration
  y_matrix=subset(stat_data[, c(2,8,9,10,11,6)], ArcId == cell)[, 2:6]   # y-Achs
  y_matrix$measured[y_matrix$measured==0] <- NA    # measured
  y_matrix$measured_conductivity[y_matrix$measured_conductivity==0] <- NA    # measured conductivity
  y_matrix$concentration[y_matrix$concentration==0] <- NA              # concentration

  # für logarithmische Scale Werte unter min_log_scale vernachlässigen
  #if(log_scale != "" && min_log_scale>0) {
  #  y_matrix[,1][y_matrix[,1]<min_log_scale] <- NA  # measured concentration
  #  y_matrix[,3][y_matrix[,3]<min_log_scale] <- NA  # calculated concentration
  #}
  # Werte unter min_axis_y vernachlässigen
  if(min_axis_y>0) {
    y_matrix[,1][y_matrix[,1]<min_axis_y] <- NA  # measured concentration
    y_matrix[,3][y_matrix[,3]<min_axis_y] <- NA  # measured conductivity
    y_matrix[,5][y_matrix[,5]<min_axis_y] <- NA  # calculated concentration
  }
  
  # Werte über max_axis_y vernachlässigen
  if(max_axis_y>0) {
    y_matrix[,1][y_matrix[,1]>max_axis_y] <- NA  # measured concentration
    y_matrix[,3][y_matrix[,3]>max_axis_y] <- NA  # measured conductivity
    y_matrix[,5][y_matrix[,5]>max_axis_y] <- NA  # calculated concentration
  }
  
  y_matrix$sicher <- NA
  y_matrix$sicher[which(y_matrix$flag_uncert==0)] <- y_matrix$measured[which(y_matrix$flag_uncert==0)]
  y_matrix$unsicher <- NA
  y_matrix$unsicher[which(y_matrix$flag_uncert!=0)] <- y_matrix$measured[which(y_matrix$flag_uncert!=0)]
  
  y_matrix$sicher.conductivity <- NA
  y_matrix$sicher.conductivity[which(y_matrix$flag_uncert_conductivity==0)] <- y_matrix$measured_conductivity[which(y_matrix$flag_uncert_conductivity==0)]
  y_matrix$unsicher.conductivity <- NA
  y_matrix$unsicher.conductivity[which(y_matrix$flag_uncert_conductivity!=0)] <- y_matrix$measured_conductivity[which(y_matrix$flag_uncert_conductivity!=0)]

  for (year in unique(floor(x_col/100)))
  {
    idx <- floor(x_col/100)==year

    # maximal 5 Reihen in der Diagramm: sicher, sicher_measured, unsicher, unsicher_measured, calculated
    # measured könnten nicht alle da sein, dann sie nicht abbilden
    diagramme_rows<-c(0,0,0,0,1)
    if (length(which(is.na(y_matrix$sicher[idx]))) != length(y_matrix$sicher[idx]))
      diagramme_rows[1]<-1 
    if (length(which(is.na(y_matrix$sicher.conductivity[idx]))) != length(y_matrix$sicher.conductivity[idx]))
      diagramme_rows[2]<-1 
    if (length(which(is.na(y_matrix$unsicher[idx]))) != length(y_matrix$unsicher[idx]))
      diagramme_rows[3]<-1 
    if (length(which(is.na(y_matrix$unsicher.conductivity[idx]))) != length(y_matrix$unsicher.conductivity[idx]))
      diagramme_rows[4]<-1 
    if(sum(diagramme_rows[1:4])==0) diagramme_rows[1]<-1 # mindestens eine Reihe measured soll da sein, auch wenn keine measured Daten da sind
    idx.reihen=which(diagramme_rows==1)
    
    color_matrix= c("red", "red", "brown4", "brown4", "blue")[idx.reihen]
    # lty:   The line type. 0=blank, 1=solid (default), 2=dashed, 3=dotted, 4=dotdash, 5=longdash, 6=twodash) 
    #        or as one of the character strings 
    #       "blank", "solid", "dashed", "dotted", "dotdash", "longdash", or "twodash", where "blank" uses 'invisible lines' (i.e., does not draw them).
    # pch:   ?points gibt es Werte fuer pch_matrix
    lty_matrix = c("blank", "blank", "blank", "blank", "blank")[idx.reihen]
    lwd_matrix=c(1, 1, 1, 1, 1)[idx.reihen]; pch_matrix = c(15, 15, 15, 15, 15)[idx.reihen]
    type_col = c("p","p","p","p","p")[idx.reihen]
    legend_matrix = c("measured", "measured", "measured uncert", "measured uncert", "calculated")[idx.reihen]
    y.columns <- c(6, 8, 7, 9, 5)[idx.reihen] #c(4,5,3)
    
    if (counter==1) {
      # neue Datei(oder Fenster) fuer Diagrammen oeffnen
      open_file(paste(filename_begin, "_",head_data$river_name,"_",head_data$IDrun,"_",counter_files,".png", sep=""), 
                rows=diag.rows, columns=diag.columns, mgp=c(1.5,0.5,0), mar=c(7.1, 4.1, 4.1, 5.1))
      counter_files <- counter_files + 1
    }
    
    diag_generate(x_col_format[idx], y_matrix[idx,][,y.columns]  # bei y_matrix spalten sicher measured, unsicher measured und concentration nehmen
                  , xlab_value = "", ylab_value = paste("concentration",unit, sep=" ")
                  , type_col = type_col, color_matrix = color_matrix
                  , lty_matrix, lwd_matrix, pch_matrix
                  , head1 = paste(head_data$river_name, ':', cell, 'concentration', year, sep=" ")
                  , head2 = head_data$ParameterName
                  , legend_matrix = legend_matrix, log_scale=log_scale
                  , is.X.date = TRUE, las=labs.las)
    
    if (counter == diag.rows * diag.columns) {
      counter=1
      dev.off()
    } else {
      counter <- counter + 1
    }
  } # for(year)
  
  # jede Zelle in eigener Datei
  counter=1
  if (!is.null(dev.list())) dev.off()
  
} # for(cell)
if (!is.null(dev.list())) dev.off()

