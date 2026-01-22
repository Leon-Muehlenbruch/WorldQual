
library(grid)
require(grid)
grid_arrange_shared_legend <- function(...,
           ncol = length(list(...)),
           nrow = 1,
           position = c("bottom", "right")) {
    
    plots <- list(...)
    position <- match.arg(position)
    g <- ggplotGrob(plots[[1]] + theme(legend.position = position))$grobs
    legend <- g[[which(sapply(g, function(x) x$name) == "guide-box")]]
    lheight <- sum(legend$height)
    lwidth <- sum(legend$width)
    gl <- lapply(plots, function(x) x + theme(legend.position = "none"))
    gl <- c(gl, ncol = ncol, nrow = nrow)
    
    combined <- switch(position,
      "bottom" = arrangeGrob(
        do.call(arrangeGrob, gl),
        legend,
        ncol = 1,
        heights = unit.c(unit(1, "npc") - lheight, lheight)
      ),
      "right" = arrangeGrob(
        do.call(arrangeGrob, gl),
        legend,
        ncol = 2,
        widths = unit.c(unit(1, "npc") - lwidth, lwidth)
      )
    )
    
    grid.newpage()
    grid.draw(combined)
    
    # return gtable invisibly
    invisible(combined)
    
}



#library(ggrepel)
getPlot2 <- function(continent) {
  my.dat <- x[which(x$geo5==continent),]
  my.dat$pos<- (cumsum(c(0, my.dat$load)) + c(my.dat$load / 2, .01))[1:nrow(my.dat)]
  my.dat$pos.x <- rep(1,nrow(my.dat)) #c(1,1,1.6,1.6,1.6)
  my.dat$gap <- 0
  my.dat$gap[1:nrow(my.dat)-1] <- my.dat$pos[2:nrow(my.dat)] - my.dat$pos[1:nrow(my.dat)-1] 
  
  
  
  for (num in 1:(nrow(my.dat)-1)) {
    if(my.dat[num,]$gap<5) {
      if(my.dat[num,]$pos.x==1)  {
        my.dat[num,]$pos <- my.dat[num,]$pos*0.97
        my.dat[num,]$pos.x <- 1.6
        my.dat[num+1,]$pos.x <- 1.6
        my.dat[num+1,]$pos <- my.dat[num+1,]$pos *1.03
      } else {
        my.dat[num,]$pos <- my.dat[num,]$pos/1.03
        my.dat[num,]$pos.x <- 1.65  #1.7
        my.dat[num+1,]$pos.x <- 1.6
        my.dat[num+1,]$pos <- my.dat[num+1,]$pos*1.03
      }
    } else if(my.dat[num,]$gap<5) {
      my.dat[num,]$pos.x <- 1.6
      my.dat[num,]$pos <- my.dat[num,]$pos*0.98
      my.dat[num+1,]$pos.x <- 1.6
      my.dat[num+1,]$pos <- my.dat[num+1,]$pos*1.02
    }
  }

  my.dat[which(my.dat$pos>100),]$pos <- my.dat[which(my.dat$pos>100),]$pos - 100
  
  plot <-  ggplot(my.dat, aes(x="", y=load, fill=group, label = round(load,2))) +
    geom_bar(width = 1, stat = "identity", color = "grey" )
  pie <- plot + coord_polar("y", start=0) +
    theme(panel.background = element_rect(fill = 'white', colour = 'white'),
          axis.text=element_blank(), axis.ticks = element_blank(), 
          axis.title=element_blank(), legend.title=element_blank())
  
  color <- c("#FFFFB3", "#BEBADA", "#FDB462", "#80B1D3","#FB8072", "#74C476")
  
  my.pie <- pie + scale_fill_manual(values = color[1:nrow(my.dat)] ) +
    geom_text(aes(x=pos.x, y = pos, label = round(load,2)), size=3) +
    labs(title=continent)
  #my.pie
  
  return(my.pie)
}
