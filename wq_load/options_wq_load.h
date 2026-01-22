/*
 * options.h
 *
 *  Created on: 22.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_WQ_LOAD_H_
#define OPTIONS_WQ_LOAD_H_

#include "../options/options.h"

class options_wq_loadClass: public optionsClass {
public:
	options_wq_loadClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		parameter_id = 310;
		following_year = 0;
		project_id = 4;
		IDScen = 91;
		manure_timing = 0;
		climate = 0;

	}
	virtual ~options_wq_loadClass();

	// Optionen für wq_load
	int parameter_id;

	// only for pesticide; 0: first year to calculatte; 1:  following year. there are a loading from december last year
	int following_year;

	int project_id;
	int IDScen;
	short manure_timing; // =0;

	short climate;				    //climate input in 0.5� = 0 ; 5min = 1; default 0
	
	// 0: gegridete population aus wq_load_[continent].cell_input 1: gegridete population aus UNF-Dateien (griddi-Output); default: 0
	short gridded_pop_from_file; 
	char* gridded_pop_path; // nur wenn  gridded_pop_from_file=!

	// hydro_input path: GBUILTUP.UNF0, GBUILTUP.UNF0, G_LANDSLOPE.UNF0 (only pesticide), G_WG3_WG2WITH5MIN.UNF4 (if climate data in 0.5°)
	//char* path_hydro_input; // gibt es schon in continent-Beschreibung (continentStruct)

	// OUTPUT von WaterGAP (hydrology): G_URBAN_RUNOFF_[year].12.UNF0, G_SURFACE_RUNOFF_[year].12.UNF0, G_GW_RUNOFF_[year].12.UNF0
	// G_SURFACE_RUNOFF_MEAN.UNF0              TP
	char* path_watergap_output;
	
	// livestock output path:  G_LIVESTOCK_NR_[year].12.UNF0
	char* path_livestock_output;
	
	// G_CORR_FACT_RTF_[year].12.UNF0
	char* path_corr_factor;
	
	// GTEMP_[year].12.UNF2, GPREC_[year].12.UNF2 (only pesticide)
	char* path_climate;

	// GNRD_[year].12.UNF1
	char* path_gnrd;

	// G_SOILEROS.UNF0                         TP
	// G_PWEATHERING.UNF0                      TP
	// GLCC[year].UNF2                         TP/TN
	char* path_tp_input;

	// CROPLAND_CORR_KM2_[year].UNF0         TP/TN
	// P_RATE_TON_KM2_[year].UNF0            TP/TN
	char* path_tp_input2;

	// G_PATMDEPOS_[year].UNF0                 TP
	char* path_tp_input3;

	// IDScen und parameterID in table name (cell_input, cell_) default: 0
	short IDInTableName;

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_WQ_LOAD_H_ */
