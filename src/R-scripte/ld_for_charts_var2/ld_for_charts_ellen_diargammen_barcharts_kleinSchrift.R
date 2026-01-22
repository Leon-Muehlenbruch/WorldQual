png(file=filename, width=700, height=700)

layout(matrix( c(1,2,3,4,4,4),nrow=2, ncol=3, byrow=TRUE), heights=c(5,1), widths=c(1,1,1))
par(mar = c(4,6,3,0),oma = c(1,0,2,0),xpd = F)

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

