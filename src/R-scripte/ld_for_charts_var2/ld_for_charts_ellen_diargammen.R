# aus ld_for_charts_ellen.R
# um nicht immer wieder Datenbankabfragen zu starten
# ld_fc, ld_tds, ld_bod  - kopien von ld für entsprechenden Parameter 
# in nächster Zeile 
parameterList = c(310,260,80)
parameter.name=c("FC", "TDS", "BOD")
parameter.number=2
for(parameter.number in 1:3){
  parameter=parameterList[parameter.number]
  print(paste(parameter.number, parameter, parameter.name[parameter.number]))

  if (parameter==310) {
    ld <- ld_fc 
  }  else if (parameter==260) {
    ld <- ld_tds 
  } else {
    ld <- ld_bod
  }
  
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
  
  #vereinheitlichte Kategorien
  ld.2<-cbind(ld_reg$diagContinent,ld_reg$dom,(ld_reg$sc+ld_reg$open_defec+ld_reg$hanging_l), ld_reg$man, ld_reg$urb, ld_reg$manure)
  colnames(ld.2)<-c("geo5", "Domestic sewered", "Domestic non-sewered", "Manufacturing", "Urban surface runoff", "Agriculture animal wastes" )
  class(ld.2[,-1])<-"numeric"
  
  
  #filename=paste(output_csv,"/",parameter.name[parameter.number],"_ld_barchart_",year_start,"_",year_end,".pdf", sep="")
  filename=paste(output_csv,"/",parameter.name[parameter.number],"_ld_barchart_",year_start,"_",year_end,".png", sep="")
  #filename="test.pdf"
  #midpoints<-barplot(as.matrix(t(ld.2[1,-1])))
  #midpoints<-barplot(as.matrix((ld.2[1,-1])))
  midpoints<-barplot(as.matrix(t(ld_reg[1,-1])))
  
  #pdf(file=filename, width=10, height =10)
  #png(file=filename, width=10, height =10, units = "mm")
  #png(file=filename, width=504, height=672)
  
  #ld_reg[,-1] <- ld_reg[,-1]/1000
  source("ld_for_charts_ellen_diargammen_barcharts.R")
  #source("ld_for_charts_ellen_diargammen_barcharts_kleinSchrift.R")  
  
  
  
  #filename=paste(output_csv, "/",parameter.name[parameter.number],"_ld_piechart_",year_start,"_",year_end,".pdf", sep="")
  filename=paste(output_csv, "/",parameter.name[parameter.number],"_ld_piechart_",year_start,"_",year_end,".png", sep="")
  #filename=paste(output_csv, "/",parameter.name[parameter.number],"_ld_piechart_",year_start,"_",year_end,"_ohneTitel_Legend.png", sep="")
  ##source("load_pie_chart.R")
  source("load_pie_chart2.R")
  
  filename=paste(output_csv, "/",parameter.name[parameter.number],"_ld_piechart_",year_start,"_",year_end,".txt", sep="")
  write.table(ld_reg, file=filename, sep = '\t')
  
} #for(parameter.number)
 
