#rm(list=ls())
library(RODBC)      #f?r die Datenbankverbindung

# umgeschrieben von Ellen Kynast im Juli 2018
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

current_dir<-"C:/Users/kynast/Documents/GridWasser/R_project/wwqa_poster_wien_042016/ld_for_charts"
output_csv<-"C:/Users/kynast/Documents/GridWasser/R_project/wwqa_poster_wien_042016/ld_for_charts/wwqa_ld_diag_pieInR"

current_dir<-"Y:/grid/USF/kynast/WorldQual_workspace/WorldQual/R-scripte/ld_for_charts"
output_csv<-"Y:/grid/USF/kynast/WorldQual_workspace/WorldQual/R-scripte/ld_for_charts/wwqa_ld_diag"

year_start<-2008
year_end<-2010
parameterList = c(310,260,80)
parameter.name=c("FC", "TDS", "BOD")
parameter.number=1
parameter=parameterList[parameter.number]

#Datenbankverbindung wird ge?ffnet
wq <- odbcConnect("grdc", believeNRows=FALSE)

setwd(current_dir)
source("grid_arrange_shared_legend.R")

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


close(wq)  #Datenbankverbindung wieder schlie?en

# erste zeil min NA's löschen
ld<-ld[-1,]

#Vereinheitlichte Farben
color<-c("#FFFFB3", "#BEBADA", "#FDB462", "#80B1D3","#FB8072", "#AAFF00")
color<-c("#FFFFB3", "#BEBADA", "#FDB462", "#80B1D3","#FB8072", "#74C476")

ld$diagContinent <- NA
ld$diagContinent[which(ld$region==1)]<-"Africa"
ld$diagContinent[which((ld$region==2|ld$region==6))] <-"Asia"
ld$diagContinent[which(ld$region==4)] <- "Latin America"
ld_reg <- aggregate(x=ld[,2:(ncol(ld)-1)], by = list(diagContinent = ld$diagContinent), FUN = sum)

ld_reg$donNonSew <- (ld_reg$sc+ld_reg$open_defec+ld_reg$hanging_l)
# diagContinent, dom, donNonSew, man, urb, manure, irrig_return_flow (irrig_return_flow gibt  nur für salz)
if (parameter==260) {
  ld_reg<-ld_reg[,c(1:2,10,3,6,7,9)]  
  names(ld_reg)<-c("geo5", "Domestic sewered", "Domestic non-sewered", "Manufacturing"
                   , "Urban surface runoff", "Agriculture animal wastes", "Agriculture irrigation return flows" )
  name<-c("Domestic sewered",  "Domestic non-sewered", "Manufacturing"
          , "Urban surface runoff", "Agriculture - animal wastes", "Agriculture - irrigation return flows" )
} else {
  ld_reg<-ld_reg[,c(1:2,9,3,6,7)]  
  names(ld_reg)<-c("geo5", "Domestic sewered", "Domestic non-sewered", "Manufacturing"
                   , "Urban surface runoff", "Agriculture animal wastes" )
  name<-c("Domestic sewered",  "Domestic non-sewered", "Manufacturing"
          , "Urban surface runoff", "Agriculture - animal wastes" )
}

filename=paste(output_csv,"/",parameter.name[parameter.number],"_ld_barchart_",year_start,"_",year_end,".pdf", sep="")
midpoints<-barplot(as.matrix(t(ld_reg[1,-1])))

pdf(file=filename, width=10, height =10)
layout(matrix( c(1,2,3,4,4,4),nrow=2, ncol=3, byrow=TRUE), heights=c(5,1), widths=c(1,1,1))
par(mar = c(4,6,3,0),oma = c(1,0,2,1),xpd = F)

column = 3

labs=c("", "", paste(parameter.name[parameter.number]," load", sep=""))
my.legend<-c(FALSE, TRUE, FALSE)

for (column in c(3,1,2)) {
  
  barplot(as.matrix(t(ld_reg[column,-1])), ylab=labs[column], axes=T, cex.lab=2, cex.axis=1.5, cex.names=1.5
          , font.axis = 2 , axisnames = FALSE
          , col=color)
  axis(side = 1, at = midpoints , 
       labels = paste(year_start, year_end, sep = "-"), 
       cex.axis=1.5, las=1)
  title(main =  ld_reg[column,1], cex.main = 2)
  
} # for(column)
plot.new()
legend("left", name, ncol = 3, fill = color, cex = 1.5, bty="n" )

dev.off()

filename=paste(output_csv, "/",parameter.name[parameter.number],"_ld_piechart_",year_start,"_",year_end,".pdf", sep="")
source("load_pie_chart.R")

filename=paste(output_csv, "/",parameter.name[parameter.number],"_ld_piechart_",year_start,"_",year_end,".txt", sep="")
write.table(ld_reg, file=filename, sep = '\t')
