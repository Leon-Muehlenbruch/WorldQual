#rm(list=ls())
#library(RODBC)      #f?r die Datenbankverbindung

# originaldatei : M:\reder\backup\Meine_Arbeiten\WWQA\arbeiten\final_report\ld_for_charts.R

# Aufgabe:
# barchart und piechart für LA, Africa und Asia
# loading: "Domestic sewered"         : ld$ld_treat_dom
#          "Domestic non-sewered"     : ld$ld_treat_sc+ld$ld_open_defec+ld$ld_hanging_l
#          "Manufacturing"            : ld$ld_treat_man
#          "Urban surface runoff"     : ld$ld_treat_urb
#          "Agriculture animal wastes": ld$ld_manure

# load soll Mittelwert von start_year; end_year sein
# für parameter: BOD (800), FC(310), TDS(260)

# LA: geoReg 4 LATIN AMERICA AND THE CARIBBEAN;  liegen in kontinenten: na, sa
#     geoSubREg 41	Caribbean      
#               42	Meso-America
#               43	South America
# Africa: geoReg 1 AFRICA                        liegen in Kontinenten: af
#         geoSubREg 11	Northern Africa         
#                   12	Central Africa
#                   13	Western Africa
#                   14	Western Indian Ocean 
#                   15	Eastern Africa
#                   16	Southern Africa
# Asia: geoReg    2	ASIA AND THE PACIFIC            liegen in Kontinenten: as, eu
#                 6	WEST ASIA
#       geoSubREg 21	South Asia                         
#                 22	South East Asia
#                 23	North West Pacific and East Asia
#                 24	Central Asia
#                 25	Australia and New Zealand
#                 26	South Pacific
#                 61	Arabian Peninsula
#                 62	Mashriq

#Datenbankverbindung wird ge?ffnet
#wq <- odbcConnect("grdc", believeNRows=FALSE)

current_dir<-"C:/Users/kynast/Documents/GridWasser/R_project/wwqa_poster_wien_042016/ld_for_charts"
output_csv<-"C:/Users/kynast/Documents/GridWasser/R_project/wwqa_poster_wien_042016/ld_for_charts/wwqa_ld_diag_pieInR3"
setwd(current_dir)
source("grid_arrange_shared_legend.R")

year_start<-2008
year_end<-2010
parameterList = c(310,260,80)
parameter.name=c("FC", "TDS", "BOD")

parameter.number=2
for(parameter.number in 1:3){
  parameter=parameterList[parameter.number]
  print(paste(parameter.number, parameter, parameter.name[parameter.number]))
  continents<-c("af", "as", "eu", "sa", "na")
  if (parameter==260) {
    ld<-matrix(ncol=9,NA)
    colnames(ld)<-c("region", "dom", "man",	"sc",	"open_defec",	"urb",	"manure",	"hanging_l", "irrig_return_flow")
  }  else {
    ld<-matrix(ncol=8,NA)
    colnames(ld)<-c("region", "dom", "man",	"sc",	"open_defec",	"urb",	"manure",	"hanging_l")
  }
  
  
  cont="af"
  for(cont in continents){
    #Kontinente festlesen GeoRegions
    if(cont=="af"){
      #Africa
      subgeo5.list <- sprintf("%02i",11:16)
      geo5.list<-1
    }else if(cont=="as"){
      #asia and the pacific
      subgeo5.list <- sprintf("%02i",c(21:26,61,62))
      geo5.list<-c(2,6)
    }else if(cont=="eu"){
      #west asia
      subgeo5.list <- sprintf("%02i",61:62)
      geo5.list<-6
    }else if(cont=="na"){
      #latin america and the caribbean
      subgeo5.list <- sprintf("%02i",41:43)
      geo5.list<-4
    }else if(cont=="sa"){
      #latin america and the caribbean
      subgeo5.list <- sprintf("%02i",41:43)
      geo5.list<-4
    }
    
    #Jahre die selectiert werden sollen speichern
    yearList<-paste(year_start,  sep="")
    #if more than one year is to be read
    if(year_start!=year_end){
      for(y in (year_start+1):year_end){
        yearList<-paste(yearList,",", as.character(y), sep="")
      }
    }
    
    region = geo5.list[1]
    for (region in geo5.list) {
      sql <- paste("SELECT c.`time`, sum(ifnull(c.`ld_treat_dom`,0)) as 'dom', sum(ifnull(c.`ld_treat_man`,0))as 'man', ",
                   "sum(ifnull(c.`ld_treat_sc`,0)) as 'sc', sum(ifnull(c.`ld_open_defec`,0)) as 'open_defec', ",
                   "sum(ifnull(c.`ld_treat_urb`,0)) as 'urb', sum(ifnull(c.`ld_manure`,0)) as 'manure', ",
                   "sum(ifnull(c.`ld_hanging_l`,0)) as 'hanging_l', ",
                   "sum(ifnull(c.`ld_irr`,0)) as 'irrig_return_flow' ",
                   "FROM wwqa_wq_load_",cont,".calc_cell_load c,  watergap_unf.mother_",cont," m, watergap_unf.country c1 ",
                   "WHERE c.`IDScen`=9 AND c.`parameter_id`=",parameter," AND c.`time` in (", yearList, ") ",
                   "AND c.`cell`=m.`arcid` AND m.`ISONUM`=c1.`ISONUM` AND c1.`RegionGEO`=",region," GROUP BY c.`time`;", sep="")
      data <- sqlQuery(wq,sql)
      
      #load as matrix speichern
      mat<-cbind(region, data)
      # Mittelwert über die jahre berechnen, ohne erste zwei Spalten mit region, Jahr
      mat <- aggregate(x = mat[,-(1:2)], by = list(region=mat$region),FUN = mean)
      if (parameter!=260) {
        mat <- mat[,1:8]
      }
      ld<-rbind(ld,mat)
      
    } # for(region)
    
  } #for(cont)
  
  
  #close(wq)  #Datenbankverbindung wieder schlie?en
  
  # erste zeil min NA's löschen
  ld<-ld[-1,]
  
  #Vereinheitlichte Farben
  color<-c("#FFFFB3", "#BEBADA", "#FDB462", "#80B1D3","#FB8072", "#AAFF00")
  color<-c("#FFFFB3", "#BEBADA", "#FDB462", "#80B1D3","#FB8072", "#74C476")
  
  ld$diagContinent <- NA
  ld$diagContinent[which(ld$region==1)]<-"Africa"
  ld$diagContinent[which((ld$region==2|ld$region==6))] <-"Asia"
  ld$diagContinent[which(ld$region==4)] <- "Latin America"
  ###### hier ist ld für Parameter fertig 
  if (parameter==310) {
    ld_fc <- ld
  }  else if (parameter==260) {
    ld_tds <- ld
  } else {
    ld_bod<- ld
  }
  
  
} # for(parameter.number)
