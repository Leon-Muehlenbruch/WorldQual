/*
 * ClassWQload.cpp
 *
 *  Created on: 30.01.2018
 *      Author: kynast
 */

#include "ClassWQload.h"

extern mysqlpp::Connection con;

using namespace std;
using namespace mysqlpp;

ClassWQ_load::ClassWQ_load(options_wq_loadClass* optionsWQ_load_ptr, int year)
//	: countries(optionsWQ_load_ptr->IDScen, year)
{
	this->year = year;
	this->optionsWQ_load_ptr = optionsWQ_load_ptr;

	// initiiren, werden später für Pestiziden aus parameter_input Tabelle gelesen
	DT50soil = 0.;
	koc = 0.;

	mysqlpp::String ParameterName;
	// checking parameter_id and parameter name
	if (checkParameter(optionsWQ_load_ptr->parameter_id, toCalc, ParameterName)) exit(1);

	// checking project_id and project- and database name, fill MyDatabase
	if (getProject(optionsWQ_load_ptr->project_id, optionsWQ_load_ptr, con, "wq_load")) exit(1);

	// checking IDScen and scenario name
	if(checkIDScen(optionsWQ_load_ptr->project_id, optionsWQ_load_ptr->IDScen)) exit(1);

	cout << "year = " << year<<endl;

	if (is_c_geogen_country) {
		cout << "c_geogen als Länderwerte in der Tabelle wq_load.country_parameter_input\n";
		cout << "c_geogen darf entweder nur in der Tabelle wq_load.country_parameter_input geben (wq_load input)\n";
		cout << "oder in der worldqual.factors.c_geogen (worldqual input). Man darf es nicht doppelt rechnen!!\n\n";
	} else {
		cout << "c_geogen_cell - Zellwerte aus der Tabelle wq_load.`parameter_cell_input (wq_load input)\n";
		cout << "c_geogen aus der Tabelle worldqual.factors wird nicht benutzt.\n\n";
	}
	//******classes connected with wq_load
	countries = new COUNTRY_CLASS(optionsWQ_load_ptr->IDScen, year);
	crops = new CROP_CLASS(optionsWQ_load_ptr->parameter_id, countries, toCalc, year);
	LS = new LS_CLASS(optionsWQ_load_ptr->parameter_id, optionsWQ_load_ptr->IDScen, countries, toCalc);
	GB = new GB_CLASS(optionsWQ_load_ptr->parameter_id, toCalc);

	//******end connected classes

	// ######################### input ###############################
	pop          = new double[countries->number][3];
	sewerConn    = new double[countries->number][2];
	SPO_treated	 = new double[countries->number];
	connectivity = new double[countries->number][5];
	rtf_man      = new double[countries->number];
	rtf_dom      = new double[countries->number];

	rtf_man_fra  = new double[countries->number][7];
	rtf_irr      = new double[countries->number];

	ef           = new double[countries->number];

	conc_man     = new double[countries->number][7];

	conc_tds  = new double[countries->number][4]; // tds concentration in return flow for sepc class 1-4 [mg/l]
	conc_urb       = new double[countries->number];    // concentration of urban runoff [mg/l] FC: cfu/100ml
	c_geogen       = new double[countries->number];    // geogen background concentration, country values [mg/l]

	conc_mining  = new double[countries->number][5];

	///=================================
	// read in parameter landmask
	if(getCELLS(optionsWQ_load_ptr, &continent)) exit(1); cout << "\nParameter Landmaske wurden eingelesen\n"<<endl;

	// ######################### cell input ###############################
	c_geogen_cell = NULL;
	if (!is_c_geogen_country) {
		c_geogen_cell  = new double[continent.ng];
	}
	pop_cell  = new double[continent.ng][3];
	rtf_man_cell  = new double[continent.ng];

	rtf_mining_cell = NULL;
	if (toCalc==1) {
		rtf_mining_cell = new double[continent.ng][5];
	}

	rtf_irr_cell   = new double[continent.ng];
	gdp            = new double[continent.ng];
	salinity       = new int[continent.ng];
	humidity       = new int[continent.ng];
	cell_area      = new double[continent.ng];
	built_up_frac  = new double[continent.ng];
	LU_cell        = new short[continent.ng];
	soil_erosion   = new double[continent.ng];
	ld_Patmdepos_rate = new double[continent.ng];
	ld_Pcweathering_rate = new double[continent.ng];
	country_cell      = new int[continent.ng];

	gcrc              = new int[continent.ng];
	grow   		   = new int[continent.ng];

	G_area    = new float[continent.nrows];
	G_land_area 	   = new char[continent.ng];

	urban_runoff_cell = new double[continent.ng][12];
	//cell_runoff_total = new double[continent.ng][12];
	//cell_runoff_total_year= new double[continent.ng];
	temperature_cell  = new short[continent.ng][12];
	temperature_cell_year = new double[continent.ng];
	//actSurRunoff 			= new double[continent.ng];
	perc_days = new char[continent.ng][12];
	precipitation_cell = NULL;
	if (toCalc==5) {
		precipitation_cell  = new short[continent.ng][12];
	}

	sur_runoff_cell = new double[continent.ng][12];
	sur_runoff_cell_mean = new double[continent.ng];
	gw_runoff_cell = new double[continent.ng][12];
	sur_runoff_cell_year = new double[continent.ng];
	ls_cell = new double[continent.ng][12];
	if (toCalc==3) {
		crop_type = new short[continent.ng];
	}
	cropland_area = new float[continent.ng];
	rate_p_cell = new float[continent.ng];

	organic_carbon = NULL;
	ld_input_pest = NULL;
	WBZ_cell = NULL;
//	g_slope = NULL;
	g_land_slope = NULL;
	// pesticide
	if (toCalc==5) {
		organic_carbon = new double[continent.ng];
		ld_input_pest = new double[continent.ng];
//		g_slope = new double[continent.ng];
		g_land_slope = new double[continent.ng];
		WBZ_cell = new double[continent.ng];
	}

	stp_failure   = new double[countries->number];
	connec_treat_unknown    = new double[countries->number];
	hanging_lat   = new double[countries->number];
	open_defec    = new double[countries->number];

	// ######################### output ###############################
	calc_con  = new double[countries->number][3];
	frac_dom  = new double[countries->number][5];
	frac_man  = new double[countries->number][5];
	frac_sc   = new double[countries->number][5];

	ld_untr_dom      = new double[countries->number];
	ld_treat_dom     = new double[countries->number];

	ld_untr_urb      = new double[countries->number];
	ld_treat_urb     = new double[countries->number];

	ldd_treat_man    = new double[countries->number];
	ldi_untr_man     = new double[countries->number];

	ldi_untr_man_fra = new double[countries->number][7];

	ldi_treat_man    = new double[countries->number];
	ld_treat_man     = new double[countries->number];

	ld_untr_sc       = new double[countries->number];
	ld_treat_sc      = new double[countries->number];
	ld_diff_untr_sc  = new double[countries->number];
	ld_hanging_l = new double[countries->number];

	ld_treat_mining  = new double[countries->number];
	ld_untr_mining   = new double[countries->number];
//		ld_untr_mining_res = new double[countries->number];

	ld_irr           = new double[countries->number];
	ld_agr           = new double[countries->number];

	ld_manure     = new double[countries->number];
	ld_geog_back  = new double[countries->number];
	ld_ind_fert   = new double[countries->number];


	// ######################## cell output ###########################

	ld_untr_dom_cell    = new double[continent.ng];
	ld_treat_dom_cell   = new double[continent.ng];

	ld_untr_urb_cell   = new double[continent.ng][12];
	ld_treat_urb_cell  = new double[continent.ng][12];
	ld_untr_urb_cell_year  = new double[continent.ng];
	ld_treat_urb_cell_year = new double[continent.ng];

	ldd_treat_man_cell  = new double[continent.ng];
	ldi_untr_man_cell   = new double[continent.ng];
	ldi_treat_man_cell  = new double[continent.ng];
	ld_treat_man_cell   = new double[continent.ng];

	ld_untr_mining_cell  = new double[continent.ng];
	ld_treat_mining_cell = new double[continent.ng];

	ld_untr_sc_cell      = new double[continent.ng];
	ld_treat_sc_cell     = new double[continent.ng];
	ld_diff_untr_sc_cell = new double[continent.ng];
	ld_hanging_l_cell    = new double[continent.ng];

	Salt_class          = new int[continent.ng];
	GDPC                = new double[continent.ng];
	SEPC                = new int[continent.ng];

	ld_irr_cell         = new double[continent.ng];
	ld_agr_cell     = new double[continent.ng][12];
	ld_agr_cell_year    = new double[continent.ng];

	ld_agr_soil_cell = NULL;
	if (toCalc == 5) {
		ld_agr_soil_cell = new double[continent.ng][12];
	}

	ld_manure_cell     = new double[continent.ng];
	ld_inorg_fert_cell = new double[continent.ng];

	ld_background_cell_year = new double[continent.ng];
	ld_Patmdepos_cell_year  = new double[continent.ng];
	ld_Pcweathering_cell_year = new double[continent.ng];

	ld_background_cell = NULL;
	if (toCalc==1) {  // for salt
		ld_background_cell    = new double[continent.ng][12];
	}


	// ################################################################

	//methods retun 0, which equals in c++ false; only if there is an error than 1 is returned and program exits
	if (get_country_input()) exit(1);
	if (get_country_parameter_input()) exit(1);

	if (get_parameter_input()) exit(1);  // wird country_cell[] gebraucht, nach dem get_cell_input() erst starten!

	if (get_cell_parameter_input()) exit(1);
	cout<<"get_cell_parameter_input done"<<endl;

	//cout<<"treat_failure="<<treat_failure<<endl;

	cout<<"\n##########all parameters read -> start of calculation##########\n"<<endl;

	if (toCalc==5) {
		for (int country=0; country<countries->number; country++) {
			ld_agr[country] = 0.0;
		}
	} else { // 0 - BOD, 1 - salt, 2 - FC (Fecal coliforms), 3 - TN, 4 - TP

		CountryLoad();
	}

	// ######################### GRID cell values #########################
	//Read in from UNF-files. A connection between arcIDs and gcrcIDs is made
	if(getGCRC(gcrc, grow, optionsWQ_load_ptr, &continent)) exit(1);   // read gcrc

	cout << "\nGRID cell values.\n";
	if (get_cell_input()) exit(1);
	cout << "\nGRID cell parameter values.\n";

	char filename[maxcharlength];

	if (toCalc==0||toCalc==1||toCalc==2||toCalc==3||toCalc==4) { // for BOD, TDS, FC, TN and TP
		sprintf(filename,"%s/G_URBAN_RUNOFF_%d.12.UNF0", optionsWQ_load_ptr->path_watergap_output, year);
		if (readInputFile(filename, 4, 12, continent.ng, &urban_runoff_cell[0][0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
	}

//	if(toCalc==1) //only for 1 = TDS used
//		if (get_cell_runoff(year, cell_runoff_total, 1, input_dir, gcrc)) exit(1);

	if (optionsWQ_load_ptr->climate==0) {

		int *G_wg3_to_wg2 = new int[continent.ng];
		sprintf(filename,"%s/G_WG3_WG2WITH5MIN.UNF4",continent.hydro_input);
		if (readInputFile(filename, 4, 1, continent.ng, &G_wg3_to_wg2[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);

		sprintf(filename, "%s/GTEMP_%d.12.UNF2", optionsWQ_load_ptr->path_climate, year);
		if (readInputFileWG2toWG3( filename, 2, 12, continent.ng, temperature_cell, gcrc, G_wg3_to_wg2)) exit(1);

		sprintf(filename, "%s/GNRD_%d.12.UNF1", optionsWQ_load_ptr->path_gnrd, year);
		if (readInputFileWG2toWG3( filename, 1, 12, continent.ng, perc_days, gcrc, G_wg3_to_wg2)) exit(1);

		if (toCalc==5) {
			sprintf(filename, "%s/GPREC_%d.12.UNF2", optionsWQ_load_ptr->path_climate, year);
			if (readInputFileWG2toWG3( filename, 2, 12, continent.ng, precipitation_cell, gcrc, G_wg3_to_wg2)) exit(1);
		}

		//cout<<"temperature_cell[144598][0]"<<temperature_cell[144598][0]<<endl;
		//cout<<"perc_days[144598][0]"<<(int)(perc_days[144598][0]+0)<<endl;
		//cout<<"perc_days[81515][0]"<<(int)perc_days[81515][0]<<endl;

		//for(int test=0; test<continent.ng; test++){
		//	cout<<"perc_days["<<test<<"].ng="<<(int)perc_days[test][0]<<endl;
		//}


		cout << "get 0.5° climate input done\n";

		delete[] G_wg3_to_wg2; G_wg3_to_wg2 = NULL;

	} else if (optionsWQ_load_ptr->climate==2) {
		int ng_wg22= 67420;
		int *G_wg3_to_wg2 = new int[continent.ng];
		sprintf(filename,"%s/G_WG3_WATCH.UNF4",continent.hydro_input);
		if (readInputFile(filename, 4, 1, continent.ng, &G_wg3_to_wg2[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);

		sprintf(filename, "%s/GTEMP_%d.12.UNF2", optionsWQ_load_ptr->path_climate, year);
		if (readInputFileWG2toWG3( filename, 2, 12, continent.ng, temperature_cell, gcrc, G_wg3_to_wg2, ng_wg22)) exit(1);

		sprintf(filename, "%s/GNRD_%d.12.UNF1", optionsWQ_load_ptr->path_gnrd, year);
		if (readInputFileWG2toWG3( filename, 1, 12, continent.ng, perc_days, gcrc, G_wg3_to_wg2, ng_wg22)) exit(1);

		if (toCalc==5) {
			sprintf(filename, "%s/GPREC_%d.12.UNF2", optionsWQ_load_ptr->path_climate, year);
			if (readInputFileWG2toWG3( filename, 2, 12, continent.ng, precipitation_cell, gcrc, G_wg3_to_wg2, ng_wg22)) exit(1);
		}

		cout << "get 0.5° CRU climate input done\n";

		delete[] G_wg3_to_wg2; G_wg3_to_wg2 = NULL;

	} else {
		sprintf(filename, "%s/GTEMP_%d.12.UNF2", optionsWQ_load_ptr->path_climate, year);
		if (readInputFile(filename, 2, 12, continent.ng, &temperature_cell[0][0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);

		sprintf(filename, "%s/GNRD_%d.12.UNF1", optionsWQ_load_ptr->path_gnrd, year);
		if (readInputFile(filename, 1, 12, continent.ng, &perc_days[0][0], gcrc, optionsWQ_load_ptr, 0)) exit(1);

		if (toCalc==5) {
			sprintf(filename, "%s/GPREC_%d.12.UNF2", optionsWQ_load_ptr->path_climate, year);
			if (readInputFile(filename, 2, 12, continent.ng, &precipitation_cell[0][0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		}
	}

	if (toCalc==5) {
		cout << "options.path_hydro_input: "<< continent.hydro_input<<endl;
//		sprintf(filename, "%s/G_RIVERSLOPE.UNF0", optionsWQ_load_ptr->path_hydro_input);
//		if (readInputFile(filename, 4, 1, continent.ng, &g_slope[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		sprintf(filename, "%s/G_LANDSLOPE.UNF0", continent.hydro_input);
		if (readInputFile(filename, 4, 1, continent.ng, &g_land_slope[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
	}

	sprintf(filename,"%s/G_SURFACE_RUNOFF_%d.12.UNF0", optionsWQ_load_ptr->path_watergap_output, year);
	if (readInputFile(filename, 4, 12, continent.ng, &sur_runoff_cell[0][0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);

	if (toCalc==1) { // if(salt)
		sprintf(filename,"%s/G_GW_RUNOFF_%d.12.UNF0", optionsWQ_load_ptr->path_watergap_output, year);
		if (readInputFile(filename, 4, 12, continent.ng, &gw_runoff_cell[0][0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
	}

	if (toCalc==4) { // if(TP)
		sprintf(filename,"%s/G_SURFACE_RUNOFF_MEAN.UNF0", optionsWQ_load_ptr->path_watergap_output);
		if (readInputFile(filename, 4, 1, continent.ng, &sur_runoff_cell_mean[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		sprintf(filename,"%s/G_SOILEROS.UNF0", optionsWQ_load_ptr->path_tp_input);
		if (readInputFile(filename, 4, 1, continent.ng, &soil_erosion[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		sprintf(filename,"%s/G_PWEATHERING.UNF0", optionsWQ_load_ptr->path_tp_input);
		if (readInputFile(filename, 4, 1, continent.ng, &ld_Pcweathering_rate[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		sprintf(filename,"%s/G_PATMDEPOS_%d.UNF0", optionsWQ_load_ptr->path_tp_input3, year);
		if (readInputFile(filename, 4, 1, continent.ng, &ld_Patmdepos_rate[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
	}

	if (toCalc==0||toCalc==1||toCalc==2 || toCalc==3 || toCalc==4) {
		sprintf(filename,"%s/G_LIVESTOCK_NR_%d.12.UNF0", optionsWQ_load_ptr->path_livestock_output, year);
		if (readInputFile(filename, 4, 12, continent.ng, &ls_cell[0][0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);

		sprintf(filename, "%s/GBUILTUP.UNF0", continent.hydro_input);
		if (readInputFile(filename, 4 , 1, continent.ng, &built_up_frac[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
	}

	//cout<<"urban_runoff_cell[144598][0]"<<urban_runoff_cell[144598][0]<<endl;
	//cout<<"sur_runoff_cell[144598][0]"<<sur_runoff_cell[144598][0]<<endl;
	//cout<<"ls_cell[144598][0]"<<ls_cell[144598][0]<<endl;

	if(toCalc==3){ //only for TN
		sprintf(filename,"%s/GLCC%d.UNF2", optionsWQ_load_ptr->path_tp_input, year);
		if (readInputFile(filename, 2, 1, continent.ng, &crop_type[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
	} else if(toCalc==4){ //only for TP
		sprintf(filename,"%s/CROPLAND_CORR_KM2_%d.UNF0", optionsWQ_load_ptr->path_tp_input2, year);
		if (readInputFile(filename, 4, 1, continent.ng, &cropland_area[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		sprintf(filename,"%s/P_RATE_TON_KM2_%d.UNF0", optionsWQ_load_ptr->path_tp_input2, year);
		if (readInputFile(filename, 4, 1, continent.ng, &rate_p_cell[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
	}

	//sprintf(filename, "%s/G_LAND_AREA.UNF1", input_dir);
	//if (readInputFile(filename, 1 , 1, continent.ng, &G_landfreq[0], gcrc)) exit(1);
	readArea(G_area, G_land_area, gcrc, optionsWQ_load_ptr, &continent);


	for  (int cell=0; cell<continent.ng; cell++) {
		//cell_runoff_total_year[cell]=0.0;
		temperature_cell_year[cell]=0;
		//actSurRunoff[cell]=0.0;
		for  (int month=0; month<12; month++) {
			//cell_runoff_total_year[cell] += cell_runoff_total[cell][month];
			temperature_cell_year[cell]  += temperature_cell[cell][month];
			sur_runoff_cell_year[cell]   += sur_runoff_cell[cell][month];
			//actSurRunoff[cell]+=sur_runoff_cell[cell][month]-urban_runoff_cell[cell][month];
			//if(cell==25451)
			//	cout<<"actSurRunoff[cell]: "<<actSurRunoff[cell]<<" sur_runoff_cell[cell][month]: "<<sur_runoff_cell[cell][month]
			//	    <<" urban_runoff_cell[cell][month]: "<<urban_runoff_cell[cell][month]<<" cell: "<<cell<<" month: "<<month<<endl;
		}
		// WaterGAP gives temperature in a unit of 100°C , therefore the division by 100 for the 12 month
		temperature_cell_year[cell]/=1200.;

	}

	CellLoad();
	LoadAgr(); // calculate ld_agr_cell, ld_agr_cell_year, ld_agr
	CountryValuesOutput();

	cout << "##################### GRID cell values OUTPUT ##############################\n";

	cout << "##################### monthly cell values OUTPUT ##############################\n";
	monthValues();
	// insert into calc_cell_load und calc_cell_info
	cout << "##################### yearly cell values OUTPUT ##############################\n";
	yearValues();
	cout << "##################### yearly cell values done ##############################\n";


} // ClassWQ_load()

ClassWQ_load::~ClassWQ_load() {
	cout << "destructor ClassWQ_load\n";
	// ######################### delete #########################
	delete[] pop;          pop          = NULL;
	delete[] connectivity; connectivity = NULL;
	delete[] sewerConn; sewerConn = NULL;
	delete[] SPO_treated; SPO_treated = NULL;
	delete[] rtf_man; rtf_man = NULL;
	delete[] rtf_irr; rtf_irr = NULL;

	delete[] ef;       ef        = NULL;
	delete[] conc_man; conc_man  = NULL;
	delete[] c_geogen; c_geogen  = NULL;
	delete[] conc_mining; conc_mining = NULL;

	delete[] conc_tds; conc_tds  = NULL;
	delete[] gcrc;     gcrc      = NULL;
	delete[] urban_runoff_cell; urban_runoff_cell = NULL;
//		delete[] cell_runoff_total; cell_runoff_total=NULL;

	delete[] G_area;         G_area      = NULL;
	delete[] G_land_area;    G_land_area = NULL;

	// ######################### output ###############################
	delete[] calc_con;       calc_con       = NULL;
	delete[] frac_dom;       frac_dom       = NULL;
	delete[] frac_man;       frac_man       = NULL;
	delete[] frac_sc;        frac_sc        = NULL;

	delete[] ld_untr_dom;    ld_untr_dom    = NULL;
	delete[] ld_treat_dom;   ld_treat_dom   = NULL;

	delete[] ld_untr_urb;    ld_untr_urb    = NULL;
	delete[] ld_treat_urb;   ld_treat_urb   = NULL;

	delete[] ldd_treat_man;  ldd_treat_man  = NULL;
	delete[] ldi_untr_man;   ldi_untr_man   = NULL;
	delete[] ldi_treat_man;  ldi_treat_man  = NULL;
	delete[] ld_treat_man;   ld_treat_man   = NULL;

	delete[] ld_untr_mining;  ld_untr_mining = NULL;
	delete[] ld_treat_mining; ld_treat_mining = NULL;

	delete[] ld_untr_sc;     ld_untr_sc     = NULL;
	delete[] ld_treat_sc;    ld_treat_sc    = NULL;
	delete[] ld_diff_untr_sc; ld_diff_untr_sc = NULL;
	delete[] ld_hanging_l; 	 ld_hanging_l = NULL;

	delete[] ld_irr;         ld_irr   = NULL;
	delete[] ld_agr;         ld_agr   = NULL;

	// ######################### cell input ###############################
	delete[] c_geogen_cell;  c_geogen_cell = NULL;
	delete[] pop_cell;       pop_cell     = NULL;
	delete[] rtf_man_cell;   rtf_man_cell = NULL;
	delete[] rtf_mining_cell; rtf_mining_cell = NULL;

	delete[] rtf_irr_cell;   rtf_irr_cell = NULL;
	delete[] gdp;            gdp          = NULL;
	delete[] salinity;       salinity     = NULL;
	delete[] humidity;       humidity     = NULL;

	delete[] country_cell;   country_cell = NULL;

	delete[] sur_runoff_cell;		sur_runoff_cell = NULL;
	delete[] gw_runoff_cell;		gw_runoff_cell = NULL;
	//delete[] sur_runoff_cell_year; 	sur_runoff_cell_year = NULL;
	delete[] ls_cell; 				ls_cell = NULL;
	delete[] crop_type;				crop_type = NULL;

	delete[] cropland_area;			cropland_area= NULL;
	delete[] rate_p_cell;			rate_p_cell= NULL;	

	if (toCalc==5) {
		delete[] organic_carbon; organic_carbon = NULL;
		delete[] ld_input_pest; ld_input_pest = NULL;
//		delete[] g_slope; g_slope = NULL;
		delete[] g_land_slope; g_land_slope = NULL;
		delete[] WBZ_cell; WBZ_cell = NULL;
	}

	// ######################## cell output ###########################

	delete[] ld_untr_dom_cell;     ld_untr_dom_cell    = NULL;
	delete[] ld_treat_dom_cell;    ld_treat_dom_cell   = NULL;

	delete[] ldd_treat_man_cell;   ldd_treat_man_cell  = NULL;
	delete[] ldi_untr_man_cell;    ldi_untr_man_cell   = NULL;
	delete[] ldi_treat_man_cell;   ldi_treat_man_cell  = NULL;
	delete[] ld_treat_man_cell;    ld_treat_man_cell   = NULL;

	delete[] ld_untr_mining_cell;  ld_untr_mining_cell  = NULL;
	delete[] ld_treat_mining_cell; ld_treat_mining_cell = NULL;

	delete[] ld_untr_sc_cell;      ld_untr_sc_cell     = NULL;
	delete[] ld_treat_sc_cell;     ld_treat_sc_cell    = NULL;
	delete[] ld_diff_untr_sc_cell; ld_diff_untr_sc_cell = NULL;
	delete[] ld_hanging_l_cell;    ld_hanging_l_cell 	= NULL;

	delete[] Salt_class;           Salt_class          = NULL;
	delete[] GDPC;                 GDPC                = NULL;
	delete[] SEPC;                 SEPC                = NULL;
	delete[] ld_irr_cell;          ld_irr_cell         = NULL;
	delete[] ld_agr_cell;          ld_agr_cell         = NULL;
	delete[] ld_agr_cell_year;     ld_agr_cell_year    = NULL;
	delete[] ld_agr_soil_cell;     ld_agr_soil_cell    = NULL;

	delete[] ld_manure_cell; 	   ld_manure_cell      = NULL;
	delete[] ld_inorg_fert_cell;   ld_inorg_fert_cell  = NULL;
	delete[] ld_background_cell_year;   ld_background_cell_year  = NULL;
	delete[] ld_background_cell;   ld_background_cell  = NULL;

	// ################################################################
	cout << "destructor ClassWQ_load done\n";
}

int ClassWQ_load::get_country_input()
{
	cout << "get_country_input():\n";
	int j;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
	//mysqlpp::Row    rowQuery;

	int country;

	//Initalisierung der Variablen
	for (country=0; country<countries->number; country++) {
		stp_failure[country]          = NODATA;
		connec_treat_unknown[country] = NODATA;
		hanging_lat[country]          = NODATA;
		open_defec[country]           = NODATA;

		rtf_man[country] = NODATA;
		rtf_dom[country] = NODATA;
		rtf_irr[country] = NODATA;
		for (int man_fra=0; man_fra<7; man_fra++)
			rtf_man_fra[country][man_fra] = NODATA;

		for (int i=0; i<3; i++) pop[country][i] = NODATA;
		for (int i=0; i<5; i++) connectivity[country][i] = NODATA;
	}
	if(toCalc==5) return 0;   // for pesticide not needed

	query.reset();
	//Scheibt den Text in sql und z�hlt die anzahl der character in j
	j=sprintf(sql,    "SELECT c.`country_id`, ");
	j+=sprintf(sql+j, "IFNULL(c.`pop_tot`,%d) as `pop_tot`, IFNULL(c.`pop_urb`,%d) as `pop_urb`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`pop_rur`,%d) as `pop_rur`, ", NODATA);

	j+=sprintf(sql+j, "IFNULL(c.`con_prim`,%d) as `con_prim`, IFNULL(c.`con_sec`,%d) as `con_sec`, ", 0, 0);
	j+=sprintf(sql+j, "IFNULL(c.`con_tert`,%d) as `con_tert`, IFNULL(c.`con_untr`,%d) as `con_untr`, ", 0, 0);
	j+=sprintf(sql+j, "IFNULL(c.`con_quat`,%d) as `con_quat`, ", 0);
	j+=sprintf(sql+j, " IFNULL(c.`rtf_man`,%d) as `rtf_man`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`rtf_irr`,%d) as `rtf_irr`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`fra_man_f`,%d)  as `fra_man_f`, IFNULL(c.`fra_man_t`,%d)  as `fra_man_t`,", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`fra_man_p`,%d)  as `fra_man_p`, IFNULL(c.`fra_man_c`,%d)  as `fra_man_c`,", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`fra_man_g`,%d)  as `fra_man_g`, IFNULL(c.`fra_man_m`,%d)  as `fra_man_m`,", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`fra_man_nd`,%d)  as `fra_man_nd`, IFNULL(c.`rtf_dom`,%d)  as `rtf_dom`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`stp_failure`,%d) as `stp_failure`, IFNULL(c.`to_treat_and_unknown`,%d) as `connec_treat_unknown`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`to_hanging_t`,%d) as `hanging_lat`, IFNULL(c.`to_open_def`,%d) as `open_defec`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`UrbSewerConn`,%d) as `UrbSewerConn`, IFNULL(c.`RurSewerConn`,%d) as `RurSewerConn`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`SPO_treat`,%d) as `SPO_treat` ", NODATA);	
	j+=sprintf(sql+j, "FROM %swq_load.country_input c ", MyDatabase);
	j+=sprintf(sql+j, "WHERE c.`IDScen`=%d AND c.`time`=%d ORDER BY c.`country_id`; ", optionsWQ_load_ptr->IDScen, year);

	query << sql;
	//cout<<"country query: "<<query<<endl;
	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif
	resQuery = query.store();
	if (resQuery.empty()) {cout << "get_country_input: error in table country_input.\n"; return 1;}

//	try
	{
		//while (rowQuery = resQuery.fetch_row())
		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			country=resQuery[rowNum]["country_id"];

			int countryPos = countries->get_countryPos(country);
			if (countryPos<0)  {cerr << "country "<< country << " not exist!!! error in country_input.\n"; return 1;}

			//Die Variablen werden mit den Werten aus der Datenbank gefuellt
			pop[countryPos][0]          = resQuery[rowNum]["pop_tot"];
			pop[countryPos][1]          = resQuery[rowNum]["pop_urb"];
			pop[countryPos][2]          = resQuery[rowNum]["pop_rur"];
			connectivity[countryPos][0] = resQuery[rowNum]["con_untr"];
			connectivity[countryPos][1] = resQuery[rowNum]["con_prim"];
			connectivity[countryPos][2] = resQuery[rowNum]["con_sec"];
			connectivity[countryPos][3] = resQuery[rowNum]["con_tert"];
			connectivity[countryPos][4] = resQuery[rowNum]["con_quat"];
			rtf_man[countryPos]         = resQuery[rowNum]["rtf_man"];
			rtf_irr[countryPos]         = resQuery[rowNum]["rtf_irr"];
			rtf_man_fra[countryPos][0]  = resQuery[rowNum]["fra_man_f"];
			rtf_man_fra[countryPos][1]  = resQuery[rowNum]["fra_man_t"];
			rtf_man_fra[countryPos][2]  = resQuery[rowNum]["fra_man_p"];
			rtf_man_fra[countryPos][3]  = resQuery[rowNum]["fra_man_c"];
			rtf_man_fra[countryPos][4]  = resQuery[rowNum]["fra_man_g"];
			rtf_man_fra[countryPos][5]  = resQuery[rowNum]["fra_man_m"];
			rtf_man_fra[countryPos][6]  = resQuery[rowNum]["fra_man_nd"];
			rtf_dom[countryPos]         = resQuery[rowNum]["rtf_dom"];
			stp_failure[countryPos]     = resQuery[rowNum]["stp_failure"];
			connec_treat_unknown[countryPos] = resQuery[rowNum]["connec_treat_unknown"];
			hanging_lat[countryPos]    = resQuery[rowNum]["hanging_lat"];
			open_defec[countryPos]     = resQuery[rowNum]["open_defec"];
			sewerConn[countryPos][0]   = resQuery[rowNum]["UrbSewerConn"];	
			sewerConn[countryPos][1]   = resQuery[rowNum]["RurSewerConn"];
			SPO_treated[countryPos]    = resQuery[rowNum]["SPO_treat"];


		}
	}

	return 0;

} // end of get_country_input

int ClassWQ_load::get_cell_parameter_input() {
	cout << "get_cell_parameter_input():\n";
	if(toCalc==5) return 0;   // for pesticide not needed

	if (is_c_geogen_country) { // c_geogen ist nicht cell-abhaengig
		cout << "c_geogen sind Laenderwerte!\n";
		return 0;
	}

	cout << "c_geogen sind Zellwerte!\n";

	int j, cell;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;

	for (cell=0; cell<continent.ng; cell++)
		c_geogen_cell[cell] = NODATA;

	query.reset();
	j=sprintf(sql,    "SELECT c.`cell` as `cell`, ");
	j+=sprintf(sql+j, "IFNULL(c.`c_geogen`,%d) as `c_geogen` ", NODATA);

	j+=sprintf(sql+j, "FROM %swq_load_%s.`parameter_cell_input` c WHERE c.`IDScen`=%d AND ", MyDatabase, optionsWQ_load_ptr->continent_abb, optionsWQ_load_ptr->IDScen);
	j+=sprintf(sql+j, "c.`parameter_id`=%d ORDER BY c.`cell`; ", optionsWQ_load_ptr->parameter_id);

	query << sql;

	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	resQuery = query.store();
	if (resQuery.empty()) {cout << "get_cell_parameter_input: error in table parameter_cell_input.\n"; return 1;}

	for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
	{
		cell    = resQuery[rowNum]["cell"]-1;
		if (cell < 0 || cell >= continent.ng) {
			cerr << "cell "<< cell << " not exist!!! error in parameter_cell_input.\n"; return 1;
		}

		c_geogen_cell[cell]    = resQuery[rowNum]["c_geogen"];
	}
	return 0;
} // end of get_cell_parameter_input

int ClassWQ_load::get_cell_pesticide_input() {
	if (toCalc!=5) { // only for pesticide
		return 0;
	}
	int j, cell;
	char sql[2000];                     // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;

	// organic carbon
	query.reset();
	j=sprintf(sql,    "SELECT c.`cell` as `cell`, ");
	j+=sprintf(sql+j, "IFNULL(c.`organic_carbon`,%d) as `organic_carbon` ", NODATA);
	j+=sprintf(sql+j, "FROM %swq_load_%s.`cell_input_pesticide` c WHERE c.`IDScen`=%d;", MyDatabase, optionsWQ_load_ptr->continent_abb, optionsWQ_load_ptr->IDScen);

	query << sql;

	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	resQuery = query.store();
	if (resQuery.empty()) {cout << "get_cell_pesticide_input: error in table cell_input_pesticide.\n"; return 1;}

	for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
	{
		cell    = resQuery[rowNum]["cell"]-1;
		if (cell < 0 || cell >= continent.ng) {
			cerr << "cell "<< cell << " not exist!!! error in cell_input_pesticide.\n"; return 1;
		}

		organic_carbon[cell]    = resQuery[rowNum]["organic_carbon"];
	}
	return 0;
} // get_cell_pesticide_input()
int ClassWQ_load::get_cell_pesticide_lastYear(int month, double* ld_dez, const string tab) { // month: [0;12)
	if (toCalc!=5) { // only for pesticide
		return 0;
	}
	if (optionsWQ_load_ptr->following_year==0 || month!=0) {
		// nur wenn JAhr nicht erster und nur für Januar kann man Dezember letztes Jahres einlesen
		return 0;
	}
//	int j, cell;
//	char sql[2000];                     // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;

	query.reset();
//	j=sprintf(sql,    "SELECT c.`cell` as `cell`, ");
//	j+=sprintf(sql+j, "IFNULL(c.`v12`,%d) as `ld_input` ", NODATA);
//	j+=sprintf(sql+j, "FROM %swq_load_%s.%s c WHERE c.`IDScen`=%d AND ", MyDatabase, optionsWQ_load_ptr->continent_abb, tab, optionsWQ_load_ptr->IDScen);
//	j+=sprintf(sql+j, "c.parameter_id = %d AND ", optionsWQ_load_ptr->parameter_id);
//	j+=sprintf(sql+j, "c.`time`=%d  ORDER BY c.`cell`; ", year-1);
//
//	query << sql;

	query << "SELECT c.`cell` as `cell`, IFNULL(c.`v12`," << NODATA << ") as `ld_input` "
			<< "FROM " << MyDatabase << "wq_load_"<< optionsWQ_load_ptr->continent_abb << "." << tab
// calc_ Tabellen mit IDScen, parameter_id in den Namen
//			<< " c WHERE c.`IDScen`="<< optionsWQ_load_ptr->IDScen << " AND "
//			<< "c.parameter_id = "<< optionsWQ_load_ptr->parameter_id << " AND "
			<< " c WHERE c.`time`= "<< year -1 << "  ORDER BY c.`cell`;";

	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	resQuery = query.store();
	if (resQuery.empty()) {
		cerr << "loading vom Dezember "<< year-1 << " not exist!!! error in " << tab << " or in OPTIONS.DAT.\n";
		return 1;
	} else { // Vorjahr wurde berechnet
		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			int cell    = resQuery[rowNum]["cell"]-1;
			if (cell < 0 || cell >= continent.ng) {
				cerr << "cell "<< cell << " not exist!!! error in calc_cell_load.\n"; return 1;
			}

			ld_dez[cell]    = resQuery[rowNum]["ld_input"];
		}

	}
	cout << "get_cell_pesticide_lastYear " << tab << " is done.\n";

	return 0;
}
int ClassWQ_load::get_cell_pesticide_input(int month) { // month: [0;12)
	// return 0: kein Pestizid oder Karte ld_input_pest für Monat wurde eingelesen
	// return 1: Fehler aufgetreten
	if (toCalc!=5) { // only for pesticide
		return 0;
	}

	int j, cell;
	char sql[2000];                     // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;

	for (int cell=0; cell<continent.ng; cell++)
	{
		ld_input_pest[cell] = 0.;
	}

//	bool mapExists = false;

	// Masse des in Zelle applizierten Pestidids
	query.reset();
	j=sprintf(sql,    "SELECT c.`cell` as `cell`, ");
	j+=sprintf(sql+j, "IFNULL(c.`ld_input`,%d) as `ld_input` ", NODATA);
	j+=sprintf(sql+j, "FROM %swq_load_%s.`cell_input_pesticide_maps` c WHERE c.`IDScen`=%d AND ", MyDatabase, optionsWQ_load_ptr->continent_abb, optionsWQ_load_ptr->IDScen);
	j+=sprintf(sql+j, "c.`time`=%d AND c.`month`=%d ORDER BY c.`cell`; ", year, month+1);

	query << sql;

	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	resQuery = query.store();
	if (!resQuery.empty()) {
		cout << "get_cell_pesticide_input() Month: " << month+1 << '\t' << resQuery.size() << endl;

		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			cell    = resQuery[rowNum]["cell"]-1;
			if (cell < 0 || cell >= continent.ng) {
				cerr << "cell "<< cell << " not exist!!! error in cell_input_pesticide_maps.\n"; return 1;
			}

			ld_input_pest[cell]    = resQuery[rowNum]["ld_input"];
		}
	} else {
		cout << "keine Karte für " << month+1 << '/' << year << endl;
	}

	return 0;

} // get_cell_pesticide_input(month)

int ClassWQ_load::get_parameter_input() {
	cout << "get_parameter_input():\n";
	int j;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
	//mysqlpp::Row    rowQuery;

	for (int i=0; i<10; i++)
		rem[i]=NODATA;
	treat_failure = NODATA;

	int MaxYear;
	if(getYear_parameter_input(MaxYear, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id, year)) return 1;

	query.reset();
	j=sprintf(sql,    "SELECT IFNULL(p.`rem_untr`,%d) as `rem_untr`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`rem_prim`,%d) as `rem_prim`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`rem_sec`,%d) as `rem_sec`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`rem_tert`,%d) as `rem_tert`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`rem_quat`,%d) as `rem_quat`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`rem_soil`,%d) as `rem_soil`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`red_fac_org`,%d) as `red_fac_org`, IFNULL(p.`red_fac_inor`,%d) as `red_fac_inor`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`k_storage`,%d) as `k_storage`,  IFNULL(p.`k_soil`,%d) as `k_soil`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`treat_failure`,%d) as `treat_failure` ", NODATA);
	if (toCalc==5) {
		j+=sprintf(sql+j, ", IFNULL(p.`DT50soil`,%d) as `DT50soil`", NODATA);
		j+=sprintf(sql+j, ", IFNULL(p.`koc`,%d) as `koc`", NODATA);
		j+=sprintf(sql+j, ", IFNULL(p.`WBZ`,%d) as `WBZ` ", NODATA);
	}
	j+=sprintf(sql+j, "FROM %swq_load.`parameter_input` p ", MyDatabase );
	j+=sprintf(sql+j, "WHERE p.`idScen`=%d AND p.`parameter_id`=%d AND p.`time`=%d ; ", optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id, MaxYear);

	query << sql;

	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif

	resQuery = query.store();
	if (resQuery.empty()) {cout << "get_parameter_input: error in table parameter_input.\n"; return 1;}

	rem[0]   = resQuery.at(0)["rem_untr"];
	rem[1]   = resQuery.at(0)["rem_prim"];
	rem[2]   = resQuery.at(0)["rem_sec"];
	rem[3]   = resQuery.at(0)["rem_tert"];
	rem[4]   = resQuery.at(0)["rem_quat"];
	rem[5]   = resQuery.at(0)["rem_soil"];
	rem[6]   = resQuery.at(0)["red_fac_org"];
	rem[7]   = resQuery.at(0)["red_fac_inor"];
	rem[8]   = resQuery.at(0)["k_storage"];
	rem[9]   = resQuery.at(0)["k_soil"];
	treat_failure  = resQuery.at(0)["treat_failure"];
	if (toCalc==5) {
		DT50soil = resQuery.at(0)["DT50soil"];
		koc = resQuery.at(0)["koc"];
		WBZ = resQuery.at(0)["WBZ"];

		cout << "Parameter Pestizide DT50soil: \t"<< DT50soil<< "\tkoc\t" << koc << "\tWBZ\t" << WBZ << endl;
	}

	if (toCalc==5) {
		query.reset();
		// für jedes land WBZ-Wert aus letzten Jahr, das <=year
		// Länader die nicht in Ergebniss vorhanden, kriegen const Wert WBZ
		j=sprintf(sql,    "SELECT c.country_id, c.timeFrom, c.WBZ ");
		j+=sprintf(sql+j, "FROM wq_load.country_parameter_input_pesticide c ");
		j+=sprintf(sql+j, ", (SELECT cIn.country_id, max(cIn.timeFrom) as maxTime ");
		j+=sprintf(sql+j, "FROM wq_load.country_parameter_input_pesticide cIn ");
		j+=sprintf(sql+j, "WHERE cIn.IDScen=%d AND cIn.parameter_id=%d", optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
		j+=sprintf(sql+j, " AND cIn.timeFrom<=%d ", year);
		j+=sprintf(sql+j, "GROUP BY cIn.IDScen, cIn.parameter_id, cIn.country_id) c1 ");
		j+=sprintf(sql+j, "WHERE c.IDScen=%d AND c.parameter_id=%d AND ", optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
		j+=sprintf(sql+j, "c1.country_id=c.country_id AND c1.maxTime=c.timeFrom;");

		query << sql;

		#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		#endif

		resQuery = query.store();
		if (!resQuery.empty()) {
			cout << "get_parameter_input: There are data in wq_load.country_parameter_input_pesticide.\t" << resQuery.size() << endl;

			// temporär array für Länderwerte WBZ aus der Tabelle, default = const WBZ
			double* WBZ_country = new double[countries->number];
			for (int country = 0; country<countries->number; country++) {
				WBZ_country[country] = WBZ;
			}

			// Wenn in der Tabelle Land vorhanden, Wert ersetzen
			for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
			{
				int country=resQuery[rowNum]["country_id"];

				int countryPos = countries->get_countryPos(country);
				if (countryPos<0)  {cerr << "country "<< country << " not exist!!! error in country_parameter_input_pesticide.\n"; return 1;}

				WBZ_country[countryPos] = resQuery[rowNum]["WBZ"];
				cout << country << '\t' << countryPos << '\t' << WBZ_country[countryPos] << endl;

			}

			// Zellenwerte schreiben
			cout << "WBZ_cell:\n";
			for (int cell = 0; cell<continent.ng; cell++) {
				int countryPos = countries->get_countryPos(country_cell[cell]);
				if (countryPos<0) WBZ_cell[cell] = NODATA;
				else WBZ_cell[cell] = WBZ_country[countryPos];

			}
			cout << "WBZ_cell done.\n";

			delete[] WBZ_country; WBZ_country = NULL;
		} else {
			for (int cell = 0; cell<continent.ng; cell++) {
				WBZ_cell[cell] = WBZ;

			}

		}
	}

	cout<<"get_parameter_input done"<<endl;

	return 0;


} // end of get_parameter_input

int ClassWQ_load::get_country_parameter_input()
{
	cout << "get_country_parameter_input():\n";
	int j;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
	//mysqlpp::Row    rowQuery;
	//cout << "year, parameter_id: " << year <<endl << parameter_id<<endl;

	int country;
	for (country=0; country<countries->number; country++) {
		ef[country]       = NODATA;
		c_geogen[country] = NODATA;
		conc_urb[country] = NODATA;
		for (int man_fra=0; man_fra<7; man_fra++)
			conc_man[country][man_fra] = NODATA;

		for (int sepc_class=0; sepc_class<4; sepc_class++)
			conc_tds[country][sepc_class] = NODATA;

		for (int resource=0; resource<5; resource++)
			conc_mining[country][resource] = NODATA;
	}
	if(toCalc==5) return 0;   // for pesticide not needed

	query.reset();
	j=sprintf(sql,    "SELECT c.`country_id`, ");
	j+=sprintf(sql+j, "IFNULL(c.`ef`,%d) as `ef`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`conc_tds1`,%d) as `conc_tds1`, IFNULL(c.`conc_tds2`,%d) as `conc_tds2`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`conc_tds3`,%d) as `conc_tds3`, IFNULL(c.`conc_tds4`,%d) as `conc_tds4`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`conc_man_f`,%d) as `conc_man_f`, IFNULL(c.`conc_man_t`,%d) as `conc_man_t`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`conc_man_p`,%d) as `conc_man_p`, IFNULL(c.`conc_man_c`,%d) as `conc_man_c`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`conc_man_g`,%d) as `conc_man_g`, IFNULL(c.`conc_man_m`,%d) as `conc_man_m`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(c.`conc_man_nd`,%d) as `conc_man_nd`, IFNULL(c.`conc_urb`,%d) as `conc_urb` ", NODATA, NODATA);
	//if (is_c_geogen_country) {
		j+=sprintf(sql+j, ", IFNULL(c.`c_geogen`,%d) as `c_geogen` ", NODATA);
	//}
	j+=sprintf(sql+j, "FROM %swq_load.`country_parameter_input` c WHERE c.`IDScen`=%d AND ", MyDatabase, optionsWQ_load_ptr->IDScen);
	j+=sprintf(sql+j, "c.`parameter_id`=%d AND c.`time`=%d ORDER BY c.`country_id`; ", optionsWQ_load_ptr->parameter_id, year);

	query << sql;

	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	resQuery = query.store();
	if (resQuery.empty()) {cout << "get_country_parameter_input: error in table country_parameter_input.\n"; return 1;}

//	try
	{

		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			country=resQuery[rowNum]["country_id"];

			int countryPos = countries->get_countryPos(country);
			if (countryPos<0)  {cerr << "country "<< country << " not exist!!! error in country_parameter_input.\n"; return 1;}

			ef[countryPos]          = resQuery[rowNum]["ef"];
			conc_man[countryPos][0] = resQuery[rowNum]["conc_man_f"];
			conc_man[countryPos][1] = resQuery[rowNum]["conc_man_t"];
			conc_man[countryPos][2] = resQuery[rowNum]["conc_man_p"];
			conc_man[countryPos][3] = resQuery[rowNum]["conc_man_c"];
			conc_man[countryPos][4] = resQuery[rowNum]["conc_man_g"];
			conc_man[countryPos][5] = resQuery[rowNum]["conc_man_m"];
			conc_man[countryPos][6] = resQuery[rowNum]["conc_man_nd"];
			conc_tds[countryPos][0] = resQuery[rowNum]["conc_tds1"];
			conc_tds[countryPos][1] = resQuery[rowNum]["conc_tds2"];
			conc_tds[countryPos][2] = resQuery[rowNum]["conc_tds3"];
			conc_tds[countryPos][3] = resQuery[rowNum]["conc_tds4"];
			conc_urb[countryPos]    = resQuery[rowNum]["conc_urb"];
			c_geogen[countryPos]    = resQuery[rowNum]["c_geogen"];

		}
		resQuery.clear();
	}

#ifdef NO_MINING
	cout << "\nkein mining wird berechnet\n";
#else
	// mining
	if (toCalc==1) { // if(salt)
		query.reset();
		j=sprintf(sql,    "SELECT c.`country_id`, ");
		j+=sprintf(sql+j, "IFNULL(c.`conc_mining_coal`,%d) as `conc_mining_coal`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(c.`conc_mining_al`,%d) as `conc_mining_al`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(c.`conc_mining_au`,%d) as `conc_mining_au`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(c.`conc_mining_cu`,%d) as `conc_mining_cu`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(c.`conc_mining_bx`,%d) as `conc_mining_bx` ", NODATA);
		j+=sprintf(sql+j, "FROM %swq_load.`country_parameter_input_tds` c WHERE c.`IDScen`=%d AND ", MyDatabase, optionsWQ_load_ptr->IDScen);
		j+=sprintf(sql+j, "c.`parameter_id`=%d AND c.`time`=%d ORDER BY c.`country_id`; ", optionsWQ_load_ptr->parameter_id, year);

		query << sql;

		//#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		//#endif
		resQuery = query.store();
		if (resQuery.empty()) {cout << "get_country_parameter_input: error in table country_parameter_input.\n"; return 1;}

		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			country=resQuery[rowNum]["country_id"];

			int countryPos = countries->get_countryPos(country);
			if (countryPos<0)  {cerr << "country "<< country << " not exist!!! error in country_parameter_input.\n"; return 1;}

			conc_mining[countryPos][0] = resQuery[rowNum]["conc_mining_coal"];
			conc_mining[countryPos][1] = resQuery[rowNum]["conc_mining_al"];
			conc_mining[countryPos][2] = resQuery[rowNum]["conc_mining_au"];
			conc_mining[countryPos][3] = resQuery[rowNum]["conc_mining_cu"];
			conc_mining[countryPos][4] = resQuery[rowNum]["conc_mining_bx"];

			if (country==170) cout << "conc_mining for "<< country << '\t'
					<< conc_mining[countryPos][0] << '\t'
					<< conc_mining[countryPos][1] << '\t'
					<< conc_mining[countryPos][2] << '\t'
					<< conc_mining[countryPos][3] << '\t'
					<< conc_mining[countryPos][4] << '\n';
		}
		resQuery.clear();
	}
#endif

	return 0;

} // end of get_country_parameter_input()

int ClassWQ_load::get_cell_input()
{
	cout << "get_cell_input():\n";

	int j;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
	//mysqlpp::Row    rowQuery;

	char TableName_cellInput[maxcharlength];
	char TableName_cellInput_tds[maxcharlength];
	if (!optionsWQ_load_ptr->IDInTableName) {
		sprintf(TableName_cellInput, "%swq_load_%s.`cell_input`", MyDatabase, optionsWQ_load_ptr->continent_abb);
		sprintf(TableName_cellInput_tds, "%swq_load_%s.`cell_input_tds`", MyDatabase, optionsWQ_load_ptr->continent_abb);
	} else {
		sprintf(TableName_cellInput, "%swq_load_%s.`cell_input_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb, optionsWQ_load_ptr->IDScen);
		sprintf(TableName_cellInput_tds, "%swq_load_%s.`cell_input_tds_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb, optionsWQ_load_ptr->IDScen);
	}

	int cell;

	for (cell=0; cell<continent.ng; cell++) {
		country_cell[cell] = NODATA;
		rtf_man_cell[cell] = NODATA;
		rtf_irr_cell[cell] = NODATA;
		gdp[cell]          = NODATA;
		salinity[cell]     = NODATA;
		humidity[cell]     = NODATA;
		LU_cell[cell]      = NODATA;

		for (int i=0; i<3; i++)
			pop_cell[cell][i] = NODATA;

		if (toCalc == 1) {
			for (int resource=0; resource<5; resource++) {
				rtf_mining_cell[cell][resource] = NODATA;
			}
		}
	}
//	if(toCalc==5) return 0;   // for pesticide needed only country_id (ISONUM)

	query.reset();

	j=sprintf(sql,    "SELECT `cell`, IFNULL(`country_id`,%d) as `country_id`, ", NODATA);
	if (!optionsWQ_load_ptr->gridded_pop_from_file) {
		// nur wenn gegriddete population aus der Tabelle eingelesen wird
		j+=sprintf(sql+j, "IFNULL(`pop_urb`,%d) as `pop_urb`, IFNULL(`pop_rur`,%d) as `pop_rur`, ", NODATA, NODATA);
		j+=sprintf(sql+j, "IFNULL(`pop_tot`,%d) as `pop_tot`, ", NODATA);
	}
	j+=sprintf(sql+j, "IFNULL(`rtf_man`,%d) as `rtf_man`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(`rtf_irr`,%d) as `rtf_irr`, IFNULL(`gdp`,%d) as `gdp`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(`salinity`,%d) as `salinity`, IFNULL(`humidity`,%d) as `humidity`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(`lu`,%d) as `lu` ", NODATA);
	j+=sprintf(sql+j, "FROM %s ", TableName_cellInput);
	if (!optionsWQ_load_ptr->IDInTableName)
		j+=sprintf(sql+j, "WHERE `IDScen`=%d AND ", optionsWQ_load_ptr->IDScen);
	else
		j+=sprintf(sql+j, "WHERE ");
	j+=sprintf(sql+j, "`time`=%d ORDER BY `cell`; ", year);

	query << sql;
	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	resQuery = query.store();
	if (resQuery.empty()) {cerr << "get_cell_input: error in table cell_input ("  << TableName_cellInput << ").\n"; return 1;}

//	try
	{

		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			cell    = resQuery[rowNum]["cell"]-1;

			country_cell[cell] = resQuery[rowNum]["country_id"];
			if (!optionsWQ_load_ptr->gridded_pop_from_file) {
				// nur wenn gegriddete population aus der Tabelle eingelesen wird
				pop_cell[cell][0]  = resQuery[rowNum]["pop_tot"];
				pop_cell[cell][1]  = resQuery[rowNum]["pop_urb"];
				pop_cell[cell][2]  = resQuery[rowNum]["pop_rur"];
			}
			rtf_man_cell[cell] = resQuery[rowNum]["rtf_man"];

			rtf_irr_cell[cell] = resQuery[rowNum]["rtf_irr"];
			gdp[cell]          = resQuery[rowNum]["gdp"];
			salinity[cell]     = resQuery[rowNum]["salinity"];
			humidity[cell]     = resQuery[rowNum]["humidity"];
			LU_cell[cell]      = resQuery[rowNum]["lu"];

		}
	}
	resQuery.clear();

	if (optionsWQ_load_ptr->gridded_pop_from_file) {
		// nur wenn gegriddete population aus der Tabelle eingelesen wird
		// Daten aus UNF-Dateien einlesen
		double *tmp_population_urb = new double[continent.ng];
		double *tmp_population_rur = new double[continent.ng];

		char filename[maxcharlength];
		sprintf(filename,"%s/GURBPOP%d.UNF0", optionsWQ_load_ptr->gridded_pop_path, year);
		if (readInputFile(filename, 4, 1, continent.ng, &tmp_population_urb[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		cout << "year " << year << " griddet urban population done\n";

		sprintf(filename,"%s/GRURPOP%d.UNF0", optionsWQ_load_ptr->gridded_pop_path, year);
		if (readInputFile(filename, 4, 1, continent.ng, &tmp_population_rur[0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		cout << "year " << year << "  griddet rural population  done\n";
		
		for (int cell=0; cell<continent.ng; cell++) {
			pop_cell[cell][0]  = tmp_population_urb[cell] + tmp_population_rur[cell];
			pop_cell[cell][1]  = tmp_population_urb[cell];
			pop_cell[cell][2]  = tmp_population_rur[cell];
		}
		
		delete[] tmp_population_urb; tmp_population_urb=NULL;
		delete[] tmp_population_rur; tmp_population_rur=NULL;
	}

#ifdef NO_MINING

	cout << "\nkein mining wird berechnet\n";
#else
	if (toCalc == 1) {
		query.reset();

		j=sprintf(sql,    "SELECT `cell`, ");
		j+=sprintf(sql+j, "IFNULL(`rtf_coal`,%d) as `rtf_coal`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(`rtf_al`,%d) as `rtf_al`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(`rtf_au`,%d) as `rtf_au`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(`rtf_co`,%d) as `rtf_co`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(`rtf_bx`,%d) as `rtf_bx` ", NODATA);
		j+=sprintf(sql+j, "\nFROM %s ", TableName_cellInput_tds);
		if (!optionsWQ_load_ptr->IDInTableName)
			j+=sprintf(sql+j, "WHERE `IDScen`=%d AND `time`=%d ORDER BY `cell`; ", optionsWQ_load_ptr->IDScen, year);
		else
			j+=sprintf(sql+j, "WHERE `time`=%d ORDER BY `cell`; ", year);

		query << sql;
		//#ifdef DEBUG_queries
			cout << "Query: " << query << endl << endl; //cin >> j;
		//#endif
		resQuery = query.store();
		if (resQuery.empty()) {cerr << "get_cell_input: error in table cell_input_tds ("<< TableName_cellInput_tds << ").\n"; return 1;}

		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			cell    = resQuery[rowNum]["cell"]-1;

			rtf_mining_cell[cell][0] = resQuery[rowNum]["rtf_coal"];
			rtf_mining_cell[cell][1] = resQuery[rowNum]["rtf_al"];
			rtf_mining_cell[cell][2] = resQuery[rowNum]["rtf_au"];
			rtf_mining_cell[cell][3] = resQuery[rowNum]["rtf_co"];
			rtf_mining_cell[cell][4] = resQuery[rowNum]["rtf_bx"];

			if (cell==660) cout << "rtf_mining_cell["<<cell<<"]=\t"
					<< rtf_mining_cell[cell][0] << '\t'
					<< rtf_mining_cell[cell][1] << '\t'
					<< rtf_mining_cell[cell][2] << '\t'
					<< rtf_mining_cell[cell][3] << '\t'
					<< rtf_mining_cell[cell][4] << '\n';
		}

	}
#endif

	return 0;

} // end of get_cell_input


int ClassWQ_load::monthValues() {

	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery;

	double teta, teta_lake;
	if (getTeta(teta, teta_lake, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id, year)) exit(1);

	char TableName[maxcharlength];
	char sql[10000]; // variable for SQL-query
	int j;

	cout<<"========================= domestic ========================="<<endl;
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_dom_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v3` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v4` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v5` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v6` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v7` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v8` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v9` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v10` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v11` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v12` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");

		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='domestic cell load [t/month], [10^10no/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}
	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND ";
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND ";
	query << "`time`= " << year << ";";
	
	query.execute();

	/*This is the original code: do NOT delete!
	 query.reset();
	 query << "INSERT INTO "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".`calc_cell_month_load_dom` (`IDScen`, `parameter_id`, `cell`, `time`, "
	 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) "
	 <<"SELECT c.`IDScen`, c.`parameter_id`, c.`cell`, c.`time`, "
	 <<"c.`ld_treat_dom`/12 as v1, c.`ld_treat_dom`/12 as v2, c.`ld_treat_dom`/12 as v3, c.`ld_treat_dom`/12 as v4, "
	 <<"c.`ld_treat_dom`/12 as v5, c.`ld_treat_dom`/12 as v6, c.`ld_treat_dom`/12 as v7, c.`ld_treat_dom`/12 as v8, "
	 <<"c.`ld_treat_dom`/12 as v9, c.`ld_treat_dom`/12 as v10, c.`ld_treat_dom`/12 as v11 , c.`ld_treat_dom`/12 as v12 "
	 << "FROM "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load c "
	 << "WHERE c.`IDScen`= " <<optionsWQ_load_ptr->IDScen<<" AND c.`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND c.`time`="<< year << "; ";

	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif

	 query.execute();

	 */

	//##################################################
	//This is the code for the correction factor -> it makes sure that the return flows from hydrology and
	//water quality model (here for domestic implicit) are the same. Furthermore the
	//return flow in the hydrology are correted to not exceed the river availablity

	double (*corr_factor)[12] = NULL;

	if (toCalc==0||toCalc==1||toCalc==2||toCalc==3||toCalc==4) { // for BOD, TDS, FC, TN and TP
		corr_factor = new double[continent.ng][12];
		// get corr_factor
		{
			char filename[maxcharlength];
			sprintf(filename,"%s/G_CORR_FACT_RTF_%d.12.UNF0", optionsWQ_load_ptr->path_corr_factor, year);
			if (readInputFile(filename, 4, 12, continent.ng, &corr_factor[0][0], gcrc, optionsWQ_load_ptr, &continent)) exit(1);
		}
		insert_into_tab(year, "dom", ld_treat_dom_cell, corr_factor);
	}
	//this updates the tables calc_cell_load and calc_country_load
	//but only for treated domestic as only this is used -> untreated is not changed!
//	query.reset();
//	query <<"update "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load c1, "
//			<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_month_load_dom c "
//			<<"set c1.`ld_treat_dom`= IFNULL(c.`v1`,0)+ IFNULL(c.`v2`,0)+ IFNULL(c.`v3`,0)+ IFNULL(c.`v4`,0)+ IFNULL(c.`v5`,0)+ IFNULL(c.`v6`,0)+ IFNULL(c.`v7`,0)+ IFNULL(c.`v8`,0)+ IFNULL(c.`v9`,0)+ IFNULL(c.`v10`,0)+ IFNULL(c.`v11`,0)+ IFNULL(c.`v12`,0) "
//			<<"WHERE c.`IDScen`="<<optionsWQ_load_ptr->IDScen<<" AND c.`parameter_id`="<<optionsWQ_load_ptr->parameter_id
//			<<" AND c.`time`="<<year<<" AND c1.`cell`=c.`cell` AND c1.`IDScen`=c.`IDScen` "
//			<<"AND c.`parameter_id`=c1.`parameter_id` AND c1.`time`=c.`time`;"<<endl;
//			//cout << "Query update calc cell load treat dom: " << query << endl << endl;
//	query.execute();

	//End of code for the correction factor
	//#############################################################################

	cout<<"========================= urban ========================="<<endl;
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_urb_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);

	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1' COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL,\n");
		j+=sprintf(sql+j, "  `v3` double default NULL,\n");
		j+=sprintf(sql+j, "  `v4` double default NULL,\n");
		j+=sprintf(sql+j, "  `v5` double default NULL,\n");
		j+=sprintf(sql+j, "  `v6` double default NULL,\n");
		j+=sprintf(sql+j, "  `v7` double default NULL,\n");
		j+=sprintf(sql+j, "  `v8` double default NULL,\n");
		j+=sprintf(sql+j, "  `v9` double default NULL,\n");
		j+=sprintf(sql+j, "  `v10` double default NULL,\n");
		j+=sprintf(sql+j, "  `v11` double default NULL,\n");
		j+=sprintf(sql+j, "  `v12` double default NULL,\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='treated urban cell load [t/month], [10^10no/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}
	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE "; 
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen << " AND ";
	//query << " `parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND ";
	query << " `time`= " << year << ";";
	query.execute();

	if (toCalc==0||toCalc==1||toCalc==2||toCalc==3||toCalc==4) { // for BOD, TDS, FC, TN and TP
		insert_into_tab(year, "urb", ld_treat_urb_cell);
	}

	cout << "========================= manufacturing =================================\n";
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_man_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
	, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);

	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL,\n");
		j+=sprintf(sql+j, "  `v3` double default NULL,\n");
		j+=sprintf(sql+j, "  `v4` double default NULL,\n");
		j+=sprintf(sql+j, "  `v5` double default NULL,\n");
		j+=sprintf(sql+j, "  `v6` double default NULL,\n");
		j+=sprintf(sql+j, "  `v7` double default NULL,\n");
		j+=sprintf(sql+j, "  `v8` double default NULL,\n");
		j+=sprintf(sql+j, "  `v9` double default NULL,\n");
		j+=sprintf(sql+j, "  `v10` double default NULL,\n");
		j+=sprintf(sql+j, "  `v11` double default NULL,\n");
		j+=sprintf(sql+j, "  `v12` double default NULL,\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");

		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='manufacturing cell load [t/month], [10^10no/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}
	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE "; 
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND ";
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id<< " AND ";
	query << " `time`= " << year << ";";
	query.execute();

	/*query.reset();
	query << "INSERT INTO "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".`calc_cell_month_load_man` (`IDScen`, `parameter_id`, `cell`, `time`, "
	 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) "
	 <<"SELECT c.`IDScen`, c.`parameter_id`, c.`cell`, c.`time`, "
	 <<"c.`ld_treat_man`/12 as v1, c.`ld_treat_man`/12 as v2, c.`ld_treat_man`/12 as v3, c.`ld_treat_man`/12 as v4, "
	 <<"c.`ld_treat_man`/12 as v5, c.`ld_treat_man`/12 as v6, c.`ld_treat_man`/12 as v7, c.`ld_treat_man`/12 as v8, "
	 <<"c.`ld_treat_man`/12 as v9, c.`ld_treat_man`/12 as v10, c.`ld_treat_man`/12 as v11 , c.`ld_treat_man`/12 as v12 "
	 << "FROM "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load c "
	 << "WHERE c.`IDScen`="<<optionsWQ_load_ptr->IDScen<<" AND c.`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND c.`time`="<< year << "; ";

	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif

	query.execute();


	query.reset();
	query << "INSERT INTO "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".`calc_cell_month_load_man` (`IDScen`, `parameter_id`, `cell`, `time`, "
	 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) "
	 <<"SELECT c.`IDScen`, c.`parameter_id`, c.`cell`, c.`time`, "
	 <<"c.`ld_treat_man`/12 as v1, c.`ld_treat_man`/12 as v2, c.`ld_treat_man`/12 as v3, c.`ld_treat_man`/12 as v4, "
	 <<"c.`ld_treat_man`/12 as v5, c.`ld_treat_man`/12 as v6, c.`ld_treat_man`/12 as v7, c.`ld_treat_man`/12 as v8, "
	 <<"c.`ld_treat_man`/12 as v9, c.`ld_treat_man`/12 as v10, c.`ld_treat_man`/12 as v11 , c.`ld_treat_man`/12 as v12 "
	 << "FROM "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load c "
	 << "WHERE c.`IDScen`="<<optionsWQ_load_ptr->IDScen<<" AND c.`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND c.`time`="<< year << "; ";

	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif

		query.execute();*/

	if (toCalc==0||toCalc==1||toCalc==2||toCalc==3||toCalc==4) { // for BOD, TDS, FC, TN and TP
		insert_into_tab(year, "man", ld_treat_man_cell, corr_factor);
	}

	//this updates the tables calc_cell_load and calc_country_load
	//but only for treated manufacturing as only this is used -> untreated is not changed!
//	query.reset();
//	query <<"update "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load c1, "
//			<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_month_load_man c "
//			<<"set c1.`ld_treat_man`= IFNULL(c.`v1`,0)+ IFNULL(c.`v2`,0)+ IFNULL(c.`v3`,0)+ IFNULL(c.`v4`,0)+ IFNULL(c.`v5`,0)+ IFNULL(c.`v6`,0)+ IFNULL(c.`v7`,0)+ IFNULL(c.`v8`,0)+ IFNULL(c.`v9`,0)+ IFNULL(c.`v10`,0)+ IFNULL(c.`v11`,0)+ IFNULL(c.`v12`,0) "
//			<<"WHERE c.`IDScen`="<<optionsWQ_load_ptr->IDScen<<" AND c.`parameter_id`="<<optionsWQ_load_ptr->parameter_id
//			<<" AND c.`time`="<<year<<" AND c1.`cell`=c.`cell` AND c1.`IDScen`=c.`IDScen` "
//			<<"AND c.`parameter_id`=c1.`parameter_id` AND c1.`time`=c.`time`;"<<endl;
//			//cout << "Query update calc cell load treat man: " << query << endl << endl;
//	query.execute();

	cout << "========================= mining =================================\n";
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_mining_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL,\n");
		j+=sprintf(sql+j, "  `v3` double default NULL,\n");
		j+=sprintf(sql+j, "  `v4` double default NULL,\n");
		j+=sprintf(sql+j, "  `v5` double default NULL,\n");
		j+=sprintf(sql+j, "  `v6` double default NULL,\n");
		j+=sprintf(sql+j, "  `v7` double default NULL,\n");
		j+=sprintf(sql+j, "  `v8` double default NULL,\n");
		j+=sprintf(sql+j, "  `v9` double default NULL,\n");
		j+=sprintf(sql+j, "  `v10` double default NULL,\n");
		j+=sprintf(sql+j, "  `v11` double default NULL,\n");
		j+=sprintf(sql+j, "  `v12` double default NULL,\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='mining cell load [t/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}
	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE "; 
	//query << "`IDScen`=" << optionsWQ_load_ptr->IDScen << " AND "; 
	//query <<"`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND "; 
	query << " `time`= " << year << ";";
	query.execute();

	if (toCalc==1) { // for TDS
		insert_into_tab(year, "mining", ld_treat_mining_cell, NULL);
	}

	cout << "========================= irrigation ====================================\n";
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_irr_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v3` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v4` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v5` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v6` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v7` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v8` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v9` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v10` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v11` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v12` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='cell load  from irrigation return flow [t/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}
	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE "; 
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND "; 
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND "; 
	query << "`time`= " << year << ";";
	query.execute();

	if (toCalc==1|| toCalc==5) { // irrigation/agriculture nur fuer Salz oder Pesticide
		insert_into_tab(year, "irr", ld_agr_cell);
	}

	cout << "========================= Masse des Pestizides im Boden ====================================\n";
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_soil_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v3` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v4` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v5` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v6` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v7` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v8` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v9` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v10` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v11` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v12` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='Masse des Pestizides im Boden [t/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}
	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND ";
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id	<< " AND "; 
	query << "`time`= " << year << ";";
	query.execute();

	if (toCalc==5) { // nur fuer Pesticide
		insert_into_tab(year, "soil", ld_agr_soil_cell);
	}
	// ====================================================================
	// calculate act_surface_runoff
	// used for manure, scattered settlements, industrial fertilizer, open defecation, geogenic background
	double (*act_surface_runoff)[12] = NULL; //WaterGAP surface runoff consists of all runoff: from land and urban areas
	double *act_surface_runoff_year  = NULL;
	double *act_surface_runoff_summer= NULL;

	if (toCalc==0||toCalc==1||toCalc==2||toCalc==3||toCalc==4) { // for BOD, TDS, FC, TN and TP
		act_surface_runoff       = new double[continent.ng][12];	//WaterGAP surface runoff consists of all runoff: from land and urban areas
		act_surface_runoff_year  = new double[continent.ng];
		act_surface_runoff_summer= new double[continent.ng];

		for (int cell=0; cell<continent.ng; cell++) {
			act_surface_runoff_year[cell]  =0.0;
			act_surface_runoff_summer[cell]=0.0;
			//if(cell==58694)
			//cout<<"sur_runoff_cell[cell][0]: "<<sur_runoff_cell[cell][0]<<" urban_runoff_cell[cell][0]: "<<urban_runoff_cell[cell][0]<<endl;

			for(int month=0; month<12; month++) {
				if (sur_runoff_cell[cell][month]==NODATA|| urban_runoff_cell[cell][month]==NODATA) act_surface_runoff[cell][month]=NODATA;
				else {
					if (sur_runoff_cell[cell][month]>=0.0 && urban_runoff_cell[cell][month]>=0.0){
						act_surface_runoff[cell][month]=sur_runoff_cell[cell][month]-urban_runoff_cell[cell][month];
						if(act_surface_runoff[cell][month]<0)
							act_surface_runoff[cell][month]=0;	//may happen because of the way WaterGAP calculates. This will be changed in WaterGAP soon
						act_surface_runoff_year[cell]+= act_surface_runoff[cell][month];
						if((month>1) && (month <10))
							act_surface_runoff_summer[cell]+=act_surface_runoff[cell][month];
					}else{
						act_surface_runoff[cell][month]=NODATA;
						act_surface_runoff_year[cell]=NODATA;
					}
				}
			} // for(month)

		} //for(cell)
	}

	// ====================================================================

	cout<<"========================= manure ========================================"<<endl;
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_manure_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL,\n");
		j+=sprintf(sql+j, "  `v3` double default NULL,\n");
		j+=sprintf(sql+j, "  `v4` double default NULL,\n");
		j+=sprintf(sql+j, "  `v5` double default NULL,\n");
		j+=sprintf(sql+j, "  `v6` double default NULL,\n");
		j+=sprintf(sql+j, "  `v7` double default NULL,\n");
		j+=sprintf(sql+j, "  `v8` double default NULL,\n");
		j+=sprintf(sql+j, "  `v9` double default NULL,\n");
		j+=sprintf(sql+j, "  `v10` double default NULL,\n");
		j+=sprintf(sql+j, "  `v11` double default NULL,\n");
		j+=sprintf(sql+j, "  `v12` double default NULL,\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='diff. sour. only manure cell ld [t/month], [10^10 cfu/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}

	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND ";
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND ";
	query << "`time`= " << year << ";";
	query.execute();


	if (toCalc==0||toCalc==1||toCalc==2||toCalc==3||toCalc==4) { // for BOD, TDS, FC, TN and TP
		// ---------------------- Abflussabhaengig ---------------------

//		if(toCalc==0||toCalc==1||toCalc==3||toCalc==4){	//BOD, TDS, TN, TP
//			//no reduction in storage is calculated here
//		}
		if(toCalc==2){
			double ld_manure_summer;
			double ld_manure_winter;
			double k = rem[8];

			//if manure is applied throughout the whole year
			if(optionsWQ_load_ptr->manure_timing==0){
				for (int cell=0; cell<continent.ng; cell++) {
					 //removal on soil
					 if (ld_manure_cell[cell]!=-9999.0)
						 ld_manure_cell[cell]=getRainDays(ld_manure_cell[cell], rem[9], perc_days, cell, year, teta, temperature_cell, optionsWQ_load_ptr->manure_timing);

				} // for(cell)
			}// end if(manure_timining==0)
			if(optionsWQ_load_ptr->manure_timing==1){
				for (int cell=0; cell<continent.ng; cell++) {
					double temp=0.0;
					double temp_winter=0.0;

					//manure amount is divided for summer and winter
					ld_manure_summer=ld_manure_cell[cell]*(8.0/12.0);
					ld_manure_winter=ld_manure_cell[cell]*(4.0/12.0);

					//decay in storage - Chicks law
					//sum of temperature of winter month
					for  (int month=0; month<12; month++){
						if((month<2) || (month >=10)){
							temp_winter+=temperature_cell[cell][month];
							//if(cell==75617||cell==76839||cell==79742||cell==150925||cell==43048)
							//	cout<<"temp_winter: "<<temp_winter<<" month: "<<month
							//		<<" cell: "<<cell<<" temperature_cell[cell][month]: "<<temperature_cell[cell][month]<<endl;;
						}
					}

					temp_winter=temp_winter/4.0;	//temperature mean of winter month
					//if(cell==75617||cell==76839||cell==79742||cell==150925||cell==43048)
					//	cout<<"temp_winter: "<<temp_winter;

					temp=k*122.0*pow(teta, (temp_winter/100.0)-20.0);	//time=122 days for winter month
					ld_manure_winter=ld_manure_winter*exp(-temp);

					//if(cell==94288|| cell==46562||cell==58694||cell==6540||cell==150925)
					//cout<<"before add load manure winter: "<<ld_manure_winter<<" ld_manure_summer: "<<ld_manure_summer<<endl;

					ld_manure_summer+=ld_manure_winter;
					//if(cell==94288|| cell==46562||cell==58694||cell==6540||cell==150925)
						//cout<<"before rain days ld_manure_summer: "<<ld_manure_summer<<endl;

					ld_manure_summer=getRainDays(ld_manure_summer, rem[9], perc_days, cell, year, teta, temperature_cell, optionsWQ_load_ptr->manure_timing);

					//if(cell==94288|| cell==46562||cell==58694||cell==6540||cell==150925)
						//cout<<"after rain days ld_manure_summer: "<<ld_manure_summer<<endl;
					ld_manure_cell[cell] = ld_manure_summer;
				} // for(cell)

			} // end if(manure_timining==1)
		}
		insert_into_tab(year, "manure", ld_manure_cell
				, act_surface_runoff, act_surface_runoff_year, act_surface_runoff_summer
				, optionsWQ_load_ptr->manure_timing);

//		query.reset();
//		query <<"update "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load c1, "
//				<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_month_load_manure c "
//				<<"set c1.`ld_manure`= IFNULL(c.`v1`,0)+ IFNULL(c.`v2`,0)+ IFNULL(c.`v3`,0)+ IFNULL(c.`v4`,0)+ IFNULL(c.`v5`,0)+ IFNULL(c.`v6`,0)+ IFNULL(c.`v7`,0)+ IFNULL(c.`v8`,0)+ IFNULL(c.`v9`,0)+ IFNULL(c.`v10`,0)+ IFNULL(c.`v11`,0)+ IFNULL(c.`v12`,0) "
//				<<"WHERE c.`IDScen`="<<optionsWQ_load_ptr->IDScen<<" AND c.`parameter_id`="<<optionsWQ_load_ptr->parameter_id
//				<<" AND c.`time`="<<year<<" AND c1.`cell`=c.`cell` AND c1.`IDScen`=c.`IDScen` "
//				<<"AND c.`parameter_id`=c1.`parameter_id` AND c1.`time`=c.`time`;"<<endl;
//		//cout << "Query update calc cell load manure: " << query << endl << endl;
//		query.execute();

		} // if (BOD, TDS, FC, TN, TP)

	cout << "========================= scattered settlements =========================\n";
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_sc_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL,\n");
		j+=sprintf(sql+j, "  `v3` double default NULL,\n");
		j+=sprintf(sql+j, "  `v4` double default NULL,\n");
		j+=sprintf(sql+j, "  `v5` double default NULL,\n");
		j+=sprintf(sql+j, "  `v6` double default NULL,\n");
		j+=sprintf(sql+j, "  `v7` double default NULL,\n");
		j+=sprintf(sql+j, "  `v8` double default NULL,\n");
		j+=sprintf(sql+j, "  `v9` double default NULL,\n");
		j+=sprintf(sql+j, "  `v10` double default NULL,\n");
		j+=sprintf(sql+j, "  `v11` double default NULL,\n");
		j+=sprintf(sql+j, "  `v12` double default NULL,\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='ld scattered settl.(A treat; B JMP)[t/month],[10^10no/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}

	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND "; 
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND ";
	query << "`time`= " << year << ";";
	query.execute();

	if (toCalc==0||toCalc==1||toCalc==2||toCalc==3||toCalc==4) { // for BOD, TDS, FC, TN and TP
		insert_into_tab(year, "sc", ld_treat_sc_cell
				, act_surface_runoff, act_surface_runoff_year, act_surface_runoff_summer, 0);
	}

	cout<<"========================= industrial fertilizer  ========================"<<endl;
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_ind_fert_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v3` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v4` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v5` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v6` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v7` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v8` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v9` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v10` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v11` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v12` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='diff. sour. only ind. fert. cell ld [t/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}
	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND "; 
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND ";
	query << "`time`= " << year << ";";
	query.execute();

	if (toCalc==0||toCalc==3||toCalc==4) { //  only for BOD, TN and TP
		// ---------------------- Abflussabhaengig ---------------------
		insert_into_tab(year, "ind_fert", ld_inorg_fert_cell
				, act_surface_runoff, act_surface_runoff_year, act_surface_runoff_summer, 0);
	}

	cout<<"========================= open defecation  =============================="<<endl;
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_open_defec_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v3` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v4` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v5` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v6` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v7` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v8` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v9` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v10` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v11` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v12` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='diff.sour. only diff.sc. cell ld [t/mth],[10^10 cfu/mth]';\n");

		query.reset();
		query << sql;
		query.execute();
	}

	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND ";
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id << " AND ";
	query << "`time`= " << year << ";";
	query.execute();

	if (toCalc==0||toCalc==1||toCalc==2||toCalc==3||toCalc==4) { //  only for BOD, FC, TN and TP
		// ---------------------- Abflussabhaengig ---------------------

		if(toCalc==2){
			for (int cell=0; cell<continent.ng; cell++) {
				if(ld_diff_untr_sc_cell[cell] != NODATA)
					ld_diff_untr_sc_cell[cell] = getRainDays(ld_diff_untr_sc_cell[cell], rem[9], perc_days, cell, year,  teta, temperature_cell, 0);
			} // for(cell)
		}

		insert_into_tab(year, "open_defec", ld_diff_untr_sc_cell
				, act_surface_runoff, act_surface_runoff_year, act_surface_runoff_summer, 0);

//		query.reset();
//		query <<"update "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load c1, "
//			<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_month_load_open_defec c "
//			<<"set c1.`ld_open_defec`= IFNULL(c.`v1`,0)+ IFNULL(c.`v2`,0)+ IFNULL(c.`v3`,0)+ IFNULL(c.`v4`,0)+ IFNULL(c.`v5`,0)+ IFNULL(c.`v6`,0)+ IFNULL(c.`v7`,0)+ IFNULL(c.`v8`,0)+ IFNULL(c.`v9`,0)+ IFNULL(c.`v10`,0)+ IFNULL(c.`v11`,0)+ IFNULL(c.`v12`,0) "
//			<<"WHERE c.`IDScen`="<<optionsWQ_load_ptr->IDScen<<" AND c.`parameter_id`="<<optionsWQ_load_ptr->parameter_id
//			<<" AND c.`time`="<<year<<" AND c1.`cell`=c.`cell` AND c1.`IDScen`=c.`IDScen` "
//			<<"AND c.`parameter_id`=c1.`parameter_id` AND c1.`time`=c.`time`;"<<endl;
//		//cout << "Query update calc cell load diff sc: " << query << endl << endl;
//		query.execute();

	} // if (BOD, TC, TN, TP)

	cout<<"========================= geogenic background  =========================="<<endl;
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_geog_back_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v3` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v4` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v5` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v6` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v7` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v8` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v9` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v10` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v11` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v12` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='diff.sour. only geo.ba. cell ld [t/month],[10^10 cfu/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}

	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND ";
	//query<< "`parameter_id`="<< optionsWQ_load_ptr->parameter_id<< " AND ";
	query << "`time`= " << year << ";";
	query.execute();

	if (toCalc==0||toCalc==3||toCalc==4) { //  only for BOD,  TN and TP
		// ---------------------- Abflussabhaengig ---------------------
		insert_into_tab(year, "geog_back", ld_background_cell_year
				, act_surface_runoff, act_surface_runoff_year, act_surface_runoff_summer, 0);
	} // if (BOD, TN, TP)
	else if (toCalc==1) { // salt
		insert_into_tab(year, "geog_back", ld_background_cell);
	}

	cout<<"========================= hanging latrines =============================="<<endl;
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_hanging_l_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v2` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v3` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v4` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v5` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v6` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v7` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v8` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v9` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v10` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v11` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		j+=sprintf(sql+j, "  `v12` double default NULL COMMENT 't/mon; FC: 10^10cfu/mon',\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='hanging latrines [t/month], [10^10no/month]';\n");

		query.reset();
		query << sql;
		query.execute();
	}

	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`=" <<optionsWQ_load_ptr->IDScen<< " AND ";
	//query << "`parameter_id`="<< optionsWQ_load_ptr->parameter_id<< " AND ";
	query << "`time`= " << year << ";";
	//cout<<"delete from mix: "<<query<<endl;
	query.execute();

	// INSERT mit SELECT ausführen in yearValues() wenn Tabelle calc_cell_load mit Jahreswerten fertig ist
	// weil Monatswerte sind immer 1/12 vom Jahreswert

	return 0;
} // end of monthValues()
int ClassWQ_load::insert_into_tab(int year, const string tab, double *ld_value_year, double (*corr_factor)[12]) {
	short comma; int counter;
	counter=0;
	mysqlpp::Query query = con.query();

	char TableName[maxcharlength];
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_%s_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb, tab.c_str()
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);

	cout << "Test1 from insert_into_tab: "<< optionsWQ_load_ptr->IDScen << ", " << optionsWQ_load_ptr->parameter_id<< ", TableName: "<< TableName << endl;

	query.reset();
	query << "INSERT INTO " << TableName << " (" << "`cell`, `time`, "
	 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) VALUES "; comma = 0;

	for (int cell=0; cell<continent.ng; cell++) {
		if (comma) query << ", ";
		else comma = 1;

		query << "(";
		//query <<optionsWQ_load_ptr->IDScen<<", "<<optionsWQ_load_ptr->parameter_id<<", ";
		query <<cell+1<<", "<<year<<", ";
		if(ld_value_year[cell]==NODATA){
			query <<"NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)";
		} else {
			if (corr_factor != NULL) {
				double corr_ld_value_year = 0.;
				for  (int month=0; month<12; month++) {
					query <<(ld_value_year[cell]/12)*corr_factor[cell][month];
					corr_ld_value_year += (ld_value_year[cell]/12)*corr_factor[cell][month];
					if (month<11) query << ", "; else query << ")";
				} // for(month)
				ld_value_year[cell] = corr_ld_value_year; // korrigierte Jahreswert
			} else {
				for  (int month=0; month<12; month++) {
					query <<(ld_value_year[cell]/12);
					if (month<11) query << ", "; else query << ")";
				} // for(month)
			}
		}

		counter++;

		#ifdef DEBUG_queries
		if (cell==0||cell==660||cell==467||cell==129627||cell==129628)
			cout << "Query: " << query << endl << endl; //cin >> j;
		#endif

		if (counter==10) {
			counter=0;
			query << ";";

			#ifdef DEBUG_queries
			if (cell==0||cell==467||cell==129627||cell==129628 || cell == 371404 || cell == 371108 || cell == 371086)
				cout << "Query: " << query << endl << endl; //cin >> j;
			#endif

			query.execute();

			query.reset();
			query << "INSERT INTO " << TableName << " (";
			//query << "`IDScen`, "<< "`parameter_id`, " ;
			query << "`cell`, `time`, "
			 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) VALUES "; comma = 0;

		}

	} // for(cell)

	if (counter>0) {
		//#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		//#endif

		query << ";";
		query.execute();
	}
	return 0;
} // end of insert_into_tab() with yearly loading and corr_factor

int ClassWQ_load::insert_into_tab(int year, const string tab, double (*ld_value)[12]) {
	short comma; int counter;
	counter=0;
	mysqlpp::Query query = con.query();

	char TableName[maxcharlength];
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_%s_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb, tab.c_str()
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);

	cout << "Test2 from insert_into_tab: "<< optionsWQ_load_ptr->IDScen << ", " << optionsWQ_load_ptr->parameter_id << ", TableName: "<< TableName << endl;

	query.reset();
	query << "INSERT INTO "<< TableName<< " (";
	//query << " `IDScen`, `parameter_id`, ";
	
	query << "`cell`, `time`, "
	 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) VALUES "; comma = 0;

	for (int cell=0; cell<continent.ng; cell++) {
		if (comma) query << ", ";
		else comma = 1;

		//query << "("<<optionsWQ_load_ptr->IDScen<<", "<<optionsWQ_load_ptr->parameter_id<<", "; 
		query << "(";
		query <<cell+1<<", "<<year<<", ";

		for  (int month=0; month<12; month++) {
			if (ld_value[cell][month]==NODATA ) query << "NULL";
			else query << ld_value[cell][month];
			if (month<11) query << ", "; else query << ")";
		} // for(month)

		counter++;

		#ifdef DEBUG_queries
		if (cell==0||cell==467||cell==129627||cell==129628)
			cout << "Query: " << query << endl << endl; //cin >> j;
		#endif

		if (counter==10) {
			counter=0;
			query << ";";

			#ifdef DEBUG_queries
			if (cell==0||cell==467||cell==129627||cell==129628 || cell == 371404 || cell == 371108 || cell == 371086)
				cout << "Query: " << query << endl << endl; //cin >> j;
			#endif

			query.execute();

			query.reset();
			query << "INSERT INTO "<< TableName<< " (";
			//query << "`IDScen`, `parameter_id`, ";
			
			query << "`cell`, `time`, "
			 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) VALUES "; comma = 0;
		}

	} // for(cell)

	if (counter>0) {
		#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		#endif

		query << ";";
		query.execute();
	}
	return 0;
} // end of insert_into_tab() with monthly loading

int ClassWQ_load::insert_into_tab(int year, const string tab, double *ld_value_year
		, double (*act_surface_runoff)[12], double *act_surface_runoff_year, double *act_surface_runoff_summer,
		// in der Funktion keine manure_timing aus options verwenden, weil diese Funktion wird nicht nur f�r manure verwendet(sc, ind_fert)
		short manure_timining) {

	char TableName[maxcharlength];
	sprintf(TableName, "%swq_load_%s.`calc_cell_month_load_%s_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb, tab.c_str()
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);

	cout << "Test3 from insert_into_tab: "<< optionsWQ_load_ptr->IDScen << ", " << optionsWQ_load_ptr->parameter_id<< ", TableName: "<< TableName << endl;

	short comma; int counter;
	counter=0;
	mysqlpp::Query query = con.query();

	query.reset();
	query << "INSERT INTO "<<TableName<< " ("; 
	//query << " (`IDScen`, `parameter_id`, "; 
	query << "`cell`, `time`, "
	 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) VALUES "; comma = 0;

	for (int cell=0; cell<continent.ng; cell++) {
		if (comma) query << ", ";
		else comma = 1;

		//query << "("<<optionsWQ_load_ptr->IDScen<<", "<< optionsWQ_load_ptr->parameter_id<<", ";
		query << "(";
		query <<cell+1<<", "<<year<<", ";

		if (act_surface_runoff_year[cell]==NODATA || act_surface_runoff_year[cell]==0.0 || ld_value_year[cell]==-9999.0) {
			query << "NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)";
			ld_value_year[cell]=-9999.0;
		} else {
			double corr_ld_value_year = 0.;
			for  (int month=0; month<12; month++) {
				if(manure_timining==0) { //if manure is applied throughout the whole year
					if (act_surface_runoff[cell][month]>0.0 && act_surface_runoff_year[cell]>0.0){
						query << ld_value_year[cell] * act_surface_runoff[cell][month] / act_surface_runoff_year[cell];
						corr_ld_value_year += ld_value_year[cell] * act_surface_runoff[cell][month] / act_surface_runoff_year[cell];
				  		/*if(cell==75617||cell==76839){
				  				cout <<" ld_manure_cell[cell]: "<< ld_manure_cell[cell]<<" act_surface_runoff[cell][month]: "<<act_surface_runoff[cell][month]<<" month: "<<month<<" act_surface_runoff_year[cell]: "<<act_surface_runoff_year[cell]<<" cell: "<<cell<<endl;
				  				//cout << "Query: " << query <<" cell:"<<cell+1<<endl;
				  		}*/
					} else query << 0.0;
				} else if (manure_timining==1) { //if manure is applied from March to October
					if((month>1) && (month <10)) {
						if (act_surface_runoff[cell][month]>0.0 && act_surface_runoff_summer[cell]>0.0 && ld_value_year[cell]!=-9999.0){
							query << ld_value_year[cell] * act_surface_runoff[cell][month] / act_surface_runoff_summer[cell];
							corr_ld_value_year += ld_value_year[cell] * act_surface_runoff[cell][month] / act_surface_runoff_summer[cell];
							/*if(cell==75617||cell==76839){
								cout <<" ld_manure_cell[cell]: "<< ld_manure_cell[cell]<<" act_surface_runoff[cell][month]: "<<act_surface_runoff[cell][month]<<" month: "<<month<<" act_surface_runoff_summer[cell]: "<<act_surface_runoff_summer[cell]<<" cell: "<<cell<<endl;
								//cout << "Query: " << query <<" cell:"<<cell+1<<endl;
							}*/
						} else query << 0.0;
					} else query << 0.0;
				}
				if (month<11) query << ", "; else query << ")";
			} // for(month)
			ld_value_year[cell] = corr_ld_value_year; // korrigierter Jahreswert
		}

		counter++;

		#ifdef DEBUG_queries
		if (cell==0||cell==467||cell==129627||cell==129628 || cell == 371404 || cell == 371108 || cell == 371086)
			cout << "Query: " << query << endl << endl; //cin >> j;
		#endif

		if (counter==10) {
			counter=0;
			query << ";";

			#ifdef DEBUG_queries
			if (cell==0||cell==467||cell==129627||cell==129628)
				cout << "Query: " << query << endl << endl; //cin >> j;
			#endif

			query.execute();

			query.reset();
			query << "INSERT INTO "<<TableName<< " ("; 
			//query << "`IDScen`, `parameter_id`, "; 
			query << "`cell`, `time`, "
			 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) VALUES "; comma = 0;
		}

	} // for(cell)

	if (counter>0) {
		//#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		//#endif

		query << ";";
		query.execute();
	}
	return 0;
} // end of insert_into_tab() with yearly loading and act_surface_runoff

// insert year values
int ClassWQ_load::yearValues() {

	char TableName[maxcharlength];
	char sql[10000]; // variable for SQL-query
	int j;
	mysqlpp::Query query = con.query();

	sprintf(TableName, "%swq_load_%s.`calc_cell_load_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
		, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
	if (!isTableExists(TableName)) {
		j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `ld_untr_dom` double default NULL COMMENT 'untreated domestic cell load [t/a], [10^10cfu/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_dom` double default NULL COMMENT 'treated domestic cell load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldd_treat_man` double default NULL COMMENT 'treated direct manufacturing cell load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man` double default NULL COMMENT 'untreated indirect manufacturing cell load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_treat_man` double default NULL COMMENT 'treated indirect manufacturing cell load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_man` double default NULL COMMENT 'treated total manufacturing cell load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_untr_mining` double default NULL COMMENT 'untreated total mining cell load [t/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_mining` double default NULL COMMENT 'treated total mining cell load [t/a]',\n");
		j+=sprintf(sql+j, "  `ld_untr_sc` double default NULL COMMENT 'untreated load from scattered settlements [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_sc` double default NULL COMMENT 'treated load from scattered settlements A) if no sub-sectors: treated sc B) if divided in sub-sectors than: septic tanks, pit latrines, unknown etc.  [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_open_defec` double default NULL COMMENT 'load from open defecation, thus diffuse scattered settlements ',\n");
		j+=sprintf(sql+j, "  `ld_irr` double default NULL COMMENT 'load from irrigation return flow [10^10t/a]',\n");
		j+=sprintf(sql+j, "  `ld_untr_urb` double default NULL COMMENT 'untreated urban country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_urb` double default NULL COMMENT 'treated urban country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_manure` double default NULL COMMENT 'load from manure  [t/a], FC: [10^10 cfu/a]',\n");
		j+=sprintf(sql+j, "  `ld_inorg_fert` double default NULL COMMENT 'load from inorganic fertilizer [t/a], no FCs in inorganic fertilizer',\n");
		j+=sprintf(sql+j, "  `ld_hanging_l` double default NULL COMMENT 'load from hanging latrines [t/a], FC: [10^10 cfu/a]',\n");
		j+=sprintf(sql+j, "  `ld_background` double default NULL COMMENT 'load from geogenic background [t/a], FC: [10^10 cfu/a]',\n");
		//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`cell`)\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci;\n");

		query.reset();
		query << sql;
		query.execute();
	}
	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE ";
	//query << "`IDScen`= " << optionsWQ_load_ptr->IDScen<< " AND ";
	//query << "`parameter_id`= " << optionsWQ_load_ptr->parameter_id<<" AND ";
	query << "`time`= " << year << ";";
	query.execute();

	if (toCalc==1) { // calc_cell_info nur fuer Salz
		sprintf(TableName, "%swq_load_%s.`calc_cell_info_%d_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb
			, optionsWQ_load_ptr->IDScen, optionsWQ_load_ptr->parameter_id);
		if (!isTableExists(TableName)) {
			j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
			//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1' COMMENT 'scenario',\n");
			j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
			j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
			j+=sprintf(sql+j, "  `Salt_class` int(10) unsigned default NULL COMMENT 'natural salt class 1 - 4 : low to high salinity',\n");
			j+=sprintf(sql+j, "  `GDPC` int(10) unsigned default NULL COMMENT 'gdp class: 1 - 3 , low to high gdp per capita',\n");
			j+=sprintf(sql+j, "  `SEPC` int(10) unsigned default NULL COMMENT 'salt concentration in return flow (mg/l) derived from salt_class and gdp class',\n");
			j+=sprintf(sql+j, "  `salt_geogen` int(10) unsigned default NULL COMMENT 'salt background yes = 1, no = 0',\n");
			j+=sprintf(sql+j, "  `land_cell_area` double default NULL COMMENT 'cell area of land surface [km2] - cell_area*(100-frac[glo_lak+glo_wet+loc_lak+loc_wet]) ; with cell area from cell_input',\n");
			//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`time`,`cell`)\n");
			j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`time`,`cell`)\n");
			j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci ROW_FORMAT=FIXED;\n");

			query.reset();
			query << sql;
			query.execute();
		}

		query.reset();
		query << "DELETE FROM "<<TableName<<" WHERE ";
		//query << "`IDScen`= " << optionsWQ_load_ptr->IDScen<<" AND ";
		query <<"`time`= " << year << ";";
		query.execute();
	}

	for (int cell=0; cell<continent.ng; cell++) {

		// #############  INSERT INTO `calc_cell_load`
		query.reset();
		query << setprecision(30);
		//query << "insert into "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb <<".calc_cell_load"
		//	<<" (`IDScen`, `parameter_id`, `cell`, `time`, ";
		query << "insert into "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load_"
			<< optionsWQ_load_ptr->IDScen << "_" << optionsWQ_load_ptr->parameter_id
			<< "(`cell`, `time`, ";
		query << "`ld_untr_dom`, `ld_treat_dom`, `ldd_treat_man`, `ldi_untr_man`, `ldi_treat_man`, `ld_treat_man`, "
		  << "`ld_untr_mining`, `ld_treat_mining`, "
		  << "`ld_untr_sc`, `ld_treat_sc`, `ld_open_defec`, `ld_irr`, `ld_untr_urb`, `ld_treat_urb`, "
		  << "`ld_hanging_l`, `ld_manure`, `ld_inorg_fert`, `ld_background`) "
		  << "values ( "; 
			//query << optionsWQ_load_ptr->IDScen << ", "; 
			//query << optionsWQ_load_ptr->parameter_id << ", "
		query << cell+1 << ", " << year << ", ";

		if (ld_untr_dom_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_untr_dom_cell[cell]<<", ";

		if (ld_treat_dom_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_treat_dom_cell[cell]<<", ";

		if (ldd_treat_man_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ldd_treat_man_cell[cell]<<", ";

		if (ldi_untr_man_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ldi_untr_man_cell[cell]<<", ";

		if (ldi_treat_man_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ldi_treat_man_cell[cell]<<", ";

		if (ld_treat_man_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_treat_man_cell[cell]<<", ";

		if (ld_untr_mining_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_untr_mining_cell[cell]<<", ";

		if (ld_treat_mining_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_treat_mining_cell[cell]<<", ";

		if (ld_untr_sc_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_untr_sc_cell[cell]<<", ";

		if (ld_treat_sc_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_treat_sc_cell[cell]<<", ";

		if (ld_diff_untr_sc_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_diff_untr_sc_cell[cell]<<", ";

		if (ld_agr_cell_year[cell]==NODATA) query <<"NULL, ";
		else query <<ld_agr_cell_year[cell]<<", ";

		if (ld_untr_urb_cell_year[cell]==NODATA) query <<"NULL, ";
		else query <<ld_untr_urb_cell_year[cell]<<", ";

		if (ld_treat_urb_cell_year[cell]==NODATA) query <<"NULL, ";
		else query <<ld_treat_urb_cell_year[cell]<<", ";

		if (ld_hanging_l_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_hanging_l_cell[cell]<<", ";

		if (ld_manure_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_manure_cell[cell]<<", ";

		if (ld_inorg_fert_cell[cell]==NODATA) query <<"NULL, ";
		else query <<ld_inorg_fert_cell[cell]<<", ";

		if (ld_background_cell_year[cell]==NODATA) query <<"NULL); ";
		else query <<ld_background_cell_year[cell]<<");";


		//#ifdef DEBUG_queries
		if (cell==0||cell==467||cell==129627||cell==129628||cell==43048)
		  cout << "Query insert into calc_cell_load: " << query << endl << endl; //cin >> j;
		//#endif

		query.execute();


		// #############  INSERT INTO `calc_cell_info`
		if (toCalc==1) { // calc_cell_info nur f�r Salz
			query.reset();
			query << setprecision(30);
			query << "insert into "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_info_" 
				<< optionsWQ_load_ptr->IDScen << "_" << optionsWQ_load_ptr->parameter_id<<" "
				<<"(`cell`, `time`, `Salt_class`, `GDPC`, `SEPC`, salt_geogen, land_cell_area) "
				<< " values ("<< cell+1 << ", " << year << ", ";

			if (Salt_class[cell]==NODATA) query <<"NULL, ";
			else query <<Salt_class[cell]<<", ";

			if (GDPC[cell]==NODATA) query <<"NULL, ";
			else query <<GDPC[cell]<<", ";

			if (SEPC[cell]==NODATA) query <<"NULL, ";
			else query <<SEPC[cell]<<", ";

			if (rtf_irr_cell[cell]==NODATA||rtf_irr_cell[cell]==0) query <<"NULL, ";
			else query <<" 0, ";

			if (cell_area[cell]==NODATA) query <<"NULL; ";
			else query <<cell_area[cell]<<");";

			#ifdef DEBUG_queries
			if (cell==0||cell==467||cell==129627||cell==129628)
			  cout << "Query: " << query << endl << endl; //cin >> j;
			#endif
			query.execute();
		}

	} // for(cell) output

	// Länderwerte korrigieren
	char TableName_cellInput[maxcharlength];
	if (!optionsWQ_load_ptr->IDInTableName) {
		sprintf(TableName_cellInput, "%swq_load_%s.`cell_input`", MyDatabase, optionsWQ_load_ptr->continent_abb);
	} else {
		sprintf(TableName_cellInput, "%swq_load_%s.`cell_input_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb, optionsWQ_load_ptr->IDScen);
	}
	query.reset();
	if (strcmp(optionsWQ_load_ptr->continent_abb,"as")==0) { // this conditional is only for Asia since the join-statement triggers error due to memory limits (instead of the "cell_input" table I use "g_country" table)
		query <<"update "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_country_load c2 "
			<<"join (SELECT c.`country_id` "
			<<", sum(IFNULL(c1.`ld_treat_dom`,0)) as ld_treat_dom "
			<<", sum(IFNULL(c1.`ld_treat_man`,0)) as ld_treat_man "
			<<", sum(IFNULL(c1.`ld_treat_mining`,0)) as ld_treat_mining "
			<<", sum(IFNULL(c1.`ld_manure`,0)) as ld_manure "
			<<", sum(IFNULL(c1.`ld_open_defec`,0)) as ld_open_defec "
			//<< ", c1.`IDScen`, c1.`parameter_id`"
			<<", " << optionsWQ_load_ptr->IDScen << ", " << optionsWQ_load_ptr->parameter_id
			<<", c1.`time` "
			<<"FROM "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".g_country c, "
			<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load_"
			<< optionsWQ_load_ptr->IDScen << "_" << optionsWQ_load_ptr->parameter_id<<" c1 "
			<<"WHERE " //c1.`IDScen`="<<optionsWQ_load_ptr->IDScen<<" AND c1.`parameter_id`="<<optionsWQ_load_ptr->parameter_id << " AND"
			<<" c1.`time`="<<year<<" AND c1.`cell`=c.`cell` "
			<<" GROUP BY c.`country_id`) as sub "
			<<"ON c2.`country_id`= sub.`country_id` "
			<<"SET c2.`ld_treat_dom`=sub.`ld_treat_dom`"
			<<", c2.`ld_treat_man`=sub.`ld_treat_man` "
			<<", c2.`ld_treat_mining`=sub.`ld_treat_mining`"
			<<", c2.`ld_manure`=sub.`ld_manure`"
			<<", c2.`ld_open_defec`=sub.`ld_open_defec` "
			<<"WHERE c2.`IDScen`="<< optionsWQ_load_ptr->IDScen << " AND c2.`parameter_id`=" << optionsWQ_load_ptr->parameter_id << " AND "
			<<"c2.`time`=sub.`time`;";
	}	else {
		query <<"update "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_country_load c2 "
			<<"join (SELECT c.`country_id` "
			<<", sum(IFNULL(c1.`ld_treat_dom`,0)) as ld_treat_dom "
			<<", sum(IFNULL(c1.`ld_treat_man`,0)) as ld_treat_man "
			<<", sum(IFNULL(c1.`ld_treat_mining`,0)) as ld_treat_mining "
			<<", sum(IFNULL(c1.`ld_manure`,0)) as ld_manure "
			<<", sum(IFNULL(c1.`ld_open_defec`,0)) as ld_open_defec "
			<<", " << optionsWQ_load_ptr->IDScen << ", " << optionsWQ_load_ptr->parameter_id
			<< ", c1.`time` "
			<<"FROM "<<TableName_cellInput<<" c, "
			<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load_"
			<< optionsWQ_load_ptr->IDScen << "_" << optionsWQ_load_ptr->parameter_id<<" c1 "
			<<"WHERE c.`IDScen`="<<optionsWQ_load_ptr->IDScen <<" AND " //c.`parameter_id`="<<optionsWQ_load_ptr->parameter_id << " AND "
			<<" c1.`time`="<<year<<" AND c1.`cell`=c.`cell` "  //<<"AND c.`IDScen`=c1.`IDScen` "
			<<"AND c.`time`=c1.`time` GROUP BY c.`country_id`) as sub "
			<<"ON c2.`country_id`= sub.`country_id` "
			<<"SET c2.`ld_treat_dom`=sub.`ld_treat_dom`"
			<<", c2.`ld_treat_man`=sub.`ld_treat_man`"
			<<", c2.`ld_treat_mining`=sub.`ld_treat_mining`"
			<<", c2.`ld_manure`=sub.`ld_manure`"
			<<", c2.`ld_open_defec`=sub.`ld_open_defec` "
			<<"WHERE c2.`IDScen`="<< optionsWQ_load_ptr->IDScen << " AND c2.`parameter_id`=" << optionsWQ_load_ptr->parameter_id << "  AND "
			<<"c2.`time`=sub.`time`;";
	}
	cout << "Query update calc country load treat dom: " << query << endl << endl;
	query.execute();

	// Monatswerte hier, weil nur 1/12 vom Jahreswert
	query.reset();
	query<<"INSERT INTO "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_month_load_hanging_l_"
		<< optionsWQ_load_ptr->IDScen << "_" << optionsWQ_load_ptr->parameter_id<<" ("
		//<<"`IDScen`, `parameter_id`, 
		<<"`cell`, `time`, "
		<<"`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) "
		<<"SELECT " //"c.`IDScen`, c.`parameter_id`," 
		<<"c.`cell`, c.`time` , "
		<<"c.`ld_hanging_l`/12 as v1, c.`ld_hanging_l`/12 as v2, c.`ld_hanging_l`/12 as v3, c.`ld_hanging_l`/12 as v4, "
		<<"c.`ld_hanging_l`/12 as v5, c.`ld_hanging_l`/12 as v6, c.`ld_hanging_l`/12 as v7, c.`ld_hanging_l`/12 as v8, "
		<<"c.`ld_hanging_l`/12 as v9, c.`ld_hanging_l`/12 as v10, c.`ld_hanging_l`/12 as v11 , c.`ld_hanging_l`/12 as v12 "
		<< "FROM "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_cell_load_"
		<< optionsWQ_load_ptr->IDScen << "_" << optionsWQ_load_ptr->parameter_id<<" c "
		<<"WHERE " //c.`IDScen`="<<optionsWQ_load_ptr->IDScen<<" AND c.`parameter_id`="<<optionsWQ_load_ptr->parameter_id << " AND "
		<<" c.`time`="<<year<<" ;";

	//if(cell==58694||cell==94288)
	//	cout << "Query mix: " << query <<" cell:"<<cell<<endl;
	query.execute();

	return 0;
} // yearValues()

double ClassWQ_load::getRainDays(double ld_init, double ks, char (*perc_days)[12], int cell, int year
		,  const double teta, short (*temperature_cell)[12], short flag){
	double ld_dec; // return value
	double t_rain=0.0;
	double temp_corr;
	double t_rain_mean=0.0;
	double temp_mean=0.0;
	int month;
	double nDaysPerMonth[12] = { 31., 28., 31., 30., 31., 30., 31., 31., 30., 31., 30., 31. };
	// wenn Schaltjahre sollen beruecksichtigt werden
	//if(year%4!=0){
	//	nDaysPerMonth[1]=28.0;
	//}else if((year%100==0) && (year%400!=0)){
	//		nDaysPerMonth[1]=28.0;
	//}else{
	//		nDaysPerMonth[1]=29.0;
	//}

	int start=0, end = 12;
	if (flag==1) { //manure
		start = 2; end = 10;
	}
	for(month=start; month<end; month++){
		//if(cell==94288|| cell==46562||cell==58694||cell==6540||cell==150925||cell==81515)
		//	cout<<"flag: "<<flag<<" month: "<<month<<endl;
		if((perc_days[cell][month]!=0)){
			if(perc_days[cell][month]==NODATA){
				cerr<<"Error in perc_days. NODATA on cell: "<<cell<<" month: "<<month<<endl;
			}else{
				t_rain=nDaysPerMonth[month]/(int)perc_days[cell][month];
				t_rain_mean+=t_rain;

				/*if(cell==94288|| cell==46562||cell==58694||cell==6540||cell==150925||cell==81515)
						cout<<"(int)perc_days[cell][month]: "<<(int)perc_days[cell][month]
						    <<" t_rain: "<<t_rain<<" temp_mean: "<<temp_mean
						    <<" temperature_cell[cell][month]: "<<temperature_cell[cell][month]/100
						    <<" cell: "<<cell<<" month: "<<month<<" t_rain_mean: "<<t_rain_mean<<endl;
				*/
			}
		}

		temp_mean+=temperature_cell[cell][month]/100;

	}//for month

	if(flag==0){	//diff sc
		t_rain_mean=t_rain_mean/12;
		temp_mean=temp_mean/12;
	}
	else{	//manure
		t_rain_mean=t_rain_mean/8;
		temp_mean=temp_mean/8;
	}

	temp_corr=ks*t_rain_mean*pow(teta, temp_mean-20.0);
	ld_dec=ld_init*exp(-temp_corr);	//it rains in a mean every t days

//	if(cell==94288|| cell==46562||cell==58694||cell==6540||cell==150925||cell==81515) {
//		cout<<"temp_corr: "<<temp_corr<<" ks: "<<ks<<" t_rain: "<<t_rain
//		<<"\npow(teta, temp_mean-20.0): "<<pow(teta, temp_mean-20.0)
//		<<" temp_mean: "<<temp_mean<<" teta: "<<teta
//		<<"\nld_dec: "<<ld_dec<<" ld_init: "<<ld_init<<" exp(-temp_corr): "<<exp(-temp_corr)
//		<<"\ncell: "<<cell
//		<<" t_rain_mean after : "<<t_rain_mean<<" temp_mean after: "<<temp_mean<<endl;
//	}else{
//		ld_dec=0.0;
//		//cout<<"if act surface runoff < 10 ld_dec: "<<ld_dec<<endl;
//	}

//	if(cell==75617||cell==76839||cell==79742||cell==150925||cell==81515){
//		cout<<"\n\ngetRainDays: ks: "<<ks<<" t_rain: "<<t_rain
//			<<"\n(int)perc_days[cell][month]: "<<(int)perc_days[cell][month]
//			<<"\nmonth: "<<month<<" cell: "<<cell
//			<<"\npow(10,-ks*t_rain): "<<pow(10,-ks*t_rain)
//			<<"\nld_dec: "<<ld_dec<<endl;
//	}

	return ld_dec;
} // getRainDays()


void ClassWQ_load::CountryLoad() {

	for (int country=0; country<countries->number; country++) {
		//cout <<setprecision(10)<< country << '\t' << countries->get_ISONUM(country)<< '\t'<< pop[country][0]<< '\t' <<pop[country][1]<< '\t' <<pop[country][2]<< '\t' <<endl;

		//cout<<"connec_treat_unknown[country]= "<<connec_treat_unknown[country]
		//	  <<" hanging_lat[country]= "<<hanging_lat[country]<<" open_defec[country]= "<<open_defec[country]<<endl;

		//========================================
		// initialization of country-values for variables which is calculated as sum of cells
		// irrigation and mining
		// only calculated for salt (toCalc==1) or pesticide (toCalc==5)
		if (toCalc==1) {
			ld_irr[country] = 0.0;
			ld_agr[country] = 0.0;

			ld_treat_mining[country] = 0.;
			ld_untr_mining[country]  = 0.;
		} else {
			ld_irr[country]           = NODATA;
			ld_agr[country]           = NODATA;

			ld_treat_mining[country] = NODATA;
			ld_untr_mining[country]  = NODATA;
		}

		//urban
		ld_untr_urb[country]  = 0.0;
		ld_treat_urb[country] = 0.0;

		//diffuse sources
		ld_manure[country]  	= 0.0;
		ld_geog_back[country]  	= 0.0;
		ld_ind_fert[country]  	= 0.0;

		//========================================
		// domestic
		// calc_country_connectivity.con_tot
		calc_con[country][0]=0.0;
		for (int i=0; i<5; i++) {
			if (connectivity[country][i]==NODATA) {calc_con[country][0]=NODATA; break;}
			calc_con[country][0]+=connectivity[country][i];
		}

		// calc_country_connectivity.con_rur, .con_urb
		if (pop[country][0]==NODATA||pop[country][1]==NODATA||pop[country][2]==NODATA||calc_con[country][0]==NODATA) {
			calc_con[country][1] = NODATA;
			calc_con[country][2] = NODATA;
		} else if (pop[country][0]==0) {
			calc_con[country][1] = 0.;
			calc_con[country][2] = 0.; 
		} else if (pop[country][1]==0) {
			calc_con[country][1] = 0.;
			calc_con[country][2]=sewerConn[country][1];
		} else if (pop[country][2]==0) {
			calc_con[country][2] = 0.;
			calc_con[country][1]=sewerConn[country][0];
		} else { //data from JMP used or Equation from Williams if data is not available (already included in the values of sewerConn)
			calc_con[country][2]=sewerConn[country][1];
			calc_con[country][1]=sewerConn[country][0];
		}
		
					
//		cout<<"country, urbconn, rurconn = " << countries->get_ISONUM(country) <<" , "<< sewerConn[country][0] <<" , "<< sewerConn[country][1] << endl;
		
//          TODO:(here is nothing to do this is just an explanation)
//          wenn urbane population kleiner als connected population, wird hier calc_con[country][2] (rural connected) positiv
//          => die gesamte urban population connected calc_con[country][1]=100.0
//          sonst ist nur ein Teil urbane population angeschlossen und keine rural (calc_con[country][2] = 0.0)
//          Es gaebe noch die Moeglichkeit die richtige Verteilung urbane und rurale Befoelkerung anzuschliessen
//           (es gibt fur einige Laender Daten). Das bringt aber nichts, weil es nur als "total connected population" benutzt wird
//
//		if (calc_con[country][2]>0.0) calc_con[country][1]=100.0; //paper Williams: Eq 2 1. if condition urban
//		else {
//			calc_con[country][2] = 0.0;
//			if (calc_con[country][0]==NODATA) calc_con[country][1]=NODATA;
//			else if (pop[country][1]==0) calc_con[country][1]=0.0;
			//paper Williams: Eq 2 2. if condition urban
//			else calc_con[country][1] = pop[country][0]* calc_con[country][0]/pop[country][1];
//		}

		if (toCalc==5) {
			ld_untr_dom[country] = NODATA;
		} else {
			//   =========== BOD, Salz, TN, TP, FC ==========
			if (ef[country]==NODATA || pop[country][1]==NODATA || pop[country][2] == NODATA
				||calc_con[country][1]==NODATA||calc_con[country][2]==NODATA)
				ld_untr_dom[country] = NODATA;
			else{  //Paper Williams 2011: Eq 1 Fracht in einem Land f�r eine bestimmte Substanz
				ld_untr_dom[country] = ef[country] *
				 (
				 	pop[country][1]*calc_con[country][1]/100.0 +
				 	pop[country][2]*calc_con[country][2]/100.0
				 );
				if(toCalc!=2){
					ld_untr_dom[country]/=1000.0;	//[t/a]
				}
			}
		}


		// TODO:(nothing to do, just a different way to calculate this)
		//  pop[country][1]*calc_con[country][1]/100.0 +
		//     pop[country][2]*calc_con[country][2]/100.0) = pop[country][0]*calc_con[country][0]/100
		//     = connected population
		//  warum dann auf rural und urban teilen?? waere auch mit total population gleicher Ergebniss

		// fraction
		if (calc_con[country][0]==NODATA || calc_con[country][0]==0)
		{
			frac_dom[country][0] = NODATA;
			frac_dom[country][1] = NODATA;
			frac_dom[country][2] = NODATA;
			frac_dom[country][3] = NODATA;
			frac_dom[country][4] = NODATA;
		}
		else { //calculation of treatments: no treatment primary, secondary, tertiary, quaternary connectivity [%]
			frac_dom[country][0]=connectivity[country][0]*100.0/calc_con[country][0];
			frac_dom[country][1]=connectivity[country][1]*100.0/calc_con[country][0];
			frac_dom[country][2]=connectivity[country][2]*100.0/calc_con[country][0];
			frac_dom[country][3]=connectivity[country][3]*100.0/calc_con[country][0];
			frac_dom[country][4]=connectivity[country][4]*100.0/calc_con[country][0];
		}


		if (ld_untr_dom[country]==NODATA ||
			frac_dom[country][0]==NODATA ||
			frac_dom[country][1]==NODATA ||
			frac_dom[country][2]==NODATA ||
			frac_dom[country][3]==NODATA ||
			frac_dom[country][4]==NODATA)

			ld_treat_dom[country]= NODATA;
		else {
			// paper Williams 2011: Eq. 4 calculation of effluent load

			//if the failure rate of STPs is known (usually developing countries)
			//than the removal rate is reduced
			if(stp_failure[country]!=NODATA){

				ld_treat_dom[country] = ld_untr_dom[country] *
					(
					 frac_dom[country][0]*(100.0 - rem[0])+
					 frac_dom[country][1]*(100.0 - rem[1]*stp_failure[country]/100.0)+
					 frac_dom[country][2]*(100.0 - rem[2]*stp_failure[country]/100.0)+
					 frac_dom[country][3]*(100.0 - rem[3]*stp_failure[country]/100.0)+
					 frac_dom[country][4]*(100.0 - rem[4]*stp_failure[country]/100.0)
					)/10000.0;

			}else
				//if the failure rate of STPs is unknown or very low (usually developed countries)
				ld_treat_dom[country] = ld_untr_dom[country] *
					(
					 frac_dom[country][0]*(100.0 - rem[0])+
					 frac_dom[country][1]*(100.0 - rem[1])+
					 frac_dom[country][2]*(100.0 - rem[2])+
					 frac_dom[country][3]*(100.0 - rem[3])+
					 frac_dom[country][4]*(100.0 - rem[4])
					)/10000.0;

		//cout<<"ld_untr_dom[country]: "<<ld_untr_dom[country]<<"\nrem[1]= "<<rem[1]<<"\nrem[0]= "
		//<<rem[0]<<"\nrem[2]= "<<rem[2]<<" frac_dom[country][1]= "<<frac_dom[country][1]
		//<<"\nfrac_dom[country][2]= "<<frac_dom[country][2]<<" frac_dom[country][0]= "
		//<<frac_dom[country][0]<<" frac_dom[country][3]= "<<frac_dom[country][3]<<" country= "
		//<<country<<" stp_failure[country]= "<<stp_failure[country]<<"\nld_treat_dom[country]= "
		//<<ld_treat_dom[country]<<endl;

		if(country==(countries->number-1))
			cout<<"domestic done"<<endl;
		}

		//========================================
		// manufacturing
		ldd_treat_man[country] = 0.0;


		ldi_untr_man[country] = 0.0;
		for (int man_fra=0; man_fra<7; man_fra++) {
			if (conc_man[country][man_fra]==NODATA || rtf_man[country]==NODATA || rtf_man_fra[country][man_fra]==NODATA) {
				ldi_untr_man_fra[country][man_fra]=NODATA;
				ldi_untr_man[country] = NODATA;
			}
			else {
				// FC       : [no/100ml] * [m3/a] * [] = [no/a * 10 000] = [10^10 no/a  / 1 000 000]
				// BOD, Salz: [mg/l] * [m3/a] * []     = [t/a / 1 000 000]
				ldi_untr_man_fra[country][man_fra] = conc_man[country][man_fra] *
					rtf_man[country] * rtf_man_fra[country][man_fra];

				/*
				if (toCalc==2)    //=========== Total coliforms ==========
					ldi_untr_man_fra[country][man_fra]*=10000.0;        // [no/a]
				else             //=========== BOD, Salz, TN, TP ==========
					ldi_untr_man_fra[country][man_fra]/=1000000.0;      // [t/a]
				*/

				if(toCalc==2)
					ldi_untr_man_fra[country][man_fra]*=1000.0;      // FC: 10^10 cfu/a - conc_man was in 10^10 cfu/L and L to m^3 was required
				else
					ldi_untr_man_fra[country][man_fra]/=1000000.0;      // [t/a]

				//sums up the loads of all manufacturing types
				if (ldi_untr_man[country] != NODATA )
					ldi_untr_man[country] += ldi_untr_man_fra[country][man_fra];

			/*	cout<<"manufactoring: ldi_untr_man: "<<ldi_untr_man[country]<<" country: "<<country<<
					" \nldi_untr_man_fra: "<<ldi_untr_man_fra[country][man_fra]
					<<" man_fra: "<<man_fra<<" \n rtf_man[country]: "<<rtf_man[country]
					<<" rtf_man_fra[country][man_fra]: "<<rtf_man_fra[country][man_fra]
				    <<" conc_man[country][man_fra]: "<<conc_man[country][man_fra]<<endl;
			*/
			}




		} // for(man_fra)



		if (connectivity[country][2]==NODATA||connectivity[country][3]==NODATA) {
			frac_man[country][2] = NODATA;
			frac_man[country][3] = NODATA;
		}
		//if no data exists. For BOD are 1 and 2 equal anyway
		else if ((connectivity[country][2]+connectivity[country][3])==0.0) {
			frac_man[country][2] = 50.0;
			frac_man[country][3] = 50.0;
		}
		else { // It is estimated that in Europe is only secondary and terticary treatment. Both becomes 'total' and the fraction of this is calculated
			frac_man[country][2] = connectivity[country][2]/(connectivity[country][2]+connectivity[country][3])*100.0;
			frac_man[country][3] = connectivity[country][3]/(connectivity[country][2]+connectivity[country][3])*100.0;
		}

		if (ldi_untr_man[country]==NODATA||frac_man[country][2]==NODATA||frac_man[country][3]==NODATA||
			rem[2]==NODATA || rem[3]==NODATA)
			ldi_treat_man[country] = NODATA;
		else{
			//if the failure rate of STPs (domestic is here the same as manufactoring) is known (usually developing countries)
			//than the removal rate is reduced
			if(stp_failure[country]!=NODATA){

				ldi_treat_man[country] = ldi_untr_man[country] *
				(
					 frac_man[country][2] * (100.0 - rem[2]*stp_failure[country]/100.0) +
					 frac_man[country][3] * (100.0 - rem[3]*stp_failure[country]/100.0)
				)/10000.0;

			}else
			//if the failure rate of STPs is unknown or very low (usually developed countries)
			//paper Williams 2011: Eq 7 without direct discharges
				ldi_treat_man[country] = ldi_untr_man[country] *
				(
						frac_man[country][2] * (100.0 - rem[2]) +
						frac_man[country][3] * (100.0 - rem[3])
				)/10000.0;
		}

		/*Direct discharges are always zero, because at the moment they are not included in this implementation, but if they
		should be integrated (all of Eq 7), the start is already here. But the formula(direct) does not exist jet.*/
		if (ldi_treat_man[country]==NODATA || ldd_treat_man[country]==NODATA)
			ld_treat_man[country] = NODATA;
		else ld_treat_man[country] = ldi_treat_man[country] + ldd_treat_man[country];

		if(country==(countries->number-1))
			cout<<"manufacturing done"<<endl;

		//========================================
		// scattered settlements
		/*            =========== BOD, salt, TN, TP, FC ========== */


			double x,y;
			if (((100.0-calc_con[country][1])>-0.0001)&&((100.0-calc_con[country][1])<0.0001)) x=0.0;
			else x=(100.0-calc_con[country][1]);

			if (x<0.0) {
				cerr << "coutry\t"<<countries->get_ISONUM(country)<<" in year "<< year<< " connectivity >100%! => x="<<x<<endl;
				x=0.0;
			}

			if (((100.0-calc_con[country][2])>-0.002)&&((100.0-calc_con[country][2])<0.002)) y=0.0;
			else y=(100.0-calc_con[country][2]);

			if (y<0.0) {
				cerr << "coutry\t"<<countries->get_ISONUM(country)<<" in year "<< year<< " connectivity >100%!=> y="<<y<<endl;
				cerr<<"calc_con[country][2]="<<calc_con[country][2]<<endl;
				y=0.0;
			}


			if (ef[country]==NODATA||pop[country][1]==NODATA||pop[country][2]==NODATA||
			  calc_con[country][1]==NODATA||calc_con[country][2]==NODATA)
			  ld_untr_sc[country] = NODATA;
			//paper William 2012 Eq 9 fraction of sewage of inhabitants which are not connected to a sewage treatment plant
			else{ ld_untr_sc[country] = ef[country] *
				(
						pop[country][1]*x/100.0 + pop[country][2]*y/100.0
				);
				if(ld_untr_sc[country]<0.0)
					cout<<"ld_untr_sc[country]="<<ld_untr_sc[country]<<" x="<<x<<" y="<<y<<" calc_con[country][1]="<<calc_con[country][1]<<" calc_con[country][2]="<<calc_con[country][2]<<endl;
				if(toCalc!=2){
					ld_untr_sc[country] /=1000.0;	//[t/a]
				}
			}


	//if detailed information on scattered settlements are available
	double sum_ld_sc=0.0;
	if(connec_treat_unknown[country]!=NODATA || hanging_lat[country]!=NODATA || open_defec[country]!=NODATA){

				if(connec_treat_unknown[country]!=NODATA)
					sum_ld_sc+=connec_treat_unknown[country];
				if(hanging_lat[country]!=NODATA)
					sum_ld_sc+=hanging_lat[country];
				if(open_defec[country]!=NODATA)
					sum_ld_sc+=open_defec[country];

				//if the sum of to_sewer, open defecation, hanging toilet and to_treat_unknown
				//is > or < 100 the percentages missing or to much are added or reduced
				//from to_treat_unknwon respectively
				// jriverac: this is not zhe case with globewq data
				miss_conn_rate=0.0;
				double sum_conn=connectivity[country][0]+connectivity[country][1]+connectivity[country][2]+connectivity[country][3]+connectivity[country][4];
				if((sum_ld_sc+sum_conn)!=100){
					//in some cases internal errors may occur -> something like 100-100=-1.42109e-14 to ensure this does not happen:
					if (((100.0-(sum_ld_sc+sum_conn))>-0.0001)
							&&((100.0-(sum_ld_sc+sum_conn))<0.0001))
						miss_conn_rate=0.0;
					else
						miss_conn_rate=100.0-(sum_ld_sc+sum_conn);
					//the sum is reduced or enlarged to ensure the percentages are correct for
					//open defecation and hanging toilet
					sum_ld_sc+=miss_conn_rate;
//					if(country==139||country==150)
//						cout<<"miss_conn_rate="<<miss_conn_rate<<" sum_ld_sc="<<sum_ld_sc<<" country="<<country
//						<<" connectivity[country][0]"<<connectivity[country][0]<<" connectivity[country][1]"<<connectivity[country][1]<<" connectivity[country][2]"<<connectivity[country][2]<<" connectivity[country][3]"<<connectivity[country][3]
//						<<" summe="<<(sum_ld_sc+connectivity[country][0]+connectivity[country][1]+connectivity[country][2]+connectivity[country][3])<<endl;

				}



				if(open_defec[country]!=NODATA && sum_ld_sc!=0){
				//if open defecation value is available this is used
				//divided by 10000 because divided by 100 returns percent as e.g.80% and by 100 more to use the same code for "paper Williams 2012 Eq 11 first part"
					//from all the not domestic load the percentage of open defecation is selected
					ld_diff_untr_sc[country]=open_defec[country]/(sum_ld_sc/10000);
				} else if (sum_ld_sc==0) {
					ld_diff_untr_sc[country]=0;
				} else {
					ld_diff_untr_sc[country]=NODATA;
				}
				
				//from all the not domestic load, the percentage of connec_treat_unknown is selected and reduced by a removal/failure rate
				if(connec_treat_unknown[country]!=NODATA && sum_ld_sc!=0){//not divided by 100 because this is done in "paper Williams 2012 Eq 11 first part"
					
					//ld_treat_sc is reduced or enlarged if the sum of all sewage disposal types is !=100%
					// % national population served with safely managed SPO (JMP: DisposedInSitu+EmptiedTreated), data available mainly for developed countries (China notable exception) 
					// secondary treatment assumed
					
					if(SPO_treated[country]!=NODATA && treat_failure!=NODATA){
						ld_treat_sc[country]=(SPO_treated[country]/(sum_ld_sc/100))*(100.0 - rem[2]*treat_failure/100.0)+((connec_treat_unknown[country]+miss_conn_rate-SPO_treated[country])/(sum_ld_sc/100))*(100.0-treat_failure);
					//	cout<<"Full info: Country,ld_treat = "<< countries->get_ISONUM(country) << " , " << ld_treat_sc[country] << endl;
					//} else if (treat_failure==NODATA && SPO_treated[country]!=NODATA) {
					//	ld_treat_sc[country]=(SPO_treated[country]/(sum_ld_sc/100))*(100.0 - rem[2])+((connec_treat_unknown[country]+miss_conn_rate-SPO_treated[country])/(sum_ld_sc/100))*(100.0-treat_failure);
					//	cout<<"missig stp_failure: Country,ld_treat = "<< countries->get_ISONUM(country) << " , " << ld_treat_sc[country] << endl;
					} else {
						ld_treat_sc[country]=((connec_treat_unknown[country]+miss_conn_rate)/(sum_ld_sc/100))*(100.0-treat_failure);
					//  cout<<"No info: Country,ld_treat = "<< countries->get_ISONUM(country) << " , " << ld_treat_sc[country] << endl;
					}
				} else if (sum_ld_sc==0) {
					ld_treat_sc[country]=0;
				} else {
					ld_treat_sc[country]=NODATA;
				}


				//cout<<"sum_ld_sc="<<sum_ld_sc<<" connec_treat_unknown[country]="<<connec_treat_unknown[country]
				//    <<"\nhanging_lat[country]="<<hanging_lat[country]<<" open_defec[country]="<<open_defec[country]
				//    <<"\nld_diff_untr_sc[country]="<<ld_diff_untr_sc[country]<<" treat_failure="<<treat_failure
				//    <<" ld_treat_sc[country]="<<ld_treat_sc[country]<<endl;

	} else {//less detailed information is available or necessary -> e.g. small islands, territories (Brunei, Liechtenstein)  

		// fraction
		if ((connectivity[country][0]==NODATA) || (connectivity[country][1]==NODATA) || (connectivity[country][2]==NODATA)
			 || (connectivity[country][3]==NODATA) || (connectivity[country][4]==NODATA)) {
			frac_sc[country][0] = NODATA;
			frac_sc[country][1] = NODATA;
			frac_sc[country][2] = NODATA;
			frac_sc[country][3] = NODATA;
			frac_sc[country][4] = NODATA;
		}
		else {
			double sum=(connectivity[country][0]+connectivity[country][1]+connectivity[country][2]+connectivity[country][3]+connectivity[country][4]);
			if (sum==0) {
				frac_sc[country][0]=0.0;
				frac_sc[country][1]=0.0;
				frac_sc[country][2]=100.0;
				frac_sc[country][3]=NODATA; // No teritaiary and quaternary
				frac_sc[country][4]=NODATA; // No teritaiary and quaternary


			}
			else {

				frac_sc[country][0]=connectivity[country][0]*100.0/sum;
				frac_sc[country][1]=connectivity[country][1]*100.0/sum;
				frac_sc[country][2]=((connectivity[country][2]+connectivity[country][3]+connectivity[country][4])*100.0)/sum;
				frac_sc[country][3]=NODATA;
				frac_sc[country][4]=NODATA;


				}
		}

		ld_treat_sc[country]=0.0;
		ld_diff_untr_sc[country]=0.0;
		//paper Williams 2012 Eq 11 second part (only the sum) sewage treatment
		for (int i=0; i<3; i++) { // for 3,4(tertiary, quaternary) not used
			if (frac_sc[country][i]==NODATA || rem[i]==NODATA) {ld_treat_sc[country]=NODATA; break;}
			else{
				if(i==0){ // no treatment
					ld_diff_untr_sc[country]=(frac_sc[country][i] * (100.0-rem[i]));
				}else	//if there is a failure rate of septic tanks, pit latrines, composting toilets and to take the unknown of bucket and unknown place into account
					if(treat_failure!=NODATA){
						ld_treat_sc[country]+= (frac_sc[country][i] * (100.0-rem[i]*(treat_failure)/100.0));
					}else
						ld_treat_sc[country]+= (frac_sc[country][i] * (100.0-rem[i]));
			}

		} //for(i)
	}//else



		//paper Williams 2012 Eq 11 first part
		if (ld_treat_sc[country]==NODATA || ld_untr_sc[country]==NODATA) ld_treat_sc[country]=NODATA;
		else {//from connec_treat_unknown
			ld_treat_sc[country]*=ld_untr_sc[country]/10000.0;
			if(ld_diff_untr_sc[country]!=NODATA) {	//from open defecation
				ld_diff_untr_sc[country]*=ld_untr_sc[country]/10000.0;
			}

			/*if(toCalc==2)
				ld_treat_sc[country]=0;*/

		}

		//cout<<"ld_treat_sc[country] before hanging latrine="<<ld_treat_sc[country]<<endl;

		//if hanging latrines exist, everything goes straight into the river
		if( hanging_lat[country]!=NODATA && ld_untr_sc[country]!= NODATA ){
        if (sum_ld_sc != 0) {
           ld_hanging_l[country]=(ld_untr_sc[country]*(hanging_lat[country]/(sum_ld_sc/100)))/100;
        }
			//ld_treat_sc[country]+=ld_hanging_l[country];
		}
		/*}else //if there is nothing regarding connec_treat_unknown only hanging toilet will be point scattted settlements
			if (ld_treat_sc[country]==NODATA && hanging_lat[country]!=NODATA && ld_untr_sc[country]!= NODATA ){
				ld_hanging_l[country]=(ld_untr_sc[country]*(hanging_lat[country]/(sum_ld_sc/100)))/100;
				ld_treat_sc[country]=ld_hanging_l[country];
		}*/
		/*if(country==countries->get_countryPos(643))
		  cout<<"hanging_lat[country]="<<hanging_lat[country]<<" ld_untr_sc[country]="<<ld_untr_sc[country]
		   <<" sum_ld_sc="<<sum_ld_sc<<" ld_treat_sc[country]="<<ld_treat_sc[country]
		   <<" ld_hanging_l[country]="<<ld_hanging_l[country]<<endl;
		 */

		/*if(country==countries->get_countryPos(643) || country==countries->get_countryPos(528)){
			cout<<"ld_diff_untr_sc[country]: "<<ld_diff_untr_sc[country]<<" country: "<<643
				<<" ld_untr_sc[country]: "<<ld_untr_sc[country]<<" frac_sc[country][3]: "<<frac_sc[country][3]
				<<"\nfrac_sc[country][0]: "<<frac_sc[country][0]<<" frac_sc[country][1]: "<<frac_sc[country][1]<<" frac_sc[country][2]: "<<frac_sc[country][2]
				<<" 100.0-rem[3]: "<<100.0-rem[3]
				<<"\nld_treat_sc[country]: "<<ld_treat_sc[country]<<endl;
		}*/

		if(country==(countries->number-1))
			cout<<"scattered settlements done"<<endl;

	} // for (country)

} // CountryLoad()

void ClassWQ_load::CellLoad() {

	for (int cell=0; cell<continent.ng; cell++) {
		int countryISO = country_cell[cell];
		int country = countries->get_countryPos(countryISO);

		// If a cell has no county than NODATA is allocated. This case only is active if
		// the user has an error in the input preparation for the cells.
		// pesticide loading will be calculated in LoadAgr()
		if (country<0 || toCalc==5) {

			ld_untr_dom_cell[cell]       = NODATA;
			ld_treat_dom_cell[cell]      = NODATA;

			ldd_treat_man_cell[cell]     = NODATA;
			ldi_untr_man_cell[cell]      = NODATA;
			ldi_treat_man_cell[cell]     = NODATA;
			ld_treat_man_cell[cell]      = NODATA;

			ld_untr_mining_cell[cell]    = NODATA;
			ld_treat_mining_cell[cell]   = NODATA;

			ld_untr_sc_cell[cell]        = NODATA;
			ld_treat_sc_cell[cell]       = NODATA;
			ld_diff_untr_sc_cell[cell]	 = NODATA;

			ld_untr_urb_cell_year[cell]  = NODATA;
			ld_treat_urb_cell_year[cell] = NODATA;

			ld_irr_cell[cell]            = NODATA;
			ld_agr_cell_year[cell]       = NODATA;


			for (int month=0; month<12; month++) {
				ld_untr_urb_cell[cell][month]    = NODATA;
				ld_treat_urb_cell[cell][month]   = NODATA;
				ld_agr_cell[cell][month]         = NODATA;
				if (toCalc==5) {  // for pesticide
					ld_agr_soil_cell[cell][month] = NODATA;
				}
				if (toCalc==1) {  // for salt
					ld_background_cell[cell][month] = NODATA;
				}
			}


			ld_manure_cell[cell] 	 = NODATA;
			ld_inorg_fert_cell[cell] = NODATA;
			ld_background_cell_year[cell] = NODATA;

			ld_hanging_l_cell[cell] 		 	 = NODATA;



			continue;
		}



		//if(cell==15654||cell==14000||cell==46562)
		//	cout<<" G_land_area[cell]: "<<(int)G_land_area[cell]<<" cell_area[cell]: "<<cell_area[cell]<<" cell: "<<cell<<endl;

		//To get only the cell area which is land
		//cell_area[cell]=cell_area[cell]*((100.0-(int)G_land_area[cell])/100.0);
		cell_area[cell]=G_area[grow[cell]-1]*(((int)G_land_area[cell])/100.0);

		/*if(cell==15654||cell==14000||cell==46562)
			cout<<" After G_landfreq[cell]: "<<(int)G_landfreq[cell]<<" cell_area[cell]: "<<cell_area[cell]
			    <<" ((100-(int)G_landfreq[cell])/100): "<<((100.0-(int)G_landfreq[cell])/100.0)
			    <<" (100-(int)G_landfreq[cell]): "<<(100.0-(int)G_landfreq[cell])<<" cell: "<<cell<<endl;
		*/
		//========================================
		// domestic
		if (ld_untr_dom[country]==NODATA||pop_cell[cell][1]==NODATA||pop_cell[cell][2]==NODATA
		  ||calc_con[country][1]==NODATA||calc_con[country][2]==NODATA
		  ||pop[country][0]==NODATA||calc_con[country][0]==NODATA||pop[country][0]==0.0||calc_con[country][0]==0.0)
		  ld_untr_dom_cell[cell]= NODATA;
		else // paper Williams: Eq 3 calculation of influent load for one cell untreated
			ld_untr_dom_cell[cell] = ld_untr_dom[country] *
			  (
			    pop_cell[cell][1] * calc_con[country][1] +
			    pop_cell[cell][2] * calc_con[country][2]
			  )/(pop[country][0]*calc_con[country][0]);
		
		if (ld_treat_dom[country]==NODATA||pop_cell[cell][1]==NODATA||pop_cell[cell][2]==NODATA
		  ||calc_con[country][1]==NODATA||calc_con[country][2]==NODATA
		  ||pop[country][0]==NODATA||calc_con[country][0]==NODATA||pop[country][0]==0.0||calc_con[country][0]==0.0)
		  ld_untr_dom_cell[cell]= NODATA;
		else // paper Williams: Eq 3 calculation of influent load for one cell treated
			ld_treat_dom_cell[cell] = ld_treat_dom[country] *
			  (
			    pop_cell[cell][1] * calc_con[country][1] +           // urban
			    pop_cell[cell][2] * calc_con[country][2]             // rural
			  )/(pop[country][0]*calc_con[country][0]);              // total

		if(cell==(continent.ng-1))
			cout<<"\ndomestic downscaling done"<<endl;

		//========================================
		// urban
		ld_untr_urb_cell_year[cell] =0.0;
		ld_treat_urb_cell_year[cell]=0.0;
		for (int month=0; month<12; month++) {
			if (urban_runoff_cell[cell][month]==NODATA || conc_urb[country]==NODATA||
				frac_dom[country][0]==NODATA ||
				frac_dom[country][1]==NODATA ||
				frac_dom[country][2]==NODATA ||
				frac_dom[country][3]==NODATA ||
				frac_dom[country][4]==NODATA)
			{
				ld_untr_urb_cell[cell][month] =NODATA;
				ld_treat_urb_cell[cell][month]=NODATA;

				ld_untr_urb_cell_year[cell] =NODATA;
				ld_treat_urb_cell_year[cell]=NODATA;

			}
			else {
				// BOD, salt: [mm/mon] * [mg/l] *[km2]     = [10^6 mg/mon] = [t/mon /1000]
				// FC:        [mm/mon] * [no/l00ml] *[km2] = [10^7 no/mon] = [10^10 no/mon /1000]
				/*paper Williams 2012 Eq 5 load from runoff; load per area; it is written 1000. because 1000 is an int
				 * and a double is returned. The dot makes sure that a double is returned*/
				ld_untr_urb_cell[cell][month] = urban_runoff_cell[cell][month] * conc_urb[country] /1000.*cell_area[cell] * built_up_frac[cell];



				//Urban runoff is treated in sewage treatments plants as well(see Williams 2012)
				//if the failure rate of STPs is known (usually developing countries)
				//than the removal rate is reduced
				if(stp_failure[country]!=NODATA){

					ld_treat_urb_cell[cell][month]= ld_untr_urb_cell[cell][month] *
					(
							frac_dom[country][0]*(100.0 - rem[0])+
							frac_dom[country][1]*(100.0 - rem[1]*stp_failure[country]/100.0)+
							frac_dom[country][2]*(100.0 - rem[2]*stp_failure[country]/100.0)+
							frac_dom[country][3]*(100.0 - rem[3]*stp_failure[country]/100.0)+
							frac_dom[country][4]*(100.0 - rem[4]*stp_failure[country]/100.0)
					)/10000.0;

				}else
				//if the failure rate of STPs is unknown or very low (usually developed countries)
					ld_treat_urb_cell[cell][month]= ld_untr_urb_cell[cell][month] *
					(
							frac_dom[country][0]*(100.0 - rem[0])+
							frac_dom[country][1]*(100.0 - rem[1])+
							frac_dom[country][2]*(100.0 - rem[2])+
							frac_dom[country][3]*(100.0 - rem[3])+
							frac_dom[country][4]*(100.0 - rem[4])
					)/10000.0;

				// use all available values for the country-year value
				ld_untr_urb[country]  += ld_untr_urb_cell[cell][month];
				ld_treat_urb[country] += ld_treat_urb_cell[cell][month];

				// country-year value only if all data is available otherwise: NODATA
				if (ld_untr_urb_cell_year[cell] !=NODATA)  ld_untr_urb_cell_year[cell] +=ld_untr_urb_cell[cell][month];
				if (ld_treat_urb_cell_year[cell]!=NODATA)  ld_treat_urb_cell_year[cell]+=ld_treat_urb_cell[cell][month];
			}

		} //for(month)

		if(cell==(continent.ng-1))
			cout<<"urban done"<<endl;

		//========================================
		// manufacturing
		if (ldd_treat_man[country]==NODATA|| rtf_man_cell[cell]==NODATA||rtf_man[country]==NODATA||rtf_man[country]==0.0)
			ldd_treat_man_cell[cell] = NODATA;
		// ldd_treat_man is always zero because direct discharges are not implemented
		else ldd_treat_man_cell[cell] = ldd_treat_man[country] * rtf_man_cell[cell]/rtf_man[country];

		if (ldi_treat_man[country]==NODATA|| rtf_man_cell[cell]==NODATA||rtf_man[country]==NODATA||rtf_man[country]==0.0)
			ldi_treat_man_cell[cell] = NODATA;
		//load of return flow of indirect discharges after treatment
		else ldi_treat_man_cell[cell] = ldi_treat_man[country] * rtf_man_cell[cell]/rtf_man[country];

		if (ldi_untr_man[country]==NODATA|| rtf_man_cell[cell]==NODATA||rtf_man[country]==NODATA||rtf_man[country]==0.0)
			ldi_untr_man_cell[cell] = NODATA;

		else ldi_untr_man_cell[cell]  = ldi_untr_man[country]  * rtf_man_cell[cell]/rtf_man[country];

		if (ld_treat_man[country]==NODATA|| rtf_man_cell[cell]==NODATA||rtf_man[country]==NODATA||rtf_man[country]==0.0)
			ld_treat_man_cell[cell] = NODATA;
		//load of direct and indirect discharges after treatment
		else ld_treat_man_cell[cell]  = ld_treat_man[country]  * rtf_man_cell[cell]/rtf_man[country];

		if(cell==(continent.ng-1))
			cout<<"manufacturing downscaling done"<<endl;

		//========================================
		// mining
		if (toCalc == 1) {
			ld_untr_mining_cell[cell] = 0.;
			for (int resource=0; resource<5; resource++) {
				if (conc_mining[country][resource]==NODATA || rtf_mining_cell[cell][resource]==NODATA) {
					//ld_untr_mining_cell[cell] = NODATA;
				} else {
					ld_untr_mining_cell[cell] += conc_mining[country][resource] * rtf_mining_cell[cell][resource] /1000000.0; // [t/a]
				}
				if (cell==660) cout << "ld_untr_mining_cell["<< cell<<"]=\t"<<ld_untr_mining_cell[cell]<<'\t'<< rtf_mining_cell[cell][resource]<<endl;
			}
			// erstmal keine klärung
			ld_treat_mining_cell[cell] = ld_untr_mining_cell[cell];

			// country
			ld_untr_mining[country] += ld_untr_mining_cell[cell];
			ld_treat_mining[country] += ld_treat_mining_cell[cell];
		}

		//========================================
		// scattered settlements

		if (ld_untr_sc[country]==NODATA || pop_cell[cell][1]==NODATA || pop_cell[cell][2]==NODATA || calc_con[country][1]==NODATA || 
		    calc_con[country][2]==NODATA||pop[country][0]==NODATA||calc_con[country][0]==NODATA||pop[country][0]==0.0)
			ld_untr_sc_cell[cell] = NODATA;
		//paper Williams 2012: Eq 10 one part of the equation
		else {
			if (ld_untr_sc[country]>0 && calc_con[country][0]<100) {
				ld_untr_sc_cell[cell] = ld_untr_sc[country] * (
			    pop_cell[cell][1] * (1-calc_con[country][1]/100) +  // urban
			    pop_cell[cell][2] * (1-calc_con[country][2]/100)    // rural
			  )/(pop[country][0]*(1-calc_con[country][0]/100));
			} else ld_untr_sc_cell[cell] = 0;
			// Debug cells
			//if (cell==809047) cout << "IM HERE 1) ld_untr_sc[country], ld_untr_sc_cell[cell], country, poptot, contot = " << ld_untr_sc[country] << "," << ld_untr_sc_cell[cell] << ","  << countries->get_ISONUM(country) << "," << pop[country][0] << "," << calc_con[country][0] <<endl;
		}
		
		if (pop_cell[cell][1]==NODATA || pop_cell[cell][2]==NODATA || calc_con[country][1]==NODATA || 
		    calc_con[country][2]==NODATA||pop[country][0]==NODATA||calc_con[country][0]==NODATA||pop[country][0]==0.0)
		{
			ld_treat_sc_cell[cell] 	   = NODATA;
			ld_diff_untr_sc_cell[cell] = NODATA;
			ld_hanging_l_cell[cell]	   = NODATA;
		}
		//paper Williams 2012: Eq 10 one part of the equation
		else {
			if(ld_treat_sc[country]==NODATA) ld_treat_sc_cell[cell] = NODATA;
			else{
				if (ld_treat_sc[country]>0 && calc_con[country][0]<100) {
					ld_treat_sc_cell[cell] = ld_treat_sc[country] * (
					pop_cell[cell][1] * (1-calc_con[country][1]/100) +  // urban
					pop_cell[cell][2] * (1-calc_con[country][2]/100)    // rural
				)/(pop[country][0]*(1-calc_con[country][0]/100));
			  } else ld_treat_sc_cell[cell] = 0;
			}
			
			if(ld_diff_untr_sc[country]==NODATA) ld_diff_untr_sc_cell[cell] = NODATA;
			else{
				if (ld_diff_untr_sc[country]>0 && calc_con[country][0]<100) {
					ld_diff_untr_sc_cell[cell] = ld_diff_untr_sc[country] * (
					pop_cell[cell][1] * (1-calc_con[country][1]/100) +  // urban
					pop_cell[cell][2] * (1-calc_con[country][2]/100)    // rural
					)/(pop[country][0]*(1-calc_con[country][0]/100));
			  } else ld_diff_untr_sc_cell[cell] = 0;
			}
			if(ld_hanging_l[country]==NODATA) ld_hanging_l_cell[cell] = NODATA;
			else{
				 if (ld_hanging_l[country] > 0 && calc_con[country][0]<100) {
					ld_hanging_l_cell[cell] = ld_hanging_l[country] * (
					pop_cell[cell][1] * (1-calc_con[country][1]/100) +  // urban
					pop_cell[cell][2] * (1-calc_con[country][2]/100)    // rural
				  )/(pop[country][0]*(1-calc_con[country][0]/100));
				} else ld_hanging_l_cell[cell]=0;
			}
		}

		if(cell==(continent.ng-1))
			cout<<"scattered settlements downscaling done"<<endl;

		//========================================
		// irrigation
		// for BOD, Fecal coliforms, TN and TP irrigation is not calculated
		if (toCalc==0 ||toCalc==2 || toCalc==3 || toCalc==4) {
			ld_irr_cell[cell] = NODATA;
			ld_irr[country]   = NODATA;
		}
		else {                       // for salt
			if (humidity[cell]==1)
				//natural salt class 1 - 4 : low to high salinity
				if (salinity[cell]==1) Salt_class[cell]=1;
				else Salt_class[cell]=2;
			else
				if (salinity[cell]==1) Salt_class[cell]=3;
				else Salt_class[cell]=4;

			// gdp class: 1 - 3 , low to high gdp per capita
			if (gdp[cell]== NODATA) GDPC[cell]=NODATA;
			else if (gdp[cell]<1000.0) GDPC[cell]=3;
			else if (gdp[cell]<10000.0) GDPC[cell]=2;
			else GDPC[cell]=1;

			// salt concentration in return flow (mg/l) derived from salt_class and sepc
			if (Salt_class[cell]==NODATA || GDPC[cell]==NODATA) SEPC[cell]=NODATA;
			else if (Salt_class[cell]==1 || (GDPC[cell]==1 && (Salt_class[cell]==2 || Salt_class[cell]==3 ))) SEPC[cell]=1;
			else if(Salt_class[cell]==2 || (Salt_class[cell]==4 && GDPC[cell]==1)) SEPC[cell]=2;
			else if(Salt_class[cell]==3 || (Salt_class[cell]==4 && GDPC[cell]==2)) SEPC[cell]=3;
			else SEPC[cell]=4;

			if (SEPC[cell]==NODATA||rtf_irr_cell[cell]==NODATA||conc_tds[country][SEPC[cell]-1]==NODATA) {
				ld_irr_cell[cell] = NODATA;
				ld_irr[country]   = NODATA;
			}
			else {
				//load of return flow for TDS
				ld_irr_cell[cell] = rtf_irr_cell[cell] * conc_tds[country][SEPC[cell]-1]/1000000.0;
				//load of the country is derived by all cells
				if (ld_irr[country]!=NODATA) ld_irr[country]  += ld_irr_cell[cell];
			}

			if (cell==136396) {
				cout << cell+1 <<'\t'<< gdp[cell]<<'\t'<<GDPC[cell]
				<<'\t'<<Salt_class[cell] <<'\t'<<SEPC[cell] <<'\t'<<ld_irr_cell[cell]
				<<'\t'<< rtf_irr_cell[cell]<<'\t'; 
				if (SEPC[cell]==NODATA) cout << NODATA;
				else cout<<conc_tds[country][SEPC[cell]-1]; 
				cout << endl;
			}

		if(cell==(continent.ng-1))
			cout<<"irrigation done"<<endl;
		}
		//========================================
		//diffuse sources

		//set the calibration parameters for TP calculations --> should be included to _runlist later
		TPpara[0] =0.04;
		TPpara[1] =800;
		TPpara[2] =-2;
		TPpara[3] =1.3e-8;      //3e-9 1.15e-8;

		// calculated for BOD, TDS, FC, TN and TP (toCalc==0, 1, 2, 3 or 4)
		if (toCalc==0||toCalc==1||toCalc==2 || toCalc==3 || toCalc==4) {


			double manure_prod_cell =0.0;				//organic (of substance) of manure from all animals on cell [t/a], FC:[10^10 cfu/a]

//				if(actSurRunoff[cell]<0.0)	//this can happy because of the way the surface runoff is calculated in WaterGAP. It will be changed there soon.
//					actSurRunoff[cell]=0.0;

			//manure

			for(int animal=0; animal<LS->number; animal++){
				if(LS->getLS_ex_rate(country, animal, toCalc, ex_ls, countries)) exit(1);
				if (ex_ls==NODATA||ls_cell[cell][animal]==NODATA) {
					manure_prod_cell = NODATA;
				} else {
					manure_prod_cell += ex_ls*ls_cell[cell][animal]; //excretion of one animal per year
					/*if (cell==94288||cell==75617|| cell==76839)
						cout<<"ex_ls: "<<ex_ls<<" ls_cell[cell][animal]: "<<ls_cell[cell][animal]
						<<" cell: "<<cell+1<<endl;*/
				}
			}

			if (manure_prod_cell==NODATA||rem[6]==NODATA||sur_runoff_cell_year[cell]==NODATA||soil_erosion[cell]==NODATA
				||sur_runoff_cell_mean[cell]==NODATA) {
				ld_manure_cell[cell] = NODATA;
			}else {
				//load multiplied with percent which can be washed out
				if(toCalc==0||toCalc==1||toCalc==2||toCalc==3){ //old method for all but TN and TP//changed by GF072015
				ld_manure_cell[cell] = manure_prod_cell*(rem[6]/100.0);
				}
				if(toCalc==4){//new method for TP //changed by GF072015
					if(manure_prod_cell==0||sur_runoff_cell_year[cell]==0||sur_runoff_cell_mean[cell]==0||built_up_frac[cell]==1||cell_area[cell]==0){
						ld_manure_cell[cell]=0;
					}else {
						ld_manure_cell[cell] = (manure_prod_cell * 1000)/(cell_area[cell]*(1-built_up_frac[cell])); //calculation to [kg/km�/yr] to use in following equation
						ld_manure_cell[cell] = ((TPpara[0] * ld_manure_cell[cell]) / (1 + pow(sur_runoff_cell_year[cell] / TPpara[1], TPpara[2]))) +
						soil_erosion[cell] * TPpara[3] * ld_manure_cell[cell] *
						(sur_runoff_cell_year[cell] / sur_runoff_cell_mean[cell]); //[kg/km�/yr] this is eq.11 of the protocol
					}
					ld_manure_cell[cell]=ld_manure_cell[cell]*cell_area[cell]*(1-built_up_frac[cell])*0.001;//[t/cell/year]
				}
				//if (cell==94288)
				//	cout<<"manure_prod_cell: "<<manure_prod_cell<<" sur_runoff_cell_year: "<<sur_runoff_cell_year[cell]<<" soil_erosion: "<<soil_erosion[cell]<<" sur_runoff_cell_mean: "<<sur_runoff_cell_mean[cell]<<" ld_manure_cell: "<<ld_manure_cell[cell]<<endl;
				ld_manure[country]+=ld_manure_cell[cell];

			}


			//industrial fertilizer


			if(toCalc==3){ //only for TN (old method with rem[7]
				ind_fert_use = crops->getCrop_Appl_Rate(country, crop_type[cell], cell);
				if(ind_fert_use==NODATA||rem[7]==NODATA||cell_area[cell]==NODATA||built_up_frac[cell]==NODATA) {
					ld_inorg_fert_cell[cell] = NODATA;
				}else {
					//application on unsealed area and percent which can be washed out
					//calculated from use by crop type and country
					ld_inorg_fert_cell[cell]=ind_fert_use*cell_area[cell]*(1-built_up_frac[cell])*(rem[7]/100.0);
				}

			} else if(toCalc==4){ //only for TP
				//---new method for industrial fertilizer (particulate and dissolved) transport to the surface water inserted by GF072015
				//ind_fert_use = crops->getCrop_Appl_Rate(country, crop_type[cell], cell);
				ind_fert_use=rate_p_cell[cell];
				if(ind_fert_use==NODATA||sur_runoff_cell_year[cell]==NODATA||sur_runoff_cell_mean[cell]==NODATA
					||soil_erosion[cell]==NODATA||cropland_area[cell]==NODATA||built_up_frac[cell]==NODATA) {
					ld_inorg_fert_cell[cell] = NODATA;
				}else {
					//application on unsealed area and percent which can be washed out
					//calculated from use by crop type and country
					if(sur_runoff_cell_year[cell]!=0 && sur_runoff_cell_mean[cell]!=0 && soil_erosion[cell]!=0 && ind_fert_use!=0){
						//ld_inorg_fert_cell[cell]= ind_fert_use*(142.0/62.0)*1000; // test balance of P2O5 in kg
						ld_inorg_fert_cell[cell] = ((TPpara[0] * ind_fert_use*1000) / (1 + pow(sur_runoff_cell_year[cell] / TPpara[1], TPpara[2]))) +
						soil_erosion[cell] * TPpara[3] * ind_fert_use *1000*
						(sur_runoff_cell_year[cell] / sur_runoff_cell_mean[cell]); //[kg/km�/yr] this is eq.11 of the protocol (calculation in kg for better comparison with other sectors)
					}else {
						ld_inorg_fert_cell[cell]=0;
					}
					ld_inorg_fert_cell[cell]=ld_inorg_fert_cell[cell]*cropland_area[cell]*(1-built_up_frac[cell])*0.001;//[t/cell/year]
					//ld_inorg_fert_cell[cell]=ld_inorg_fert_cell[cell]*cell_area[cell]*(1-built_up_frac[cell])*0.001;//[t/cell/year]
				}
			} else {
				ld_inorg_fert_cell[cell] = NODATA;
			}

			if(cell==247431){
				cout <<"Country: " <<country << '\t' << toCalc<< endl;
				cout <<"Country: " <<countries->get_ISONUM(country) << endl; cout<<"crop type: ";
				if (toCalc==3) {cout <<crop_type[cell];} else cout << "NA";
				cout <<" load cell: "<<ld_inorg_fert_cell[cell]<<" ind_fert_use: "<<ind_fert_use<<" sur_runoff_cell_year: "<<sur_runoff_cell_year[cell]
				    <<" soil_erosion: "<<soil_erosion[cell]<<" sur_runoff_cell_mean: "<<sur_runoff_cell_mean[cell]
				    <<" built_up_frac: "<<built_up_frac[cell]<<" cropland_area[cell]:"<<cropland_area[cell]<<" cell_area[cell]:"<<cell_area[cell]<<endl;
			}
			//---


			if(ld_inorg_fert_cell[cell]!=NODATA || ld_inorg_fert_cell[cell]>=0){

				ld_ind_fert[country]+=ld_inorg_fert_cell[cell];
			}

			/*if(cell==46562|| cell ==8||cell==55314)
				cout<<"ld_inorg_fert_cell[cell]: "<<ld_inorg_fert_cell[cell]<<" ind_fert_use: "<<ind_fert_use<<" \n cell_area[cell]: "
				<<cell_area[cell]<<" (1-built_up_frac[cell]). "<<(1-built_up_frac[cell])<<" (rem[7]/100): "<<(rem[7]/100)<<" \n cell: "
				<<cell<<" crop_type[cell]: "<<crop_type[cell]<<" countries->get_ISONUM(country): "<<countries->get_ISONUM(country)
				<<" actSurRunoff[cell]: "<<actSurRunoff[cell]<<" conc_sur_runoff_lu: "<<conc_sur_runoff_lu
				<<" LU_cell[cell]: "<<LU_cell[cell]<<endl;
			 */



			//geogenic background

			if(toCalc==3){ //old method for TN
				if(GB->getGeoBackData(LU_cell[cell], geo_back_rate)) exit(1);

				ld_background_cell_year[cell]=geo_back_rate*cell_area[cell];
				ld_geog_back[country]+=ld_background_cell_year[cell];


				if (cell==94288|| cell==46562)
					cout<<"geogenic background: "<<geo_back_rate<<" cell_area[cell]: "<<cell_area[cell]
					    <<" cell: "<<cell<<" ld_background_cell_year[cell]: "<<ld_background_cell_year[cell]<<endl;

			}else ld_background_cell_year[cell]=NODATA;  // spaeter fuer Salz (toCalc==1) wird umdefiniert

			//---new method for TP; included by GF072015
			if(toCalc==4){
			//atmospheric deposition
				if(ld_Patmdepos_rate[cell]==NODATA||sur_runoff_cell_year[cell]==NODATA||sur_runoff_cell_mean[cell]==NODATA
						||soil_erosion[cell]==NODATA||cell_area[cell]==NODATA) {
						ld_Patmdepos_cell_year[cell] = NODATA;
				}else {
						//the part from atmdepos that is washed out or lost by erosion:
						if(sur_runoff_cell_year[cell]!=0 && sur_runoff_cell_mean[cell]!=0 && soil_erosion[cell]!=0){
							ld_Patmdepos_rate[cell] = ((TPpara[0] * ld_Patmdepos_rate[cell]) / (1 + pow(sur_runoff_cell_year[cell] / TPpara[1], TPpara[2]))) +
							soil_erosion[cell] * TPpara[3] * ld_Patmdepos_rate[cell] *
							(sur_runoff_cell_year[cell] / sur_runoff_cell_mean[cell]); //[kg/km�/year] this is eq.13 of the protocol
						}else {
							ld_Patmdepos_rate[cell]=0;
						}
						ld_Patmdepos_cell_year[cell] = ld_Patmdepos_rate[cell] * (1-built_up_frac[cell]) * cell_area[cell] * 0.001; //[t/cell/year]
				}

			//chemical weathering
				if(sur_runoff_cell_year[cell]==NODATA||sur_runoff_cell_mean[cell]==NODATA||cell_area[cell]==NODATA||ld_Pcweathering_rate[cell]==NODATA) {
						ld_Pcweathering_cell_year[cell] = NODATA;
				}else {
						//average annual total P rate of chemical weathering:
						//correction with actual annual runoff
						if(sur_runoff_cell_year[cell]!=0 && sur_runoff_cell_mean[cell]!=0 && cell_area[cell]!=0 && ld_Pcweathering_rate[cell]!=0){
							ld_Pcweathering_rate[cell] = ld_Pcweathering_rate[cell] * (sur_runoff_cell_year[cell] / sur_runoff_cell_mean[cell]); //[kg/km�/year] this is eq.14 of the protocol
						}else {
							ld_Pcweathering_rate[cell]=0;
						}
						ld_Pcweathering_cell_year[cell]=ld_Pcweathering_rate[cell]*(1-built_up_frac[cell])*cell_area[cell]*0.001; //[t/cell/year]
				}
			//total background load
				ld_background_cell_year[cell]=ld_Patmdepos_cell_year[cell]+ld_Pcweathering_cell_year[cell];
				ld_geog_back[country]+=ld_background_cell_year[cell];
			}
			//---
			if (cell ==436369){
				cout<<"-->TEST cell: "<<cell<<endl;
				cout<<"   cell_area[cell]: "<<cell_area[cell]<<"; built_up_frac[cell]: "<<built_up_frac[cell]<<endl;
				cout<<"   soil_erosion[cell]: "<<soil_erosion[cell]<<endl;
				cout<<"   sur_runoff_cell_mean[cell]: "<<sur_runoff_cell_mean[cell]<<"; sur_runoff_cell_year[cell]: "<<sur_runoff_cell_year[cell]<<endl;
				cout<<"   ind_fert_use [t/km�]: "<<ind_fert_use<<endl;
				cout<<"   ld_manure_cell_rate[cell]: "<<ld_manure_cell[cell]<<"; ld_inorg_fert_cell[cell]: "<<ld_inorg_fert_cell[cell]<<endl;
				cout<<"   ld_Pcweathering_cell_year[cell] + ld_Patmdepos_cell_year[cell] = ld_geog_background_cell_year[cell]: "<<ld_Pcweathering_cell_year[cell]<<" + "<<ld_Patmdepos_cell_year[cell]<<" = "<<ld_background_cell_year[cell]<<endl;
			}

			//untreated scattered settlements
			//just a fraction will be diluted
			if(ld_diff_untr_sc_cell[cell]==NODATA || rem[6]==NODATA) ld_diff_untr_sc_cell[cell]=NODATA;
			else{
				ld_diff_untr_sc_cell[cell]=ld_diff_untr_sc_cell[cell]*(rem[6]/100);
			}


			/*if (cell==94288|| cell==46562)
			cout<<"glcc: "<<crop_type[cell]<<"\n ld_manure: "<<ld_manure_cell[cell]<<" ld_inorg_fert: "<<ld_inorg_fert_cell[cell]
			    <<" ld_backgound: "<<ld_background_cell_year[cell]<<" cell: "<<cell<<endl;
			*/


		}else {
			ld_manure_cell[cell]	   = NODATA;
			ld_inorg_fert_cell[cell]   = NODATA;
			ld_background_cell_year[cell]   = NODATA;
			ld_diff_untr_sc_cell[cell] = NODATA;
		}   //end if-else (diffuse sources)

		if(cell==(continent.ng-1))
			cout<<"diffuse sources (substance specific: organic fertilizer, inorganic fertilizer, geogenic background) done"<<endl;

	} // for(cell)

	if (toCalc==4) {
			cout<<"###################"<<endl;
			cout<<"TP model parameters:"<<endl;
			cout<<"TP(1,2,3,4)= "<<TPpara[0]<<", "<<TPpara[1]<<", "<<TPpara[2]<<", "<<TPpara[3]<<endl;
	}

} // CellLoad()

void ClassWQ_load::LoadAgr() {
    // block to calculate ld_agr_cell, ld_agr_cell_year, ld_agr

	//return flow irrigation partitions into 12 month
	double (*rtf_irr_distrib)[12] = new double[continent.ng][12];  // [%]
	char sql[2000];

	for (int cell=0; cell<continent.ng; cell++)
		for  (int month=0; month<12; month++) {
			rtf_irr_distrib[cell][month] = NODATA;
//			ld_agr_cell[cell][month] = 0.;
		}

	if (toCalc==1) { // irrigation only for salt
		mysqlpp::Query query = con.query();
		mysqlpp::StoreQueryResult resQuery;

		char TableName_cellInput_rtf_irr[maxcharlength];
		if (!optionsWQ_load_ptr->IDInTableName) {
			sprintf(TableName_cellInput_rtf_irr, "%swq_load_%s.`cell_input_rtf_irr`", MyDatabase, optionsWQ_load_ptr->continent_abb);
		} else {
			sprintf(TableName_cellInput_rtf_irr, "%swq_load_%s.`cell_input_rtf_irr_%d`", MyDatabase, optionsWQ_load_ptr->continent_abb, optionsWQ_load_ptr->IDScen);
		}

		query.reset();

		int j=sprintf(sql,    "SELECT `cell`, IFNULL(`v1`,%d) as `v1`, ", NODATA);
		j+=sprintf(sql+j, "IFNULL(`v2`,%d) as `v2`, IFNULL(`v3`,%d) as `v3`, ", NODATA, NODATA);
		j+=sprintf(sql+j, "IFNULL(`v4`,%d) as `v4`, IFNULL(`v5`,%d) as `v5`, ", NODATA, NODATA);
		j+=sprintf(sql+j, "IFNULL(`v6`,%d) as `v6`, IFNULL(`v7`,%d) as `v7`, ", NODATA, NODATA);
		j+=sprintf(sql+j, "IFNULL(`v8`,%d) as `v8`, IFNULL(`v9`,%d) as `v9`, ", NODATA, NODATA);
		j+=sprintf(sql+j, "IFNULL(`v10`,%d) as `v10`, IFNULL(`v11`,%d) as `v11`, ", NODATA, NODATA);
		j+=sprintf(sql+j, "IFNULL(`v12`,%d) as `v12` ", NODATA);
		j+=sprintf(sql+j, "FROM %s ", TableName_cellInput_rtf_irr );
		if (!optionsWQ_load_ptr->IDInTableName)
			j+=sprintf(sql+j, "WHERE `IDScen`=%d AND `time`=%d ORDER BY `cell`; ", optionsWQ_load_ptr->IDScen, year);
		else
			j+=sprintf(sql+j, "WHERE `time`=%d ORDER BY `cell`; ", year);

		query << sql;
		//#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		//#endif
		resQuery = query.store();
		if (resQuery.empty()) {
			cerr << "monthValues for irrigation: error in table cell_input_rtf_irr.\n"; exit(1);
		}

		int cell;
		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			cell=resQuery[rowNum]["cell"]-1;
			if (cell>continent.ng) {
				cerr << "cell "<< cell << " not exist! error in monthValues table cell_input_rtf_irr.\n"; exit(1);
			}

			rtf_irr_distrib[cell][0]  = resQuery[rowNum]["v1"];
			rtf_irr_distrib[cell][1]  = resQuery[rowNum]["v2"];
			rtf_irr_distrib[cell][2]  = resQuery[rowNum]["v3"];
			rtf_irr_distrib[cell][3]  = resQuery[rowNum]["v4"];
			rtf_irr_distrib[cell][4]  = resQuery[rowNum]["v5"];
			rtf_irr_distrib[cell][5]  = resQuery[rowNum]["v6"];
			rtf_irr_distrib[cell][6]  = resQuery[rowNum]["v7"];
			rtf_irr_distrib[cell][7]  = resQuery[rowNum]["v8"];
			rtf_irr_distrib[cell][8]  = resQuery[rowNum]["v9"];
			rtf_irr_distrib[cell][9]  = resQuery[rowNum]["v10"];
			rtf_irr_distrib[cell][10] = resQuery[rowNum]["v11"];
			rtf_irr_distrib[cell][11] = resQuery[rowNum]["v12"];

		}
	} // if(salt)

	if (toCalc==1) { // if(salt)
		for (int cell=0; cell<continent.ng; cell++) {
			int country = countries->get_countryPos(country_cell[cell]);
			ld_background_cell_year[cell] = 0.;

			for (int month=0; month<12; month++) {
				// load of geogenic background
				double runoff_cell=0.;
				if (sur_runoff_cell[cell][month]==NODATA && gw_runoff_cell[cell][month]==NODATA)
					runoff_cell = NODATA;
				else {
					if (sur_runoff_cell[cell][month]!=NODATA ) runoff_cell += sur_runoff_cell[cell][month];
					if (gw_runoff_cell[cell][month]!=NODATA ) runoff_cell += gw_runoff_cell[cell][month];
				}

				if (runoff_cell!=NODATA ) {
					if (is_c_geogen_country && country>=0 && c_geogen[country]!=NODATA) {
						// [mm/month]  *[km2] * [mg/l] = [10^6 mg/mon] = [t/mon /1000]
						ld_background_cell[cell][month] = runoff_cell * cell_area[cell] / 1000. * c_geogen[country];
						ld_geog_back[country]+=ld_background_cell[cell][month];
						ld_background_cell_year[cell]+=ld_background_cell[cell][month];
					} else if (!is_c_geogen_country && c_geogen_cell[cell]!=NODATA) {
						// [mm/month]  *[km2] * [t/km3] = [t/mon /1000 000]
						ld_background_cell[cell][month] = runoff_cell * cell_area[cell] / 1e6 * c_geogen_cell[cell];
						ld_geog_back[country]+=ld_background_cell[cell][month];
						ld_background_cell_year[cell]+=ld_background_cell[cell][month];
					} else {
						ld_background_cell[cell][month] = NODATA;
					}
				} else {
					ld_background_cell[cell][month] = NODATA;
				}

				if (cell == 0 || cell == 371404 || cell == 371108 || cell == 371086) {
					cout << cell << '\t' << month<< '\t'
							<< sur_runoff_cell[cell][month]<< '\t'
							<< gw_runoff_cell[cell][month]<< '\t'
							<< runoff_cell<< '\t'
							<< cell_area[cell]<< '\t';
					if (!is_c_geogen_country) cout << c_geogen_cell[cell]<< '\t';
					cout << G_area[grow[cell]-1]<< '\t'
							<< (int)G_land_area[cell]<< '\t'
							<< grow[cell] << '\n';
				}

				if (rtf_irr_cell[cell]==NODATA || ld_irr_cell[cell]==NODATA || rtf_irr_distrib[cell][month]==NODATA) {
					ld_agr_cell[cell][month] = NODATA;
				}
				//if there is no irrigation the there is no treatment of irrigated water either
				else if (rtf_irr_cell[cell]==0) {
					ld_agr_cell[cell][month] = 0.;
				}
				else {
					ld_agr_cell[cell][month] = ld_irr_cell[cell] * rtf_irr_distrib[cell][month] / rtf_irr_cell[cell];
					if (ld_background_cell[cell][month]!=NODATA ) {
						ld_agr_cell[cell][month] -= ld_background_cell[cell][month];
						//if negative - this could happen because variable is calculated, but normally this should not happen
						if (ld_agr_cell[cell][month]<0) ld_agr_cell[cell][month]=0;
					}
				}

				// sum cell values of ld_agr in cell value for a year (ld_agr_cell_year) or for a country (ld_agr)
				if (country>=0) {
					if (ld_agr[country] == NODATA || ld_agr_cell[cell][month] == NODATA) ld_agr[country] = NODATA;
					else ld_agr[country] += ld_agr_cell[cell][month];
				}

				if(ld_agr_cell_year[cell] == NODATA || ld_agr_cell[cell][month] == NODATA) ld_agr_cell_year[cell] = NODATA;
				else ld_agr_cell_year[cell] += ld_agr_cell[cell][month];
			} // for(month)

		} // for(cell)
	} else if (toCalc==5) {
		if (get_cell_pesticide_input()) exit(1);  // organic_carbon einlesen
		// Januar
		if(get_cell_pesticide_input(0)) exit(1); // ld_input_pest[jan]

		if (optionsWQ_load_ptr->following_year==0) {
			for (int cell=0; cell<continent.ng; cell++) {
				calc_ld_agr_cell(cell, 0, 0, 0);
			} // for(cell)
		} else {
			double *ld_agr_soil_cell_dec = new double[continent.ng];
			double *ld_agr_cell_dec = new double[continent.ng];
			{
				std::ostringstream table;
				table << "calc_" << optionsWQ_load_ptr->IDScen << "_" << optionsWQ_load_ptr->parameter_id << "_cell_month_load_soil";
				string TableName = table.str();
				table.clear();
				if(get_cell_pesticide_lastYear(0, ld_agr_soil_cell_dec, TableName)) exit(1);
				
				table << "calc_" << optionsWQ_load_ptr->IDScen << "_" << optionsWQ_load_ptr->parameter_id << "_cell_month_load_irr";
				TableName = table.str();
				if(get_cell_pesticide_lastYear(0, ld_agr_cell_dec, TableName)) exit(1);
			}
			for (int cell=0; cell<continent.ng; cell++) {
				calc_ld_agr_cell(cell, 0, ld_agr_soil_cell_dec[cell], ld_agr_cell_dec[cell]);
			} // for(cell)
			delete[] ld_agr_soil_cell_dec; ld_agr_soil_cell_dec = NULL;
			delete[] ld_agr_cell_dec; ld_agr_cell_dec = NULL;
		}
		// Feb - Dez
		for (int month=1; month<12; month++) {
			if (get_cell_pesticide_input(month)) { // es gab Fehler in Input
				exit(1);
			}
			else { // Pestiziden kann man rechnen
				cout << "Month: " << month+1 << endl;
				for (int cell=0; cell<continent.ng; cell++) {
					int country = countries->get_countryPos(country_cell[cell]);

					if (country<0 || organic_carbon[cell]==NODATA||ld_input_pest[cell]==NODATA||sur_runoff_cell[cell][month]==NODATA) {
						ld_agr_soil_cell[cell][month] = NODATA;
						ld_agr_cell[cell][month] = NODATA;
						continue; // zur nächsten Zelle
					}
					calc_ld_agr_cell(cell, month, ld_agr_soil_cell[cell][month-1], ld_agr_cell[cell][month-1]);

					// sum cell values of ld_agr in cell value for a year (ld_agr_cell_year) or for a country (ld_agr)
					if (ld_agr[country] != NODATA && ld_agr_cell[cell][month] != NODATA) {
						ld_agr[country] += ld_agr_cell[cell][month];
					}

					if(ld_agr_cell[cell][month] == NODATA) {
						ld_agr_cell_year[cell] = NODATA;
					}
					else ld_agr_cell_year[cell] += ld_agr_cell[cell][month];



				}  // for(cell)
			}
		} // for(month)
	} else { // if not salt and not pesticide
		for (int cell=0; cell<continent.ng; cell++) {
			int country = countries->get_countryPos(country_cell[cell]);
			for (int month=0; month<12; month++) {
				//ld_background_cell[cell][month] = NODATA;
				ld_agr_cell[cell][month] = NODATA;
				ld_agr_cell_year[cell] = NODATA;
				if (country>=0) ld_agr[country] = NODATA;
			}
		}
	}

	//clean up
	delete[] rtf_irr_distrib; rtf_irr_distrib = NULL;

} // void LoadAgr()

void ClassWQ_load::CountryValuesOutput() {
	cout << "##################### country values OUTPUT ##############################\n";
	char TableName[maxcharlength];
	sprintf(TableName, "%swq_load_%s.`calc_country_connectivity`", MyDatabase, optionsWQ_load_ptr->continent_abb);
	char TableName2[maxcharlength];
	sprintf(TableName2, "%swq_load_%s.`calc_country_load`", MyDatabase, optionsWQ_load_ptr->continent_abb);
	char sql[10000];
	mysqlpp::Query query = con.query();

	if (!isTableExists(TableName)) {
		int j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
		j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		j+=sprintf(sql+j, "  `country_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `con_rur` double default NULL COMMENT 'rural connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `con_urb` double unsigned default NULL COMMENT 'urban connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `con_tot` double unsigned default NULL COMMENT 'total connecitivty [%%]',\n");
		j+=sprintf(sql+j, "  `frac_dom_untr` double unsigned default NULL COMMENT 'fraction of no domestic treatment [%%]',\n");
		j+=sprintf(sql+j, "  `frac_dom_prim` double unsigned default NULL COMMENT 'fraction of primary domestic connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `frac_dom_sec` double unsigned default NULL COMMENT 'fraction of secondary domestic connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `frac_dom_tert` double unsigned default NULL COMMENT 'fraction of tertiary domestic connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `frac_dom_quat` double unsigned default NULL COMMENT 'fraction of quaternary domestic connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `frac_man_sec` double unsigned default NULL COMMENT 'fraction of secondary manufacturing connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `frac_man_tert` double unsigned default NULL COMMENT 'fraction of tertiary manufacturing connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `frac_sc_untr` double unsigned default NULL COMMENT 'fraction of no treatment scattered settlements connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `frac_sc_prim` double unsigned default NULL COMMENT 'fraction of primary scattered settlements connectivity [%%]',\n");
		j+=sprintf(sql+j, "  `frac_sc_sec` double unsigned default NULL COMMENT 'fraction of secondary scattered settlements connectivity [%%]',\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`time`,`country_id`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci ROW_FORMAT=FIXED;\n");

		query.reset();
		query << sql;
		query.execute();
	}
	if (!isTableExists(TableName2)) {
		int j=sprintf(sql,    "CREATE TABLE  %s (", TableName2);
		j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
		j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `country_id` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
		j+=sprintf(sql+j, "  `ld_untr_dom` double unsigned default NULL COMMENT 'untreated domestic country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_dom` double unsigned default NULL COMMENT 'treated domestic country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldd_treat_man` double unsigned default NULL COMMENT 'treated direct manufacturing country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man` double unsigned default NULL COMMENT 'untreated indirect manufacturing country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man_f` double unsigned default NULL COMMENT 'untreated indirect manufacturing (food, drink, tobacco) country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man_t` double unsigned default NULL COMMENT 'untreated indirect manufacturing (textiles) country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man_p` double unsigned default NULL COMMENT 'untreated indirect manufacturing (paper, palp, printing) country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man_c` double unsigned default NULL COMMENT 'untreated indirect manufacturing (chemical industry) country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man_g` double unsigned default NULL COMMENT 'untreated indirect manufacturing (glas, ceramics, cement) country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man_m` double unsigned default NULL COMMENT 'untreated indirect manufacturing (metals) country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_untr_man_nd` double unsigned default NULL COMMENT 'untreated indirect manufacturing (other) country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ldi_treat_man` double unsigned default NULL COMMENT 'treated indirect manufacturing country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_man` double unsigned default NULL COMMENT 'treated total manufacturing country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_untr_mining` double unsigned default NULL COMMENT 'untreated mining country load [t/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_mining` double unsigned default NULL COMMENT 'treated mining country load [t/a]',\n");
		j+=sprintf(sql+j, "  `ld_untr_sc` double unsigned default NULL COMMENT 'untreated load from scattered settlements [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_sc` double unsigned default NULL COMMENT 'treated load from scattered settlements A) if no sub-sectors: treated sc B) if divided in sub-sectors than: septic tanks, pit latrines, unknown etc.  [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_open_defec` double unsigned default NULL COMMENT 'load from open defecation, thus diffuse scattered settelements',\n");
		j+=sprintf(sql+j, "  `ld_irr` double unsigned default NULL COMMENT 'load from irrigation return flow [t/a]',\n");
		j+=sprintf(sql+j, "  `ld_untr_urb` double default NULL COMMENT 'untreated urban country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_treat_urb` double default NULL COMMENT 'treated urban country load [t/a], [10^10no/a]',\n");
		j+=sprintf(sql+j, "  `ld_manure` double default NULL COMMENT 'load from the diffuse source of organic fertizer (manure) [t/a], FC [10^10 cfu/a]',\n");
		j+=sprintf(sql+j, "  `ld_geog_back` double default NULL COMMENT 'load from geogenic background  [t/a]',\n");
		j+=sprintf(sql+j, "  `ld_hanging_l` double default NULL COMMENT 'load from hanging latrines [t/a], FC: [10^10 cfu/a]',\n");
		j+=sprintf(sql+j, "  `ld_ind_fert` double default NULL COMMENT 'load from inorganic fertilizer  [t/a]',\n");
		j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`country_id`)\n");
		j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci ROW_FORMAT=FIXED;\n");

		query.reset();
		query << sql;
		query.execute();
	}

	// calc_country_connectivity ist nicht Parameterabh�ngig,
	// wenn gleichzeitig zwei Runs gestartet wurden f�r unterschiedliche Parameter aber gleiche Szenario und Jahre
	// soll verhindert werden, dass einer Client f�ngt Daten einzuf�gen und anderer l�scht sie
	// deswegen Tabelle blockieren und erst nach letztem INSERT wieder frei geben
	query.reset();
	query << "LOCK TABLES " << TableName << " WRITE, "  << TableName2 <<" WRITE;";
	cout << query << "...\n";
	query.execute();

	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE `idScen`= "
		<< optionsWQ_load_ptr->IDScen<<" AND `time`= " << year << ";";
	query.execute();

	sprintf(TableName, "%swq_load_%s.`calc_country_load`", MyDatabase, optionsWQ_load_ptr->continent_abb);

	query.reset();
	query << "DELETE FROM "<<TableName<<" WHERE `idScen`= "
		<< optionsWQ_load_ptr->IDScen
		<<" AND `parameter_id`= " << optionsWQ_load_ptr->parameter_id
		<< " AND `time`= " << year << ";";
	query.execute();

	for (int country=0; country<countries->number; country++) {

		// #############  INSERT INTO `calc_country_load`
		query.reset();
		query << setprecision(30);
		query << "insert into "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_country_load (`IDScen`, `parameter_id`, `country_id`, `time`, \n"
		  << "`ld_untr_dom`, `ld_treat_dom`, "
		  << "`ldd_treat_man`, `ldi_untr_man`, `ldi_treat_man`, `ld_treat_man`, \n"
		  << "`ldi_untr_man_f`, `ldi_untr_man_t`, `ldi_untr_man_p`, `ldi_untr_man_c`, "
		  << "`ldi_untr_man_g`, `ldi_untr_man_m`, `ldi_untr_man_nd`, \n"
		  << "`ld_treat_mining`, `ld_untr_mining`, "
		  << "`ld_untr_sc`, `ld_treat_sc`, `ld_open_defec`, `ld_irr`, "
		  << "`ld_untr_urb`, `ld_treat_urb`, `ld_manure`, `ld_geog_back`, `ld_hanging_l`, `ld_ind_fert` ) \n"
		  << " values ("<<optionsWQ_load_ptr->IDScen<<", "<< optionsWQ_load_ptr->parameter_id<<", " << countries->get_ISONUM(country)<<", " <<year<<", ";

		if (ld_untr_dom[country]==NODATA) query <<"NULL, ";
		else query <<ld_untr_dom[country]<<", ";

		if (ld_treat_dom[country]==NODATA) query <<"NULL, ";
		else query <<ld_treat_dom[country]<<", ";

		if (ldd_treat_man[country]==NODATA) query <<"NULL, ";
		else query <<ldd_treat_man[country]<<", ";

		if (ldi_untr_man[country]==NODATA) query <<"NULL, ";
		else query <<ldi_untr_man[country]<<", ";

		if (ldi_treat_man[country]==NODATA) query <<"NULL, ";
		else query <<ldi_treat_man[country]<<", ";

		if (ld_treat_man[country]==NODATA) query <<"NULL, ";
		else query <<ld_treat_man[country]<<", ";

		for (int man_fra=0; man_fra<7; man_fra++)
			if (ldi_untr_man_fra[country][man_fra]==NODATA) query <<"NULL, ";
			else query <<ldi_untr_man_fra[country][man_fra]<<", ";

		if (ld_treat_mining[country]==NODATA) query <<"NULL, ";
		else query <<ld_treat_mining[country]<<", ";

		if (ld_untr_mining[country]==NODATA) query <<"NULL, ";
		else query << ld_untr_mining[country]<<", ";

		if (ld_untr_sc[country]==NODATA) query <<"NULL, ";
		else query <<ld_untr_sc[country]<<", ";

		if (ld_treat_sc[country]==NODATA) query <<"NULL, ";
		else query <<ld_treat_sc[country]<<", ";

		if (ld_diff_untr_sc[country]==NODATA) query <<"NULL, ";
		else query <<ld_diff_untr_sc[country]<<", ";

		if (ld_agr[country]==NODATA) query <<"NULL, ";
		else query <<ld_agr[country]<<", ";

		if (ld_untr_urb[country]==NODATA) query <<"NULL, ";
		else query <<ld_untr_urb[country]<<", ";

		if (ld_treat_urb[country]==NODATA) query <<"NULL, ";
					else query <<ld_treat_urb[country]<<", ";

		if (ld_manure[country]==NODATA) query <<"NULL, ";
					else query <<ld_manure[country]<<", ";

		if (ld_geog_back[country]==NODATA) query <<"NULL, ";
					else query <<ld_geog_back[country]<<", ";

		if (ld_hanging_l[country]==NODATA) query <<"NULL, ";
					else query <<ld_hanging_l[country]<<", ";

		if (ld_ind_fert[country]==NODATA) query <<"NULL); ";
		else query <<ld_ind_fert[country]<<");";



		//#ifdef DEBUG_queries
		  //cout << "Query: " << query.preview() << endl << endl; //cin >> j;
		  cout << "Query: " << query << endl << endl; //cin >> j;
		//#endif
		query.execute();

		//cout << country << '\t'<<countries->get_ISONUM(country)<<"\tcalc_country_connectivity\n";
		// #############  INSERT INTO `calc_country_connectivity`
		query.reset();
		query << setprecision(30);
		query << "insert into "<<MyDatabase<<"wq_load_"<<optionsWQ_load_ptr->continent_abb<<".calc_country_connectivity "
		  << "(`IDScen`, `country_id`, `time`, `con_rur`, `con_urb`, `con_tot`, "
		  << "`frac_dom_untr`, `frac_dom_prim`, `frac_dom_sec`, `frac_dom_tert`, `frac_dom_quat`, `frac_man_sec`, `frac_man_tert`, "
		  << "`frac_sc_untr`, `frac_sc_prim`, `frac_sc_sec`) "
		  << " values ("<<optionsWQ_load_ptr->IDScen<<", "<< countries->get_ISONUM(country)<<", " <<year<<", ";

		if (calc_con[country][2]==NODATA) query <<"NULL, ";
		else query <<calc_con[country][2]<<", ";

		if (calc_con[country][1]==NODATA) query <<"NULL, ";
		else query <<calc_con[country][1]<<", ";

		if (calc_con[country][0]==NODATA) query <<"NULL, ";
		else query <<calc_con[country][0]<<", ";

		if (frac_dom[country][0]==NODATA) query <<"NULL, ";
		else query <<frac_dom[country][0]<<", ";

		if (frac_dom[country][1]==NODATA) query <<"NULL, ";
		else query <<frac_dom[country][1]<<", ";

		if (frac_dom[country][2]==NODATA) query <<"NULL, ";
		else query <<frac_dom[country][2]<<", ";

		if (frac_dom[country][3]==NODATA) query <<"NULL, ";
		else query <<frac_dom[country][3]<<", ";

		if (frac_dom[country][4]==NODATA) query <<"NULL, ";
		else query <<frac_dom[country][4]<<", ";

		if (frac_man[country][2]==NODATA) query <<"NULL, ";
		else query <<frac_man[country][2]<<", ";

		if (frac_man[country][3]==NODATA) query <<"NULL, ";
		else query <<frac_man[country][3]<<", ";

		if (frac_sc[country][0]==NODATA) query <<"NULL, ";
		else query <<frac_sc[country][0]<<", ";

		if (frac_sc[country][1]==NODATA) query <<"NULL, ";
		else query <<frac_sc[country][1]<<", ";

		if (frac_sc[country][2]==NODATA) query <<"NULL);";
		else query <<frac_sc[country][2]<<");";

		//#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		//#endif
		query.execute();

	} // for (country) OUTPUT

	query.reset();
	query << "UNLOCK TABLES;";
	cout << query << endl;
	query.execute();

} // void CountryValuesOutput()

void ClassWQ_load::calc_ld_agr_cell(int cell, int month, double ld_agr_soil_lastMonth, double ld_agr_lastMonth) {
	if (cell < 0 || cell >= continent.ng) {
		cerr << "cell "<< cell << " not exist!!! error in calc_ld_agr_cell.\n"; return;
	}
	if (month < 0 || month >= 12) {
		cerr << "month "<< month+1 << " not exist!!! error in calc_ld_agr_cell.\n"; return;
	}

	if (organic_carbon[cell] == NODATA) {
		ld_agr_soil_cell[cell][month] = NODATA;
		ld_agr_cell[cell][month] = NODATA;
	} else {
		double f1 = 1.;
//		if (g_slope[cell]<0.2) {
//			f1 = 0.001423 * pow(g_slope[cell]*100.,2.) + 0.02153 * g_slope[cell]*100.;
//		}

		if (g_land_slope[cell]<20.) {
			f1 = 0.001423 * pow(g_land_slope[cell],2.) + 0.02153 * g_land_slope[cell];
		}
		double f2 = pow(0.83, WBZ_cell[cell]);
		double f = f1 * f2;

		double Kd = koc * organic_carbon[cell]/100.;

		double Dt = 1.;  // Einheit gleich mit DT50soil: Monat
		double exponent = exp(-Dt * log(2.)/DT50soil);
		ld_agr_soil_cell[cell][month] = ld_input_pest[cell] + ld_agr_soil_lastMonth * exponent - ld_agr_lastMonth;
		ld_agr_cell[cell][month] = 0;
		if (precipitation_cell[cell][month]>0.) {
			ld_agr_cell[cell][month] = ld_agr_soil_cell[cell][month] *
					sur_runoff_cell[cell][month] / precipitation_cell[cell][month] * f /(1+Kd);
		}

		if (cell==0 || cell==79140 || cell==80363 || cell==110551)
		{
			cout << cell+1 << '\t' << month+1 << '\t' << g_land_slope[cell]
					<< '\t'  << f1 << '\t' << f2 << '\t' << f << '\t' << koc
					<< '\t' << organic_carbon[cell]<< '\t' <<Kd << '\t' << exponent
					<< '\t' << DT50soil<< '\t' << sur_runoff_cell[cell][month]
					<< '\t' << precipitation_cell[cell][month]<< '\t' << ld_agr_cell[cell][month]
					<< '\t' << ld_input_pest[cell] << '\t' << ld_agr_soil_cell[cell][month] << '\t'
					<< ld_agr_soil_lastMonth << '\t' << ld_agr_lastMonth
					<< endl;
		}
	} // nicht NODATA

} // calc_ld_agr_cell
