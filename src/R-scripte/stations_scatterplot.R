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
  filename_begin <- "stations_scatterplot"
} else {
  filename_begin=paste("stations_scatterplot_",filename_prefix, sep="")
}

# initialisierung nur für den Fall, dass alle Dateien keine Daten liefern
stat_data<-data.frame(dummy=integer())  

if (diag.type==3) {
  liste <- dir(path=filename,pattern='.*\\.txt') 
  #if(length(liste)>100) liste<-liste[1:100]  # zum Testzwecken reichen mir 2 Dateien aus
  if(length(liste)==0) {
    stop("Keine Inputdateien im Ordner!")
  }
  used.input.number <- 1
  for (input.number in c(1:length(liste))) {
    x<-file_wq_stat_stations(paste(filename,"/",liste[input.number], sep=""))
    
    if(dim(x[[2]])[1]>0) { # nur wenn Datensatz nicht leer ist
      tmp_head_data<-x[[1]]
      tmp_stat_data<-x[[2]]
      rm(x)
      
      # # # # # # # # # # # # # # # # # # # # # # 
      # Liste der Stationen ueberpruefen nur wenn diag.type != 0 
      # (eine Diagramm mit allen Stationen aus der Inputdatei soll alle Stationen aus der Datei beinhalten,
      # auch wenn die Stationen in anderen Datei schon gibt) 
      # Falls Station schon in anderen Datei verwendet wurde, loeschen aus stat_data
      # Falls nicht - einfuegen in den vector stations.list
      if (diag.type != 0) {
        tmp_stat_data <- stations.list.check(stations.list, tmp_stat_data)
      }
      
      if (dim(tmp_stat_data)[1]==0) { # Wenn keine Daten nach dem stations.list.check vorhanden sind
        #message("in der Datei ", paste(filename,"/",liste[input.number], sep=""), " keine neue Daten vorhanden!")
      } else {
        tmp_head_data$input.number <- input.number
        tmp_stat_data$input.number <- input.number
        
        if (used.input.number ==1) {
          head_data<- tmp_head_data
          stat_data<- tmp_stat_data      
        } else{
          head_data<- rbind(head_data, tmp_head_data)
          stat_data<- rbind(stat_data, tmp_stat_data)
        }
        used.input.number <- used.input.number + 1
        
      } 
      
      rm(tmp_head_data)
      rm(tmp_stat_data)      
    } else {
      rm(x)
    }
    

  } # for(input.number)
} else { # nur eine Datei
  x<-file_wq_stat_stations(filename)
  
  if(dim(x[[2]])[1]>0) { # nur wenn Datensatz nicht leer ist
    head_data<-x[[1]]
    stat_data<-x[[2]]
    
    # # # # # # # # # # # # # # # # # # # # # # 
    # Liste der Stationen ueberpruefen nur wenn diag.type != 0 
    # (eine Diagramm mit allen Stationen aus der Inputdatei soll alle Stationen aus der Datei beinhalten,
    # auch wenn die Stationen in anderen Datei schon gibt) 
    # Falls Station schon in anderen Datei verwendet wurde, loeschen aus stat_data
    # Falls nicht - einfuegen in den vector stations.list
    if (diag.type != 0) {
      stat_data <- stations.list.check(stations.list, stat_data)
    }
    
    # um glechen format für alles zu haben
    head_data$input.number <- 1
    stat_data$input.number <- 1
    
  }
  rm(x)
}
# # # # # # # # # # # # # # # # # # # # # # 
if (dim(stat_data)[1]==0) { # Zeilenanzahl
  stop("keine daten vorhanden!")
}

# Zaehler fuer Diagrammen im Fenster
counter=1

# Zaehler fuer Outputdateien
counter_files <- counter.files.check(counter.files)

# initiiren 
x_col_allstations = NULL
y_matrix_allstations= NULL

if (length(row.names(subset(stat_data[, c(2,6,8)], (!is.na(measured) | !is.na(measured_conductivity)))))==0) {
  stop("Keine Messdaten vorhanden!")
}

# Diagrammen nur fuer die Zellen, die gemessene Werte haben
# 2: ArcID, 6: measured, 8: measured_conductivity
for (cell in unique(subset(stat_data[, c(2,6,8)], (!is.na(measured) | !is.na(measured_conductivity)))[, 1]))
{  

  #Diagramm fuer eine Station####################################
  # 2: ArcID, 3: date
  x_col=subset(stat_data[, c(2,3)], ArcId == cell)[, 2]
  #x_col_format<-format(ISOdate(floor(x_col/100),x_col%%100,1), datum.format)
  x_col_format<-ISOdate(floor(x_col/100),x_col%%100,1)
  
  #
  # 2: ArcID; 6-7: measured concentration & flag uncert; 4: calculated concentration; 11: input.number
  # 8-9: measured conductivity concentration & flag uncert conductivity;
  y_matrix=subset(stat_data[, c(2, 6, 7, 8, 9, 4, 11)], ArcId == cell)[, 2:7]   # y-Achs
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
  
  # Wenn y_matrix$measured und calculated haben Werte, die gleichzitig nicht NA sind weiter machen
  if ((length(y_matrix$measured[which(!is.na(y_matrix$Konz_1..mg.l) & !is.na(y_matrix$measured))])>0) ||
      (length(y_matrix$measured_conductivity[which(!is.na(y_matrix$Konz_1..mg.l) 
                                & !is.na(y_matrix$measured_conductivity))])>0)) {
    y_matrix$sicher <- NA
    y_matrix$sicher[which(y_matrix$flag_uncert==0)] <- y_matrix$measured[which(y_matrix$flag_uncert==0)]
    y_matrix$unsicher <- NA
    y_matrix$unsicher[which(y_matrix$flag_uncert!=0)] <- y_matrix$measured[which(y_matrix$flag_uncert!=0)]
    
    y_matrix$sicher.conductivity <- NA
    y_matrix$sicher.conductivity[which(y_matrix$flag_uncert_conductivity==0)] <- y_matrix$measured_conductivity[which(y_matrix$flag_uncert_conductivity==0)]
    y_matrix$unsicher.conductivity <- NA
    y_matrix$unsicher.conductivity[which(y_matrix$flag_uncert_conductivity!=0)] <- y_matrix$measured_conductivity[which(y_matrix$flag_uncert_conductivity!=0)]

    # unsichere Werte Dreieck (17) sonst Rechteck (15)
    y_matrix$pch <- scatter.pch
    y_matrix$pch[which(y_matrix$flag_uncert==1)] <- scatter.pch.uncert

    y_matrix$pch.conductivity <- scatter.pch
    y_matrix$pch.conductivity[which(y_matrix$flag_uncert_conductivity==1)] <- scatter.pch.uncert
    
    if (diag.type == 0 | diag.type == 2 | diag.type == 3) {  # Daten sammeln fuer Diagramm mit allen Stationen
      x_col_allstations <- append(x_col_allstations, x_col)
      # Spalte bas0_id in y_matrix einfuegen in jede Zelle gleichen Wert aus head_data
      # damit Order nach merge gleich bleibt, Reihenfolge in der Spalte order speichern und nach dem merge 
      # nach order sortieren, danach Spalte order löschen, sie wird weiter nicht benoetigt
      y_matrix$order <- 1:nrow(y_matrix)
      y_matrix<-merge(y_matrix, head_data[,c("input.number", "bas0_id")], by = "input.number", all.x=TRUE)
      y_matrix<-y_matrix[sort(y_matrix$order,index.return=TRUE)[[2]],]
      y_matrix$order<-NULL
      y_matrix_allstations<-rbind(y_matrix_allstations, y_matrix)
    }
    
    if (diag.type == 1 | diag.type == 2) { # Diagramm fuer eine Station erstellen
      if (counter==1) {
        #open_file(paste(filename_begin,"_",head_data$river_name,"_",head_data$IDrun,"_",counter_files,".png", sep="")
        #          , rows=diag.rows, columns=diag.columns)
        open_file(paste(filename_begin, "_",head_data$bas0_id,"_",head_data$IDrun,"_",counter_files,".png", sep="")
                  , rows=diag.rows, columns=diag.columns, width=30, height=30)
        counter_files <- counter_files + 1
        idx_bas0_id<-which(counter.files$bas0_id==head_data$bas0_id)
        counter.files[idx_bas0_id,]$counter_files <- counter.files[idx_bas0_id,]$counter_files+1
      }
      color_matrix= c("red")
      lty_matrix = c(0); lwd_matrix=c(1); pch_matrix = c(scatter.pch)
      
      diag_generate(c(y_matrix$measured, y_matrix$measured_conductivity)
                    , as.matrix(c(y_matrix$"Konz_1..mg.l",y_matrix$"Konz_1..mg.l"))
                    , xlab_value="observed", ylab_value="simulated", type_col=c("p")
                    , color_matrix = color_matrix
                    , lty_matrix = lty_matrix, lwd_matrix = lwd_matrix
                    , pch_matrix = c(y_matrix$pch, y_matrix$pch.conductivity)
                    , head1=head_data$ParameterName
                    #, head2=paste("bas0: ",head_data$bas0_id)
                    #, head2=paste(head_data$river_name, ':', cell) 
                    , head2=paste("bas0: ",head_data$bas0_id, ",  ArcID: ", cell, sep="") 
                    , legend_matrix = NULL, log_scale=log_scale, plot_diag=T, points.size = scatter.points.size)
      
      if (counter == diag.rows * diag.columns) {
        counter=1
        dev.off()
      } else {
        counter <- counter + 1
      }
    }
    
  } # end of if: y_matrix$measured hat Werte, die nicht NA sind
  
} # for(cell)
if (!is.null(dev.list())) dev.off()


#Diagramm mit allen Stationen########################
if (diag.type == 0 | diag.type == 2 | diag.type == 3) {
  # Diagramm nur erstellen, wenn gemessene Daten gibt
  if (is.null(y_matrix_allstations)) {
    print(paste("Keine Daten in ",filename))
  }
  else if (length(which(!is.na(y_matrix_allstations$measured))) > 0 | 
             length(which(!is.na(y_matrix_allstations$measured_conductivity))) > 0) {

    if (length(unique(y_matrix_allstations$input.number)) == 1) { # nur eine Inputdatei: dann auch ein bas0_id
      filename=paste(filename_begin,"_",head_data$river_name,"_",head_data$IDrun,".png", sep="")
      #filename=paste(filename_begin,"_",head_data$bas0_id,"_",head_data$IDrun,".png", sep="")
      color_matrix= c("red")
      lty_matrix = c(0); lwd_matrix=c(1)   #; pch_matrix = c(15)

      input.number <- 1
      head1 = head_data$ParameterName
      head2=paste("bas0: ",head_data$bas0_id, ",  river: ", head_data$river_name, sep="")
      
    } else {
      filename=paste(filename_begin, ".png", sep="")
      if (scatter.color.type == 1) {
        # farben fuer jede Datei
        input.number <- length(unique(y_matrix_allstations$input.number))
      } else if (scatter.color.type == 0) {
        # farben nach bas0_id
        input.number <- length(unique(y_matrix_allstations$bas0_id))
      } else {
        # einfarbig
        input.number <- 1
      }
      
      # default: end= max(1, n - 1)/n, dann wird Farbskala mit rot anfangen und enden
      color_matrix <- rainbow(input.number, end = 0.85)
      
      lty_matrix = rep(0, input.number); lwd_matrix=rep(1, input.number)
      #pch_matrix = rep(15, input.number)
      head1 <- ""
      head2 <- ""
    }
    
    if (scatter.color.type == 1) {
      # Jede Datei hat eigene Farbe
      y_matrix_allstations$input.number.durchgehend <- as.factor(y_matrix_allstations$input.number)
      levels(y_matrix_allstations$input.number.durchgehend) <- as.factor(c(1:input.number))
      y_matrix_allstations$col <- color_matrix[y_matrix_allstations$input.number.durchgehend]
    } else if (scatter.color.type == 0) {
      # jeder bas0_id hat eigene Farbe
      y_matrix_allstations$bas0_id.durchgehend <- as.factor(y_matrix_allstations$bas0_id)
      y_matrix_allstations$bas0_id.durchgehend0 <- y_matrix_allstations$bas0_id.durchgehend
      levels(y_matrix_allstations$bas0_id.durchgehend) <- as.factor(c(1:input.number))
      y_matrix_allstations$col <- color_matrix[y_matrix_allstations$bas0_id.durchgehend]
    } else {
      # einfarbig
      y_matrix_allstations$col <- "black"
    }
    
    # unsichere Werte Dreieck (17) sonst Rechteck (15)
    y_matrix_allstations$pch <- scatter.pch
    y_matrix_allstations$pch[which(y_matrix_allstations$flag_uncert==1)] <- scatter.pch.uncert
    #y_matrix_allstations$pch[which(y_matrix_allstations$input.number>30)] <- 19
    
    y_matrix_allstations$pch.conductivity <- scatter.pch
    y_matrix_allstations$pch.conductivity[which(y_matrix_allstations$flag_uncert_conductivity==1)] <- scatter.pch.uncert
    
    if (diag.type == 3 && scatter.color.type != 3 ) {
      # zwei Diagrammen: erste scatterplot, zweite: Farbenzuordnung
      open_file(filename, rows=2, columns=1, mar=c(3.1, 4.1, 1.1, 5.1), width=30, height=30)
      # zweite Diagramm mit Farben soll weniger Platz nehmen (1/5)
      layout(matrix(c(1,2), 2, 1, byrow = TRUE), heights=c(4,1))
    } else {
      # nur eine Diagramm: scatterplot
      #open_file(filename, rows=1, columns=1)
      open_file(filename, rows=1, columns=1, mar=c(7.1, 4.1, 4.1, 5.1), mgp=c(2,0.5,0))
      # mgp=c(1.5,0.5,0) The margin line (in mex units) for the axis title, axis labels and axis line.
      # mar=c(7.1, 4.1, 4.1, 5.1) c(bottom, left, top, right)
    }
    #if (length(which(is.na(y_matrix$unsicher))) != length(y_matrix$unsicher)) {}
    
    idx<-!is.na(y_matrix_allstations$measured)
    idx.conductivity<-!is.na(y_matrix_allstations$measured_conductivity)
    diag_generate(c(y_matrix_allstations$measured[idx], y_matrix_allstations$measured_conductivity[idx.conductivity])
                  , as.matrix(c(y_matrix_allstations$"Konz_1..mg.l"[idx]
                                ,y_matrix_allstations$"Konz_1..mg.l"[idx.conductivity]))
                  , xlab_value = "observed", ylab_value = "simulated", type_col=c("p")
                  , color_matrix = c(y_matrix_allstations$col[idx], y_matrix_allstations$col[idx.conductivity]) # c("black") 
                  , lty_matrix = lty_matrix, lwd_matrix = lwd_matrix
                  , pch_matrix = c(y_matrix_allstations$pch[idx], y_matrix_allstations$pch.conductivity[idx.conductivity])
                  , head1 = head1, head2=head2, 
                  , legend_matrix = c(""), log_scale=log_scale, plot_diag=T, points.size = scatter.points.size
                  , cex.axis = scatter.cex.axis, cex.lab = scatter.cex.lab)
    
    if (diag.type == 3 && scatter.color.type != 3 ) { # legende
      plot(x=1:input.number, y=rep(0,input.number), col=color_matrix[1:input.number]
           , yaxt="n" # keine y-Achse
           , bty="n"  # keinen Rahmen 
           , xlim=c(1, input.number), xlab="", ylab="", pch=scatter.pch)
    }
    
    dev.off()  
    
    # Falls Groesse der Abbildung nicht passt, bitte in Funktion legend() Parameter anpassen und 
    # Zeilen innerhalb diesen if-Abfrage noch mal ausfuehren. Man muss nicht ganzen Script laufen lassen, wenn
    # er lange läuft und danach noch keinen anderer Daten zerstoert hat
    # y.intersp - Abstand zwischen den Zeilen
    # cex - Schriftgroesse
    # ncol - Anzahl den Spalten in der Legende
    if (diag.type == 3 && scatter.color.type == 0) { # legende mit Farben in anderen Datei erstellen
      open_file(paste(filename_begin, "legend.png", sep=""), width=30, height=30, res=200
                , rows=1, columns=1, mar=c(5.1, 4.1, 4.1, 2.1), mgp=c(3,1,0))
      plot.new()
      legend("center", legend=levels(y_matrix_allstations$bas0_id.durchgehend0), col = color_matrix,
             pch=scatter.pch, y.intersp=1.5, cex=1.1, ncol=7)
      if (!is.null(dev.list())) dev.off()
    }
  }
} 
