#Verzeichnisse bestimmen Funktionen und Daten einlesen####
# Unser Verzeichniss, hier werden Grafikdateien geschrieben
setwd(current_dir)
getwd()

# so kriegen wir englische Beschriftung in Datum
#Sys.setlocale("LC_TIME","English") 
Sys.setlocale("LC_TIME","en_US.UTF-8") 


source(paste(r_script_dir,"/","diagramm_functions.R", sep=""))
# 
if (filename_prefix=="") {
  filename_begin <- ""
} else {
  filename_begin=paste(filename_prefix, "_", sep="")
}

x<-file_wq_stat(filename)

if(dim(x[[2]])[1]>0) { # nur wenn Datensatz nicht leer ist

head_data<-x[[1]]
stat_data<-x[[2]]
rm(x)

# # # # # # # # # # # # # # # # # # # # # # 
# Zaehler fuer Diagrammen im Fenster
counter=1
# Zaehler fuer Outputdateien
counter_files=1

for (month in unique(stat_data$date)) 
#for (month in c(200001, 200002, 200003))
{

	#Diagramm 1####################################
  if (substr(RiverSection.diag,1,1) != 0) {
    if (counter==1) {
      # neue Datei(oder Fenster) fuer Diagrammen oeffnen
      open_file(paste(filename_begin, head_data$river_name,"_",head_data$IDrun,"_",counter_files,".png", sep=""), 
                , rows=diag.rows, columns=diag.columns)
    }
    # 3: date; 5: length(km)
    x_col=subset(stat_data[, c(3,5)], date == month)[, 2]
    # 3: date; 8-9: measured concentration & flag uncert; 
    # 10-11: measured conductivity & flag uncert conductivity; 6: calculated concentration
    y_matrix=subset(stat_data[, c(3, 8, 9, 10, 11, 6)], date == month)[, 2:6]   # y-Achse
    y_matrix[,1][y_matrix[,1]==0] <- NA    # measured
    y_matrix[,3][y_matrix[,3]==0] <- NA    # measured conductivity
    y_matrix[,5][y_matrix[,5]==0] <- NA    # concentration
    
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
    lwd_matrix=c(8, 8, 8, 8, 1)[idx.reihen]; pch_matrix = c(15, 15, 15, 15, NA)[idx.reihen]
    type_col = c("p","p","p","p","l")[idx.reihen]
    legend_matrix = c("measured", "measured", "measured uncert", "measured uncert", "calculated")[idx.reihen]
    y.columns <- c(6, 8, 7, 9, 5)[idx.reihen] #c(4,5,3)

    diag_generate(x_col, y_matrix[,y.columns]
                  , xlab_value = "length [km]", ylab_value = paste("concentration",unit, sep=" ")
                  , type_col = type_col
                  , color_matrix, lty_matrix, lwd_matrix, pch_matrix
                  , head1 = head_data$ParameterName
                  , head2 = paste(head_data$river_name, ':', format(ISOdate(floor(month/100),month%%100,1), "%b %Y"))
                  , legend_matrix = legend_matrix, log_scale=log_scale)
    x <- next_diag(counter, counter_files, device.off= FALSE)
    counter <- x[1]; counter_files <-x[2]
  
  } # Diagramm 1
  
  #Diagramm 2####################################
  if (substr(RiverSection.diag,2,2) != 0) {
    if (counter==1) {
      # neue Datei(oder Fenster) fuer Diagrammen oeffnen
      open_file(paste(filename_begin, head_data$river_name,"_",head_data$IDrun,"_",counter_files,".png", sep=""), 
                , rows=diag.rows, columns=diag.columns)
    }
    # 3: date; 5: river length
    x_col=subset(stat_data[, c(3,5)], date == month)[, 2]
    # 3: date; 14: river discharge; 25: measured grdc; 28-29: measured others & flag uncert
    y_matrix=subset(stat_data[, c(3,14,25,28,29)], date == month)[, 2:5]   # y-Achse
    y_matrix[,1][y_matrix[,1]==0] <- NA
    y_matrix[,2][y_matrix[,2]==0] <- NA
    y_matrix[,3][y_matrix[,3]==0] <- NA
    
    y_matrix$sicher <- NA
    y_matrix$sicher[which(y_matrix$flag_uncert.discharge==0)] <- y_matrix$"measured others"[which(y_matrix$flag_uncert.discharge==0)]
    y_matrix$unsicher <- NA
    y_matrix$unsicher[which(y_matrix$flag_uncert.discharge!=0)] <- y_matrix$"measured others"[which(y_matrix$flag_uncert.discharge!=0)]
    
    if (length(which(is.na(y_matrix$unsicher))) != length(y_matrix$unsicher)) {
      unsicher_gibts = TRUE
      color_matrix= c("blue", "red", "purple", "brown4", "darkolivegreen3")
      # lty:   The line type. 0=blank, 1=solid (default), 2=dashed, 3=dotted, 4=dotdash, 5=longdash, 6=twodash) 
      #        or as one of the character strings 
      #       "blank", "solid", "dashed", "dotted", "dotdash", "longdash", or "twodash", where "blank" uses 'invisible lines' (i.e., does not draw them).
      # pch:   ?points gibt es Werte fuer pch_matrix
      lty_matrix = c("longdash", "blank", "blank", "blank", "solid")
      lwd_matrix=c(1, 8, 8, 8, 1); pch_matrix = c(NA, 16, 16, 16, NA)
      type_col = c("l", "p", "p", "p", "l")
      legend_matrix = c("river discharge"  # expression(paste("river discharge [",m^3, "/s]", sep=""))
                       , "measured river discharge", "measured others", "measured others uncert"
                       , "monthly grid cell loading")
      y.columns <- c(1,2,5,6)
    } else {
      unsicher_gibts = FALSE
      color_matrix= c("blue", "red", "purple", "darkolivegreen3")
      lty_matrix = c("longdash", "blank", "blank", "solid")
      lwd_matrix=c(1, 8, 8, 1); pch_matrix = c(NA, 16, 16, NA)
      type_col = c("l", "p", "p", "l")
      legend_matrix = c("river discharge"   #expression(paste("river discharge [",m^3, "/s]", sep=""))
                        , "measured river discharge", "measured others", "monthly grid cell loading")
      y.columns <- c(1,2,5)
    }
    matplot(x_col, y_matrix[,y.columns], type=type_col
            , xlab="length [km]", ylab=expression(paste("river discharge [",m^3, "/s]", sep=""))
            , col = color_matrix, lty=lty_matrix, lwd = lwd_matrix, pch=pch_matrix, log=log_scale)
    
    mtext(head_data$ParameterName, side = 3, line=2.5, col=1, cex=1)
    mtext(paste(head_data$river_name, ':', format(ISOdate(floor(month/100),month%%100,1), "%b %Y")), side = 3, line=1, col=1, cex=1)
    
    if(log_scale == "y") {
      y_legend = 10^(par("usr")[3]-(par("usr")[4] -par("usr")[3])/3.5)
    } else {
      y_legend = par("usr")[3]-(par("usr")[4] -par("usr")[3])/3.5
    }
    legend(x=0, y=y_legend 
           , legend=legend_matrix
           , horiz=TRUE
           #, horiz=F, ncol=2
           , lty=lty_matrix, pch=pch_matrix, col=color_matrix, bty = "n", cex=1.2)
    
    par(new = TRUE)  # Reihe in letzten Diagramm zeichnen. Reihe hat andere y-Achse, deswegen getrennt zeichnen. Y-Achse rechts
    # erstmal Reihe zeichnen ohne Achsen-Beschriftung, weil x gibt es schon und y soll rechts sein
    # axes = FALSE damit ziffern an achsen nicht oben auf ersten geschrieben werden
    # ann  = FALSE damit beschriftung der achse nicht oben auf ersten geschrieben werden
    # 3: date; 5: river length; 15: montly grid cell loading
    y_matrix=subset(stat_data[, c(3,5,15)], date == month)[, 2:3]    # y-Achse
    y_matrix[,2][y_matrix[,2]==0] <- NA 
    
    # für logarithmische Scale Werte unter min_log_scale vernachlässigen
    #if(log_scale != "" && min_log_scale>0) {
    #  y_matrix[,2][y_matrix[,2]<min_log_scale] <- NA    # montly grid cell loading
    #}
    # Werte unter min_axis_y vernachlässigen
    if(min_axis_y>0) {
      y_matrix[,2][y_matrix[,2]<min_axis_y] <- NA    # montly grid cell loading
    }
    
    # Werte über max_axis_y vernachlässigen
    if(max_axis_y>0) {
      y_matrix[,2][y_matrix[,2]>max_axis_y] <- NA    # montly grid cell loading
    }
    
    
    plot(y_matrix, type="l", col = color_matrix[length(color_matrix)], ylab=NA, axes=FALSE, ann = FALSE, log=log_scale)
    # erste Parameter side = 4 - rechts; (1=below, 2=left, 3=above and 4=right.)
    # las - Ausrichtung der Beschriftung 
    axis(4, col.axis="black", las = 3)
    mtext("montly grid cell loading [t/month]", side = 4, line=2, col=1)
    
    x <- next_diag(counter, counter_files, device.off= FALSE)
    counter <- x[1]; counter_files <-x[2]
  
  } # Diagramm 2
  
	
  #Diagramm 3####################################
  if (substr(RiverSection.diag,3,3) != 0) {
    if (counter==1) {
      # neue Datei(oder Fenster) fuer Diagrammen oeffnen
      open_file(paste(filename_begin, head_data$river_name,"_",head_data$IDrun,"_",counter_files,".png", sep=""), 
                , rows=diag.rows, columns=diag.columns)
    }
    # 2 Reihen: monthly loading accumulated (t/month); measured monthly loading
    # 3: date; 5: river length
    x_col=subset(stat_data[, c(3,5)], date == month)[, 2]
    xlimit<-range(x_col)
    # 3: date; 19: monthly grid cell loading accumulated; 26-27: measured loadind accumulated & flag uncert
    y_matrix=subset(stat_data[, c(3,19,26,27)], date == month)[, 2:4]    # y-Achse
    y_matrix[,1][y_matrix[,1]==0] <- NA
    y_matrix[,2][y_matrix[,2]==0] <- NA
    
    # für logarithmische Scale Werte unter min_log_scale vernachlässigen
    #if(log_scale != "" && min_log_scale>0) {
    #  y_matrix[,1][y_matrix[,1]<min_log_scale] <- NA    # monthly grid cell loading accumulated
    #  y_matrix[,2][y_matrix[,2]<min_log_scale] <- NA    # measured loadind accumulated
    #}
    # Werte unter min_axis_y vernachlässigen
    if(min_axis_y>0) {
      y_matrix[,1][y_matrix[,1]<min_axis_y] <- NA    # monthly grid cell loading accumulated
      y_matrix[,2][y_matrix[,2]<min_axis_y] <- NA    # measured loadind accumulated
    }
    
    # Werte über max_axis_y vernachlässigen
    if(max_axis_y>0) {
      y_matrix[,1][y_matrix[,1]>max_axis_y] <- NA    # monthly grid cell loading accumulated
      y_matrix[,2][y_matrix[,2]>max_axis_y] <- NA    # measured loadind accumulated
    }
    
    y_matrix$sicher <- NA
    y_matrix$sicher[which(y_matrix$flag_uncert.loading==0)] <- y_matrix$"measured loading"[which(y_matrix$flag_uncert.loading==0)]
    y_matrix$unsicher <- NA
    y_matrix$unsicher[which(y_matrix$flag_uncert.loading!=0)] <- y_matrix$"measured loading"[which(y_matrix$flag_uncert.loading!=0)]
    
    if (length(which(is.na(y_matrix$unsicher))) != length(y_matrix$unsicher)) {
      unsicher_gibts = TRUE
      color_matrix= c("darkolivegreen3", "brown4", "red")
      lty_matrix = c("solid", "blank", "blank"); lwd_matrix=c(1, 8, 8); pch_matrix = c(NA, 17, 17)
      type_col = c("l", "p", "p")
      legend_matrix = c("monthly loading accumulated", "measured monthly loading", "measured monthly loading uncert")
      y.columns <- c(1,4,5)
    } else {
      unsicher_gibts = FALSE
      color_matrix= c("darkolivegreen3", "red")
      lty_matrix = c("solid", "blank"); lwd_matrix=c(1, 8); pch_matrix = c(NA, 17)
      type_col = c("l", "p")
      legend_matrix = c("monthly loading accumulated", "measured monthly loading")
      y.columns <- c(1,4)
    }
    
    matplot(x_col, y_matrix[,y.columns], type=type_col, xlab="length [km]", ylab=" monthly loading accumulated [t/month]", 
            col = color_matrix, lty=lty_matrix, lwd = lwd_matrix, pch=pch_matrix, log=log_scale,
            , xlim=xlimit)
    mtext(head_data$ParameterName, side = 3, line=2.5, col=1, cex=1)
    mtext(paste(head_data$river_name, ':', format(ISOdate(floor(month/100),month%%100,1), "%b %Y")), side = 3, line=1, col=1, cex=1)
    
    if(log_scale == "y") {
      y_legend = 10^(par("usr")[3]-(par("usr")[4] -par("usr")[3])/3.5)
    } else {
      y_legend = par("usr")[3]-(par("usr")[4] -par("usr")[3])/3.5
    }
    
    legend(x=0, y=y_legend, legend=legend_matrix
           , horiz=TRUE
           #, horiz=F, ncol=2
           , lty=lty_matrix, pch=pch_matrix, col=color_matrix, bty = "n", cex=1.2)
    
    # wenn man hier device.off= TRUE, dann wird für jede 3 Diagramen eigen Datei geöffnet, 
    # auch wenn diag.rows*diag.columns nicht erreicht ist
    x <- next_diag(counter, counter_files, device.off= TRUE)
    counter <- x[1]; counter_files <-x[2]   
  } # Diagramm 3

} # for(month)

if (!is.null(dev.list())) dev.off()

} else {
  message("in der Datei ", filename, " keine Daten vorhanden!")
}
