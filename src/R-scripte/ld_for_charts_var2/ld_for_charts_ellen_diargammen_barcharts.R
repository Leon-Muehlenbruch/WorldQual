png(file=filename, width=3000, height=1600)

layout(matrix( c(1,2,3,4,4,4),nrow=2, ncol=3, byrow=TRUE), heights=c(3,1), widths=c(1,1,1))
par(mar = c(4,10,12,0),oma = c(1,10,2,0),xpd = F)

column = 3

labs=c("", "", paste(parameter.name[parameter.number]," load", sep=""))
my.legend<-c(FALSE, TRUE, FALSE)

for (column in c(3,1,2)) 
#for (column in c(3)) 
  {
  
  barplot(as.matrix(t(ld_reg[column,-1])), ylab=NULL, axes=T, cex.axis=8 #4  #8
          , font.axis = 2 , font.lab = 2, axisnames = FALSE
          , col=color)
  axis(side = 1, at = midpoints , 
       labels = paste(year_start, year_end, sep = "-"), 
       cex.axis=8
       , font.axis = 2, las=1, line = 5)
  if (column==3) {
    mtext(paste(parameter.name[parameter.number]," load", sep=""), side=2, line=10, cex=7)
  }
  title(main =  ld_reg[column,1], cex.main = 12, line = 4)
  
} # for(column)
plot.new()
par(mar = c(1,0,1,0),oma = c(0,0,0,0))
legend("center", name, ncol = 2, fill = color, cex = 10, bty="n" )

dev.off()

