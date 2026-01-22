library(grid)
require(grid)
library(ggrepel)

get_legend<-function(myggplot){
  tmp <- ggplot_gtable(ggplot_build(myggplot)) 
  leg <- which(sapply(tmp$grobs, function(x) x$name) == "guide-box")
  legend <- tmp$grobs[[leg]]
  return(legend)
}

grid_arrange_shared_legend <- function(..., ncol = length(list(...)), nrow = 1, position = c("bottom", "right")) {
    
    plots <- list(...)
    position <- match.arg(position)
#    g <- ggplotGrob(plots[[1]] + theme(legend.position = position))$grobs
    tmp.plot<-plots[[1]] + theme(legend.position = position) + theme(legend.text = element_text(size = rel(1.4)))
    tmp.plot <- tmp.plot + guides(fill=guide_legend(ncol=3,nrow=2,byrow=FALSE , keywidth = 1, keyheight = 1))
    tmp.plot <- tmp.plot + theme(plot.margin = unit(c(0,0,0,0), "pt"))
    g <- ggplotGrob(tmp.plot)$grobs

    legend <- g[[which(sapply(g, function(x) x$name) == "guide-box")]]

    lheight <- sum(legend$height)*0.5 # -2*legend$height[1] #*0.6
    lwidth <- sum(legend$width)

    gl <- lapply(plots, function(x) x + theme(legend.position = "none"
                           , plot.title = element_text(size = rel(1.6)
                           #, plot.margin = unit(c(0.1,0.1,0.1,0.1), "mm")
                )))
    
    gl <- c(gl, ncol = ncol, nrow = nrow)
    
    combined <- switch(
      position,
      "bottom" = arrangeGrob(
        do.call(arrangeGrob, gl), legend, ncol = 1, heights = unit.c(unit(1, "npc") - lheight, lheight)
      )
    ,
      
      "right" = arrangeGrob(
        do.call(arrangeGrob, gl), legend, ncol = 2, widths = unit.c(unit(1, "npc") - lwidth, lwidth)
      )
    )
    
    grid.newpage()
    grid.draw(combined)
    
    # return gtable invisibly
    invisible(combined)
    
  }


getPlot <- function(continent) {
  
  plot <-  ggplot(x[which(x$geo5==continent),], aes(x="", y=load, fill=group, label = round(load,2))) +
    geom_bar(width = 1, stat = "identity" )
  pie <- plot + coord_polar("y", start=0) +
    theme(panel.background = element_rect(fill = 'white', colour = 'white'),
          axis.text=element_blank(),
          axis.title=element_blank(), legend.title=element_blank())
  lab.pos<-cumsum(x[which(x$geo5==continent),]$load) - x[which(x$geo5==continent),]$load/2
  
  lab.pos.x <-rep(1,length(lab.pos))
  tmp <- lab.pos[2:length(lab.pos)] - lab.pos[1:(length(lab.pos)-1)]
  for (num in 1:(length(lab.pos)-1)) {
    if(tmp[num]<5) {
      if(lab.pos.x[num]==1)  {
        lab.pos[num] <- lab.pos[num]*0.98
        lab.pos.x[num] <- 1.8  #1.6
        lab.pos.x[num+1] <- 1.8 #1.6
        lab.pos[num+1] <- lab.pos[num+1]*1.01
      } else {
        lab.pos[num] <- lab.pos[num]/1.01
        lab.pos.x[num] <- 1.7
        lab.pos.x[num+1] <- 1.6
        lab.pos[num+1] <- lab.pos[num+1]*1.01
      }
    } else if(tmp[num]<5) {
      lab.pos.x[num] <- 1.6
      lab.pos[num] <- lab.pos[num]*0.98
      lab.pos.x[num+1] <- 1.6
      lab.pos[num+1] <- lab.pos[num+1]*1.03
    }
  }
  
  color <- c("#FFFFB3", "#BEBADA", "#FDB462", "#80B1D3","#FB8072", "#74C476")
  
  my.pie2 <- pie + scale_fill_manual(values = color[1:length(lab.pos)] ) +
    #geom_text(aes(x=lab.pos.x, y = lab.pos, label = round(load,2))) +
    labs(title=continent) 
  my.pie2 +  geom_text(aes(x=lab.pos.x, y = lab.pos))
  pie +  geom_text(aes(x=lab.pos.x, y = lab.pos))
  
  my.pie <- pie + scale_fill_manual(values = color[1:length(lab.pos)] ) +
    geom_text_repel(aes(x=lab.pos.x, y = lab.pos, label = round(load,2))) +
    labs(title=continent) 
  
  return(my.pie)
}

getPlot2 <- function(continent) {
  my.dat <- x[which(x$geo5==continent),]
  my.dat$pos<- (cumsum(c(0, my.dat$load)) + c(my.dat$load / 2, .01))[1:nrow(my.dat)]
  my.dat$pos.x <- rep(1.1,nrow(my.dat)) #c(1,1,1.6,1.6,1.6)
  my.dat$gap <- 0
  my.dat$gap[1:nrow(my.dat)-1] <- my.dat$pos[2:nrow(my.dat)] - my.dat$pos[1:nrow(my.dat)-1] 
  
  
  
  for (num in 1:(nrow(my.dat)-1)) {
    if(my.dat[num,]$gap<5) {
      if(my.dat[num,]$pos.x==1.1)  {
        my.dat[num,]$pos <- my.dat[num,]$pos*0.95  #0.97
        my.dat[num,]$pos.x <- 1.5 #1.3  #1.6
        my.dat[num+1,]$pos.x <- 1.3 #1.3  #1.6
        my.dat[num+1,]$pos <- my.dat[num+1,]$pos *1.04
      } else {
        my.dat[num,]$pos <- my.dat[num,]$pos/1.03
        my.dat[num,]$pos.x <- 1.3 #1.6 #1.7 #1.65  #1.7
        my.dat[num+1,]$pos.x <- 1.5 #1.4 #1.6
        my.dat[num+1,]$pos <- my.dat[num+1,]$pos*1.03
      }
    } 
    #    else {  # if(my.dat[num,]$gap<5)
    #      my.dat[num,]$pos.x <- 1.6
    #      my.dat[num,]$pos <- my.dat[num,]$pos*0.98
    #      my.dat[num+1,]$pos.x <- 1.6
    #      my.dat[num+1,]$pos <- my.dat[num+1,]$pos*1.02
    #    }
  }
  
  my.dat[which(my.dat$pos>100),]$pos <- my.dat[which(my.dat$pos>100),]$pos - 100
  
  plot <-  ggplot(my.dat, aes(x="", y=load, fill=group, label = round(load,2))) +
    geom_bar(width = 1, stat = "identity", color = "grey")
  pie <- plot + coord_polar("y", start=0) +
    theme(panel.background = element_rect(fill = 'white', colour = 'white'),
          axis.text=element_blank(), axis.ticks = element_blank(), 
          axis.title=element_blank(), legend.title=element_blank() #, title =element_text(size = rel(3))
          )
  
  color <- c("#FFFF2D", "#0000FF", "#F39B36", "#99FFFF","#FF0000", "#00CC00")
  color <- c("#FFFFB3", "#BEBADA", "#FDB462", "#80B1D3","#FB8072", "#74C476")
  
  my.pie <- pie + scale_fill_manual(values = color[1:nrow(my.dat)] ) +
    geom_text(aes(x=pos.x, y = pos, label = round(load,2)) , size = rel(4)
              ) + 
    labs(title=continent)   # auskommentiert wenn man keine Kontinentennamen schreiben möchte
  #my.pie
  
  return(my.pie)
}
