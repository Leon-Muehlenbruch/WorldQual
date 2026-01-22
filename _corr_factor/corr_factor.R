setwd("/home/USF/eisner/WWQA/_corr_factor")

cont <- c("eu","af","as","au","na","sa")
ng <- c(180721, 371410, 841703, 109084, 461694, 226852)

readunf <- function(file, ncells, nval, nbyte, type){
	tmp <- matrix(data = readBin(file, type, n=ncells*nval, size=nbyte, endian = "big"),
		nrow = ncells,
		ncol = nval, 
		byrow = T)
	
	return(tmp)
}

for(i in 1:6){
#for (i in 2:2){

	p_irr <- paste("/home/USF/eisner/WWQA/gwswuse/",cont[i],"/INPUT_WU/",sep="")
	p_dom <- paste("/home/USF/eisner/WWQA/rtf_worldqual/OUTPUT_",cont[i],"/",sep="")
	p_man <- ifelse(cont[i]=="eu",paste("/home/USF/kynast/waterGAP3/waterUse/projects/base/griddi_varDens/OUTPUT_2005_hyde31_092012/",cont[i],"_pop_dens/",sep=""),
					paste("/home/USF/kynast/waterGAP3/waterUse/projects/base/griddi_varDens/OUTPUT_2005_hyde31_092012/",cont[i],"/",sep=""))
					
	for(yr in 1990:2010){
	#for(yr in 1990:1990){
	
		file <- paste(p_irr,"G_IRRIG_RTF_SW_m3_",yr,".12.UNF0", sep="")
		irr.rtf <- readunf(file,ng[i],12,4,numeric())
		
		file <- paste(p_dom,"G_DOM_RTF_CORR_m3_",yr,".UNF0", sep="")
		dom.rtf <- readunf(file,ng[i],1,4,numeric())
		
		file <- paste(p_man,"G_MAN_WW_m3_",yr,".UNF0",sep="")
		man.ww <- readunf(file,ng[i],1,4,numeric())
		file <- paste(p_man,"G_MAN_WC_m3_",yr,".UNF0",sep="")
		man.wc <- readunf(file,ng[i],1,4,numeric())
		file <- paste(p_man,"G_MAN_WCOOL_m3_",yr,".UNF0",sep="")
		man.wcool <- readunf(file,ng[i],1,4,numeric())
		man.rtf <- man.ww - man.wc - man.wcool
		
		tot.rtf <- apply(irr.rtf,2,"+",dom.rtf/12+man.rtf/12)
		
		file <- paste("/home/USF/eisner/WWQA/availability/",cont[i],"/G_RIVER_AVAIL_",yr,".12.UNF0",sep="")
		avail <- readunf(file,ng[i],12,4,numeric())*10^9
		
		corr <- ifelse((avail>0) & avail<tot.rtf,avail/tot.rtf,1)
		
		file <- paste(cont[i],"/G_CORR_FACT_RTF_",yr,".12.UNF0",sep="")
		writeBin(as.vector(t(corr)), file, size=4,endian = "big")
	}			
}

###

