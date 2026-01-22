#Verzeichnisse bestimmen Funktionen und Daten einlesen####
# Unser Verzeichniss
setwd(current_dir)
getwd()

source(paste(r_script_dir,"/","diagramm_functions.R", sep=""))

# 
if (filename_prefix=="") {
  filename_begin <- "scen"
} else {
  filename_begin=paste("scen_",filename_prefix, sep="")
}

# Daten einlesen (mit wq_stat generiert)####
#### erste Datei
x<-file_wq_stat(filename)
if(dim(x[[2]])[1]>0) { # nur wenn Datensatz nicht leer ist
  head_data<-x[[1]]
  stat_data1<-x [[2]]
  rm(x)
} else {
  message("in der Datei1 ", filename, " keine Daten vorhanden!")
  stat_data1<-data.frame(dummy=integer())
}

#### zweite Datei
x<-file_wq_stat(filename2)
if(dim(x[[2]])[1]>0) { # nur wenn Datensatz nicht leer ist
  head_data[2,]<-x[[1]]
  stat_data2<-x [[2]]
  rm(x)
} else {
  message("in der Datei2 ", filename2, " keine Daten vorhanden!")
  stat_data2<-data.frame(dummy=integer())
}

scen.number<-c(1,2)
#### 3. Datei
if (filename3!="") {
  x<-file_wq_stat(filename3)
} else {
  x<-list(0,0)
}
if(filename3!="" && dim(x[[2]])[1]>0) { # nur wenn Datensatz nicht leer ist
  head_data[3,]<-x[[1]]
  stat_data3<-x [[2]]
  scen.number[3]<-3 # damit szenario aufgenommen wird
  rm(x)
} else {
  if (filename3!="") message("in der Datei3 ", filename3, " keine Daten vorhanden!")
  stat_data3<-data.frame(dummy=integer())
}

#### 4. Datei
if (filename4!="") {
  x<-file_wq_stat(filename4)
} else {
  x<-list(0,0)
}
if(filename4!="" && dim(x[[2]])[1]>0) { # nur wenn Datensatz nicht leer ist
  head_data[4,]<-x[[1]]
  stat_data4<-x [[2]]
  scen.number[4]<-4 # damit szenario aufgenommen wird
  rm(x)
} else {
  if (filename4!="") message("in der Datei4 ", filename4, " keine Daten vorhanden!")
  stat_data4<-data.frame(dummy=integer())
}

if (dim(stat_data1)[1]>0 || dim(stat_data2)[1]>0 || dim(stat_data3)[1]>0 || dim(stat_data4)[1]>0 ) {
  
# # # # # # # # # # # # # # # # # # # # # # 
head_data$scen <- scen[scen.number]

# nur zum testzwecken, weil nur eine datei als input################
# 6: calculated concentration; 14: river discharge
#stat_data2[,6]<-stat_data2[,6]*1.1
#stat_data3[,6]<-stat_data3[,6]*1.2
#stat_data4[,6]<-stat_data4[,6]*1.3
#stat_data2[,14]<-stat_data2[,14]*1.1
#stat_data3[,14]<-stat_data3[,14]*1.2
#stat_data4[,14]<-stat_data4[,14]*1.3

# # # # # # # # # # # # # # # # # # # # # # 
# damit stop() richtig funktioniert, starten den script mit source("RiverSection_scen.R")
# Dateien sollen von Gleichen Flussabschnitt, gleiche Zeitreihe sein, untersschied nur Szenarien!
if (length(unique(head_data$arcid_begin)) != 1 ) {
  stop("Dateien muessen gleichen Flussabschnitt beinhalten!")
} 
if (length(unique(head_data$arcid_end)) != 1 ) {
  stop("Dateien muessen gleichen Flussabschnitt beinhalten!")
}
if (length(unique(head_data$river_name)) != 1 ) {
  stop("Dateien muessen gleichen Fluss beinhalten!")
}
# # # # # # # # # # # # # # # # # # # # # # 

# Zaehler fuer Diagrammen im Fenster
counter=1
# Zaehler fuer Outputdateien
counter_files=1

# # # # # # # # # # # # # # # # # # # # # # 

for (month in unique(stat_data1$date)) {

  # Diagramm 1 ####################################
  if (substr(RiverSection.diag,1,1) != 0) {
    if (counter==1) {
      # neue Datei(oder Fenster) fuer Diagrammen oeffnen
      open_file(paste(filename_begin, "_",head_data$river_name[1],"_",head_data$IDrun[1],"_",counter_files,".png", sep="")
                , rows=diag.rows, columns=diag.columns)
    }
    # 3: date; 5: river length
    x_col=subset(stat_data1[, c(3,5)], date == month)[, 2]
    # 3: date; 6: calculated concentration
    y_matrix=subset(stat_data1[, c(3, 6)], date == month)   # y-Achse
    y_matrix$date<-NULL # Spalte date wird nicht weiter gebraucht
    names(y_matrix)<-"conc"
    y_matrix$conc[y_matrix$conc==0] <- NA  # calculated concentration
    
    y_matrix$conc2=subset(stat_data2[, c(3,6)], date == month)[, 2]   # y-Achse
    y_matrix$conc2[y_matrix$conc2==0] <- NA
    
    if (dim(stat_data3)[1]>0) {
      y_matrix$conc3=subset(stat_data3[, c(3,6)], date == month)[, 2]   # y-Achse
      y_matrix$conc3[y_matrix$conc3==0] <- NA
    }
    
    if (dim(stat_data4)[1]>0) {
      y_matrix$conc4=subset(stat_data4[, c(3,6)], date == month)[, 2]   # y-Achse
      y_matrix$conc4[y_matrix$conc4==0] <- NA
    }
    
    # für logarithmische Scale Werte unter min_log_scale vernachlässigen
    #if(log_scale != "" && min_log_scale>0) {
    #  y_matrix[,1][y_matrix[,1]<min_log_scale] <- NA    # calculated concentration scen1
    #  y_matrix[,2][y_matrix[,2]<min_log_scale] <- NA    # calculated concentration scen2
    #  y_matrix[,3][y_matrix[,3]<min_log_scale] <- NA    # calculated concentration scen3
    #  y_matrix[,4][y_matrix[,4]<min_log_scale] <- NA    # calculated concentration scen4
    #}
    
    # Werte unter min_axis_y vernachlässigen
    if(min_axis_y>0) {
      y_matrix[,1][y_matrix[,1]<min_axis_y] <- NA    # calculated concentration scen1
      y_matrix[,2][y_matrix[,2]<min_axis_y] <- NA    # calculated concentration scen2
      if (dim(stat_data3)[1]>0) y_matrix[,3][y_matrix[,3]<min_axis_y] <- NA    # calculated concentration scen3
      if (dim(stat_data4)[1]>0) y_matrix[,4][y_matrix[,4]<min_axis_y] <- NA    # calculated concentration scen4
    }
    
    # Werte über max_axis_y vernachlässigen
    if(max_axis_y>0) {
      y_matrix[,1][y_matrix[,1]>max_axis_y] <- NA    # calculated concentration scen1
      y_matrix[,2][y_matrix[,2]>max_axis_y] <- NA    # calculated concentration scen2
      if (dim(stat_data3)[1]>0) y_matrix[,3][y_matrix[,3]>max_axis_y] <- NA    # calculated concentration scen3
      if (dim(stat_data4)[1]>0) y_matrix[,4][y_matrix[,4]>max_axis_y] <- NA    # calculated concentration scen4
    }

    color_matrix= c("blue", "gray", "magenta", "cyan")[scen.number]
    lty_matrix = c(1, 1, 1, 1)[scen.number]; lwd_matrix=c(2, 2, 2, 2)[scen.number]; pch_matrix = c(NA, NA, NA, NA)[scen.number]
    
    diag_generate(x_col, y_matrix, xlab_value = "length [km]", ylab_value = paste("concentration",unit, sep=" ")
                  , type_col = c("l", "l", "l","l")[scen.number], color_matrix = color_matrix
                  , lty_matrix = lty_matrix, lwd_matrix = lwd_matrix, pch_matrix = pch_matrix
                  , head1 = paste(head_data$river_name, ':', month), head2 = head_data$ParameterName
                  , legend_matrix = head_data$scen, log_scale=log_scale)

    x <- next_diag(counter, counter_files, device.off= FALSE)
    counter <- x[1]; counter_files <-x[2]
    
  } # Diagramm 1
  
  # Diagramm 2 ####################################
  if (substr(RiverSection.diag,2,2) != 0) {
    if (counter==1) {
      # neue Datei(oder Fenster) fuer Diagrammen oeffnen
      open_file(paste(filename_begin, "_",head_data$river_name[1],"_",head_data$IDrun[1],"_",counter_files,".png", sep="")
                , rows=diag.rows, columns=diag.columns)
    }
    x_col=subset(stat_data1[, c(3,5)], date == month)[, 2]
    # 3: date; 14: river discharge;
    y_matrix=subset(stat_data1[, c(3,8,14)], date == month)[, 2:3]   # y-Achse
    y_matrix[,2][y_matrix[,2]==0] <- NA
    y_matrix[,1] <- NULL   # gebraucht wird nur Spalte river discharge
    names(y_matrix)<-"rdis1"
    
    y_matrix$rdis2=subset(stat_data2[, c(3,14)], date == month)[, 2]   # y-Achse
    y_matrix$rdis2[y_matrix$rdis2==0] <- NA
    
    if (dim(stat_data3)[1]>0) {
      y_matrix$rdis3=subset(stat_data3[, c(3,14)], date == month)[, 2]   # y-Achse
      y_matrix$rdis3[y_matrix$rdis3==0] <- NA
    }
    
    if (dim(stat_data4)[1]>0) {
      y_matrix$rdis4=subset(stat_data4[, c(3,14)], date == month)[, 2]   # y-Achse
      y_matrix$rdis4[y_matrix$rdis4==0] <- NA
    }
    
    # für logarithmische Scale Werte unter min_log_scale vernachlässigen
    #if(log_scale != "" && min_log_scale>0) {
    #  y_matrix[,1][y_matrix[,1]<min_log_scale] <- NA    # calculated concentration scen1
    #  y_matrix[,2][y_matrix[,2]<min_log_scale] <- NA    # calculated concentration scen2
    #  y_matrix[,3][y_matrix[,3]<min_log_scale] <- NA    # calculated concentration scen3
    #  y_matrix[,4][y_matrix[,4]<min_log_scale] <- NA    # calculated concentration scen4
    #}
    
    # Werte unter min_axis_y vernachlässigen
    if(min_axis_y>0) {
      y_matrix[,1][y_matrix[,1]<min_axis_y] <- NA    # calculated concentration scen1
      y_matrix[,2][y_matrix[,2]<min_axis_y] <- NA    # calculated concentration scen2
      if (dim(stat_data3)[1]>0) y_matrix[,3][y_matrix[,3]<min_axis_y] <- NA    # calculated concentration scen3
      if (dim(stat_data4)[1]>0) y_matrix[,4][y_matrix[,4]<min_axis_y] <- NA    # calculated concentration scen4
    }
    
    # Werte über max_axis_y vernachlässigen
    if(max_axis_y>0) {
      y_matrix[,1][y_matrix[,1]>max_axis_y] <- NA    # calculated concentration scen1
      y_matrix[,2][y_matrix[,2]>max_axis_y] <- NA    # calculated concentration scen2
      if (dim(stat_data3)[1]>0) y_matrix[,3][y_matrix[,3]>max_axis_y] <- NA    # calculated concentration scen3
      if (dim(stat_data4)[1]>0) y_matrix[,4][y_matrix[,4]>max_axis_y] <- NA    # calculated concentration scen4
    }

    # mit colors() Farbenliste sehen
    color_matrix= c("blue", "gray", "magenta", "cyan")[scen.number]
    lty_matrix = c(1, 1, 1, 1)[scen.number]; lwd_matrix=c(2, 2, 2, 2)[scen.number]; pch_matrix = c(NA, NA, NA, NA)[scen.number]
    
    diag_generate(x_col, y_matrix, xlab_value = "length [km]", ylab_value = expression(paste("river discharge [",m^3, "/s]", sep=""))
                  , type_col = c("l", "l", "l","l")[scen.number], color_matrix = color_matrix
                  , lty_matrix = lty_matrix, lwd_matrix = lwd_matrix, pch_matrix = pch_matrix
                  , head1 = paste(head_data$river_name, ':', month), head2= head_data$ParameterName
                  , legend_matrix = head_data$scen, log_scale=log_scale)

    x <- next_diag(counter, counter_files, device.off= FALSE)
    counter <- x[1]; counter_files <-x[2]
  } # Diagramm 2
  
  #Diagramm 3####################################
  if (substr(RiverSection.diag,3,3) != 0) {
    if (counter==1) {
      # neue Datei(oder Fenster) fuer Diagrammen oeffnen
      open_file(paste(filename_begin, "_",head_data$river_name[1],"_",head_data$IDrun[1],"_",counter_files,".png", sep="")
                , rows=diag.rows, columns=diag.columns)
    }
    # 3: date; 5: river length
    x_col=subset(stat_data1[, c(3,5)], date == month)[, 2]
    xlimit<-range(x_col)
    # 3: date; 19: monthly grid cell loading accumulated; 26-27: measured loadind accumulated & flag uncert
    #y_matrix=subset(stat_data1[, c(3,19)], date == month)[, 2]    # y-Achse
    y_matrix=subset(stat_data1[, c(3,19)], date == month)    # y-Achse
    y_matrix[,2][y_matrix[,2]==0] <- NA # calculated loading
    y_matrix2=subset(stat_data2[, c(3,19)], date == month)    # y-Achse
    names(y_matrix2)[2]<-paste(names(y_matrix2)[2],"2", sep="")
    y_matrix<-cbind(y_matrix, y_matrix2[2])
    y_matrix[,3][y_matrix[,3]==0] <- NA # calculated loading2
    rm(y_matrix2)
    y_matrix$date<-NULL
    

    # für logarithmische Scale Werte unter min_log_scale vernachlässigen
    #if(log_scale != "" && min_log_scale>0) {
    #  y_matrix[,1][y_matrix[,1]<min_log_scale] <- NA    # monthly grid cell loading accumulated
    #  y_matrix[,2][y_matrix[,2]<min_log_scale] <- NA    # measured loadind accumulated
    #}
    # Werte unter min_axis_y vernachlässigen
    if(min_axis_y>0) {
      y_matrix[,1][y_matrix[,1]<min_axis_y] <- NA    # monthly grid cell loading accumulated
      y_matrix[,2][y_matrix[,2]<min_axis_y] <- NA    # monthly grid cell loading accumulated2
    }
    
    # Werte über max_axis_y vernachlässigen
    if(max_axis_y>0) {
      y_matrix[,1][y_matrix[,1]>max_axis_y] <- NA    # monthly grid cell loading accumulated
      y_matrix[,2][y_matrix[,2]>max_axis_y] <- NA    # monthly grid cell loading accumulated2
    }
    
    color_matrix= c("blue", "gray", "magenta", "cyan")[scen.number]
    lty_matrix = c(1, 1, 1, 1)[scen.number]; lwd_matrix=c(2, 2, 2, 2)[scen.number]; pch_matrix = c(NA, NA, NA, NA)[scen.number]
    type_col =  c("l", "l", "l","l")[scen.number]
    legend_matrix = head_data$scen

    matplot(x_col, y_matrix, type=type_col, xlab="length [km]", ylab=" monthly loading accumulated [t/month]", 
            col = color_matrix, lty=lty_matrix, lwd = lwd_matrix, pch=pch_matrix, log=log_scale,
            xlim=xlimit)
    mtext(head_data$ParameterName, side = 3, line=2.5, col=1, cex=1)
    mtext(paste(head_data$river_name, ':', format(ISOdate(floor(month/100),month%%100,1), "%b %Y")), side = 3, line=1, col=1, cex=1)
    
    if(log_scale == "y") {
      y_legend = 10^(par("usr")[3]-(par("usr")[4] -par("usr")[3])/3.5)
    } else {
      y_legend = par("usr")[3]-(par("usr")[4] -par("usr")[3])/3.5
    }
    
    legend(x=0, y=y_legend, legend=head_data$scen #legend_matrix
           , horiz=TRUE
           #, horiz=F, ncol=2
           , lty=lty_matrix, pch=pch_matrix, col=color_matrix, bty = "n", cex=1.2)
    
    # wenn man hier device.off= TRUE, dann wird für jede 3 Diagramen eigen Datei geöffnet, 
    # auch wenn diag.rows*diag.columns nicht erreicht ist
    x <- next_diag(counter, counter_files, device.off= FALSE)
    counter <- x[1]; counter_files <-x[2]
  } # Diagramm 3
  
  
} # for(month)

if (!is.null(dev.list())) dev.off()
} else {
  message("in der Datei ", filename, " keine Daten vorhanden!")
}
