library(ggplot2) # Lade die ggplot2-Bibliothek
#require(ggplot2)
library(gridExtra)  # für arrangeGrob()



ld_reg$sum<- ld_reg$`Domestic sewered` + ld_reg$`Domestic non-sewered` + 
  ld_reg$Manufacturing+ ld_reg$`Urban surface runoff` + ld_reg$`Agriculture animal wastes`
if (parameter==260)
  ld_reg$sum<- ld_reg$sum +ld_reg$`Agriculture irrigation return flows`
  
ld_reg[,2:(ncol(ld_reg)-1)]<-ld_reg[,2:(ncol(ld_reg)-1)]/ld_reg$sum *100

x<-data.frame(geo5=character(), load=numeric(), group=character())
for (col.num in 2:(ncol(ld_reg)-1)) {
  x<-rbind(x,cbind(geo5=ld_reg[,1], load=ld_reg[,col.num],group=names(ld_reg)[col.num]))
  
}
x$load<-as.numeric(as.character(x$load))


continent=unique(x$geo5)[1]
continent=unique(x$geo5)[2]
continent=unique(x$geo5)[3]

plot1 <- getPlot2(unique(x$geo5)[1])
plot2 <- getPlot2(unique(x$geo5)[2])
plot3 <- getPlot2(unique(x$geo5)[3])

#plot1 <- plot1 + theme(legend.text=element_text(size=rel(0.5)))
#plot2 <- plot2 + theme(legend.text=element_text(size=rel(0.5)))
#plot3 <- plot3 + theme(legend.text=element_text(size=rel(0.5)))

#grid.arrange(plot1, plot2, plot3, nrow = 1)
#plot1 <- plot1 + theme(legend.position = "none")
#plot2 <- plot2 + theme(legend.position = "none")

pdf(file=filename, width=10, height =4)
grid_arrange_shared_legend(plot3, plot1, plot2)
grid_arrange_shared_legend(plot3, plot1, plot2, position = "right")
dev.off()


 
