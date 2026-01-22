/*
 * general_functions.cpp
 *
 *  Created on: 08.01.2013
 *      Author: reder
 */

#include "general_functions.h"
#include <string>


using namespace std;
using namespace mysqlpp;

const int   NODATA                 = -9999;
//const int   IDVersion_const        = 3;     // in watergap_unf.watergap_region(IDVersion, IDRegion)
//const int   IDReg_const            = 1;     // 2 version watergap2, region=world   3 version watergap3, region=1 Europa

//###########################METHODS TO CALCULATE HYDROLOGY#######################

/*
 * Method to return the outflow cells as an array
 */
int get_outflc(int *g_outflc, const optionsClass* options_ptr, const continentStruct* continent_ptr) {
	int j, cell;
	char sql[2000]; // variable for SQL-query
	mysqlpp::StoreQueryResult res;
	mysqlpp::Query query = con.query();

	//query.reset();
	j=sprintf(sql,    "SELECT g.`cell`, g.`outflowCell` FROM watergap_unf.`g_outflc` g ");
	j+=sprintf(sql+j, "WHERE g.IDVersion=%d AND g.IDReg=%d;", options_ptr->IDVersion, options_ptr->IDReg );
	cout << sql << endl;

	query.reset();
	query << sql;
	res = query.store();

//	try
	{

		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
		{

			cell=res[rowNum]["cell"];
			if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in watergap_unf.g_outflc.\n"; return 1;}
			g_outflc[cell-1]=res[rowNum]["outflowCell"];
		}
	}//try for res

	return 0;
} // end of get_outflc()



/*
 * method to get one outflow cell - this is very time consuming, better to use
 * get_outflc once at the beginning
 */
int get_outflowCell(int cell, const optionsClass* options_ptr){
	int j;
	char sql[2000]; // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	query.reset();
	j=sprintf(sql, "SELECT g.outflowCell FROM watergap_unf.g_outflc g ");
	j+=sprintf(sql+j, "WHERE g.cell=%d AND g.IDVersion=%d AND g.IDReg=%d;", cell, options_ptr->IDVersion, options_ptr->IDReg);
	query << sql;
	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl;
	#endif
	res = query.store();
	int outflowCell=res.at(0)["outflowCell"];

	return outflowCell;

} // end of get_outflowCell()

/*
 * Method returns an array with all the possible 8 neighbouring inflow cells
 */
int get_inflow(int (*inflow)[8], const optionsClass* options_ptr, const continentStruct* continent_ptr) {
	int j, cell, errorValue=0;
	char sql[2000]; // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	query.reset();
	j=sprintf(sql, "SELECT g.cell, g.SW, g.S, g.SE, g.sink, g.W, g.E, g.NW, g.N, g.NE FROM watergap_unf.g_inflc_arc_id g \n");
	j+=sprintf(sql+j, "WHERE g.IDVersion=%d AND g.IDReg=%d;", options_ptr->IDVersion, options_ptr->IDReg );
	query << sql;
	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif
	  res = query.store();
	if (res.empty()) return 1;  // Zelle nicht in berechnetem Region

//	try
	{

		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
		{
			cell=res[rowNum]["cell"];
			if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in g_inflc_arc_id.\n"; return 1;}
			inflow[cell-1][0]=res[rowNum]["SW"];    if (inflow[cell-1][0]>continent_ptr->ng) errorValue=1;
			inflow[cell-1][1]=res[rowNum]["S"];     if (inflow[cell-1][1]>continent_ptr->ng) errorValue=1;
			inflow[cell-1][2]=res[rowNum]["SE"];    if (inflow[cell-1][2]>continent_ptr->ng) errorValue=1;
			inflow[cell-1][3]=res[rowNum]["W"];     if (inflow[cell-1][3]>continent_ptr->ng) errorValue=1;
			inflow[cell-1][4]=res[rowNum]["E"];     if (inflow[cell-1][4]>continent_ptr->ng) errorValue=1;
			inflow[cell-1][5]=res[rowNum]["NW"];    if (inflow[cell-1][5]>continent_ptr->ng) errorValue=1;
			inflow[cell-1][6]=res[rowNum]["N"];     if (inflow[cell-1][6]>continent_ptr->ng) errorValue=1;
			inflow[cell-1][7]=res[rowNum]["NE"];    if (inflow[cell-1][7]>continent_ptr->ng) errorValue=1;
		}
	}//try for res


	if (errorValue) cerr << "error in watergap_unf.watergap_unf.g_inflc_arc_id\n";
	return errorValue;
} // end of get_inflow()

/*
 * Method returns temperature dependent decay value for each cell
 * furthermore, c_geogen (background geogen conc was used for old implementation)
 * and decay rate lake -> decay in lakes was tried to model but did not work jet
 * once lake decay is modeled this can be used (19.04.2013).
 */
int get_factor(int IDrun, double *factor, double *c_geogen, double *factor_lake
		, int toCalc, const optionsClass* options_ptr, const continentStruct* continent_ptr) {
	int j, cell;
	char sql[2000]; // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	if (toCalc==0)
		for (int cell = 1; cell<continent_ptr->ng; cell++) {
			factor[cell-1] = 0.00000266;
			factor_lake[cell-1] = NODATA;
		}
	else if (toCalc==2)
		for (int cell = 1; cell<continent_ptr->ng; cell++) {
			factor[cell-1] = 0.00000944;
			factor_lake[cell-1] = NODATA;
		}
	else if (is_c_geogen_country && toCalc==1) { 
        query.reset();
        j=sprintf(sql, "SELECT f.cell, IFNULL(f.c_geogen, %d) as `c_geogen` ", NODATA);
        j+=sprintf(sql+j, " FROM %sworldqual_%s.factors_tds f;", MyDatabase, options_ptr->continent_abb);
	
        query << sql;
        #ifdef DEBUG_queries
        cout << "Query: " << query << endl << endl; //cin >> j;
        #endif
        res = query.store();

        if (res.empty()) {
            cerr << "get_factor: error in table factors_tds. table is not exists or empty." << endl;
            return 1;  // Zelle nicht in berechnetem Region
        }

        //    try
        {
            for (unsigned int rowNum=0; rowNum<res.size(); rowNum++) {
                cell=res[rowNum]["cell"];
                if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in get_factor.\n"; return 1;}
                factor[cell-1]      = 0.;
                c_geogen[cell-1]    = res[rowNum]["c_geogen"];
                factor_lake[cell-1] = NODATA;
		}//for
            }
    	}
	else  {
		query.reset();
		j=sprintf(sql, "SELECT f.cell, IFNULL(f.decomposition,%d) as `decomposition` ", NODATA);
		if (is_c_geogen_country) j+=sprintf(sql+j, ", IFNULL(f.c_geogen, %d) as `c_geogen`", NODATA);
		j+=sprintf(sql+j, ", IFNULL(f.decay_rate_lake,%d) as `decay_rate_lake` FROM %sworldqual_%s.factors f WHERE f.IDrun=%d;", NODATA, MyDatabase, options_ptr->continent_abb, IDrun);

		query << sql;
		#ifdef DEBUG_queries
		cout << "Query: " << query << endl << endl; //cin >> j;
		#endif
		res = query.store();

		if (res.empty()) {
			cerr << "get_factor: error in table factors. table is not exists or empty." << endl;
			return 1;  // Zelle nicht in berechnetem Region
		}

		//	try
		{
			if (is_c_geogen_country && toCalc==1) {
				for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
				{
					cell=res[rowNum]["cell"];
					if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in get_factor.\n"; return 1;}
					factor[cell-1]      = res[rowNum]["decomposition"];
					c_geogen[cell-1]    = res[rowNum]["c_geogen"];
					factor_lake[cell-1] = res[rowNum]["decay_rate_lake"];


				}
			} else {
				for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
				{
					cell=res[rowNum]["cell"];
					if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in get_factor.\n"; return 1;}
					factor[cell-1]      = res[rowNum]["decomposition"];
					factor_lake[cell-1] = res[rowNum]["decay_rate_lake"];


				}
			}

		}//try for res
	}



	return 0;
} // end of get_factor()

/*
 * Method to return the cell runoff from UNF
 */
int get_cell_runoff(int actual_year, double (*cell_runoff)[12], int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr) {
	char filename[maxcharlength];
	sprintf(filename,"%s/G_CELL_RUNOFF_%d.12.UNF0", options_ptr->path_watergap_output, actual_year);

	if (readInputFile(filename, 4, 12, continent_ptr->ng, &cell_runoff[0][0], gcrc, options_ptr, continent_ptr)) return 1;

	return 0;

} // end get_cell_runoff()

/*
 * Method to return the groundwater runoff from UNF
 */
int get_gw_runoff(int actual_year, double (*cell_runoff)[12], int *gcrc, int *grow, const options_wqClass* options_ptr, const continentStruct* continent_ptr) {
	//===========================================================
	//cell_run_off aus UNF-Datei
	char filename[maxcharlength];
	sprintf(filename,"%s/G_GW_RUNOFF_%d.12.UNF0", options_ptr->path_watergap_output, actual_year);

	// mm/month
	if (readInputFile(filename, 4, 12, continent_ptr->ng, &cell_runoff[0][0], gcrc, options_ptr, continent_ptr)) return 1;

	int valuesPerCell = 12;
	///=================================
	//
	float *G_area    = new float[continent_ptr->nrows];

	if (readArea(G_area, NULL, gcrc, options_ptr, continent_ptr)) return 1; // NULL, weil G_land_area wird nicht gebraucht

	for (int cell=0; cell<continent_ptr->ng; cell++) {
		for (int month=0; month<valuesPerCell; month++) {
			// continent[4] - cellFraction
			//cell_runoff[gcrc[cell]-1][month] /=(G_area[grow[gcrc[cell]-1]]*G_landfreq[gcrc[cell]-1]/continent[4]);
			cell_runoff[cell][month] *=(G_area[grow[cell]-1]*1.e-6);

		}
	} // for(cell)

	delete[] G_area;     G_area=NULL;
	return 0;

} // end gw_cell_runoff()

/*
 * Method to return the river availability from UNF
 */
int get_Q(int actual_year, double (*Q_out)[12], int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr) {

	char filename[maxcharlength];
	sprintf(filename,"%s/G_RIVER_AVAIL_%d.12.UNF0", options_ptr->path_watergap_output, actual_year);

	if (readInputFile(filename, 4, 12, continent_ptr->ng, &Q_out[0][0], gcrc, options_ptr, continent_ptr, 0.)) return 1;

	return 0;
} // end of get_Q()


/*
 * Method to return the water temperature the database
 * If the database is empty water_temperature.cpp has to be run
 * Method without IDScen in parameter list for normal run
 * Method with IDScen is for the sensitvity analysis where the water temperature
 * has different scenarios
 */
int get_waterTemp(int actual_year, double (*water_temp)[12], short UseWaterTemp, int IDTemp
		, const optionsClass* options_ptr, const continentStruct* continent_ptr, int IDScen) {

	if (!UseWaterTemp) return 0;  // Wassertemperatur wird nicht gebraucht
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	int j, cell;
	//===========================================================

	char sql[2000]; // variable for SQL-query

	char  climateTemp[maxcharlength];
	{
		query.reset();
		j=sprintf(sql,  "SELECT IFNULL(w.Name,IDTemp)  as `Name`, IFNULL(w.climate, '') as `climate`, IFNULL(w.`comment`, '') as `comment` ");
		j+=sprintf(sql+j,  "FROM wq_general._water_temperature_list w WHERE w.`IDTemp`=%d;", IDTemp);
		query << sql;
		res = query.store();
		if (res.empty()) {cerr << "ERROR: Wrong parameter: IDTemp" << endl; return 1;}  // falsche IDTemp
		mysqlpp::String NameTemp=res.at(0)["Name"];
		mysqlpp::String climateTemp_str=res.at(0)["climate"];
		sprintf(climateTemp,"%s",climateTemp_str.c_str());
		mysqlpp::String commentTemp=res.at(0)["comment"];
		cout << "IDTemp = " <<IDTemp<<" (" << NameTemp <<" "<< commentTemp<<") Tabellen wq_climate_" << climateTemp << "_*"<<endl;

	}
	query.reset();

	j=sprintf(sql,    "SELECT cell, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12\n ");
	j+=sprintf(sql+j, " FROM wq_climate.%s_water_temperature_%s  w ", climateTemp, options_ptr->continent_abb);
	j+=sprintf(sql+j, " WHERE w.IDTemp=%d AND w.year=%d", IDTemp, actual_year);
	if (IDScen>0) {
		j+=sprintf(sql+j, " AND w.`IDScen`=%d", IDScen);
	}
	j+=sprintf(sql+j, ";");
	query << sql;
	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif
	  res = query.store();
	if (res.empty()) {
		cerr << "get_waterTemp:  error in water_temperature, IDTemp "<<IDTemp<<", year " << actual_year;
		if (IDScen>0) cerr << ", IDScen " << IDScen;
		cerr << " not exist!"<<endl;
		return 1;
	}

//	try
	{

		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
		{
			cell=res[rowNum]["cell"];
			if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in g_waterTemp.\n"; return 1;}

			water_temp[cell-1][0]  = res[rowNum]["v1"];
			water_temp[cell-1][1]  = res[rowNum]["v2"];
			water_temp[cell-1][2]  = res[rowNum]["v3"];
			water_temp[cell-1][3]  = res[rowNum]["v4"];
			water_temp[cell-1][4]  = res[rowNum]["v5"];
			water_temp[cell-1][5]  = res[rowNum]["v6"];
			water_temp[cell-1][6]  = res[rowNum]["v7"];
			water_temp[cell-1][7]  = res[rowNum]["v8"];
			water_temp[cell-1][8]  = res[rowNum]["v9"];
			water_temp[cell-1][9]  = res[rowNum]["v10"];
			water_temp[cell-1][10] = res[rowNum]["v11"];
			water_temp[cell-1][11] = res[rowNum]["v12"];

		}
	}//try for res
/*	catch (const mysqlpp::EndOfResults&) {
		// Last query result received.  Exit normally.
	} // catch
*/
	//===========================================================

	return 0;

} // end get_waterTemp() with IDScen

/*
 * Method to calculate the flow velocity
 */
double get_flowVelocity(double Q_out, double g_slope) {   // [m/sec]

	double flowVelocity;
	double n  = 0.035;  // [s/m**(1/3)]
	double b  = 2.71 * pow(Q_out, 0.557); // this must be substituted with bankfull discharge
	double h  = 0.35 * pow(Q_out, 0.341);
	double r_hyd;

	if ((b + 2.0*h)==0)
		r_hyd = 0.0;
	else
		r_hyd   = b * h /(b + 2*h);

	flowVelocity   = 1./n * pow(r_hyd, 2.0/3.0) * pow(g_slope, 0.5);


	if (flowVelocity<=0.1)
		flowVelocity=0.1;

	return flowVelocity;
} // end of get_flowVelocity

bool getTeta(double &teta, double &teta_lake, const int IDScen, const int parameter_id, const int year) {

	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
		//mysqlpp::Row    rowQuery;
	char sql[2000];                        // variable for SQL-query

	int MaxYear;
	if(getYear_parameter_input(MaxYear, IDScen, parameter_id, year)) return 1;

	query.reset();
	int j=sprintf(sql, "SELECT IFNULL(p.`teta`,-1) as `teta`");
	j+=sprintf(sql+j, ", IFNULL(p.`teta_lake`,IFNULL(p.`teta`,-1)) as `teta_lake` ");  // wenn keine teta_lake eingegeben ist, teta nutzen
	j+=sprintf(sql+j, " FROM %swq_load.parameter_input p", MyDatabase);
	j+=sprintf(sql+j, " WHERE p.IDScen=%d AND p.parameter_id=%d AND p.time=%d;", IDScen, parameter_id, MaxYear);
	query << sql;
	//cout<<"query: "<<query<<endl;
	resQuery = query.store();
	if (resQuery.empty()) {
		cerr << "Error in getTeta: parameter_id wrong!\n" << endl;
		return 1;
	}

	teta = resQuery.at(0)["teta"];
	teta_lake = resQuery.at(0)["teta_lake"];
	cout << "year:\t"<< year << "\tTETA:\t"<<teta<<"\tTETA_lake:\t"<<teta_lake<<endl;

	return 0;
}

bool  getYear_parameter_input(int &MaxYear, const int IDScen, const int parameter_id, const int year) {
	cout << "getYear_parameter_input():\n";

	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
	char sql[2000];                        // variable for SQL-query

	// Feld time in der Tabelle soll als "YearFrom" betrachtet werden, finden aus welchem Jahr Daten eingelesen werden:
	// maximale Jahr, der nicht gr��er als year ist
	int j=sprintf(sql, "SELECT max(p.time) as MaxYear FROM %swq_load.parameter_input p", MyDatabase);
	j+=sprintf(sql+j, " WHERE p.IDScen=%d AND p.parameter_id=%d AND p.time<=%d;", IDScen, parameter_id, year);

	query.reset();
	query << sql;

#ifdef DEBUG_queries
cout << "Query: " << query << endl << endl; //cin >> j;
#endif
	resQuery = query.store();
	if (resQuery.at(0)["MaxYear"]==mysqlpp::null) {
		cerr << "Error in getYear_parameter_input: data for IDScen " << IDScen
				<< " parameter_id " << parameter_id << " and year " << year
				<< " not exist!" << endl;
		return 1;
	} else {
		MaxYear = resQuery.at(0)["MaxYear"];
	}

	return 0;
}

//#######################METHODS TO CALCULATE WATER CONCENTRATIONS################

/*
 * Method to load TSS values, basin averages, modeled data - at the moment from
 * Beusen et al. 2005. Once we model TSS ourselves this method should be updated!
 * Only used for FC at the moment (19.04.2013).
 */
int getTSSBasinValues(double *tss_cell, const int IDReg, const continentStruct* continent_ptr){
	int j;

	char sql[20000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	query.reset();
	j=sprintf(sql, "SELECT g.arc_id, m.`Yld_TSS` FROM wq_orig_data_fc.globalnews_worldqual_ids g, wq_orig_data_fc.globalnews_modeled_data m ");
	j+=sprintf(sql+j, " WHERE g.`IDReg`=%d AND g.`basinNr`=m.`basinNr`; ", IDReg);
	query << sql;
	//cout<<"query: "<<query<<endl;
	res = query.store();


	if (res.empty()) {cerr << "getTSSBasinValues: error in operation to get TSS.\n"; return 1;}
	{
		int cell;
		//cout<<"res.size(): "<<res.size()<<endl;
		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){
			cell = res[rowNum]["arc_id"];
			if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in getTSSBasinValues.\n"; return 1;}
			tss_cell[cell-1] 	= res[rowNum]["Yld_TSS"];
		}
	}
	return 0;
}

/*
 * Method to calculate the attenuation coefficient
 * Only used for FC at the moment (19.04.2013)
 */
int calculateAttenuation(double *ke,  double *tss_cell, int IDScen, int parameter_id, int year,
		double (*Q_out)[12], int *gcrc, int *grow, const optionsClass* options_ptr, const continentStruct* continent_ptr)
{
	double *keRel = new double[2];
	//loads regression parameters alpha and beta from the database
	getKeRelations(keRel, IDScen, parameter_id, year);

	double alpha = keRel[0];		//from linear regression of ke and TSS
	double beta = keRel[1];		//from linear regression of ke and TSS

	//std::list<double> listArray[continent_ptr->ng];
	//std::list<double>::const_iterator iter;

	//cout<<" alpha: "<<alpha<<" beta: "<<beta<<endl;


	float *G_area    = new float[continent_ptr->nrows];
	char *G_land_area = new char[continent_ptr->ng];
	double *cell_area = new double[continent_ptr->ng];

	if (readArea(G_area, G_land_area, gcrc, options_ptr, continent_ptr)) return 1;;

	//Initialization
	for(int m=0; m<continent_ptr->ng;m++){
		ke[m]=NODATA;
	}


	double Q_out_year=0.0;
	for(int cell=0; cell<continent_ptr->ng;cell++){
		// modeled tss data is used, as regression function
		for(int month=0;month<12;month++){
			Q_out_year+=Q_out[cell][month];
		}
		cell_area[cell]=G_area[grow[cell]-1]*(((int)G_land_area[cell])/100.0);
		if(Q_out_year==0.0) ke[cell]=NODATA;
		else ke[cell]=alpha*((tss_cell[cell]*cell_area[cell]*pow(10,-3))/(Q_out_year*365))+beta;	//Mg/km^3 in mg/L -> 1 Mg = 10^9 mg and 1km^3 = 10^12 L; in days because the regression is made for single days and TSS ld is the sum of a whole year

		//cout<<"ke[cell]="<<ke[cell]<<" alpha="<<alpha<<" tss_cell[cell]="<<tss_cell[cell]<<" Q_out_year="<<Q_out_year<<" Q_out[cell][7]="<<Q_out[cell][7]
		//		<<" beta"<<beta<<" cell:"<<cell<<"\ncell_area[cell]="<<cell_area[cell]<<" G_area[grow[cell]-1]="<<G_area[grow[cell]-1]<<" (int)G_landfreq[cell]"<<(int)G_landfreq[cell]<<endl;
		Q_out_year=0.0;
	}


	/*for(int j=0; j<continent_ptr->ng;j++){
			cout<<"ke[j]="<<ke[j]<<" j= "<<j<<endl;
	}*/

	delete[] keRel;       keRel = NULL;
	delete[] G_area;      G_area = NULL;
	delete[] G_land_area; G_land_area = NULL;
	delete[] cell_area;   cell_area = NULL;

	cout<<"calculateAttenuation done"<<endl;
	return 0;
}


/*
 * Method to load the regression parameters alpha and beta from the database
 * for the ke -> TSS relation
 */
int getKeRelations(double *keRel, const int IDScen, const int  parameter_id, const int  year){
	int j;
	char sql[20000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	int MaxYear;
	if(getYear_parameter_input(MaxYear, IDScen, parameter_id, year)) return 1;

	query.reset();
	j=sprintf(sql, "SELECT p.`ke_tss_reg_alpha`, p.`ke_tss_reg_beta` FROM %swq_load.parameter_input p ", MyDatabase);
	j+=sprintf(sql+j, " WHERE p.`IDScen`=%d AND p.`parameter_id`=%d AND p.`time`=%d ;", IDScen, parameter_id, MaxYear );
	query << sql;
	//cout<<"query: "<<query<<endl;
	res = query.store();


	if (res.empty()) {cerr << "getKeRelations: error in operation to get getKeRelations in table parameter_input.\n"; return 1;}
	{

		keRel[0] 	= res[0]["ke_tss_reg_alpha"];
		keRel[1] 	= res[0]["ke_tss_reg_beta"];

	}
	return 0;
}

/*
 *Method to load the sunlight inactivation and sediment velocity rate from the database
 *Only used for FC at the moment (19.04.2013).
 */
int getDecayRates(double *decRate,  int IDScen, int parameter_id, int year){
	int j;
	char sql[20000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	int MaxYear;
	if(getYear_parameter_input(MaxYear, IDScen, parameter_id, year)) return 1;

	query.reset();
	j=sprintf(sql,    "SELECT IFNULL(p.`sed_veloc`,%d) as 'sed_veloc' ,  IFNULL(p.`ks`,%d) as 'ks' ", NODATA, NODATA);
	j+=sprintf(sql+j, "FROM %swq_load.`parameter_input` p ", MyDatabase);
	j+=sprintf(sql+j, "WHERE p.`idScen`=%d AND p.`parameter_id`=%d AND p.`time`=%d ; ", IDScen, parameter_id, MaxYear);
	query << sql;
	//cout<<"query: "<<query<<endl;
	res = query.store();

	if (res.empty()) {cerr << "getDecayRates: error in table parameter_input.\n"; return 1;}
	{

		decRate[0]    = res[0]["sed_veloc"];
		decRate[1]	  = res[0]["ks"];
	}
	cout<<"getDecayRates done"<<endl;
	return 0;
}

/*
 * Method to get the calculated load for each cell from the database
 */
int get_s_cell(int IDScen, int parameter_id, int actual_year, double (*s_cell)[12],
		const optionsClass* options_ptr, const continentStruct* continent_ptr, short loadingType, int toCalc) {
	cout << "get_s_cell: loadingType = " << loadingType << endl;
	if (loadingType==2) return get_s_cell_direct(IDScen, parameter_id, actual_year, s_cell, options_ptr, continent_ptr, loadingType, toCalc);
	
	int j, cell, month;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	//int Time_load;                         // in Initialisierungsjahren load f�r Startjahr nutzen
	//if (actual_year<start) Time_load = start*100 + month;
	//else                   Time_load = actual_year*100 + month;

	for (cell=0; cell<continent_ptr->ng; cell++)
		for (month=1; month<=12; month++)
			s_cell[cell][month-1]=-9999;
	
	char TableName[maxcharlength];
	switch (loadingType) {
		case 0: 
			sprintf(TableName, "%sworldqual_%s.`load`", MyDatabase, options_ptr->continent_abb);
			j=sprintf(sql,    "SELECT cell, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12 from %s", TableName);
			j+=sprintf(sql+j, " WHERE l.`IDScen`=%d and l.`parameter_id`=%d and l.`year`=%d;", IDScen, parameter_id, actual_year);
			break;
		case 1:
			sprintf(TableName, "%sworldqual_%s.`load_%d_%d`", MyDatabase, options_ptr->continent_abb, IDScen, parameter_id);
			j=sprintf(sql,    "SELECT cell, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12 from %s", TableName);
			j+=sprintf(sql+j, " WHERE l.`year`=%d;", actual_year);
			break;
	}

	query.reset();
	query << sql;

//	#ifdef DEBUG_queries
//	  cout << "Query: " << query << endl << endl; //cin >> j;
//	#endif
	  res = query.store();
	if (res.empty()) {cout << "get_s_cell: error in table load.\n"; return 1;}

//	try
	{

		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
		{
			cell=res[rowNum]["cell"];
			if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in load.\n"; return 1;}

			s_cell[cell-1][0]  = res[rowNum]["v1"];
			s_cell[cell-1][1]  = res[rowNum]["v2"];
			s_cell[cell-1][2]  = res[rowNum]["v3"];
			s_cell[cell-1][3]  = res[rowNum]["v4"];
			s_cell[cell-1][4]  = res[rowNum]["v5"];
			s_cell[cell-1][5]  = res[rowNum]["v6"];
			s_cell[cell-1][6]  = res[rowNum]["v7"];
			s_cell[cell-1][7]  = res[rowNum]["v8"];
			s_cell[cell-1][8]  = res[rowNum]["v9"];
			s_cell[cell-1][9]  = res[rowNum]["v10"];
			s_cell[cell-1][10] = res[rowNum]["v11"];
			s_cell[cell-1][11] = res[rowNum]["v12"];
//			geogen_info[cell-1]= res[rowNum]["geogen"]; // wenn==0 kein geogener Hintergrund beruecksichtigen

			for (int month=1; month<=12; month++)
			  if (s_cell[cell-1][month-1] <0.0){
				  s_cell[cell-1][month-1]=0.0;
				  cerr<< "cell\t"<< cell << "\tmonth" << month << "\tload is negative - this should never happen!"<<endl;
			  }

		}
	}//try for res

	return 0;

} // end of s_cell()


/*
 * Method to get the calculated load for each cell from the database
 */
int get_s_cell_direct(int IDScen, int parameter_id, int actual_year, double (*s_cell)[12],
		const optionsClass* options_ptr, const continentStruct* continent_ptr, short loadingType, int toCalc) {
		cout << "get_s_cell_direct " << loadingType<< endl;
	
	if (loadingType!=2) return 1;  // nur für direkten einlesen loadings aus Ergebnisse wq_load
	loadStruct loadingStruct;
	fillLoadStruct (&loadingStruct, toCalc);
	
	int j, cell, month;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	//int Time_load;                         // in Initialisierungsjahren load f�r Startjahr nutzen
	//if (actual_year<start) Time_load = start*100 + month;
	//else                   Time_load = actual_year*100 + month;

	for (cell=0; cell<continent_ptr->ng; cell++)
		for (month=1; month<=12; month++)
			s_cell[cell][month-1]=0;
	
	char TableName[maxcharlength];
	
	for (int type=0; type<loadTypes; type++) {
		if (!loadingStruct.loadType[type]) continue;
		cout << "for TableName " << loadingStruct.loadTab[type] << " " << IDScen << " " << parameter_id << endl;
		sprintf(TableName, "%swq_load_%s.calc_cell_month_load_%s_%d_%d", MyDatabase, options_ptr->continent_abb, loadingStruct.loadTab[type], IDScen, parameter_id);
		cout << TableName << endl;
		j=sprintf(sql,    "SELECT cell, IFNULL(v1,0) as v1, IFNULL(v2,0) as v2, IFNULL(v3,0) as v3, IFNULL(v4,0) as v4, IFNULL(v5,0) as v5, IFNULL(v6,0) as v6");
		j+=sprintf(sql+j, ", IFNULL(v7,0) as v7, IFNULL(v8,0) as v8, IFNULL(v9,0) as v9, IFNULL(v10,0) as v10, IFNULL(v11,0) as v11, IFNULL(v12,0) as v12 from %s l", TableName);
		j+=sprintf(sql+j, " WHERE l.`time`=%d;", actual_year);

		query.reset();
		query << sql;

			res = query.store();
		if (res.empty()) {cout << "get_s_cell_direct: error in table " << TableName << "\n"; return 1;}

		cout << "sql: " << sql << endl << res.size() << endl;
		double tmpValue;
		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++) {
			cell=res[rowNum]["cell"];
			if (cell<=0 || cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in table " << TableName << ".\n"; return 1;}


			tmpValue = res[rowNum]["v1"] ;  if (tmpValue>0) s_cell[cell-1][0] += tmpValue;
			tmpValue = res[rowNum]["v2"] ;  if (tmpValue>0) s_cell[cell-1][1] += tmpValue;
			tmpValue = res[rowNum]["v3"] ;  if (tmpValue>0) s_cell[cell-1][2] += tmpValue;
			tmpValue = res[rowNum]["v4"] ;  if (tmpValue>0) s_cell[cell-1][3] += tmpValue;
			tmpValue = res[rowNum]["v5"] ;  if (tmpValue>0) s_cell[cell-1][4] += tmpValue;
			tmpValue = res[rowNum]["v6"] ;  if (tmpValue>0) s_cell[cell-1][5] += tmpValue;
			tmpValue = res[rowNum]["v7"] ;  if (tmpValue>0) s_cell[cell-1][6] += tmpValue;
			tmpValue = res[rowNum]["v8"] ;  if (tmpValue>0) s_cell[cell-1][7] += tmpValue;
			tmpValue = res[rowNum]["v9"] ;  if (tmpValue>0) s_cell[cell-1][8] += tmpValue;
			tmpValue = res[rowNum]["v10"];  if (tmpValue>0) s_cell[cell-1][9] += tmpValue;
			tmpValue = res[rowNum]["v11"];  if (tmpValue>0) s_cell[cell-1][10]+= tmpValue;
			tmpValue = res[rowNum]["v12"];  if (tmpValue>0) s_cell[cell-1][11]+= tmpValue;
//			geogen_info[cell-1]= res[rowNum]["geogen"]; // wenn==0 kein geogener Hintergrund beruecksichtigen


		} //for(rowNum)

	} // for(type)


	return 0;

} // end of s_cell()

void fillLoadStruct (loadStruct *loadStruct_ptr, int toCalc) {
	strcpy(loadStruct_ptr->loadTab[0], "dom");        strcpy(loadStruct_ptr->loadTabAlias[0], "cd");      
	strcpy(loadStruct_ptr->loadTab[1], "urb");        strcpy(loadStruct_ptr->loadTabAlias[1], "cu");      
	strcpy(loadStruct_ptr->loadTab[2], "man");        strcpy(loadStruct_ptr->loadTabAlias[2], "cm");      
	strcpy(loadStruct_ptr->loadTab[3], "sc");         strcpy(loadStruct_ptr->loadTabAlias[3], "cs");      
	strcpy(loadStruct_ptr->loadTab[4], "open_defec"); strcpy(loadStruct_ptr->loadTabAlias[4], "cdsc");    
	strcpy(loadStruct_ptr->loadTab[5], "hanging_l");  strcpy(loadStruct_ptr->loadTabAlias[5], "hl");      
	strcpy(loadStruct_ptr->loadTab[6], "manure");     strcpy(loadStruct_ptr->loadTabAlias[6], "cmanure"); 
	strcpy(loadStruct_ptr->loadTab[7], "irr");        strcpy(loadStruct_ptr->loadTabAlias[7], "cirr");    
	strcpy(loadStruct_ptr->loadTab[8], "geog_back");  strcpy(loadStruct_ptr->loadTabAlias[8], "cg");      
	strcpy(loadStruct_ptr->loadTab[9], "ind_fert");   strcpy(loadStruct_ptr->loadTabAlias[9], "cif");     
	strcpy(loadStruct_ptr->loadTab[10], "mining");    strcpy(loadStruct_ptr->loadTabAlias[10],"cmining"); 

	strcpy(loadStruct_ptr->loadTabLongName[0], "domestic");
	strcpy(loadStruct_ptr->loadTabLongName[1], "urban surface runoff");
	strcpy(loadStruct_ptr->loadTabLongName[2], "manufacturing");
	strcpy(loadStruct_ptr->loadTabLongName[3], "scattered settlements");
	strcpy(loadStruct_ptr->loadTabLongName[4], "open defecation");
	strcpy(loadStruct_ptr->loadTabLongName[5], "hanging latrines");
	strcpy(loadStruct_ptr->loadTabLongName[6], "manure application");
	strcpy(loadStruct_ptr->loadTabLongName[7], "irrigation");
	strcpy(loadStruct_ptr->loadTabLongName[8], "geogen background");
	strcpy(loadStruct_ptr->loadTabLongName[9], "inorganic fertilizer");
	strcpy(loadStruct_ptr->loadTabLongName[9], "mining");


		if(toCalc==0||toCalc==2) { //fo FC and BOD
			loadStruct_ptr->loadType[0] = 1; // dom
			loadStruct_ptr->loadType[1] = 1; // urb
			loadStruct_ptr->loadType[2] = 1; // man
			loadStruct_ptr->loadType[3] = 1; // sc
			loadStruct_ptr->loadType[4] = 1; // open_defec
			loadStruct_ptr->loadType[5] = 0; // hanging_l  soll 1 sein
			loadStruct_ptr->loadType[6] = 1; // manure
			loadStruct_ptr->loadType[7] = 0; // irr
			loadStruct_ptr->loadType[8] = 0; // geog_back
			loadStruct_ptr->loadType[9] = 0; // ind_fert
			loadStruct_ptr->loadType[10]= 0; // mining
		} else if(toCalc==1) { //for TDS
			loadStruct_ptr->loadType[0] = 1; // dom
			loadStruct_ptr->loadType[1] = 1; // urb
			loadStruct_ptr->loadType[2] = 1; // man
			loadStruct_ptr->loadType[3] = 1; // sc
			loadStruct_ptr->loadType[4] = 1; // open_defec
			loadStruct_ptr->loadType[5] = 0; // hanging_l soll 1 sein
			loadStruct_ptr->loadType[6] = 1; // manure
			loadStruct_ptr->loadType[7] = 1; // irr
			loadStruct_ptr->loadType[8] = 1; // geog_back
			loadStruct_ptr->loadType[9] = 0; // ind_fert
			loadStruct_ptr->loadType[10]= 1; // mining
		} else if(toCalc==3||toCalc==4) { //for TN an TP
			loadStruct_ptr->loadType[0] = 1; // dom
			loadStruct_ptr->loadType[1] = 1; // urb
			loadStruct_ptr->loadType[2] = 1; // man
			loadStruct_ptr->loadType[3] = 1; // sc
			loadStruct_ptr->loadType[4] = 1; // open_defec
			loadStruct_ptr->loadType[5] = 0; // hanging_l soll 1 sein
			loadStruct_ptr->loadType[6] = 1; // manure
			loadStruct_ptr->loadType[7] = 0; // irr
			loadStruct_ptr->loadType[8] = 1; // geog_back
			loadStruct_ptr->loadType[9] = 1; // ind_fert
			loadStruct_ptr->loadType[10]= 0; // mining
		} else if(toCalc==5) { //for pesticide
			loadStruct_ptr->loadType[0] = 0; // dom
			loadStruct_ptr->loadType[1] = 0; // urb
			loadStruct_ptr->loadType[2] = 0; // man
			loadStruct_ptr->loadType[3] = 0; // sc
			loadStruct_ptr->loadType[4] = 0; // open_defec
			loadStruct_ptr->loadType[5] = 0; // hanging_l
			loadStruct_ptr->loadType[6] = 0; // manure
			loadStruct_ptr->loadType[7] = 1; // irr
			loadStruct_ptr->loadType[8] = 0; // geog_back
			loadStruct_ptr->loadType[9] = 0; // ind_fert
			loadStruct_ptr->loadType[10]= 0; // mining
		}
		//cout of sectors which are calculated
		cout<<"\nSectors added in load are:\n";
		for (int type=0; type<loadTypes; type++) {
			if (loadStruct_ptr->loadType[type]) cout << loadStruct_ptr->loadTabLongName[type] << endl;
		}
		cout << endl;

}

//#############################GENERAL INPUT FOR MANY .CPP#####################


int readArea(float *G_area, char *G_land_area, int *gcrc, const optionsClass* options_ptr, const continentStruct* continent_ptr) {
	char filename[maxcharlength];

	sprintf(filename, "%s/GAREA.UNF0", continent_ptr->hydro_input);
	if (readInputFile(filename, 4, 1, continent_ptr->nrows, &G_area[0], gcrc, options_ptr, continent_ptr)) return 1;
//	sprintf(filename, "%s/GFREQ.UNF1", hydro_input);
//	if (readInputFile(filename, 1, 1, continent[0], &G_landfreq[0], gcrc)) return 1;

	if (G_land_area != NULL) {
		// land_area = cell_area - wettland - lake - ocean,
		sprintf(filename, "%s/G_LAND_AREA.UNF1", options_ptr->input_dir);
		if (readInputFile(filename, 1 , 1, continent_ptr->ng, &G_land_area[0], gcrc, options_ptr, continent_ptr)) exit(1);
	}

	return 0;
}

int readInputFile(char *input_file, int  type_size, int  valuesPerCell, long number_of_cells
		, double *G_array, int *gcrc, const optionsClass* options_ptr, const continentStruct* continent_ptr, float nodata)
{

	cout <<"readInputFile for double\n";
	cout<<"type_size: "<<type_size<<" valuesPerCell: "<<valuesPerCell<<" number_of_cells: "<<number_of_cells<<endl;
	float *G_tmpArray = new float[number_of_cells*valuesPerCell];
	if(readInputFile(input_file, type_size, valuesPerCell, number_of_cells, G_tmpArray, gcrc, options_ptr, continent_ptr, nodata)) return 1;

	for (long cell=0; cell<number_of_cells*valuesPerCell; cell++)
		G_array[cell] = G_tmpArray[cell];

	delete[] G_tmpArray; G_tmpArray = NULL;

	return 0;
}

int getCELLS(const optionsClass* options_ptr, continentStruct* continent_ptr) {

	char sql[2000];                        // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	int j=sprintf(sql, "SELECT w.`ng`, w.`ng_land`, w.`nrow`, w.`ncol`, w.`hydro_input`, w.`cellFraction` ");
	j+=sprintf(sql+j, "FROM watergap_unf.watergap_region w WHERE w.`IDVersion`=%d AND w.`IDReg`=%d ", options_ptr->IDVersion, options_ptr->IDReg);
	j+=sprintf(sql+j, "AND w.`ng` is not NULL AND w.`ng_land` is not NULL AND w.`nrow` is not NULL AND w.`ncol` is not NULL ");
	j+=sprintf(sql+j, "AND w.`hydro_input` is not NULL AND w.`cellFraction` is not NULL;");

	query.reset();
	query << sql;
	res = query.store();
	if (res.empty()) {
		cerr << "IDVersion und IDRegion sind falsch!\n" << endl;
		return 1;
	}
	//row = res.fetch_row();
	continent_ptr->ng = res[0]["ng"];
	continent_ptr->ng_land = res[0]["ng_land"];
	continent_ptr->nrows = res[0]["nrow"];
	continent_ptr->ncols = res[0]["ncol"];
	continent_ptr->cellFraction = res[0]["cellFraction"];
//	sprintf(continent_ptr->hydro_input, res[0]["hydro_input"]);
//	sprintf(continent_ptr->hydro_input, res[0]["hydro_input"].c_str());
	strcpy(continent_ptr->hydro_input, res[0]["hydro_input"].c_str());
//	strncpy(continent_ptr->hydro_input, res[0]["hydro_input"].c_str(), sizeof(res[0]["hydro_input"].c_str()));
	cout << "ng          :\t"<< continent_ptr->ng << endl;
	cout << "ng_land     :\t"<< continent_ptr->ng_land << endl;
	cout << "nrows       :\t"<< continent_ptr->nrows << endl;
	cout << "ncols       :\t"<< continent_ptr->ncols << endl;
	cout << "cellFraction:\t"<< continent_ptr->cellFraction << endl;
	cout << "hydro_input : "<<continent_ptr->hydro_input<<'\t'<< sizeof(res[0]["hydro_input"].c_str())<<endl;
	cout << "res[0][hydro_input].c_str():\t"<<res[0]["hydro_input"].c_str()<<endl;

	return 0;
} // getCELLS()

//
// method to get arcid and global id
// arc_glob_id[arcid - 1] = globalID
int getArcGlobId(int *arc_glob_id, const optionsClass* options_ptr, const continentStruct* continent_ptr){
	char sql[2000];                        // variable for SQL-query
	int cell;
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	int j=sprintf(sql, "SELECT g.`cell`, g.`GLOBALID` as `global_id` FROM watergap_unf.gcrc g ");
	j+=sprintf(sql+j, "WHERE g.`IDVersion`=%d AND g.`IDReg`=%d AND g.`cell`>0;", options_ptr->IDVersion, options_ptr->IDReg);

	cout<<sql<<endl;
	query.reset();
	query << sql;
	res = query.store();
	if (res.empty()) {
		cout << "IDVersion und IDRegion sind falsch!\n" << endl;
		return 1;
	}
	for (unsigned int rowNum=0; rowNum<res.size(); rowNum++) {
		//row = res.fetch_row();
		cell = res[rowNum]["cell"]; // ArcID
		if (cell<1 || cell>continent_ptr->ng) {
			cout << "Fehler in watergap_unf.gcrc: gcrc_num = \t"<<cell << endl << endl;
			return 1;
		}
		arc_glob_id[cell-1] = res[rowNum]["global_id"];
	}
	return 0;
}

int getGCRC(int *gcrc, int *grow, const optionsClass* options_ptr, const continentStruct* continent_ptr) {
	char sql[2000];                        // variable for SQL-query
	int cell;
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	int j=sprintf(sql, "SELECT g.`cell`, g.`cell_land_water`, g.`row` FROM watergap_unf.gcrc g ");
	j+=sprintf(sql+j, "WHERE g.`IDVersion`=%d AND g.`IDReg`=%d AND g.`cell`>0;", options_ptr->IDVersion, options_ptr->IDReg);

	cout<<sql<<endl;
	query.reset();
	query << sql;
	res = query.store();
	if (res.empty()) {
		cout << "IDVersion und IDRegion sind falsch!\n" << endl;
		return 1;
	}
	for (unsigned int rowNum=0; rowNum<res.size(); rowNum++) {
		//row = res.fetch_row();
		cell = res[rowNum]["cell_land_water"]; // gcrc_num
		if (cell<1 || cell>continent_ptr->ng) {
			cout << "Fehler in watergap_unf.gcrc: gcrc_num = \t"<<cell << endl << endl;
			return 1;
		}
		gcrc[cell-1] = res[rowNum]["cell"]; // gcrc[gcrc_num-1] = arcid
		if (gcrc[cell-1]<1 || gcrc[cell-1]>continent_ptr->ng) {
			cout << "Fehler in watergap_unf.gcrc: ArcID = \t"<< gcrc[cell-1] << '\t'<<cell<< endl << endl;
			return 1;
		}
		grow[gcrc[cell-1]-1] = res[rowNum]["row"]; // grow[arcid-1] = row_num
	}
	return 0;
} //getGCRC()


bool IsNumber(char* str)
{
	int length = strlen(str);
	char zeichen;
	for(int i=0; i<length; i++)
	{
		zeichen = str[i];
		if(! (zeichen >= 48 && zeichen <= 57) ) // 48=ASCII-CODE fr 0  57=ASCII-CODE fr 9
			return false;
	}
	return true;
}

bool getProject(int project_id, const optionsClass* options_ptr, mysqlpp::Connection con, const char* application)
// application soll entweder "worldqual" oder "wq_load" sein, daraus wird Datenbankname: "wwqa_worldqual_af" oder "senanlys_wq_load_eu"...
{
	// checking project_id and project- and database name
	char sql[2000]; // variable for SQL-query
	mysqlpp::Query query = con.query();

	query.reset();
	int j = sprintf(sql, "SELECT p.Project_Name, IFNULL(p.database_name,'') as `database_name`, p.description ");
	j+=sprintf(sql+j, "FROM wq_general._project p WHERE p.project_id= %d;", project_id);
	query << sql;
	mysqlpp::StoreQueryResult res = query.store();
	if (res.empty()) { cerr << "ERROR: Wrong parameter: project_id"<< endl; return 1;}

	mysqlpp::String Project_Name=res.at(0)["Project_Name"];
	mysqlpp::String MyDatabase_str = res.at(0)["database_name"];
	mysqlpp::String Project_description=res.at(0)["description"];
	cout << "\nproject_id = " <<project_id<<" (" << Project_Name <<")"<<endl;
	cout << "Project_description: "<< Project_description << endl<< endl;
	sprintf(MyDatabase,"%s",MyDatabase_str.c_str());
	if(strcmp(MyDatabase,"")==0)
			cout << "Database: The CESR version of the database is used. Not a project dependent database" << endl << endl;
	else{
		sprintf(MyDatabase,"%s_", MyDatabase);
		cout << "Database: The database of the project: " << MyDatabase <<" is used."<< endl << endl;
	}

    con.disconnect();
    char start_db[maxcharlength]; sprintf(start_db, "%s%s_%s", MyDatabase, application, options_ptr->continent_abb);
	if (!con.connect(&start_db[0], &options_ptr->MyHost[0], &options_ptr->MyUser[0], &options_ptr->MyPassword[0],3306)) {
		cout << "database " << start_db << " connection failed..." << endl;
		return 1;
	}
	////////////////////////////////
	cout << "database " << start_db << " connection ok..." << endl; //return 0;
	////////////////////////////////

	return 0;
}

bool checkParameter(int parameter_id, int& toCalc, mysqlpp::String& ParameterName, bool ReadToCalc) {
	// checking parameter_id and parameter name

	char sql[2000]; // variable for SQL-query
	int j;
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	query.reset();
	j=sprintf(sql,    "SELECT w.`parameter`, IFNULL(w.`typeAlgoID`, %d) as `typeAlgoID` FROM wq_general.wq_parameter w", NODATA);
	j+=sprintf(sql+j, " WHERE w.`parameter_id`=%d;", parameter_id);
	query << sql;
	res = query.store();
	cout<<query<<endl;
	if (res.empty()) {cerr << "ERROR: Wrong parameter: parameter_id" << endl; return 1;}
	ParameterName=res.at(0)["parameter"];
	cout << "parameter_id: "<<parameter_id << " ParameterName: " << ParameterName << endl;

	toCalc=res.at(0)["typeAlgoID"];
	//cout<<"toCalc = "<<toCalc<<endl;
	if (ReadToCalc) { // toCalc nur wenn's gebraucht
		if(toCalc==NODATA)
			cerr<<"Error: typeAlgoID must not be null in table wq_general.wq_parameter!"<<endl;

		switch (toCalc) {
		case 0: cout << "BOD\n"; break;
		case 1: cout << "salt\n"; break;
		case 2: cout << "Fecal coliforms\n"; break;
		case 3: cout << "TN\n"; break;
		case 4: cout << "TP\n"; break;
		case 5: cout << "pesticide\n"; break;
		default:
			cerr << "ERROR: Wrong parameter type" << endl;
			cerr << "usage:  0 (BOD), 1 (salt), 2 (Fecal coliforms), 3 TN, 4 TP, 5 pesticide\n"; return 1;
			break;
		}
	}

	return 0;
} // checkParameter()

bool checkIDScen(int project_id, int IDScen) {
	// checking IDScen and scenario name
	char sql[2000]; // variable for SQL-query
	int j;
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	query.reset();
	j=sprintf(sql,    "SELECT s.`idScen`, s.`ScenName` FROM wq_general._szenario s ");
	j+=sprintf(sql+j, "WHERE s.`idScen` = %d AND s.project_id= %d; ", IDScen, project_id);
	query << sql;
	res = query.store();
	if (res.empty()) {cerr << "ERROR: idScen" << IDScen << " not exists in wq_general._szenario."<< endl; return 1;}
	mysqlpp::String SzenarioName=res.at(0)["ScenName"];
	cout << "IDScen = " <<IDScen<<" (" << SzenarioName <<")"<<endl;

	return 0;
} //checkIDScen

short checkIDRun(int IDrun, mysqlpp::String& runName, int &parameter_id_input, int &parameter_id, int &parameter_id_load
		, int &project_id, int &IDScen, int &IDScen_wq_load
		, int &IDTemp, double &Q_low_limit, short &Q_low_limit_type, short &FlgCons, short &FlgLake, short &UseWaterTemp) {
	// checkind IDrun: read parameters from wq_general._runlist
	// return 0 wenn alles OK; 1: wenn Fehler aufgetreten; 2: wenn keinen Eintrag f�r IDrun gibt
	// Unterschied zwischen 1 und 2 f�r copy_run notwendig
	int j;
	char sql[10000]; // variable for SQL-query
	mysqlpp::Query query = con.query();

	query.reset();
	j=sprintf(sql,    "SELECT s.`Name`, project_id, ");
	j+=sprintf(sql+j, "IFNULL(s.`IDScen`,-1) as `IDScen`, IFNULL(s.`IDScen_wq_load`,IFNULL(s.`IDScen`,-1)) as `IDScen_wq_load`,  ");
	j+=sprintf(sql+j, "IFNULL(s.`conservative`,0) as `conservative`, ");
	j+=sprintf(sql+j, "IFNULL(s.`lake`,0) as `lake`, ");
	j+=sprintf(sql+j, "s.`parameter_id_input`, s.`parameter_id`,  ");
	j+=sprintf(sql+j, "IFNULL(s.`IDTemp`,-1) as `IDTemp`, "); //IFNULL(s.`teta`,-1) as `teta`, ");
	//j+=sprintf(sql+j, "IFNULL(s.`teta_lake`,IFNULL(s.`teta`,-1)) as `teta_lake`, ");  // wenn keine teta_lake eingegeben ist, teta nutzen
	j+=sprintf(sql+j, "s.`parameter_id_load`,  ");
	j+=sprintf(sql+j, "IFNULL(s.`Q_low_limit`,%d) as `Q_low_limit`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(s.`Q_low_limit_type`,%d) as `Q_low_limit_type` ", NODATA);
	j+=sprintf(sql+j, "FROM wq_general._runlist s ");
	j+=sprintf(sql+j, "WHERE s.IDrun=%d ;", IDrun);
	query << sql;
	cout<<"Query: "<<query<<endl;
	mysqlpp::StoreQueryResult res = query.store();
	if (res.empty()) {cerr << "ERROR: Wrong parameter: IDrun" << endl; return 2;}  // falsche IDrun

	runName=res.at(0)["Name"];
	parameter_id_input=res.at(0)["parameter_id_input"];
	parameter_id=res.at(0)["parameter_id"];
	parameter_id_load=res.at(0)["parameter_id_load"];
	cout<<"parameter_id: "<<parameter_id<<endl;
	project_id=res.at(0)["project_id"];
	IDScen=res.at(0)["IDScen"];
	IDScen_wq_load=res.at(0)["IDScen_wq_load"];
	cout << "IDrun = " <<IDrun<<" (" << runName <<")"<<endl;
	IDTemp = res.at(0)["IDTemp"];
//		TETA   = res.at(0)["TETA"];
//		TETA_lake   = res.at(0)["TETA_lake"];
	Q_low_limit = res.at(0)["Q_low_limit"];
	Q_low_limit_type = res.at(0)["Q_low_limit_type"];
	cout << "IDTemp: "<<IDTemp<<endl; //"\tTETA: "<<TETA<<"\tTETA_lake: "<<TETA_lake<<endl;
	cout << "Q_low_limit: "<< Q_low_limit << endl;
	if(Q_low_limit==NODATA) {
		cout<<"Es wird ohne Q_low_limit berechnet."<<endl;
		Q_low_limit_type = NODATA;
	}
	else {
		if (Q_low_limit_type) cout << "bei Q_out<= " << Q_low_limit << " Konzentration wird = C0\n";
		else cout << "bei Q_out<= " << Q_low_limit << " Konzentration wird = 0\n";
	}

	FlgCons   = res.at(0)["conservative"];
	FlgLake   = res.at(0)["lake"];

	if (IDTemp<0) {
		cout << "keine Wassertemperatur wird benutzt.\n";
		UseWaterTemp=0;
	}
	else {
		UseWaterTemp=1;
		query.reset();
		j=sprintf(sql,  "SELECT * FROM wq_general._water_temperature_list w WHERE w.`IDTemp`=%d;", IDTemp);
		query << sql;
		res = query.store();
		if (res.empty()) {cerr << "ERROR: Wrong parameter: IDTemp" << endl; return 1;}  // falsche IDTemp
		mysqlpp::String NameTemp=res.at(0)["Name"];
		mysqlpp::String climateTemp=res.at(0)["climate"];
		mysqlpp::String commentTemp=res.at(0)["comment"];
		cout << "IDTemp = " <<IDTemp<<" (" << NameTemp <<" "<< commentTemp<<")"<<endl;
	}

	if (FlgCons==1) cout << "konservativer Stoff wird berechnet.\n";
	else  if (FlgCons == 0){
		cout << "nicht-konservativer Stoff wird berechnet.\n";
	}
	else {
		cerr << "ERROR: Error in _runlist.conservative. Can be ==1 (conservative substance) or ==0/NULL (non-conservative substance)\n";
		return 1;
	}

	if (FlgLake==1) cout << "mit Abbau in den Seen/Reservoirs wird berechnet.\n";
	else if (FlgLake == 0){
		cout << "ohne Abbau in den Seen/Reservoirs wird berechnet.\n";
	}
	else {
		cerr << "ERROR: Error in _runlist.lake. Can be ==1 (with decomposition in lakes/reservoirs) or ==0/NULL (without decomposition in lakes/reservoirs)\n";
		return 1;
	}
cout << "xxxxxxxxcheckIDRun() OK\n";
	return 0;
} // checkIDRun()

short checkIDRun(int IDrun, mysqlpp::String& runName, int& parameter_id_input, int& parameter_id, int& parameter_id_load
		, int& project_id, int& IDScen, int& IDScen_wq_load){
	// Funktion mit der kleineren Argumenteniste, wenn IDTemp, Q_low_limit, Q_low_limit, Q_low_limit_type, FlgCons, FlgLake, UseWaterTemp
	// werden nicht gebraucht, kann man diese Funktion ausrufen
	int IDTemp;
	double Q_low_limit;
	short Q_low_limit_type, FlgCons, FlgLake, UseWaterTemp;
	return checkIDRun(IDrun, runName, parameter_id_input, parameter_id, parameter_id_load
			, project_id, IDScen, IDScen_wq_load
			, IDTemp, Q_low_limit, Q_low_limit_type, FlgCons, FlgLake, UseWaterTemp);
	return 0;
}// checkIDRun() mit kleineren Argumentenliste

bool isTableExists(const char* TableName) {
	// return 1: Fehler passiert
	// return 0: alles OK, Tabelle hat schon existiert
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	query.reset();
	query << "DESCRIBE "<<TableName<<";" ;

	try {
		res = query.store();
		// wenn kein Fehler bei DESCRIBE, Tabelle existiert schon, kein create table notwendig.
		//...
	}//try for resQuery
	catch (const mysqlpp::BadQuery& er) {
		// Fehler bei DESCRIBE - Tabelle nicht existiert, erzeugen
		cout << er.what() << ". Create Table..." << endl;
		return false;
	} // catch
	return true;

}
short CreateTable(const char* TableName, const char* sql_createTable, bool QueryExec, ofstream& mySQLfile, bool& TableExists) {
	// return 1: Fehler passiert
	// return 0: alles OK, Tabelle hat schon existiert
	mysqlpp::Query query = con.query();

	if (isTableExists(TableName)) {
		TableExists = true;
	} else {
		TableExists = false;
		if (!mySQLfile.is_open()) {
			cout << sql_createTable << endl << endl;
		} else {
			mySQLfile << sql_createTable << endl << endl;
		}
		if(QueryExec) {
			query.reset();
			query << sql_createTable;
//			cout << query;
			if (!query.execute()) {
				cout << "Error in CREATE TABLE "<<TableName << endl;
				return 1;
			}
			TableExists = true;
		}

	}

	return 0;
} // isTableExist()

int get_CellParam(double *riverLength, int *RoutOrder, double *g_slope, double *flowVelocity_const, int *gcrc
		, const options_wqClass* options_ptr, const continentStruct* continent_ptr) {
	char filename[maxcharlength];
	int j, cell;
	char sql[maxcharlength]; // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	cout << "options.input_dir: "<< options_ptr->path_hydro_routing<<endl;
	sprintf(filename, "%s/G_RIVERSLOPE.UNF0", options_ptr->path_hydro_routing);
	if (readInputFile(filename, 4, 1, continent_ptr->ng, &g_slope[0], gcrc, options_ptr, continent_ptr)) {
		cout << "error in "<< filename<<endl;
		exit(1);
	}
	cout << "file "<< filename<< " OK"<<endl;
/*	for(int i=0; i<continent_ptr->ng;i++){
		cout<<"get_CellParam: g_slope["<<i<<"]: "<<g_slope[i]<<endl;
	}
*/

	query.reset();
	j=sprintf(sql,    "SELECT f.`cell`, f.`river_length`, f.`rout_order`, IFNULL(u,%d) as `u` ", NODATA);
	j+=sprintf(sql+j, "FROM %sworldqual_%s.flow_velocity f;", MyDatabase, options_ptr->continent_abb);
	query << sql;
	res = query.store();
	cout<<"before rowNum "<< res.size()<<endl;
//	try
	{
		if (RoutOrder == NULL) { // in wq_stat wird kein RoutOrder gebraucht, Funktion mit NULL ausgerufen
			for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
			{
				cell=res[rowNum]["cell"];
				if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in flow_velocity.\n"; return 1;}
				riverLength[cell-1] = res[rowNum]["river_length"];
				flowVelocity_const[cell-1]     = res[rowNum]["u"];
			}
		} else { // in worldqual wird RoutOrder benutzt, weil gesamt Kontinent berechnet wird
			for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
			{
				cell=res[rowNum]["cell"];
				if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in flow_velocity.\n"; return 1;}
				riverLength[cell-1] = res[rowNum]["river_length"];
				RoutOrder[res[rowNum]["rout_order"]-1] = cell;
				flowVelocity_const[cell-1]     = res[rowNum]["u"];
			}
		}

	}
	//cout<<"after rowNum"<<endl;
	cout << "get_CellParam OK\n";
	return 0;
}

int get_ArcID(const optionsClass* options_ptr, int arcid_global) {
	int j;
	char sql[2000]; // variable for SQL-query
	mysqlpp::Query query = con.query();
	mysqlpp::StoreQueryResult res;

	j=sprintf(sql,    "SELECT `cell` FROM watergap_unf.`gcrc` g ");
	j+=sprintf(sql+j, "WHERE g.`IDVersion`=%d AND g.`IDReg`=%d AND g.`GLOBALID`=%d ;", options_ptr->IDVersion, options_ptr->IDReg, arcid_global);

	query.reset();
	query << sql;
	res = query.store();
	if (res.empty()) return -9999;  // arcid_global existiert nicht
	int arcid = res.at(0)["cell"];
	cout << "***** get_ArcID: arcid_global = " << arcid_global << " arcid = " << arcid << endl;
	return arcid;

} // end od get_ArcID()

int  setCountryInput(int number_countries, int (*count_cont)[4], std::string var_one, std::string var_two, std::string var_three, int IDScen, int actual_year){
	mysqlpp::Query query = con.query();
	mysqlpp::SimpleResult res; // mysqlpp::ResNSel    resExec;

	//correct the value for the countries which are part of more than one continent
	for(int i=0;i<number_countries;i++){

			//This is hard coded, because it is assumed that the country borders
			//and continent definition will stay the same
			//It is not the nicest way to implement this, feel free to find a better solution
			//If no data exist for one continent, than there will not be an update, because
			//no country_id which are alike can be found

			if(count_cont[i][1]==5 && count_cont[i][2]==6){

				query.reset();
				//the queries for each continent tupel are the same
				query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
					<<"JOIN (SELECT country_id, "
					<<"sum(total_"<<var_one<<") as "<<var_one<<", sum(total_"<<var_two<<") as "<<var_two<<", sum(total_"<<var_three<<") as "<<var_three<<" "
					<<"FROM (SELECT c1.`country_id` as country_id, "
					<<"c1.`"<<var_one<<"` as total_"<<var_one<<", c1.`"<<var_two<<"` as total_"<<var_two<<", c1.`"<<var_three<<"` as total_"<<var_three<<" "
					<<"FROM "<<MyDatabase<<"wq_load_na.cell_input c1 "
					<<"WHERE c1.`IDScen`="<<IDScen<<" AND c1.`time`="<<actual_year<<" AND c1.`country_id`="<<count_cont[i][0]
					<<" UNION ALL "
					<<"SELECT c2.`country_id` as country_id, "
					<<"c2.`"<<var_one<<"` as total_"<<var_one<<", c2.`"<<var_two<<"` as total_"<<var_two<<", c2.`"<<var_three<<"` as total_"<<var_three<<" "
					<<"FROM "<<MyDatabase<<"wq_load_sa.cell_input c2 "
					<<"WHERE c2.`IDScen`="<<IDScen<<" AND c2.`time`="<<actual_year<<" AND c2.`country_id`="<<count_cont[i][0]
					<<" ) AS foo) as sub "
					<<"ON c.`country_id`=sub.`country_id` "
					<<"SET c.`"<<var_one<<"`= sub.`"<<var_one<<"`, c.`"<<var_two<<"`= sub.`"<<var_two<<"`, c.`"<<var_three<<"`= sub.`"<<var_three<<"` "
					<<"WHERE c.`IDScen`="<<IDScen<<" AND c.`time`="<<actual_year<<";";

				cout<<"update country_input na, sa: "<<query<<endl;
				res = query.execute();

			}
			//Ist auskommentiert, da es sich um folgende L�nder handelt:
			//Fiji, Kiribati, US Minor Outlying Islands
			//diese L�nder sind nicht im country parameter input, weil eine Berechnung
			//f�r die Inseln auf 5min keinen Sinn mancht. Falls jemals die Inseln
			//gerecht werden sollen, kann das einkommentiert werden
		/*	if(count_cont[i][1]==3 && count_cont[i][2]==4){

				query.reset();
				query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
				<<"JOIN (SELECT country_id, "
				<<"sum(total_"<<var_one<<") as "<<var_one<<", sum(total_"<<var_two<<") as "<<var_two<<", sum(total_"<<var_three<<") as "<<var_three<<" "
				<<"FROM (SELECT c1.`country_id` as country_id, "
				<<"c1.`"<<var_one<<"` as total_"<<var_one<<", c1.`"<<var_two<<"` as total_"<<var_two<<", c1.`"<<var_three<<"` as total_"<<var_three<<" "
				<<"FROM "<<MyDatabase<<"wq_load_as.cell_input c1 "
				<<"WHERE c1.`IDScen`="<<IDScen<<" AND c1.`time`="<<actual_year<<" AND c1.`country_id`="<<count_cont[i][0]
				<<" UNION ALL "
				<<"SELECT c2.`country_id` as country_id, "
				<<"c2.`"<<var_one<<"` as total_"<<var_one<<", c2.`"<<var_two<<"` as total_"<<var_two<<", c2.`"<<var_three<<"` as total_"<<var_three<<" "
				<<"FROM "<<MyDatabase<<"wq_load_au.cell_input c2 "
				<<"WHERE c2.`IDScen`="<<IDScen<<" AND c2.`time`="<<actual_year<<" AND c2.`country_id`="<<count_cont[i][0]
				<<" ) AS foo) as sub "
				<<"ON c.`country_id`=sub.`country_id` "
				<<"SET c.`"<<var_one<<"`= sub.`"<<var_one<<"`, c.`"<<var_two<<"`= sub.`"<<var_two<<"`, c.`"<<var_three<<"`= sub.`"<<var_three<<"` "
				<<"WHERE c.`IDScen`="<<IDScen<<" AND c.`time`="<<actual_year<<";";


				cout<<"update country_input as, au: "<<query<<endl;
				res = query.execute();

								}*/

			if(count_cont[i][1]==1 && count_cont[i][2]==3){


				query.reset();
				query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
					<<"JOIN (SELECT country_id, "
					<<"sum(total_"<<var_one<<") as "<<var_one<<", sum(total_"<<var_two<<") as "<<var_two<<", sum(total_"<<var_three<<") as "<<var_three<<" "
					<<"FROM (SELECT c1.`country_id` as country_id, "
					<<"c1.`"<<var_one<<"` as total_"<<var_one<<", c1.`"<<var_two<<"` as total_"<<var_two<<", c1.`"<<var_three<<"` as total_"<<var_three<<" "
					<<"FROM "<<MyDatabase<<"wq_load_eu.cell_input c1 "
					<<"WHERE c1.`IDScen`="<<IDScen<<" AND c1.`time`="<<actual_year<<" AND c1.`country_id`="<<count_cont[i][0]
					<<" UNION ALL "
					<<"SELECT c2.`country_id` as country_id, "
					<<"c2.`"<<var_one<<"` as total_"<<var_one<<", c2.`"<<var_two<<"` as total_"<<var_two<<", c2.`"<<var_three<<"` as total_"<<var_three<<" "
					<<"FROM "<<MyDatabase<<"wq_load_as.cell_input c2 "
					<<"WHERE c2.`IDScen`="<<IDScen<<" AND c2.`time`="<<actual_year<<" AND c2.`country_id`="<<count_cont[i][0]
					<<" ) AS foo) as sub "
					<<"ON c.`country_id`=sub.`country_id` "
					<<"SET c.`"<<var_one<<"`= sub.`"<<var_one<<"`, c.`"<<var_two<<"`= sub.`"<<var_two<<"`, c.`"<<var_three<<"`= sub.`"<<var_three<<"` "
					<<"WHERE c.`IDScen`="<<IDScen<<" AND c.`time`="<<actual_year<<";";

				cout<<"update country_input eu, as: "<<query<<endl;
				res = query.execute();

			}

			if(count_cont[i][1]==1 && count_cont[i][2]==2){

				query.reset();
				query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
					<<"JOIN (SELECT country_id, "
					<<"sum(total_"<<var_one<<") as "<<var_one<<", sum(total_"<<var_two<<") as "<<var_two<<", sum(total_"<<var_three<<") as "<<var_three<<" "
					<<"FROM (SELECT c1.`country_id` as country_id, "
					<<"c1.`"<<var_one<<"` as total_"<<var_one<<", c1.`"<<var_two<<"` as total_"<<var_two<<", c1.`"<<var_three<<"` as total_"<<var_three<<" "
					<<"FROM "<<MyDatabase<<"wq_load_eu.cell_input c1 "
					<<"WHERE c1.`IDScen`="<<IDScen<<" AND c1.`time`="<<actual_year<<" AND c1.`country_id`="<<count_cont[i][0]
					<<" UNION ALL "
					<<"SELECT c2.`country_id` as country_id, "
					<<"c2.`"<<var_one<<"` as total_"<<var_one<<", c2.`"<<var_two<<"` as total_"<<var_two<<", c2.`"<<var_three<<"` as total_"<<var_three<<" "
					<<"FROM "<<MyDatabase<<"wq_load_af.cell_input c2 "
					<<"WHERE c2.`IDScen`="<<IDScen<<" AND c2.`time`="<<actual_year<<" AND c2.`country_id`="<<count_cont[i][0]
					<<" ) AS foo) as sub "
					<<"ON c.`country_id`=sub.`country_id` "
					<<"SET c.`"<<var_one<<"`= sub.`"<<var_one<<"`, c.`"<<var_two<<"`= sub.`"<<var_two<<"`, c.`"<<var_three<<"`= sub.`"<<var_three<<"` "
					<<"WHERE c.`IDScen`="<<IDScen<<" AND c.`time`="<<actual_year<<";";

				cout<<"update country_input eu, af: "<<query<<endl;
				res = query.execute();

								}

			if(count_cont[i][1]==3 && count_cont[i][2]==5){

				query.reset();
				query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
					<<"JOIN (SELECT country_id, "
					<<"sum(total_"<<var_one<<") as "<<var_one<<", sum(total_"<<var_two<<") as "<<var_two<<", sum(total_"<<var_three<<") as "<<var_three<<" "
					<<"FROM (SELECT c1.`country_id` as country_id, "
					<<"c1.`"<<var_one<<"` as total_"<<var_one<<", c1.`"<<var_two<<"` as total_"<<var_two<<", c1.`"<<var_three<<"` as total_"<<var_three<<" "
					<<"FROM "<<MyDatabase<<"wq_load_as.cell_input c1 "
					<<"WHERE c1.`IDScen`="<<IDScen<<" AND c1.`time`="<<actual_year<<" AND c1.`country_id`="<<count_cont[i][0]
					<<" UNION ALL "
					<<"SELECT c2.`country_id` as country_id, "
					<<"c2.`"<<var_one<<"` as total_"<<var_one<<", c2.`"<<var_two<<"` as total_"<<var_two<<", c2.`"<<var_three<<"` as total_"<<var_three<<" "
					<<"FROM "<<MyDatabase<<"wq_load_na.cell_input c2 "
					<<"WHERE c2.`IDScen`="<<IDScen<<" AND c2.`time`="<<actual_year<<" AND c2.`country_id`="<<count_cont[i][0]
					<<" ) AS foo) as sub "
					<<"ON c.`country_id`=sub.`country_id` "
					<<"SET c.`"<<var_one<<"`= sub.`"<<var_one<<"`, c.`"<<var_two<<"`= sub.`"<<var_two<<"`, c.`"<<var_three<<"`= sub.`"<<var_three<<"` "
					<<"WHERE c.`IDScen`="<<IDScen<<" AND c.`time`="<<actual_year<<";";

				cout<<"update country_input na, as: "<<query<<endl;
				res = query.execute();

								}

		if(count_cont[i][1]==1 && count_cont[i][2]==2 && count_cont[i][3]==3){

			query.reset();
			query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
				<<"JOIN (SELECT country_id, "
				<<"sum(total_"<<var_one<<") as "<<var_one<<", sum(total_"<<var_two<<") as "<<var_two<<", sum(total_"<<var_three<<") as "<<var_three<<" "
				<<"FROM (SELECT c1.`country_id` as country_id, "
				<<"c1.`"<<var_one<<"` as total_"<<var_one<<", c1.`"<<var_two<<"` as total_"<<var_two<<", c1.`"<<var_three<<"` as total_"<<var_three<<" "
				<<"FROM "<<MyDatabase<<"wq_load_eu.cell_input c1 "
				<<"WHERE c1.`IDScen`="<<IDScen<<" AND c1.`time`="<<actual_year<<" AND c1.`country_id`="<<count_cont[i][0]
				<<" UNION ALL "
				<<"SELECT c2.`country_id` as country_id, "
				<<"c2.`"<<var_one<<"` as total_"<<var_one<<", c2.`"<<var_two<<"` as total_"<<var_two<<", c2.`"<<var_three<<"` as total_"<<var_three<<" "
				<<"FROM "<<MyDatabase<<"wq_load_af.cell_input c2 "
				<<"WHERE c2.`IDScen`="<<IDScen<<" AND c2.`time`="<<actual_year<<" AND c2.`country_id`="<<count_cont[i][0]
				<<" UNION ALL "
				<<"SELECT c3.`country_id` as country_id, "
				<<"c3.`"<<var_one<<"` as total_"<<var_one<<", c3.`"<<var_two<<"` as total_"<<var_two<<", c3.`"<<var_three<<"` as total_"<<var_three<<" "
				<<"FROM "<<MyDatabase<<"wq_load_as.cell_input c3 "
				<<"WHERE c3.`IDScen`="<<IDScen<<" AND c3.`time`="<<actual_year<<" AND c3.`country_id`="<<count_cont[i][0]
				<<" ) AS foo) as sub "
				<<"ON c.`country_id`=sub.`country_id` "
				<<"SET c.`"<<var_one<<"`= sub.`"<<var_one<<"`, c.`"<<var_two<<"`= sub.`"<<var_two<<"`, c.`"<<var_three<<"`= sub.`"<<var_three<<"` "
				<<"WHERE c.`IDScen`="<<IDScen<<" AND c.`time`="<<actual_year<<";";




			cout<<"update country_input eu, af, as: "<<query<<endl;
			res = query.execute();


	   }

	}
	return 0;
}

//  Flussname einlesen aus der STAT.DAT
//  liest in rivername alles bis n�chsten tabulator
void getRiverName(char* rivername, ifstream &inputfile) {
	char ch; int count=0;
	inputfile.get(ch);
	do {
		if (ch=='\n') {cout << '*'; inputfile.get(ch); continue;}
		if (isspace(ch)) ch=' ';
		rivername[count++]=ch;
		inputfile.get(ch);
	} while (inputfile && (ch!='\t') && count<maxcharlength);
	rivername[count]='\0';
	if (ch!='\t') inputfile.ignore(256,'\t');
}
//################################READ UNF-FILES#################################



void SimMemorySwap(char* sourceArray,
               char* destArray,
               const unsigned short dataTypeSize,
               const unsigned long  arrayByteSize)
// swap bytes within a given array (needed to reformat between Intel and SUN
// binary-data format).
// sourceArray and destArray can be the same (swap by means of a third variable)
// Example:
// short int tmpArray[3]={1,2,3};
// short int tmpArray2[3];
// wzMemSwap((char*)tmpArray, (char*)tmpArray2,sizeof(short int),
//            sizeof(tmpArray));
{
#ifdef _DEBUG
  cout << "Call of function SimMemorySwap()." << endl;
#endif

  unsigned long sourcePosition,
                destPosition;

  //----- don't swap bytes
  if (dataTypeSize == 1) return;

  if (sourceArray == destArray) {
    //----- swap bytes within source array by means of a third variable
    char tmpByteValue;

    for (unsigned long i=0; i<arrayByteSize; i += dataTypeSize) {
      for (unsigned short j=0; j<dataTypeSize/2; j++) {
        sourcePosition = i+j;
        destPosition   = (unsigned long) i+dataTypeSize-j-1;
        tmpByteValue                = sourceArray[sourcePosition];
        sourceArray[sourcePosition] = sourceArray[destPosition];
        sourceArray[destPosition]   = tmpByteValue;
      }
    }
  } //sourceArray == destArray
  else {
    //----- swap bytes directly from source to destination
    for (unsigned long i=0; i<arrayByteSize; i += dataTypeSize) {
      for (unsigned short j=0; j<dataTypeSize; j++) {
        sourcePosition = i+j;
        destPosition   = i+dataTypeSize-j-1;
        destArray[destPosition] = sourceArray[sourcePosition];
      }
    }
  }
} //SimMemorySwap()


template <class T> int readUnf(char *input_file, T *output_grid, int valuesPerCell, int cells) {
	int x=read_file(input_file, valuesPerCell*cells, output_grid);

	#ifdef BYTESWAP_NECESSARY
	  //----- swap bytes
	  SimMemorySwap((char*)output_grid, (char*)output_grid, sizeof(T),
	            valuesPerCell*cells*sizeof(T));
	#endif

	return x;
}




template <class T> int read_file(char *file, int n_values, T *grid) {
  int x = 0;
  FILE *file_ptr;
  file_ptr=fopen(file,"r");
  if (file_ptr != NULL)
    {
      x = fread(grid,sizeof(T),n_values,file_ptr);
    }
  else
    printf("\nError while opening file '%s'. \n\n",file);
  fclose(file_ptr);
  return x;
}


double WaterTemp(short int t, double* ConstTemp) {
	// t = Lufttemperatur * 100 
	// y= C0/(1+exp(C1*t+C2))
	
	// eu - Kurve
	// double C0 = 32;
	// double C1 = -0.138816;
	// double C2 = 2.04018;
	
	// globale Kurve
//	double C0 = 32;
//	double C1 = -0.132169;
//	double C2 = 1.99697;
//
//	double waterTemp=C0/(1.0+exp(C1*t/100.0+C2));

	double waterTemp=ConstTemp[0]/(1.0+exp(ConstTemp[1]*t/100.0+ConstTemp[2]));
	return waterTemp;	
}

short checkIDTemp(int IDTemp, double* ConstTemp, char* climateTemp) {
	mysqlpp::Query query = con.query();
	char sql[2000]; // variable for SQL-query

	query.reset();
	int j=sprintf(sql,"SELECT w.IDTemp, IFNULL(w.Name,IDTemp)  as `Name`, IFNULL(w.climate, '') as `climate`");
	j+=sprintf(sql+j, ", IFNULL(w.C0, %d) as `C0`, IFNULL(w.C1, %d) as `C1`, IFNULL(w.C2, %d) as `C2`", NODATA, NODATA, NODATA);
	j+=sprintf(sql+j, ", IFNULL(w.`comment`, '') as `comment`\n ");
	j+=sprintf(sql+j, "FROM wq_general._water_temperature_list w WHERE w.IDTemp = %d;", IDTemp);

	query << sql;
	cout << sql << endl;

	mysqlpp::StoreQueryResult res = query.store();
	if (res.empty()) {
		cerr << "ERROR: Wrong parameter: IDTemp"<< endl;
		cerr << "	or Tabelle wq_general._water_temperature_list f�r IDTemp = "<< IDTemp << " muss gef�llt werden!\n";
		return 1;
	}

	mysqlpp::String water_temperature_Name=res.at(0)["Name"];
	mysqlpp::String water_temperature_comment=res.at(0)["comment"];
	mysqlpp::String climateTemp_str=res.at(0)["climate"];
	sprintf(climateTemp,"%s",climateTemp_str.c_str());
	if(strcmp(climateTemp,"")==0) {
		cerr << "Tabelle wq_general._water_temperature_list f�r IDTemp = "<< IDTemp << "muss gef�llt werden!\n";
		cerr << "F�llen Sie Spalte 'climate' in der Tabelle wq_general._water_temperature_list f�r IDTemp = " << IDTemp << endl << endl;
		return 1;
	}

	ConstTemp[0] = res.at(0)["C0"];
	ConstTemp[1] = res.at(0)["C1"];
	ConstTemp[2] = res.at(0)["C2"];

	cout << "==============================================================\n";
	cout << "Wassertemperaturkurve: \n";
	cout << "y= C0/(1+exp(C1*t+C2))\n";
	cout << "t = Lufttemperatur * 100 \n";
	cout << "IDTemp\t" << IDTemp << "\t" << water_temperature_Name <<endl;
	cout << "climate \t" << climateTemp << endl;
	cout << water_temperature_comment << endl;
	cout << "C0 = \t" << ConstTemp[0] << endl;
	cout << "C1 = \t" << ConstTemp[1] << endl;
	cout << "C2 = \t" << ConstTemp[2] << endl;
	cout << "==============================================================\n";

	if (ConstTemp[0] == NODATA || ConstTemp[1] == NODATA || ConstTemp[2] == NODATA) {
		cerr << "Tabelle wq_general._water_temperature_list f�r IDTemp = "<< IDTemp << "muss gef�llt werden!\n";
		return 1;
	}

	return 0;
} // checkIDTemp()
