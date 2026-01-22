# wq_stat: RiverSection.R,  RiverSection_scen.R, station.R 
# wq_stat_stations: station_years.R, stations_scatterplot.R
# starten:
# source("[pfad]/start.R")
###################
rm(list=ls())



###############
#VOM NUTZER EINGEBEN

cont<-"af"
runid<-"run14_gr_3000_all"
startid<-203100009
subt<-"FC"
name<-"fecal coliform bacteria"
parameter_id<-310
load_id<-314
round<-TRUE

# Verzeichniss, hier werden Grafikdateien geschrieben
current_dir="C:/Users/reder/Documents/Meine_Arbeiten/paper von Klara/3. Paper - AF sensitivity analysis/Auswertung/analyse/FC"
output_dir=paste("C:/Users/reder/Documents/Meine_Arbeiten/paper von Klara/3. Paper - AF sensitivity analysis/Auswertung/analyse/FC/corr_factor/",cont,"/input_file",sep="")
setwd(current_dir)
path_files=paste("Z:/run/wq_stat_stations/FC/wwqa_idscen_9_",cont,"_corr_factor/",runid, sep="")
###############

setwd(path_files)
liste <- dir(path=path_files,pattern='.*\\.txt')
##liste <- c("datein.txt","pfclkjl.txt")
##combine all txt column-wise to one data.frame


txt <- read.csv( liste[1],as.is=T, sep="\t", skip=8)


if(length(liste) > 1){
  for(i in 2:length(liste)){
    dummy <- read.csv(liste[i],as.is=T, sep="\t", skip=9)
    #names(dummy)[2] <- gsub("\\.txt","",liste[i])
    #txt <- merge(txt,dummy,by.x=1,by.y=1,all=T)
    names(dummy) <- names(txt)
    txt <- rbind(txt, dummy)
    rm(dummy)
  }                           
}


if(round){
  txt$"Konz_1..mg.l"<-round(txt[,"Konz_1..mg.l"])
  txt$"measured"<-round(txt[,"measured"])
}

#wenn null dann NA setzen
txt$"measured"<-ifelse(txt[,"measured"]!=0, txt[,"measured"], NA)
txt$"Konz_1..mg.l"<-ifelse(txt[,"Konz_1..mg.l"]!=0, txt[,"Konz_1..mg.l"], NA)

txt$"measured"<-ifelse(txt[,"rout_area"]>=3000, txt[,"measured"], NA)

#doppelte Einträge löschen
txt_un<-unique(txt[,-1])
txt<-cbind(1,txt_un)


 

#ACHTUNG: header fehlt!
write.table(txt, file=paste(output_dir,"/",cont,"_id_scen_9_rout_",runid,".txt", 
                            sep=""), sep="\t", row.names =F, quote=F)






