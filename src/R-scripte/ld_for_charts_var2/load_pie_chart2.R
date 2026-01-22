#library(ggplot2) # Lade die ggplot2-Bibliothek
#require(ggplot2)
#library(gridExtra)

# data.frame x erstellen ####
ld_reg$sum<- ld_reg$`Domestic sewered` + ld_reg$`Domestic non-sewered` + 
  ld_reg$Manufacturing+ ld_reg$`Urban surface runoff` + ld_reg$`Agriculture animal wastes`
if (parameter==260)
  ld_reg$sum<- ld_reg$sum +ld_reg$`Agriculture irrigation return flows`

ld_reg[,2:(ncol(ld_reg)-1)]<-ld_reg[,2:(ncol(ld_reg)-1)]/ld_reg$sum *100

x<-data.frame(geo5=character(), load=numeric(), group=character())
for (col.num in 2:(ncol(ld_reg)-1)) {
  #x<-rbind(x,cbind(geo5=ld_reg[,1], load=ld_reg[,col.num],group=names(ld_reg)[col.num]))
  x<-rbind(x,cbind(geo5=ld_reg[,1], load=ld_reg[,col.num],group=paste(names(ld_reg)[col.num]," ", sep="")))
  
}
x$load<-as.numeric(as.character(x$load))


# plots erstellen ####
plot1 <- getPlot2(unique(x$geo5)[1])
plot2 <- getPlot2(unique(x$geo5)[2])
plot3 <- getPlot2(unique(x$geo5)[3])

# in einem plot legende formatieren
plot1 <- 
  plot1 + theme(legend.position="bottom"
     #, legend.key.size = unit(5, "mm")
     # für TDS 1.25, weil mit 1.35 passt Legende nicht in png Datei (wegen irrigation)
     #, legend.text = element_text(size = rel(1.35))   # legende Text größer, damit auf der Karte Diagramm sichtbar wird
     , legend.text = element_text(size = rel(1.25))   # legende Text größer, damit auf der Karte Diagramm sichtbar wird
     
)
# vergrößerte Legende passt nicht mehr in eine Zeile
plot1 <- plot1 + guides(fill=guide_legend(ncol=3,nrow=2,byrow=FALSE
    #, keywidth = 1, keyheight = 1
    #, label.hjust = rep(1,5), label.vjust = rep(1,5)
    ) )

# legende aus plot kopieren
legend <- get_legend(plot1)

# in den plots legenden löschen
#plot1 <- plot1 + theme(legend.position="none", plot.title = element_text(size = rel(1.5)))+ theme(plot.margin = unit(c(0,0,0,0), "pt"))
#plot2 <- plot2 + theme(legend.position="none", plot.title = element_text(size = rel(1.5)))+ theme(plot.margin = unit(c(0,0,0,0), "pt"))
#plot3 <- plot3 + theme(legend.position="none", plot.title = element_text(size = rel(1.5)))+ theme(plot.margin = unit(c(0,0,0,0), "pt"))
plot1 <- plot1 + theme(legend.position="none")+ theme(plot.margin = unit(c(0,0,0,0), "pt"), title =element_text(size = rel(1.5)))
plot2 <- plot2 + theme(legend.position="none")+ theme(plot.margin = unit(c(0,0,0,0), "pt"), title =element_text(size = rel(1.5)))
plot3 <- plot3 + theme(legend.position="none")+ theme(plot.margin = unit(c(0,0,0,0), "pt"), title =element_text(size = rel(1.5)))

#png(file=filename, width=2972, height =1190, res = 400)
#png(file=filename, width=2972, height =1190, res = 200)
##png(file=filename, width=2972, height =1190)
#grid.arrange(plot3, plot1, plot2, legend ,left="",top="",bottom="",right="", ncol=3
#             , layout_matrix = rbind(c(1,2,3), c(4,4,4)) #, widths = c(5,5,5), 
#             #, padding = unit(0, "line")
#             #, heights = unit(c(3,8), c("in", "mm"))
#             )
##grid.arrange(plot3, plot1, plot2, legend, ncol=3)
#
#dev.off()

png(file=filename, width=2972, height =1190, res = 400)
#png(file=filename, width=2972, height =1190)
tmp.grob <- arrangeGrob(plot3, plot1, plot2, ncol=3)
grid.arrange(tmp.grob, legend ,left="",top="",bottom="",right="" #, ncol=3
             #, layout_matrix = rbind(c(1,2,3), c(4,4,4)) #, widths = c(5,5,5), 
             #, padding = unit(0, "line")
             , heights = unit(c(0.85, 0.15), c("npc", "npc"))
)
#grid.arrange(plot3, plot1, plot2, legend, ncol=3)

dev.off()
