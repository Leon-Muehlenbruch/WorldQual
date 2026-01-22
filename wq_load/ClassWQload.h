/*
 * ClassWQload.h
 *
 *  Created on: 30.01.2018
 *      Author: kynast
 */

#ifndef CLASSWQLOAD_H_
#define CLASSWQLOAD_H_

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <math.h>
#include "country.h"
#include "general_functions.h"
#include "options_wq_load.h"
#include "ls.h"
#include "crop.h"
#include "gb.h"

#ifndef DEBUG_queries
#define DEBUG_queries
//#undef DEBUG_queries
#endif

#define NO_MINING

class ClassWQ_load {
public:
	ClassWQ_load(options_wq_loadClass* options_ptr, int year);
	virtual ~ClassWQ_load();

private:
	int toCalc; // 0 - BOD, 1 - salt, 2 - FC (Fecal coliforms), 3 - TN, 4 - TP, 5 - Pestiziden
	int year;
	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input
	options_wq_loadClass* optionsWQ_load_ptr;

	COUNTRY_CLASS* countries; 	// class containing information about the countries
	CROP_CLASS* crops; // class containing information about the crop types
	LS_CLASS* LS; // class containing information about livestock
	GB_CLASS* GB; // class containing information about geogenic background: e.g. deposition

	double ind_fert_use; //inorganic fertilizer use per country and crop type [t/km^2]
	double ex_ls;	//excretion rate [t/(a*head)], FC: [10^10 cfu/(a*head)]
	//double conc_sur_runoff_lu; //concentration of surface runoff depending on landuse type and inorganic fertilizer, background [t/L] only applies for BOD
	double geo_back_rate;	//deposition [t/(a*ha)] for TN and TP; concentration of surface runoff [t/L] for BOD;  not applied to FC

	// ######################### input ###############################
	double (*pop)[3];  // population total, urban, rural population [number]
	double (*sewerConn)[2];  // sewer connectivity urban, rural [number]
	double *SPO_treated;  // % national population served with safely managed SPO (JMP: DisposedInSitu+EmptiedTreated) 
	double (*connectivity)[5];  // no treatment, primary, secondary, tertiary, quaternary  connectivity [%]
	double *rtf_man;     // manufacturing returnflow [m3/a]
	double *rtf_dom;     // domestic returnflow [m3/a]

	// fraction of manufacturing returnflow [%]
	// 0 - food, drink, tobacco
	// 1 - textiles
	// 2 - paper, palp, printing
	// 3 - chemical industry
	// 4 - glas, ceramics, cement
	// 5 - metals
	// 6 - other
	double (*rtf_man_fra)[7];

	double *rtf_irr;        // irrigation returnflow (m3/a]

	// 0 - no treatment removal rate
	// 1 - primary
	// 2 - secondary
	// 3 - tertiary
	// 4 - quaternary
	// 5 - removal rate in soil
	// 6 - reduction factor organic from manure
	// 7 - reduction factor inorganic
	// 8 - removal rate of manure in storage
	// 9 - removal rate of manure on soil
	double rem[10];

	// 0 - Lmax; maximal freaction of DP load [-]
	// 1 - a; defines relationship between runoff and DP leaching (see eq. 8 in protocol)
	// 2 - b; defines relationship between runoff and DP leaching (see eq. 8 in protocol)
	// 3 - c; defines PP with eroded sediments and P input (see eq. 8 in protocol)
	double TPpara[4];

	double *ef; // emission factor from population [kg/cap/year]; FC: 10^10 cfu/(a*head)

	// manufacturing concentration of determinand in return flow [mg/l]; FC: 10^10 cfu/L
	// 0 - food, drink, tobacco
	// 1 - textiles
	// 2 - paper, palp, printing
	// 3 - chemical industry
	// 4 - glas, ceramics, cement
	// 5 - metals
	// 6 - other
	double (*conc_man)[7];

	double (*conc_tds)[4]; // tds concentration in return flow for sepc class 1-4 [mg/l]
	double *conc_urb;    // concentration of urban runoff [mg/l] FC: cfu/100ml
	double *c_geogen;    // geogen background concentration, country values [mg/l]

	// tds concentration in return flow for sepc class 1-4 [mg/l]
	// 0 - coal; 1- Aluminium; 2 - gold; 3 - copper; 4 - bauxit
	double (*conc_mining)[5];

	// ######################### cell input ###############################
	// Hintergrundkonzentration, geogen background concentration [t/km3]
	double *c_geogen_cell;
	double (*pop_cell)[3];  // population total, urban, rural population [number]
	double *rtf_man_cell;     // manufacturing returnflow [m3/a]

	// mining returnflow for resource [m3/a]
	// 0 - coal; 1- Aluminium; 2 - gold; 3 - copper; 4 - bauxit
	double (*rtf_mining_cell)[5];

	double *rtf_irr_cell;  // irrigation returnflow [m3/a]
	double *gdp;           // gdp per capita [US$]
	int    *salinity;      // primary salinity of soil 1 no, 2 yes
	int    *humidity;      // humid: 1, arid: 2
	double *cell_area;     // cell area [km2]
	double *built_up_frac; // built up fraction of cell area [-] (versiegelte Flaeche)
	short  *LU_cell;       // land use class
	double *soil_erosion;  // mean soil erosion [kg/km�/yr] (e.g. FAO LADA erosion data)
	double *ld_Patmdepos_rate;   // mean atmospheric TP deposition rate [kg/km�/yr] (e.g. Mahowald et al. 2008)
	double *ld_Pcweathering_rate;// mean TP chemical weathering rate [kg/km�/yr] (e.g. Hartmann et al. 2014)
	int *country_cell;           // isonum - country for cell

	int *gcrc;   // gcrc[gcrc-1]=ArcID
	int *grow;   // grow[gcrc-1]=row

	float *G_area;     // cell area, Wert fuer jede Zeile, nrows
	char *G_land_area; //fraction of cell which is land area [%]

	double (*urban_runoff_cell)[12];  // [mm/mon]
	//double (*cell_runoff_total)[12];  // [km3/month]
	//double (*cell_runoff_total_year);      // [km3/year]         Jahressumme
	short  (*temperature_cell)[12];   // [100 deg Celsius]
	double (*temperature_cell_year);      // [deg Celsius]  Mittlere Jahrewswert
	//double (*actSurRunoff); 	 //surface runoff - urban runoff aus watergap
	char   (*perc_days)[12];    //number of days with rain each month and cell
	short  (*precipitation_cell)[12];   // [mm]

	double (*sur_runoff_cell)[12];	//surface runoff [mm/month = L/(m^2*month)]
	double *sur_runoff_cell_mean;		//mean annual surface runoff[mm/a]
	double (*gw_runoff_cell)[12];	//groundwater inflow into streams [mm/month = L/(m^2*month)]
	double *sur_runoff_cell_year;		//surface runoff [mm/a]
	double (*ls_cell)[12];			//number of animal on cell [animal/(cell*a)]
	short *crop_type;						//crop type dominant on cell
	float *cropland_area;     // cropland area in the cell
	float *rate_p_cell;     // cropland area in the cell

	// ######## PESTIZIDE
	double *organic_carbon; // Gehalt an organischen Kohlenstoff im Boden [%] (pesticide)
	// Masse des in Zelle applizierten Pestizids [t/mon];
	//Input nur für Monate, wo es Eintrag gab, sonst == 0
	double *ld_input_pest;
	double DT50soil; // Halbwertszeit des Pestizids im Boden [month] TODO: Einheit bestimmen!
	double koc; // Bodenadsorptionkoeffizient [-]
	double WBZ; // Breite von Pufferrandstreife [m]
	double* WBZ_cell; // Breite von Pufferrandstreife [m], Zellwerte

//	double *g_slope;    // [m/m]  Prozent: g_slope*100
	double *g_land_slope;    // [%]

	// PESTIZIDE #########

	// percentage of sewage treatment plants (STP) not working or in a bad condition in a country
	double *stp_failure;

	// percentage of failure of septic tanks, pit latrines, composting toilets and to take the unknown of bucket and unknown place into account
	double treat_failure; //[1]

	// percent of total pop connected to septic tanks, pit latrines, unknown place, bucket latrine and composting toilet -> some treatment but much is unknown
	double *connec_treat_unknown;

	// percent of total population connected to hanging latrines
	double *hanging_lat;

	// percent of total population practicing open defecation
	double *open_defec;

	//variable with is !=0 if the sewage disposal types are !=100%
	double miss_conn_rate;

	// ######################### output ###############################
	double (*calc_con)[3];  // total, urban, rural connectivity [%]
	double (*frac_dom)[5];  // fraction of no treatment, primary, secondary, tertiary, quaternary domestic connectivity [%]
	double (*frac_man)[5];  // fraction of no treatment, primary, secondary, tertiary, quaternary manufacturing connectivity [%]
	double (*frac_sc)[5];   // fraction of no treatment, primary, secondary, tertiary, quaternary scattered settlements connectivity [%]

	double (*ld_untr_dom);  // untreated domestic country load [t/a] FC: [10^10 no/a]
	double (*ld_treat_dom); // treated domestic country load [t/a]   FC: [10^10 no/a]

	double (*ld_untr_urb);  // untreated urban country load [t/month] FC: [10^10 no/a]
	double (*ld_treat_urb); // treated urban country load [t/month]   FC: [10^10 no/a]

	// treated direct manufacturing country load [t/a]     FC: [10^10 no/a]
	double (*ldd_treat_man);
	// untreated indirect manufacturing country load [t/a] FC: [10^10 no/a]
	double (*ldi_untr_man);
	// untreated indirect manufacturing country load, manufacturing fractions [t/a]     FC: [10^10 no/a]
	double (*ldi_untr_man_fra)[7];
	double (*ldi_treat_man);  // treated indirect manufacturing country load [t/a]   FC: [10^10 no/a]
	double (*ld_treat_man);   // treated total manufacturing country load [t/a]      FC: [10^10 no/a]

	double (*ld_untr_sc);      // untreated load from scattered settlements [t/a] FC: [10^10 no/a] - load which is produced before any treatment processes begin
	double (*ld_treat_sc);     // treated load from scattered settlements sub-sector pit latrines, septic tank, unknown etc. [t/a]   FC: [10^10 no/a]
	double (*ld_diff_untr_sc); // load from open defecation e.g. diffuse scattered settlements [t/a]   FC: [10^10 no/a]
	double (*ld_hanging_l);    // load from hanging latrines country value [t/a], FC [10^10 cfu/a]

	double (*ld_treat_mining);  // treated mining country load [t/a]
	double (*ld_untr_mining);  // untreated mining country load [t/a]
//	double (*ld_untr_mining_res)[5];  // untreated mining country load, resources [t/a]

	double (*ld_irr);  // load from irrigation return flow [t/a]
	// salt: load from irrigated agriculture (ld_irr - s_geogen) [t/a]
	// pecticide: load from agriculture [t/a]
	double (*ld_agr);

	double (*ld_manure);    // load from organic fertilizer  [t/a]   FC: [10^10 cfu/a]
	double (*ld_geog_back); // load from geogenic background  [t/a]
	double (*ld_ind_fert);  // load from industrial fertilizer  [t/a]


	// ######################## cell output ###########################

	double *ld_untr_dom_cell;    // untreated domestic cell load [t/a]  FC: [10^10 no/a]
	double *ld_treat_dom_cell;   // treated domestic cell load [t/a]    FC: [10^10 no/a]

	double (*ld_untr_urb_cell)[12];  // untreated urban cell load [t/month] FC: [10^10 no/a]
	double (*ld_treat_urb_cell)[12]; // treated urban cell load [t/month]   FC: [10^10 no/a]
	double *ld_untr_urb_cell_year;  // untreated urban cell load [t/a]     FC: [10^10 no/a]
	double *ld_treat_urb_cell_year;  // treated urban cell load [t/a]       FC: [10^10 no/a]

	double *ldd_treat_man_cell; // treated direct manufacturing cell load [t/a]     FC: [10^10 no/a]
	double *ldi_untr_man_cell;  // untreated indirect manufacturing cell load [t/a] FC: [10^10 no/a]
	double *ldi_treat_man_cell; // treated indirect manufacturing cell load [t/a]   FC: [10^10 no/a]
	double *ld_treat_man_cell;  // treated total manufacturing cell load [t/a]      FC: [10^10 no/a]

	double *ld_untr_mining_cell;  // untreated mining cell load [t/a]
	double *ld_treat_mining_cell; // treated total mining cell load [t/a]

	double *ld_untr_sc_cell;      // untreated load from scattered settlements [t/a]  FC: [10^10 no/a]
	double *ld_treat_sc_cell;     // treated load from scattered settlements sub-sector pit latrines, septic tank, unknown etc. [t/a]    FC: [10^10 no/a]
	double *ld_diff_untr_sc_cell; // load from open defecation e.g. diffuse scattered settlements [t/a]   FC: [10^10 no/a]
	double *ld_hanging_l_cell;    // load from hanging latrines cell value [t/a], FC [10^10 cfu/a]



	int    *Salt_class;  // natural salt class 1 - 4 : low to high salinity
	double *GDPC;        // gdp class: 1 - 3 , low to high gdp per capita
	int    *SEPC;        // salt concentration in return flow (mg/l) derived from salt_class and gdp class
	double *ld_irr_cell; // treated load from irrigation return flow [t/a]

	// salt: treated load from irrigated agriculture (ld_irr - s_geogen) [t/month] cell_runoff_total * c_geogen
	// pesticide: load from agriculture [t/month]
	double (*ld_agr_cell)[12];

	// pesticide: Masse des Pestizids [t/month]
	double (*ld_agr_soil_cell)[12];

	// salt: treated load from irrigation return flow [t/a]
	// pesticide: load from agriculture [t/a]
	double *ld_agr_cell_year;

	double *ld_manure_cell;      //load from organic fertilizer (livestock) [t/a], FC [10^10 cfu/a]
	double *ld_inorg_fert_cell; //load from inorganic fertilizer [t/a]

	// for salt calculated month-values ld_background_cell,
	// for other parameter year-value ld_background_cell_year, that will be split in monthValues with act_surface_runoff
	double *ld_background_cell_year;	  //load of total geogenic background [t/a], FC [10^10 cfu/a]
	double *ld_Patmdepos_cell_year;		//load of atmospheric P deposition [t/a]
	double *ld_Pcweathering_cell_year;	//load of P chemical weathering [t/a]
	double (*ld_background_cell)[12];	  //load of geogenic background [t/month] for salt

	int  get_country_input();
	int  get_parameter_input();
	int  get_country_parameter_input();
	int  get_cell_input();

//	int  get_cell_parameter_input(double *c_geogen_cell, options_wq_loadClass* optionsWQ_load_ptr, const continentStruct* continent_ptr);
	int  get_cell_parameter_input();

	int  get_cell_pesticide_input();  // read organic_carbon
	int  get_cell_pesticide_input(int month);
	int  get_cell_pesticide_lastYear(int month, double* ld_dez, const string tab);
	void calc_ld_agr_cell(int cell, int month, double ld_agr_soil_lastMonth, double ld_agr_lastMonth);

	//int  get_cell_runoff(int actual_year, double (*cell_runoff)[12], short InputType, char *input_dir, int *gcrc);

	void CountryLoad(); // calculate country loading
	void CellLoad();    // calculate cell loading
	void LoadAgr();     // calculate calculate ld_agr_cell, ld_agr_cell_year, ld_agr
	void CountryValuesOutput();

	// Monatswerte rechnen und in die Tabellen speichern; wenn gebraucht Jahreswerte mitkorrigieren
	int insert_into_tab(int year, const string tab, double *ld_value_year, double (*corr_factor)[12]);
	int insert_into_tab(int year, const string tab, double (*ld_value)[12]);
	int insert_into_tab(int year, const string tab, double *ld_value_year,
			double (*act_surface_runoff)[12], double *act_surface_runoff_year, double *act_surface_runoff_summer
			, short manure_timining);
	int  monthValues();
	// Jahreswerte in die Tabellen schreiben
	int yearValues();
	double getRainDays(double ld_init, double ks, char (*perc_days)[12], int cell, int year,  const double teta, short (*temperature_cell)[12], short flag);


};

#endif /* CLASSWQLOAD_H_ */
