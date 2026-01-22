// WorldQual.cpp
// 05-09-2007
// Ellen Teichert
//**

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>  //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include <list>
#include "../general_function/timestring.h"
#include "../general_function/general_functions.h"
#include "options_wq.h"
//#include "def.h"

//#define BYTESWAP_NECESSARY


#define DEBUG
//#define DEBUG_queries
//#undef DEBUG_queries

//options_wqClass optionsWQ;
//int continent[5];  // 0 = ng (number of cells); 1 - ng_land (number of land cells); 2 - nrows (number of rows); 3 - ncols (number of columns); 4 - cellFraction

//char  hydro_input[maxcharlength];              // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Inputpath
char  MyDatabase[maxcharlength];


mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::StoreQueryResult     res;
//mysqlpp::UseQueryResult     Use_res;
mysqlpp::Row        row;


sql_create_1(Liste_X, 1, 0,
			 sql_int_unsigned, cell)
sql_create_2(Routing_upd, 1, 0,
			 sql_int_unsigned, cell,
			 sql_int_unsigned, counter)


struct Substance {
	int IDrun;
	int date;
	int cell;
	double Value;
};



set <Liste_X> resListe_X;
set <Liste_X>::iterator itListe_X;


void   updateRouting(int cell, options_wqClass* optionsWQ_ptr);
void   get_Liste(int IDrun, int modus, options_wqClass* optionsWQ_ptr);
int    calculate_riverLength(double *riverLength, int *g_outflc, options_wqClass* optionsWQ, int *gcrc, const continentStruct* continent_ptr);
int    calculate_RoutOrder(int *RoutOrder, int *g_outflc, int IDrun, options_wqClass* optionsWQ, const continentStruct* continent_ptr);
int    get_inflow_count(double *inflow_count, options_wqClass* optionsWQ_ptr, const continentStruct* continent_ptr);
int    get_waterTempAir(int actual_year, double (*water_temp)[12], short UseWaterTemp, double* ConstTemp, int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr);
//int    get_Q(int actual_year, double (*Q_out)[12], int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr);








int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram worldqual started at: " << getTimeString() << endl << endl;
	options_wqClass optionsWQ(argc, &argv[0], "IDrun startYear endYear");

	if (argc<4) {
		optionsWQ.Help();
//		cout << "\n./worldqual_5 IDrun startYear endYear [-ooptions_file]\n";
//		cout << "default options file is OPTIONS.DAT\n";
//    		[-calc] [-const velocity] [-h hostname] [-u user] [-p password] [-v version] [-r region] [-i path] [-c climate]"
//         << "\n\nparameters apart from IDrun, startYear, endYear must not be in order. "
//         << "\nif one (or more) parameters is missing the following default values are used:  "
////         << "\n\nDATABASE      \"empty string\""
//         << "\nHOST          \"MySQL\"    "
//         << "\nUSER          \"worldqual\""
//         << "\nPASSWORD      \"*******\"     "
//         << "\nIDVersion     3 (WaterGAP3)"
//         << "\nIDReg         1 (Europa) \n\n"
//         << "\n-calc flow velocity will be calculated within this program."
//         << "\n-const flow velocity will be calculated with a constant value. Value shall be entered in [m/sec]."
//         << "\nif neigther -calc or -const were entered flow velocity is taken from  MySQL-table or from UNF file.\n"
//         << "\n-i            data will be read from UNF files and not from the MySQL-tables.\n"
//         << "\n-c climate input on 0.5�=> 0\n";
         return 0;
	}

	if(!IsNumber(argv[1]) || !IsNumber(argv[2]) || !IsNumber(argv[3])) {
		cerr << "ERROR: Wrong parameter" << endl;
		optionsWQ.Help();
//		cerr << "usage: ./worldqual_5 IDrun start end [-ooptions_file]"<<endl;
		exit(1);
	}

	int IDrun     = atoi(argv[1]);
	int start     = atoi(argv[2]);
	int end       = atoi(argv[3]);
	int parameter_id_input, parameter_id, parameter_id_load;

	cout << "IDrun:      " << IDrun      << endl;
	cout << "years:      " << start << " - " << end << endl;

	if (start>end) {
		cerr << "ERROR: Wrong parameter. start year> end year!!!" << endl;
		optionsWQ.Help();
//		cerr << "usage: ./worldqual IDrun start end [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region]"<<endl;
		exit(1);
	}

	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 4 = 1 + 3 = programmname + 3 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(optionsWQ.init(argc-4, &argv[4])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input
	mysqlpp::String runName;
	int toCalc; // 0 - BOD, 1 - salt, 2 - FC (Fecal coliforms), 3 - TN, 4 - TP
	int project_id, IDScen, IDScen_wq_load;
	int IDTemp; double TETA, TETA_lake;      // Parameter zum Wassertemperatur
	// Wenn Q_out in (0; Q_low_limit] Kozentration C0 weiter geben an unten liegende Zelle (oder =0 setzen); [m3/s] und in [km3/month]
	double Q_low_limit, Q_low_limit_km3month;
	short Q_low_limit_type; // bei Q_low_limit != NULL: 0 Konzentration auf 0 setzen; 1: Konzentration C0 weiter geben
	short FlgCons; // ==1 conservative substance; ==0 non-conservative substance
	short FlgLake; // ==1 mit Abbau in den Seen/Reservoirs ; ==0 keine Abbau in den Seen/Reservoirs
	short UseWaterTemp; // == 0 keine Wassertemperatur wird benutzt; == 1 mit Wassertemperatur

	const short nDaysPerMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


	if (is_c_geogen_country) {
		cout << "=========================================\n";
		cout << "gilt nur fuer salz:\n";
		cout << "c_geogen als L�nderwerte in der Tabelle wq_load.country_parameter_input\n";
		cout << "c_geogen darf entweder nur in der Tabelle wq_load.country_parameter_input geben (wq_load input)\n";
		cout << "oder in der worldqual.factors.c_geogen (worldqual input). Man darf es nicht doppelt rechnen!!\n\n";
		cout << "=========================================\n";
	} else {
		cout << "=========================================\n";
		cout << "gilt nur fuer salz:\n";
		cout << "c_geogen_cell - Zellwerte aus der Tabelle wq_load.`parameter_cell_input (wq_load input)\n";
		cout << "c_geogen aus der Tabelle worldqual.factors wird nicht benutzt.\n\n";
		cout << "=========================================\n";
	}

	//char headerline[maxcharlength]; // variable for line input of header
	int j;
	try 
	{
		char sql[10000]; // variable for SQL-query
		//char sql_conc_tmp[4000]; // variable for SQL-query

		cout << "start:" << endl;
        char start_db[maxcharlength]; //sprintf(start_db, "%sworldqual_%s", MyDatabase, optionsWQ.continent_abb);
		sprintf(start_db, "wq_general");

		if (!con.connect(&start_db[0], &optionsWQ.MyHost[0], &optionsWQ.MyUser[0], &optionsWQ.MyPassword[0],3306)) {
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl; //return 0;
		////////////////////////////////

		// Pruefen IDrun
		if(checkIDRun(IDrun, runName, parameter_id_input, parameter_id, parameter_id_load
				, project_id, IDScen, IDScen_wq_load
				, IDTemp, Q_low_limit, Q_low_limit_type, FlgCons, FlgLake, UseWaterTemp)) exit(1);

		// checking project_id and project- and database name, fill MyDatabase
		if (getProject(project_id, &optionsWQ, con, "worldqual")) exit(1);

		// checking IDScen and scenario name
		if(checkIDScen(project_id, IDScen)) exit(1);
		if (IDScen != IDScen_wq_load) {
			cout << "IDScen_wq_load: ";
			if(checkIDScen(project_id, IDScen_wq_load)) exit(1);
		}

		mysqlpp::String ParameterName;
		// checking parameter_id and parameter name
		if (checkParameter(parameter_id, toCalc, ParameterName)) exit(1);

		// DAten für Wassertemperaturkurve
		char  climateTemp[maxcharlength];
		double ConstTemp[3];  // Konstanten f�r die Temperaturkurve y= C0/(1+exp(C1*t+C2))
		if (UseWaterTemp && optionsWQ.isWaterTempFromAir ) {
			//checking IDTemp und f�llen ConstTemp
			if (checkIDTemp(IDTemp, ConstTemp, climateTemp)) exit(1);
		}

		/*
		// Pruefen, ob load vorhanden ist
		query.reset();
		if (optionsWQ.loadingType==0) {
			j=sprintf(sql,    "SELECT count(*) as cells FROM %sworldqual_%s.`load` l WHERE l.`IDScen`=%d AND ", MyDatabase, optionsWQ.continent_abb, IDScen);
			j+=sprintf(sql+j, "l.`parameter_id`=%d and `year` between %d and %d;", parameter_id_input, start, end);
		} else if (optionsWQ.loadingType==1) {
			j=sprintf(sql,    "SELECT count(*) as cells FROM %sworldqual_%s.`load_%d_%d` l WHERE ", MyDatabase, optionsWQ.continent_abb, IDScen, parameter_id_input);
			j+=sprintf(sql+j, " `year` between %d and %d;", start, end);
		} else {
			sprintf(TableName, "%swq_load_%s.calc_cell_month_load_%s_%d_%d", MyDatabase, optionsWQ.continent_abb, loadingStruct.loadTab[type], IDScen, parameter_id_input);
			j=sprintf(sql,    "SELECT count(*) as cells FROM %s l WHERE ", TableName);
			j+=sprintf(sql+j, " `time` between %d and %d;", start, end);
		}
		query << sql;
		//cout<<query<<endl;
		res = query.store();
		int cells=res.at(0)["cells"];

		if (cells==0) {cerr << "ERROR: Kein INPUT vorhanden\n"; exit(1);}
		//cout << "cells = " << cells <<endl;
		cout <<"----------------\n";
		*/

		///=================================
		// Parameter Landmaske einlesen
		if(getCELLS(&optionsWQ, &continent)) exit(1); cout << "Parameter Landmaske wurden eingelesen\n";

		int *gcrc;  // gcrc[gcrc-1]=ArcID
		int *grow;  // grow[ArcID-1]=row
		//if (InputType)   // einlesen nicht nur wenn Input aus UNF-Dateien, weil immer GFREQ.UNF1 eingelesen wird
		{  // bei UNF-Dateien wird Umwandlung gcrc -> ArcID benoetigt
			gcrc = new int[continent.ng];
			grow = new int[continent.ng];
			if(getGCRC(gcrc, grow, &optionsWQ, &continent)) exit(1);
		}

		double *riverLength = new double[continent.ng];
		int    *RoutOrder   = new int[continent.ng];

		///=================================
		//
		int (*inflow)[8]    = new int[continent.ng][8];    // Zuflusszellen
		int *g_outflc       = new int[continent.ng];       // Abflusszelle


		cout << "get_outflc"<<endl;
		if (get_outflc(g_outflc, &optionsWQ, &continent)) exit(1);
		cout << "get_inflow"<<endl;
		if (get_inflow(inflow, &optionsWQ, &continent))   exit(1);

		///=================================
		if (optionsWQ.riverLength) {
			cout << "calculate river length ..\n";
			if (calculate_riverLength(riverLength, g_outflc, &optionsWQ, gcrc, &continent)) exit(1);
		} else {
			cout << "no calculate river length.\n";
			//get_riverLength(riverLength);
		}
		if (optionsWQ.routingOrder) {
			cout << "calculate routing order ..\n";
			if (calculate_RoutOrder(RoutOrder, g_outflc, IDrun, &optionsWQ, &continent)) exit(1);
		} else {
			cout << "no calculate routing order.\n";
			//get_RoutOrder(RoutOrder);
		}

		///=================================
		// read input which is not dependent on time or substance

		double *g_slope            = new double[continent.ng];    // zur Berechnung variablen Fliessgeschwindigkeit
		double *flowVelocity_const = new double[continent.ng];    // Fliessgeschwindigkeit aus der Tabelle

		get_CellParam(riverLength, RoutOrder, g_slope, flowVelocity_const, gcrc, &optionsWQ, &continent);
		cout << "get_CellParam done" << endl;

		///=================================
		// input einlesen, der von Stoff abhaengig ist

		double *decomposition    = new double[continent.ng];    // Abbaufaktor(1/sec, 1/Monat)
		// Hintergrundkonzentration in WorldQual wird nur in alten Projekten benutzt, neu: in wq_load
		double *c_geogen         = NULL; // Hintergrundkonzentration, geogen background concentration [t/km3]
		if (is_c_geogen_country && toCalc==1)
			c_geogen         = new double[continent.ng];
		double *decay_rate_lake  = new double[continent.ng];    // Abbaufaktor in den Seen [1/sec] (1/sec, 1/Monat)


		if (get_factor(IDrun, decomposition, c_geogen, decay_rate_lake, toCalc, &optionsWQ, &continent)) exit(1);
		cout << "get_factor done\n";

		// zur Kontrolle, wenn es keinen Fehler im Input und RoutOrder gibt, soll es inflow_count[cell]==0, wenn man die Zelle rechnet
		// am Anfang ist es Anzahl der Zustromzellen
		double *inflow_count  = new double[continent.ng];
		double ksd;											//sedimentation rate [1/month]
		double sed_veloc;									//sedimentation velocity [m/d]

		double *ke				   = new double[continent.ng];		//Calculated attenuation coefficient [1/m]
		double *tss_cell		   = new double[continent.ng];		//TSS yield Mg/(a*km^2) -> (Megagram/(year*km^2))
		double ks;													//sunlight inactivation [cm�/cal]; only used for FC
		double *decRate			   =new double[2];					//0 - sedimentation velocity [m/d]; 1 -  sunlight inactivation [cm�/cal]; only used for FC

		///=================================
		// input, der von der Zeit abhaengig ist
		double (*cell_runoff_total)[12] = new double[continent.ng][12];  // [km3] cell runoff
		double (*Q_out)[12]             = new double[continent.ng][12];  // river availability [km3/month]
		double (*s_cell)[12]            = new double[continent.ng][12];  // monthly grid cell loading [t/month]; FC: 10^10cfu/month
//		double (*geogen_info)           = new double[continent.ng];      // wenn == 0 kein geogener Hintergrund beruecksichtigen

		double (*concentration)[12]     = new double[continent.ng][12];  // monthly grid cell concentration [t/km3]
		double (*concentration2)[12]    = new double[continent.ng][12];  // monthly grid cell concentration [t/km3] second formula

		double (*flowVelocity_var)[12]; flowVelocity_var=NULL;           // Fliessgeschwindigkeit aus UNF-Datei
		if (optionsWQ.flowVelocityVariant==0 && optionsWQ.InputType)
			flowVelocity_var = new double[continent.ng][12];

		double (*water_temp)[12]; water_temp=NULL;           // Wassertemperatur
		if (UseWaterTemp)
			water_temp = new double[continent.ng][12];

		double (*G_monthlyGloLakeStorage)[12];  // [km3]
		double (*G_monthlyResStorage)[12];      // [km3]

		if (FlgLake) {
			G_monthlyGloLakeStorage = new double[continent.ng][12];  // [km3]
			G_monthlyResStorage     = new double[continent.ng][12];  // [km3]
		}

		short (*radiation)[12] = new short[continent.ng][12];	//solar radiation in W/m^2

		///=================================
		// andere variablen
		double X_cell;                  // Position innerhalb der Zelle
		double L;                       // Fliesslaenge

		double flowVelocity;            // [m/sec]  // u=86.4 km/day = 1 m/s  Fliessgeschwindigkeit
		double flowVelocity_month;      // [km/Monat]

		double factor_month;            // Abbaufaktor bei wassertemperatur = 20�C [1/Monat]
		double factorLake_month;        // Abbaufaktor im See bei wassertemperatur = 20�C [1/Monat]

		double dec_T = 0.;             // Abbaufaktor bei wassertemperatur in der Zelle [1/Monat]
		double decLake_T = 0.;         // Abbaufaktor im See bei wassertemperatur in der Zelle [1/Monat]

		double krad;					//only for FC: die-off rate due to solar radiation
		double depth;					//depth of a waterbody

		double Q_in[8][12], C_in[8][12], C_in_2[8][12]; // Durchfluss und Konzentration (2 Varianten) von Zuflusszellen
		double C0, C0_2, Q_in_sum, Q_in_max, Qd, Ac_1, Ac_2, Cd_1, Cd_2;
		double Qx_in, Qx_out;           // ersatzwerte, wenn ==0

		for (int actual_year=start; actual_year<=end; actual_year++) {
			cout << "year: " << actual_year<< endl;
			if (getTeta(TETA, TETA_lake, IDScen, parameter_id, actual_year)) exit(1);
			if (IDTemp<0 || TETA<0) {
				cout << "keine Wassertemperatur f�r das Jahr " << actual_year << " wird benutzt.\n";
				UseWaterTemp=0;
			} else {
				UseWaterTemp=1;
				cout << "Wassertemperatur f�r das Jahr " << actual_year << " wird benutzt.\n";
			}
			if (get_inflow_count(inflow_count, &optionsWQ, &continent)) exit(1);  // fuer jedes Jahr neu einlesen
			cout << "get_inflow_count done"<< endl;

			if(toCalc==1){
				if (get_cell_runoff(actual_year, cell_runoff_total, gcrc, &optionsWQ, &continent)){  cerr<<"error in get_cell_runoff"<<endl; exit(1); }
				cout << "get_cell_runoff done\n";
			}

			if(strcmp(MyDatabase,"sen_anlys_")!=0){
				
				if (UseWaterTemp && optionsWQ.isWaterTempFromAir) {
					if (get_waterTempAir(actual_year, water_temp, UseWaterTemp, ConstTemp, gcrc, &optionsWQ, &continent)) {
						cerr<<"error in get_waterTemp from air temperature"<<endl; exit(1);
					}
				} else {
					if (get_waterTemp(actual_year, water_temp, UseWaterTemp, IDTemp, &optionsWQ, &continent)) {
						cerr<<"error in get_waterTemp from DB"<<endl; exit(1);
					}
				}

				cout<<"get waterTemp done"<<endl;

			}else {
				if (get_waterTemp(actual_year, water_temp, UseWaterTemp, IDTemp, &optionsWQ, &continent, IDScen)) { cerr<<"error in get_waterTemp with IdScen"<<endl; exit(1);}
				cout<<"get waterTempwith IdScen done"<<endl;
			}


			if (get_Q(actual_year, Q_out, gcrc, &optionsWQ, &continent)) exit(1);            cout << "get_Q done\n";


			//for(int test=0;test<12;test++)
			//	cout<<"Q_out[141951]:"<<Q_out[141950][test]<<"month"<<test<<endl;

//			if (get_s_cell(IDScen, parameter_id_input, actual_year, s_cell, geogen_info)) exit(1);  cout << "get_s_cell done\n";
			if (get_s_cell(IDScen, parameter_id_input, actual_year, s_cell, &optionsWQ, &continent, optionsWQ.loadingType, toCalc)) exit(1);  cout << "get_s_cell done\n";

			if (optionsWQ.flowVelocityVariant==0 && optionsWQ.InputType) { // Fliessgeschwindigkein aus UNF-Datei
				char filename[maxcharlength];
				sprintf(filename,"%s/G_RIVER_VELOCITY_%d.12.UNF0", optionsWQ.path_watergap_output, actual_year);
				if (readInputFile(filename, 4, 12, continent.ng, &flowVelocity_var[0][0], gcrc, &optionsWQ, &continent))  exit(1);
			}
			if (FlgLake) {  // nur einlesen wenn Abbau in den Seen berechnet wird
				char filename[maxcharlength];

				sprintf(filename,"%s/G_GLO_LAKE_STOR_MEAN_%d.12.UNF0", optionsWQ.path_watergap_output, actual_year);
				if (readInputFile(filename, 4, 12, continent.ng, &G_monthlyGloLakeStorage[0][0], gcrc, &optionsWQ, &continent, 0.)) exit(1);
				cout << "get G_GLO_LAKE_STOR_ done\n";

				sprintf(filename,"%s/G_RES_STOR_MEAN_%d.12.UNF0", optionsWQ.path_watergap_output, actual_year);
				if (readInputFile(filename, 4, 12, continent.ng, &G_monthlyResStorage[0][0], gcrc, &optionsWQ, &continent, 0.))
				{
					//exit(1);            // wenn Datei nicht existiert, = 0 setzen, waterGAP wurde ohne Reservoirs berechnet
					cout << "file "<<filename<<" not exists! G_monthlyResStorage = 0 for all cells and months.\n";
					for (int Cell_value=1; Cell_value<=continent.ng; Cell_value++)
					  for (int month=1; month<=12; month++)
					    G_monthlyResStorage[Cell_value-1][month-1] = 0.0;
				}
				cout << "get G_RES_STOR_ done\n";

			}

			if(toCalc==2 ){

				//**********Initialization of decay rate parameters***********/
				if (optionsWQ.climate==0 || optionsWQ.climate==2) {
					char filename[maxcharlength];
					int *G_wg3_to_wg2 = new int[continent.ng];
					int ng_wg22;
					if (optionsWQ.climate==0) {
						ng_wg22 = 70412;
						sprintf(filename,"%s/G_WG3_WG2WITH5MIN.UNF4", optionsWQ.path_hydro_input);
					} else {
						ng_wg22 = 67420;
						sprintf(filename,"%s/G_WG3_WATCH.UNF4", optionsWQ.path_hydro_input);
					}
					if (readInputFile(filename, 4, 1, continent.ng, &G_wg3_to_wg2[0], gcrc, &optionsWQ, &continent)) exit(1);

					sprintf(filename, "%s/GSHORTWAVE_%d.12.UNF2", optionsWQ.path_climate, actual_year);
					if (readInputFileWG2toWG3( filename, 2, 12, continent.ng, radiation, gcrc, G_wg3_to_wg2, ng_wg22)) exit(1);

					//Testen, was es �ndert, wenn radiation 50 h�her
							/*	for(int i=0; i< continent.ng;i++)
									for(int j=0; j<12;j++){
										radiation[i][j]=radiation[i][j]+100.0;
										j++;
									}*/

					cout << "get GSHORTWAVE_ done\n";

					/*for(int i=0;i<100;i++){
						for(int j=0;j<12;j++){
							cout<<"G_wg3_to_wg2["<<"i"<<"]:"<<G_wg3_to_wg2[i]<<endl;
							cout<<"radiation["<<i<<"]["<<j<<"]:"<<radiation[i][j]<<endl;
						}
					}*/

					delete[] G_wg3_to_wg2; G_wg3_to_wg2 = NULL;
					
				} else {
					char filename[maxcharlength];
					sprintf(filename, "%s/GSHORTWAVE_%d.12.UNF2", optionsWQ.path_climate, actual_year);
					if (readInputFile(filename, 2, 12, continent.ng, &radiation[0][0], gcrc, &optionsWQ, &continent, 0)) exit(1);

					cout << "get GSHORTWAVE_ done\n";

				}

			}
			if(toCalc==2){
				if (getDecayRates(decRate, IDScen, parameter_id, actual_year)) exit(1);
				sed_veloc=decRate[0];
				ks=decRate[1];

				//cout<<"\nsedimenation rate: sed_veloc: "<<sed_veloc<<endl;
				//cout<<"inactivation rate at sunlight: ks: "<<ks<<endl;

			}
			if(toCalc==2){	//seperated in case get_decay e.g. sedimentation shall be used for another substance too
				//************Calculation of the attenuation coefficient***********//
				if (getTSSBasinValues(tss_cell, optionsWQ.IDReg, &continent)) exit(1);
				/*
				for(int i=0; i<continent.ng;i++){
						cout<<"tss_cell[i]="<<tss_cell[i]<<" i: "<<i<<endl;
				}*/

				//Sobald die TSS Daten da sind kann dieser Teil programmiert werden!
				//getTSSconcentration();

				//calculates the attenuation coefficient
				if (calculateAttenuation(ke, tss_cell,  IDScen,  parameter_id,  actual_year, Q_out, gcrc, grow, &optionsWQ, &continent)) exit(1);
			}




			//Fuer alle Zellen wird jeweils der geroutete Wert, oder einer aus get_CellParam() uebergeben
			for (int routNumber=0; routNumber<continent.ng; routNumber++) {
				int Cell_value=RoutOrder[routNumber];
				//cout<<"Cell_value: "<<Cell_value<<endl;

				//================== testausgabe
//				if (Cell_value==125449||Cell_value==125888||Cell_value==125891||Cell_value==105370||Cell_value==105959)
				if (Cell_value==49091||Cell_value==371405||Cell_value==371109||Cell_value==371087)
				{
					cout << "\n####### Cell_value      =\t" <<Cell_value <<endl;
					//cout << "cell_runoff_total       =\t" <<cell_runoff_total[Cell_value-1][0] <<endl;
					cout << "Q_out                   =\t" <<Q_out[Cell_value-1][0] <<endl;
					cout << "s_cell                  =\t" <<s_cell[Cell_value-1][0] <<endl;
					cout << "RoutOrder[routNumber]   =\t" <<RoutOrder[routNumber] <<endl;
					cout << "decomposition           =\t" <<decomposition[Cell_value-1] <<endl;
					cout << "decay_rate_lake         =\t" <<decay_rate_lake[Cell_value-1] <<endl;
					if (is_c_geogen_country && toCalc==1)
						cout << "c_geogen                =\t" <<c_geogen[Cell_value-1] <<endl;
					cout << "inflow_count            =\t" <<inflow_count[Cell_value-1] <<endl;
					cout << "g_slope                 =\t" <<g_slope[Cell_value-1] <<endl;
					cout << "parameter_id_input      =\t" <<parameter_id_input <<endl;
					cout << "g_outflc                =\t" <<g_outflc[Cell_value-1] <<endl;
				}

				//calculate_RoutOrder wurde bereits ausgefuehrt, es sollten alle Zellen ge-routet sein.
				if (inflow_count[Cell_value-1]!=0) {
					cerr << "cell " << Cell_value << " : not all routed from upstream. Check input.\n"; exit (1);
				}
				L=riverLength[Cell_value-1];

				/*Falls nach der Berechnung von calc_RoutingOrder eine beliebige Zelle z.B in der Datenbank veraendert wurde,
				wird hier zur Sicherheit noch einmal mitge-routet. Denn die if Abfrage sagt nur, dass die aktuelle Zelle richtig
				ge-routet wurde, wohingegen, dass mit anderen nicht umbedingt der Fall sein muss.*/
				inflow_count[Cell_value-1]--;

				int outflowCell = g_outflc[Cell_value-1];  // Abflusszelle finden
				if (outflowCell>=0) inflow_count[outflowCell-1]--;

				X_cell = L;  // default - Position am Ende der Zelle




				for (int month=1; month<=12; month++) {
					if(Q_low_limit==NODATA) Q_low_limit_km3month=NODATA;
					else Q_low_limit_km3month = Q_low_limit * 3600.*24.*nDaysPerMonth[month-1] / 1e9;
					if (s_cell[Cell_value-1][month-1]==-9999) {cout << "cell " << Cell_value<<" hat kein load!\n"; continue;} // kein load fuer die Zelle vorhanden - wird nicht berechnet

					C0 = 0.0; C0_2 = 0.0; Q_in_sum = 0.0; Q_in_max = 0.0;

					//fuer die aktuelle Zelle werden alle acht Nachbarzellen betrachtet
					for (int i=0; i<8; i++) {
						if (inflow[Cell_value-1][i]==0) {
							C_in[i][month-1] = 0.0; C_in_2[i][month-1] = 0.0;
							Q_in[i][month-1] = 0.0;
						}
						else {
							/*inflow ist die Zellennummer, aus der der Inflow kommt; im vorherigen Schritt wurde der
							Inflow fuer die aktuelle Zelle bereits berechnet. Die erste Zelle hat keinen Inflow,
							sondern nur das was in der Zelle selbst produziert wird.*/
							C_in[i][month-1] = concentration[inflow[Cell_value-1][i]-1][month-1];
							C_in_2[i][month-1] = concentration2[inflow[Cell_value-1][i]-1][month-1];
							Q_in[i][month-1] = Q_out[inflow[Cell_value-1][i]-1][month-1];
						}

						//================== testausgabe
						//if (Cell_value==94073||Cell_value==68569 ||Cell_value==73873)
						//	cout << i<<'\t'<< month << '\t'<<Q_in[i][month-1]<< '\t'<<inflow[Cell_value-1][i]<<endl;
						//Fracht aus allen Zufluessen zusammen
						C0       += Q_in[i][month-1]*C_in[i][month-1];
						C0_2     += Q_in[i][month-1]*C_in_2[i][month-1];
						//Summes der Zufluesse
						Q_in_sum += Q_in[i][month-1];
						
						//maximalen Zufluss abspeichern
						if (Q_in_max<Q_in[i][month-1]) Q_in_max = Q_in[i][month-1];
					} //for (i)

					//=======================================================
					// Fliessgeschwindigkeit bestimmen
					//=======================================================
					switch (optionsWQ.flowVelocityVariant) {
						case 0:                                 // Tabelle oder UNF
							if (!optionsWQ.InputType) flowVelocity = flowVelocity_const[Cell_value-1];
							else            flowVelocity = flowVelocity_var[Cell_value-1][month-1];
							break;
						case 1:                                 // rechnen
							flowVelocity = get_flowVelocity(Q_out[Cell_value-1][month-1]*(1000000000./(3600.*24.*nDaysPerMonth[month-1])), // [m3/sec]
										g_slope[Cell_value-1]);                    // [m/sec]
							break;
						case 2:                                 // const fur jede Zelle
							flowVelocity = optionsWQ.ConstVelocity;    // [m/s]
							break;
					};


					flowVelocity_month = flowVelocity*3600*24*nDaysPerMonth[month-1]/1000;                 // m/sec -> km/Monat
					factor_month       = decomposition[Cell_value-1] * (3600*24*nDaysPerMonth[month-1]);   // 1/sec -> 1/Monat
					factorLake_month   = decay_rate_lake[Cell_value-1] * (3600*24*nDaysPerMonth[month-1]); // 1/sec -> 1/Monat


					//Qd     = ((Q_in_sum==0|| Q_out[month-1]==0)? log(Q_out[month-1]/10e-12)/L :log(Q_out[month-1]/Q_in_sum)/L);
					if (Q_in_sum==0) Qx_in  = 10e-12; else Qx_in  = Q_in_sum;
					if (Q_out[Cell_value-1][month-1]==0)    Qx_out = 10e-12; else Qx_out = Q_out[Cell_value-1][month-1];
					//log ist der natuerliche Logarithmus
					Qd = log(Qx_out/Qx_in)/L;

					//s ist immer die Substanz und c die Konzentration; geogen ist die Hintergundfracht/konzentration
					double s_geogen;
//
					// bei is_c_geogen_country == 0 fuer Salz s_geogen wird in wq_load berechet und ist in der Fracht s_cell bereits drin
					// ACHTUNG! Unterschied zu diesem Code:
					// in wq_load wird surface runoff + groundwater inflow into streams(gw_runoff_cell) statt  cell_runoff_total benutzt!
					if(is_c_geogen_country && toCalc==1){
						//s_geogen= c_geogen[Cell_value-1] * cell_runoff_total[Cell_value-1][month-1];
						//ohne runoff gibt es keine Hintergrundfracht
						if (cell_runoff_total[Cell_value-1][month-1]<0) s_geogen = 0.0;
						//Hintergrundfracht wird berechnet
						else s_geogen= c_geogen[Cell_value-1] * cell_runoff_total[Cell_value-1][month-1];
					}else{
						s_geogen=0.0;	//im mixed Ansatz ist in der Fracht s_cell bereits der geogene Hintergrund mit drin
					}



					//Die Fracht der Zelle pro Monat plus Hintergrundfracht
					double S_input  = s_cell[Cell_value-1][month-1]+s_geogen;        // [t/month]

					//in case the river availability on the cell is below a threshold
					//nothing is produced nor routed through this cell
					if (Q_low_limit_km3month!=NODATA && !Q_low_limit_type && Q_out[Cell_value-1][month-1]<= Q_low_limit_km3month ){
						concentration[Cell_value-1][month-1]  =0.;
						concentration2[Cell_value-1][month-1] =0.;
						if(Cell_value-1==2)
							cout<<"threshold 0 used cell="<<Cell_value<<endl;
					}else{
					//==================================================================
					if (FlgCons) { // conservative substance
						//ATTENTION: If a new process is implemented here, it probably
						//has to be implemented in wq_stat.cpp as well
						//There a similar but slightly different calculation is applied
						//wenn es keinen Zufluss gab, dann ist C0 Null, sonst wird aus C0 eine Konzentration aus dem Zufluss, vorher war in C0 eine Fracht
						C0     = (Q_in_sum==0? 0.0 :C0/ Q_in_sum);
						C0_2   = (Q_in_sum==0? 0.0 :C0_2/ Q_in_sum);

						if (Q_low_limit_km3month!=NODATA && Q_low_limit_type && Q_out[Cell_value-1][month-1]<= Q_low_limit_km3month) {
							concentration[Cell_value-1][month-1]  =C0;
							concentration2[Cell_value-1][month-1] =C0_2;
							if(Cell_value-1==2)
								cout<<"threshold C0 used cell="<<Cell_value<<endl;
						} else {

							Cd_2 = 0.0;  //only for non conservative substance

							if ( Q_out[Cell_value-1][month-1]<= 0.0) Cd_1 = 0;
							//Konzentration im Abfluss, aus der Fracht, die in der Zelle entsteht
							else Cd_1 = S_input/Q_out[Cell_value-1][month-1];

							if (Q_out[Cell_value-1][month-1]== 0.0) {
							  concentration[Cell_value-1][month-1] = 0.0;
							  concentration2[Cell_value-1][month-1] = 0.0;
							}
							else { //Vo� 2011 Eq 6 Konzentration fuer conservative substances
							  double exponent=exp(-Qd * L);
							  /*e hoch (-ln(Qout/Qin)) = 1/[e hoch (ln(Qout/Qin))] =Qin/Qout
							  Aufgrund des steady state darf Qin nicht groesser sein als Qout, Qout kann aber groesser
							  als Qin sein, da es noch Zufluss aus der Zelle selbst gibt.*/
							  if (exponent>1.0) exponent = 1.0;
							  concentration[Cell_value-1][month-1] = C0   * exponent + Cd_1 * (1.0 - exponent);
							  concentration2[Cell_value-1][month-1] = 0.0;
							}
						}

					} // if(FlgCons)
					else { // non-conservative substance
						//ATTENTION: If a new process is implemented here, it probably
						//has to be implemented in wq_stat.cpp as well
						//There a similar but slightly different calculation is applied
					//==================================================================
						//wenn es keinen Abfluss gab, dann ist C0 Null, sonst wird aus C0 eine Konzentration aus dem Abfluss, vorher war in C0 eine Fracht

						C0     = (Q_out[Cell_value-1][month-1]==0? 0.0 :C0/ Q_out[Cell_value-1][month-1]);
						C0_2   = (Q_out[Cell_value-1][month-1]==0? 0.0 :C0_2/ Q_out[Cell_value-1][month-1]);

						if (Q_low_limit_km3month!=NODATA && Q_low_limit_type && Q_out[Cell_value-1][month-1]<= Q_low_limit_km3month) {
							concentration[Cell_value-1][month-1]  =C0;
							concentration2[Cell_value-1][month-1] =C0_2;
							if(Cell_value-1==2)
								cout<<"threshold C0 used cell="<<Cell_value<<endl;
						} else { // kein Q_low_limit
							if(toCalc==2){//FC
								//Ansatz von Thomann und M�ller
								//depth is in m because the formula from Verzano 2012 enters Q_out in m^3/s and returns m
								depth=0.349*pow(Q_out[Cell_value-1][month-1]*(1000000000./(3600.*24.*nDaysPerMonth[month-1])),0.341); //depth in m
								if(sed_veloc==NODATA||depth==0) ksd=0.0;
								else
									ksd=(sed_veloc*nDaysPerMonth[month-1])/depth;
								//if(Cell_value==125891)
								//	cout<<"ksd="<<ksd<<" sed_veloc="<<sed_veloc<<" depth="<<depth<<" Q_out[Cell_value-1][month-1]="<<Q_out[Cell_value-1][month-1]<<" nDaysPerMonth[month-1]="<<nDaysPerMonth[month-1]<<endl;

							}
							if(toCalc==2){ //seperated in case the sedimentation shall be used for another substance too
								if(Q_out[Cell_value-1][month-1]==0||Q_out[Cell_value-1][month-1]==NODATA||ke[Cell_value-1]==0||ke[Cell_value-1]==NODATA){
									krad=0.0;
								}else{
									//if nan appear, this should never be the case!
									if(((ks*radiation[Cell_value-1][month-1]*((860*24*nDaysPerMonth[month-1])/10000))/(depth*ke[Cell_value-1]))*(1-exp(-ke[Cell_value-1]*depth))!=((ks*radiation[Cell_value-1][month-1]*((860*24*nDaysPerMonth[month-1])/10000))/(depth*ke[Cell_value-1]))*(1-exp(-ke[Cell_value-1]*depth))){
										cerr<<"Nan: ((ks*radiation[Cell_value-1][month-1]*((860*24*nDaysPerMonth[month-1])/10000))/(depth*ke[Cell_value-1]))*(1-exp(-ke[Cell_value-1]*depth))="<<((ks*radiation[Cell_value-1][month-1]*((860*24*nDaysPerMonth[month-1])/10000))/(depth*ke[Cell_value-1]))*(1-exp(-ke[Cell_value-1]*depth))
												<<"\nks="<<ks<<" radiation[Cell_value-1][month-1]="<<radiation[Cell_value-1][month-1]<<" nDaysPerMonth[month-1]="<<nDaysPerMonth[month-1]<<"\ndepth="<<depth<<" ke[Cell_value-1])="<<ke[Cell_value-1]
												<<" Cell_value="<<Cell_value<< " month="<<month<<endl;
									}
									//ke[Cell_value-1]=0.3;
									krad=((ks*radiation[Cell_value-1][month-1]*((860*24*nDaysPerMonth[month-1])/10000))/(depth*ke[Cell_value-1]))*(1-exp(-ke[Cell_value-1]*depth));	//radiation: W/m^2 in cal/cm^2
								}

							}else{
								dec_T = factor_month; //Abbaurate je nach Substanz
								krad=0.0;
								ksd=0.0;
							}


							if (UseWaterTemp) {  //Vo� 2011 Eq 5 temperature decay
							    //dec_T =    factor_month * pow(TETA,water_temp[Cell_value-1][month-1]-20.0);
								dec_T =   (factor_month * pow(TETA,water_temp[Cell_value-1][month-1]-20.0)) + krad +ksd;

								//this should neve happen
								if(isinf(dec_T)){
									cerr<<"decT=inf!"<<endl;

									cerr<<"radiation[month][cell]: "<<radiation[Cell_value-1][month-1]
										<<" cell-1: "<<Cell_value-1<<" month-1: "<<month-1<< " year: "<<actual_year
										<<"\nkrad: "<<krad<<" dec_t: "<<dec_T<<" factor_month: "<<factor_month
										<<" ks: "<<ks<<" ke[Cell_value-1]="<<ke[Cell_value-1]
										<<" TETA: "<<TETA
										<<"\npow(TETA,water_temp[Cell_value-1][month-1]-20.0)="<<pow(TETA,water_temp[Cell_value-1][month-1]-20.0)
										<<"\n(factor_month * pow(TETA,water_temp[Cell_value-1][month-1]-20.0))="<<(factor_month * pow(TETA,water_temp[Cell_value-1][month-1]-20.0))
										<<"\nQ_out[Cell_value-1][month-1]: "<<Q_out[Cell_value-1][month-1]
										<<"\nflowVelocity_month: "<<flowVelocity_month
										<<"\nwater_temp[Cell_value-1][month-1]: "<<water_temp[Cell_value-1][month-1]
										<<"\nksd: "<<ksd<<" sed_veloc: "<<sed_veloc<<" depth: "<<depth<<" nDaysPerMonth[month-1]: "<<nDaysPerMonth[month-1]<<endl;


									exit(1);
								}

								if(Cell_value==156606 || Cell_value==79127)
									cout<<"radiation[month][cell]: "<<radiation[Cell_value-1][month-1]
										<<" cell-1: "<<Cell_value-1<<" month-1: "<<month-1<< " year: "<<actual_year
										<<"\nkrad: "<<krad<<" dec_t: "<<dec_T<<" factor_month: "<<factor_month
										<<" ks: "<<ks<<" ke[Cell_value-1]="<<ke[Cell_value-1]
										<<" Q_out[Cell_value-1][month-1]: "<<Q_out[Cell_value-1][month-1]
										<<" flowVelocity_month: "<<flowVelocity_month
										<<"\nksd: "<<ksd<<" sed_veloc: "<<sed_veloc<<" depth: "<<depth<<" nDaysPerMonth[month-1]: "<<nDaysPerMonth[month-1]<<endl;

							/*	//     Guadiana 			- Mosel/Rhine 			- Po 			- Vistula 			- Meuse 				- South Tyne	  - North Tyne			-Duero
								if(Cell_value==156606 || Cell_value==79127|| Cell_value==124333 || Cell_value==91924 || Cell_value==90671 || Cell_value==61794 || Cell_value==61798 || Cell_value==141951)
									cout<<"radiation[month][cell]: "<<radiation[Cell_value-1][month-1]
									<<" cell-1: "<<Cell_value-1<<" month-1: "<<month-1<< " year: "<<actual_year
									<<"\nkrad: "<<krad<<" dec_t: "<<dec_T<<" factor_month: "<<factor_month
									<<" ks: "<<ks<<" ke[Cell_value-1]="<<ke[Cell_value-1]
									<<" Q_out[Cell_value-1][month-1]: "<<Q_out[Cell_value-1][month-1]
									<<" flowVelocity_month: "<<flowVelocity_month
									<<"\nksd: "<<ksd<<" sed_veloc: "<<sed_veloc<<" depth: "<<depth<<" nDaysPerMonth[month-1]: "<<nDaysPerMonth[month-1]<<endl;
							*/
								/*
								// 		Mhlatuze			Mfolozi					Mthala				Phongolo				Pienaars1			Pienaars2
								if(Cell_value==361690 || Cell_value==361116|| Cell_value==358889 || Cell_value==358678 || Cell_value==354133 || Cell_value==353912)
									cout<<"radiation[month][cell]: "<<radiation[Cell_value-1][month-1]
									<<" cell-1: "<<Cell_value-1<<" month-1: "<<month-1<< " year: "<<actual_year
									<<"\nkrad: "<<krad<<" dec_t: "<<dec_T<<" factor_month: "<<factor_month
									<<" ks: "<<ks<<" ke[Cell_value-1]="<<ke[Cell_value-1]
									<<" Q_out[Cell_value-1][month-1]: "<<Q_out[Cell_value-1][month-1]
									<<" flowVelocity_month: "<<flowVelocity_month
									<<"\nksd: "<<ksd<<" sed_veloc: "<<sed_veloc<<" depth: "<<depth<<" nDaysPerMonth[month-1]: "<<nDaysPerMonth[month-1]<<endl;
								*/

							}


							//Berechnet die Flaeche vom Abfluss
							Ac_1   = Q_out[Cell_value-1][month-1]/flowVelocity_month;

							//Berechnet die Flaeche vom Abfluss und Zufluss
							Ac_2   = (Q_in_sum+Q_out[Cell_value-1][month-1])/(2*flowVelocity_month);

							//Auch wenn Q_in_sum != 0 und Q_out=0 soll Ac_2 Null sein
							if (Q_out[Cell_value-1][month-1]==0.0) Ac_2 = 0.0;

							//Vo� 2011 Eq 3 Fracht geteilt durch das, was im Fluss abgebaut wird; zwei ineinandergeschachtelte if-Schleifen
							Cd_1 = (Ac_1==0? 0.0 : dec_T==0.0? 0.0 : S_input/L/Ac_1/dec_T);
							Cd_2 = (Ac_2==0? 0.0 : dec_T==0.0? 0.0 : S_input/L/Ac_2/dec_T);



							//Vo� 2011 Eq 1 Konzentration fuer non-conservative substances
							concentration[Cell_value-1][month-1]  = C0   * exp(-dec_T * L/flowVelocity_month)
								+ Cd_1 * (1.0 - exp(-dec_T * L/flowVelocity_month));

							concentration2[Cell_value-1][month-1] = C0_2 * exp(-dec_T * L/flowVelocity_month)
								+ Cd_2 * (1.0 - exp(-dec_T * L/flowVelocity_month));

						} // kein Q_low_limit

					//==================================================================
					} // non-conservative substance

//lake
					//============================ LAKE ============================
					// TODO: z.Zeit wird nicht benutzt und in wq_stat nicht gibt, bitte wenn Lake-Modul fertig gestellt wird,
					//       ihn auch in wq_stat reintun!
					if (FlgLake && factorLake_month > 0.0                     // Abbau im See/Reservoir wenn Abbau und See/Reservoir gibt
						&& (G_monthlyGloLakeStorage[Cell_value-1][month-1]>0.0 || G_monthlyResStorage[Cell_value-1][month-1]>0.0)
						&& Q_out[Cell_value-1][month-1] > Q_low_limit_km3month) {
						// load im See
						double W  = concentration[Cell_value-1][month-1]  * Q_out[Cell_value-1][month-1];
						double W2 = concentration2[Cell_value-1][month-1] * Q_out[Cell_value-1][month-1];
						
						decLake_T = factorLake_month;  // Abbaufaktor in Seen
						if (UseWaterTemp) { //Vo� 2011 Eq 5 temperature decay hier fuer Seen
							decLake_T = factorLake_month * pow(TETA_lake,water_temp[Cell_value-1][month-1]-20.0);
						}
						
						if (Cell_value==38702||Cell_value==107708) {
							cout << "Cell_value              =\t" <<Cell_value <<endl;
							cout << "month                   =\t" <<month <<endl;
							cout << "factorLake_month        =\t" <<factorLake_month <<endl;
							cout << "Q_out                   =\t" <<Q_out[Cell_value-1][month-1] <<endl;
							cout << "G_monthlyGloLakeStorage =\t" <<G_monthlyGloLakeStorage[Cell_value-1][month-1] <<endl;
							cout << "G_monthlyResStorage     =\t" <<G_monthlyResStorage[Cell_value-1][month-1] <<endl;
							cout << "con                     =\t" <<concentration[Cell_value-1][month-1] <<endl;
							cout << "con2                    =\t" <<concentration2[Cell_value-1][month-1] <<endl;
							cout << "W                       =\t" <<W <<endl;
							cout << "W2                      =\t" <<W2 <<endl;
							cout << "decLake_T               =\t" <<decLake_T << endl;
						}
						
						//Konzentratin ergibt sich aus der Fracht geteilt durch ein h�heres Wasservolumen der Seen etc. (Storages)
						concentration[Cell_value-1][month-1] = W / (Q_out[Cell_value-1][month-1] 
							+ decLake_T*(G_monthlyGloLakeStorage[Cell_value-1][month-1]+G_monthlyResStorage[Cell_value-1][month-1]) );

						concentration2[Cell_value-1][month-1] = W2 / (Q_out[Cell_value-1][month-1] 
							+ decLake_T*(G_monthlyGloLakeStorage[Cell_value-1][month-1]+G_monthlyResStorage[Cell_value-1][month-1]) );
					}
					//============================ LAKE ============================
//.........
//
					/**/										// 		Mhlatuze			Mfolozi					Mthala				Phongolo				Pienaars1			Pienaars2
					//if (concentration[Cell_value-1][month-1]<0 || (Cell_value==361690 || Cell_value==361116|| Cell_value==358889 || Cell_value==358678 || Cell_value==354133 || Cell_value==353912)) {
					//if (concentration[Cell_value-1][month-1]<0 || (Cell_value==26137 || Cell_value==26574|| Cell_value==26136 )) {
					//     Guadiana 			- Mosel/Rhine 			- Moselle 			- Vistula 			- Meuse 				- South Tyne				-Duero			-South Tyne 	 -North Tyne 			- Duero				-  Duero			-  Duero			-  Duero			-  Duero			-  Duero
					//if(concentration[Cell_value-1][month-1]<0 || Cell_value==91237 || Cell_value==79127|| Cell_value==124333 || Cell_value==91924 || Cell_value==141506 || Cell_value==61794 || Cell_value==61798 || Cell_value==141951 || Cell_value==141945|| Cell_value==141946|| Cell_value==141940|| Cell_value==141941|| Cell_value==141508){

//					if(concentration[Cell_value-1][month-1]<0 || Cell_value==91237 || Cell_value==79127)
					if(concentration[Cell_value-1][month-1]<0 || Cell_value==371405 || Cell_value==371109 || Cell_value==371087)
					{


						cout << "Cell_value       =\t" <<Cell_value <<endl;
						cout << "month            =\t" <<month <<endl;
						//cout << "NumberInflow =\t" <<NumberInflow << endl;
						//cout << "Time         =\t" <<Time << endl;
						cout << "factor           =\t" <<decomposition[Cell_value-1] <<endl;
						cout << "factor_month     =\t" <<factor_month <<endl;
						cout << "factorLake_month =\t" <<factorLake_month <<endl;
						cout << "Q_out            =\t" <<Q_out[Cell_value-1][month-1] <<endl;
						cout << "Q_in_sum         =\t" <<Q_in_sum <<endl;
						cout << "s_cell           =\t" <<s_cell[Cell_value-1][month-1] <<endl;
						cout << "s_geogen         =\t" <<s_geogen <<endl;
						cout << "C0 =\t" <<C0   << "\t" << C0_2 << endl;
						cout << "Cd =\t" <<Cd_1 << "\t" << Cd_2 << endl;
						cout << "Qd =\t" <<Qd   << "\nL =\t" << L << "\nflowVelocity =\t"  << flowVelocity << endl;
						cout << "flowVelocity_month =\t"<<flowVelocity_month<<endl;
						cout << "Ac =\t" <<Ac_1 << "\t" << Ac_2 << endl;
						if(is_c_geogen_country && toCalc==1)
							cout << "cell_runoff_total=\t"<<cell_runoff_total[Cell_value-1][month-1]<<endl;
						
						// bei nicht konservativen Substanzen, wenn Wassertemperatrur benutzt wurde
						if (factor_month!=0.0 && UseWaterTemp) {
							cout << "water_temp =\t" <<water_temp[Cell_value-1][month-1] << endl;
							cout << "dec_T      =\t" <<dec_T << endl;
						}
						//cout << "cell_runoff_total[Cell_value-1][month-1]=\t"<<cell_runoff_total[Cell_value-1][month-1]<<endl;
						cout << "Konz_1 =\t" <<concentration[Cell_value-1][month-1]   << "\t" << concentration2[Cell_value-1][month-1] << endl;
						cout << "================================================\n";
					}/**/

				  }//end else Qout above a threshold
				} // for (month)
				
				
			} // for (routNumber)
			// Ergebnisse speichern
			

			query.reset();
			query << "DESCRIBE `"<<MyDatabase<<"wq_out_"<<optionsWQ.continent_abb<<"`.concentration_" <<IDrun<<";" ;

			try 
			{
				res = query.store();
				// wenn kein Fehler bei DESCRIBE, Tabelle existiert schon, dann Zeilen aus berechneten Periode loeschen.

				query.reset();
				query << "LOCK TABLES "<<MyDatabase<<"wq_out_"<<optionsWQ.continent_abb<<".concentration_" <<IDrun<<" WRITE;";
				query.execute();
				cout << "LOCK TABLES "<<MyDatabase<<"wq_out_"<<optionsWQ.continent_abb<<".concentration_" <<IDrun<<"...\n";
        
				query.reset();
				j=sprintf(sql,"DELETE from `%swq_out_%s`.`concentration_%d` where date between %d and %d;", MyDatabase, optionsWQ.continent_abb, IDrun, actual_year*100+1, actual_year*100+12);
				query << sql;

				if (!query.execute()) {
					cerr << "Error in delete from `concentration`";
					return 1;
				}

				cout << "delete from `concentration` done\n";

			}//try for resQuery
			catch (const mysqlpp::BadQuery&) {
				// Fehler bei DESCRIBE - Tabelle nicht existiert, Tabelle erzeugen und dann blokieren zum Schreiben

				query.reset();
				j=sprintf(sql,"CREATE TABLE  `%swq_out_%s`.`concentration_%d` (", MyDatabase, optionsWQ.continent_abb, IDrun);
				j+=sprintf(sql+j," `date` int(10) unsigned NOT NULL COMMENT 'year*100+month',");
				j+=sprintf(sql+j," `cell` int(10) unsigned NOT NULL,");
				j+=sprintf(sql+j," `con` double default NULL COMMENT 't/km3; FC: 10^10cfu/km^3',");
				j+=sprintf(sql+j," `con2` double default NULL,");
				j+=sprintf(sql+j," PRIMARY KEY  USING BTREE (`date`,`cell`)");
				j+=sprintf(sql+j," ) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci");
				j+=sprintf(sql+j," COMMENT='montly grid cell concentration (t/km3; FC: 10^10cfu/km^3)';");
				
				query << sql;
				if (!query.execute()) {
					cout << "Error in CREATE TABLE `"<<MyDatabase<<"wq_out_"<<optionsWQ.continent_abb<<"`.`concentration_"<<IDrun<<"`";
					return 1;
				}
				cout << "CREATE TABLE `"<<MyDatabase<<"wq_out_"<<optionsWQ.continent_abb<<"`.`concentration_"<<IDrun<<"` done\n";

				query.reset();
				query << "LOCK TABLES `"<<MyDatabase<<"wq_out_"<<optionsWQ.continent_abb<<"`.`concentration_"<<IDrun<<"` WRITE;";
				query.execute();
				cout << "LOCK TABLES `"<<MyDatabase<<"wq_out_"<<optionsWQ.continent_abb<<"`.`concentration_"<<IDrun<<"`...\n";
			} // catch

			int counter=0;
			j=sprintf(sql,"INSERT INTO `%swq_out_%s`.concentration_%d (`date`, `cell`, `con`, `con2`) VALUES ", MyDatabase, optionsWQ.continent_abb, IDrun);
			for (int month=1; month<=12; month++) 
			  for (int Cell_value=1; Cell_value<=continent.ng; Cell_value++) {
			  	if (counter==10) {
			  		counter=0;
			  		j+=sprintf(sql+j,";");
			  		query.reset(); query << sql;
					//#ifdef DEBUG_queries
			  		//	cout << "Query: " << query << endl << endl; //cin >> j;
					//#endif

			  		query.execute();
			  		j=sprintf(sql,"INSERT INTO `%swq_out_%s`.concentration_%d (`date`, `cell`, `con`, `con2`) VALUES ", MyDatabase, optionsWQ.continent_abb, IDrun);
			  		
			  	}
			  	//if (Cell_value==105370||Cell_value==105959 ||Cell_value==121553)
				if (counter>0) j+=sprintf(sql+j,", ");
				j+=sprintf(sql+j, "(%d, %d, %.5f, %.5f)", actual_year*100+month, Cell_value, concentration[Cell_value-1][month-1], concentration2[Cell_value-1][month-1]);
				counter++;

			} // for(Cell_value)
			if (counter>0) {
		  		j+=sprintf(sql+j, ";");
		  		query.reset(); query << sql;
		  		query.execute();
			}
        
			
			
			cout << "INSERT results into `"<<MyDatabase<<"wq_out_"<<optionsWQ.continent_abb<<"`.`concentration_"<<IDrun<<"` year " << actual_year<<" done\n";
            
            query.reset();
			query << "UNLOCK TABLES;";
			query.execute();

			
		} // for(actual_year)
		


	}  // end try
	catch (const mysqlpp::BadQuery& er) {
        // Handle any query errors
        cerr << "Query error: " << er.what() << "." << endl;
        //return -1;
    }
	catch (const mysqlpp::BadConversion& er) {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
                "\tretrieved data size: " << (int)er.retrieved <<
                ", actual size: " << (int)er.actual_size << endl;
        //return -1;
    }
	//catch (const mysqlpp::SQLQueryNEParms& er) {}  // version mysql++.2.3.3  SQLQueryNEParms renamed to BadParamCount
	catch (const mysqlpp::BadParamCount& er) {
		cerr << "parms error: " << er.what() << endl;
	}
	catch (const std::exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		//return 1;
	}
    catch (char * str) {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << str << endl;
        //return -1;
    }

	//cout << "end...";  cin >> j;

	// print current date
	cout <<"\nProgram worldqual endet at: " << getTimeString() << endl << endl;
	return 0;
} // end of main()


void updateRouting(int cell, options_wqClass* optionsWQ_ptr) {
	int j;
	char sql[2000]; // variable for SQL-query

	//if (cell==4693||cell==4692) cout << "cell = "<<cell<<" updaterouting!\n";

	query.reset();
	j=sprintf(sql, "update %sworldqual_%s.routing set inflow_done=inflow_done-1 where cell=%d", MyDatabase, optionsWQ_ptr->continent_abb,  cell);
	query << sql;
	query.execute();
} // end of updateRouting()

void get_Liste(int IDrun, int modus, options_wqClass* optionsWQ_ptr) {
	int j;
	char sql[2000]; // variable for SQL-query

	if (modus==0) j=sprintf(sql, "SELECT cell FROM %sworldqual_%s.routing WHERE inflow_done=0 ", MyDatabase, optionsWQ_ptr->continent_abb);
	else          j=sprintf(sql, "SELECT cell FROM %sworldqual_%s.routing WHERE inflow_done>0 ", MyDatabase, optionsWQ_ptr->continent_abb);

	resListe_X.clear();

	query.reset();
	query << sql;
	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif
	query.storein(resListe_X);
} // end of get_Liste()





int calculate_riverLength(double *riverLength, int *g_outflc, options_wqClass* optionsWQ_ptr, int *gcrc, const continentStruct* continent_ptr) {
	int j, cell, cell_out;
	double length;
	char sql[2000]; // variable for SQL-query
	mysqlpp::StoreQueryResult resQuery;
	mysqlpp::Row    rowQery;
	
	int *gc = new int[continent_ptr->ng];
	int *gr = new int[continent_ptr->ng];
	double (*g_cell_dist)[9] = new double[continent_ptr->nrows][9];
	double *sinuosity = new double[continent_ptr->ng];
	
	// lock tables
	query.reset();
	query << "LOCK TABLES "<<MyDatabase<<"worldqual_"<<optionsWQ_ptr->continent_abb<<".`flow_velocity` as `f` WRITE, watergap_unf.`gr` as `gr` READ, `watergap_unf`.`gc` as `gc` READ, ";
	query << " watergap_unf.`g_outflc` READ;";
	query.execute();
	cout << "LOCK TABLES "<<MyDatabase<<"worldqual_"<<optionsWQ_ptr->continent_abb<<".`routing`...\n";

	query.reset();
	j=sprintf(sql,    "SELECT gr.cell, gr.row FROM watergap_unf.gr gr");
	j+=sprintf(sql+j, " WHERE gr.IDVersion=%d AND gr.IDReg=%d;", optionsWQ_ptr->IDVersion, optionsWQ_ptr->IDReg);
	query << sql;
	resQuery = query.store();

//	try 
	{
		//while (rowQery = resQuery.fetch_row()) 
		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			cell=resQuery[rowNum]["cell"]; //cell=rowQery["cell"];
			if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in watergap_unf.gr.\n"; return 1;}
			gr[cell-1]=resQuery[rowNum]["row"]; //rowQery["row"];
			if (gr[cell-1]>continent_ptr->nrows) {cerr << "row "<< gr[cell-1] << " not exist! error in watergap_unf.gr.\n"; return 1;}
		}
	}//try for resQuery


	query.reset();
	j=sprintf(sql,    "SELECT gc.cell, gc.column FROM watergap_unf.gc gc");
	j+=sprintf(sql+j, " WHERE gc.IDVersion=%d AND gc.IDReg=%d;", optionsWQ_ptr->IDVersion, optionsWQ_ptr->IDReg);
	query << sql;
	cout<<"Query gc:"<<query<<endl;
	resQuery = query.store(); 

//	try 
	{
		//while (rowQery = resQuery.fetch_row()) 
		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			cell=resQuery[rowNum]["cell"]; //rowQery["cell"];
			if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in watergap_unf.gc.\n"; return 1;}
			gc[cell-1]=resQuery[rowNum]["column"]; //rowQery["column"];
			if (gc[cell-1]>continent_ptr->ncols) {cerr << "column "<< gc[cell-1] << " not exist! error in watergap_unf.gc.\n"; return 1;}
		}
		cout << "gc read in done\n";
	}//try for resQuery


	cout<<"GCELLDIST:continent.ng"<<continent_ptr->ng<<"continent.ng_land"<<continent_ptr->ng_land<<"continent.nrows"
		<<continent_ptr->nrows<<"continent.ncols"<<continent_ptr->ncols<<"continent.cellFraction"<<continent_ptr->cellFraction<<endl;
	char filename[maxcharlength];
	sprintf(filename, "%s/GCELLDIST.9.UNF0", optionsWQ_ptr->path_hydro_routing);
	if (readInputFile(filename, 4, 9, continent_ptr->nrows, &g_cell_dist[0][0], gcrc, optionsWQ_ptr, continent_ptr)) exit(1);


	/*for(int i=0;i<continent.nrows;i++){
		for(int j=0;j<9;j++){
			cout <<"g_cell_dist["<<i<<"]["<<j<<"] "<<g_cell_dist[i][j]<<endl;
		}
	}*/


	sprintf(filename, "%s/G_MEANDERING_RATIO.UNF0", optionsWQ_ptr->path_hydro_input);
	if (readInputFile(filename, 4, 1, continent_ptr->ng, &sinuosity[0], gcrc, optionsWQ_ptr, continent_ptr)) exit(1);


	/*for(int i=0;i<continent.ng;i++){
		cout <<"sinuosity["<<i<<"] "<<sinuosity[i]<<endl;
	}*/

	for (cell=1; cell<=continent_ptr->ng; cell++) {

		cell_out=g_outflc[cell-1];
		if (cell_out==0) {if (optionsWQ_ptr->IDVersion==2)  length = 55.0; else length= 9.0;} //keine Abflusszelle (sinkt oder Abfluss ins Ocean)
		else {
			if (gr[cell-1]==gr[cell_out-1])     length=g_cell_dist[gr[cell-1]-1][3];  // west
			else if (gr[cell-1]>gr[cell_out-1]) {
				if (gc[cell-1]==gc[cell_out-1]) length=g_cell_dist[gr[cell-1]-1][7];  // north
				else                        length=g_cell_dist[gr[cell-1]-1][8];  // north-east
			}
			else {
				if (gc[cell-1]==gc[cell_out-1]) length=g_cell_dist[gr[cell-1]-1][1];  // south
				else                        length=g_cell_dist[gr[cell-1]-1][2];  // south-east
			}
		} // else( cell_out!=0)

		if(length==-99.0)
			cerr<<"length is -99, this should not happen: "<<length
			<<"\ngr[cell-1]="<<gr[cell-1]<<" gr[cell_out-1]="<<gr[cell_out-1]
			<<" g_cell_dist[gr[cell-1]-1][8]="<<g_cell_dist[gr[cell-1]-1][8]
			<<" gc[cell-1]="<<gc[cell-1]<<" gc[cell_out-1]="<<gc[cell_out-1]
			<<" cell="<<cell<<" cell_out="<<cell_out-1<<endl;

		if (optionsWQ_ptr->IDVersion==3) {
			if (cell_out==0) length*=sinuosity[cell-1];
			else length*=(sinuosity[cell-1]+sinuosity[cell_out-1])/2;
		}

		riverLength[cell-1]=length;



	
	
		j=sprintf(sql,    "update %sworldqual_%s.flow_velocity f SET f.`river_length`=%f where cell=%d; ", MyDatabase, optionsWQ_ptr->continent_abb, riverLength[cell-1], cell);
		query.reset();
		query << sql;
		//cout<<query<<endl;
		query.execute();
	} // for(cell)

	query.reset();
	query << "UNLOCK TABLES;";
	query.execute();

	delete[] gc;          gc=NULL;
	delete[] gr;          gr=NULL;
	delete[] g_cell_dist; g_cell_dist=NULL;
	delete[] sinuosity;   sinuosity=NULL;
	
	cout<<"calculate river length done"<<endl;
	return 0;
} // calculate_riverLength()





int calculate_RoutOrder(int *RoutOrder, int *g_outflc, int IDrun, options_wqClass* optionsWQ_ptr, const continentStruct* continent_ptr) {
	int j;
	char sql[2000]; // variable for SQL-query
	mysqlpp::StoreQueryResult resQuery;
	mysqlpp::Row    rowQery;

	cout << "calculate routing order.\n";

	query.reset();
	query << "LOCK TABLES "<<MyDatabase<<"worldqual_"<<optionsWQ_ptr->continent_abb<<".`routing` WRITE, watergap_unf.g_inflc_arc_id as `g` READ, `flow_velocity` as `f` WRITE;";
	query.execute();
	cout << "LOCK TABLES `routing`...\n";

	query.reset();
	j=sprintf(sql,"DELETE from %sworldqual_%s.`routing`;", MyDatabase, optionsWQ_ptr->continent_abb);
	query << sql;
	if (!query.execute()) {
		cout << "Error in delete from routing";
		return 1;
	}
	cout << "delete from routing done\n";

	j=sprintf(sql,"insert into %sworldqual_%s.routing (cell, inflow_count, inflow_done) ", MyDatabase, optionsWQ_ptr->continent_abb);
	j+=sprintf(sql+j, " SELECT g.cell, ");
	j+=sprintf(sql+j, " if(g.SW>0,1,0)+if(g.S>0,1,0)+if(g.SE>0,1,0)+if(g.W>0,1,0)+if(g.E>0,1,0)+if(g.NW>0,1,0)+if(g.N>0,1,0)+if(g.NE>0,1,0) as inflow_count, ");
	j+=sprintf(sql+j, " if(g.SW>0,1,0)+if(g.S>0,1,0)+if(g.SE>0,1,0)+if(g.W>0,1,0)+if(g.E>0,1,0)+if(g.NW>0,1,0)+if(g.N>0,1,0)+if(g.NE>0,1,0) as inflow_done ");
	j+=sprintf(sql+j, "  FROM watergap_unf.g_inflc_arc_id `g` WHERE g.IDVersion=%d AND g.IDReg=%d;", optionsWQ_ptr->IDVersion, optionsWQ_ptr->IDReg );
	query.reset();
	query << sql;
	query.execute();
	cout << "insert into routing done\n";

	get_Liste(IDrun, 0, optionsWQ_ptr);   // Zellenliste, die man schon routen darf (kein Zufluss haben aus nicht kalkullierten Zellen)

	short Step=0;
	int Zellcounter=0;
	int routCounter=0;

	while (!resListe_X.empty()) {
		Step++;
		Zellcounter+=resListe_X.size();
		cout << "Step = " << Step << " count: " << (int)resListe_X.size() << "\tinsgesamt: "<< Zellcounter<<  endl;

		for (itListe_X=resListe_X.begin(); itListe_X!=resListe_X.end(); itListe_X++) {

			//cout << " cell = " << itListe_X->cell;
			// zeiger->x ist fuer Zeiger das aequivalent zu objekt.x
			int Cell_value=itListe_X->cell;
			RoutOrder[routCounter++] = Cell_value;

			//update in der Datenbank; um eins verringert
			updateRouting(Cell_value, optionsWQ_ptr);

			int outflowCell = g_outflc[Cell_value-1];  // Abflusszelle finden

			// update routing fuer naechste Zelle
			if (outflowCell>0) 	updateRouting(outflowCell, optionsWQ_ptr);  // fuer outflowCell eine Zelle mit nicht kallkulierten Zufluss weniger

		} // for (itListe_X)

		// neue resListe_X
		// Zellenliste, die man schon routen darf (kein Zufluss haben aus nicht kalkullierten Zellen) fuer naechsten Schritt
		//In get_liste werden jetzt die Zellen ausgewaehlt, bei denen der inflow Null ist.
		get_Liste(IDrun, 0, optionsWQ_ptr);

	} //while (resListe_X.num_rows()>0)
	// Pruefen, ob es Zellen geblieben sind, die nicht zu ende geroutet sind
	// Falls ja, g_inflc_arc_id und g_outflc passen nicht zu einander
	get_Liste(IDrun, 1, optionsWQ_ptr);
	if (!resListe_X.empty()) {
		cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
			 << "Attention!!! Not all cells were routed!\n "
			 << " count: " << (int)resListe_X.size()
			 << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";

		return 1;
	}

	for (routCounter=0; routCounter<continent_ptr->ng; routCounter++) {
		j=sprintf(sql,    "update %sworldqual_%s.flow_velocity f SET f.`rout_order`=%d where cell=%d; ", MyDatabase, optionsWQ_ptr->continent_abb, routCounter+1, RoutOrder[routCounter]);
		query.reset();
		query << sql;
		query.execute();
	} // for (routCounter)

	query.reset();
	query << "UNLOCK TABLES;";
	query.execute();


	return 0;

} // calculate_RoutOrder()


int get_inflow_count(double *inflow_count, options_wqClass* optionsWQ_ptr, const continentStruct* continent_ptr) {
	int j, cell;
	char sql[2000]; // variable for SQL-query
	mysqlpp::StoreQueryResult resQuery;
	mysqlpp::Row    rowQery;

	query.reset();
	j=sprintf(sql, "SELECT r.`cell`, r.`inflow_count` FROM %sworldqual_%s.routing r;", MyDatabase, optionsWQ_ptr->continent_abb);
	query << sql;
	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif
	resQuery = query.store();

	if (resQuery.empty()) {
		cerr << "error in table routing.\n";
		return 1;
	}

//	try
	{
		//while (rowQery = resQuery.fetch_row())
		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			cell=resQuery[rowNum]["cell"]; //rowQery["cell"];
			if (cell>continent_ptr->ng) {cerr << "cell "<< cell << " not exist! error in routing.\n"; return 1;}
			inflow_count[cell-1]   = resQuery[rowNum]["inflow_count"]; //rowQery["inflow_count"];
		}
	}//try for resQuery


	return 0;
} // end of get_inflow_count

/*
 * Method to return the water temperature 
 * Wassertemperatur wird aus Lufttemperatur berechnet
 * has different scenarios
 */
int get_waterTempAir(int actual_year, double (*water_temp)[12], short UseWaterTemp, double* ConstTemp, int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr) {
	if (!UseWaterTemp) return 0;  // Wassertemperatur wird nicht gebraucht
	if (!options_ptr->isWaterTempFromAir) return 0;  // Wassertemperatur wird nicht aus air temperature berechnet
	char filename[maxcharlength];

	if (options_ptr->climate==0 || options_ptr->climate==2) {
		// Für alle 5'' Zellen mittlere Wert von Höhen allen 5'' Zellen, die in einer 0.5° Zelle befindet.
		float *G_elev_mean = new float[continent_ptr->ng];
		int *G_wg3_to_wg2 = new int[continent_ptr->ng];
		sprintf(filename,"%s/G_ELEV_MEAN_WATCH.UNF0",continent_ptr->hydro_input);
		if (readInputFile(filename, 4, 1, continent_ptr->ng, &G_elev_mean[0], gcrc, options_ptr, continent_ptr)) exit(1);

		// wird nur Wert G_elev_range[Cell_value][0] benutzt: Mittlere Höhe der 5'' Zelle
		// andere 25 Werte sind Höhe einer der Unterzellen (1' Zelle, 5'' geteilt 5X5)
		short (*G_elev_range)[26] = new short[continent_ptr->ng][26];
		sprintf(filename,"%s/G_ELEV_RANGE.26.UNF2",continent_ptr->hydro_input);
		if (readInputFile(filename, 2, 26, continent_ptr->ng_land, &G_elev_range[0][0], gcrc, options_ptr, continent_ptr)) exit(1);

		int ng_wg22;
		short  (*temperature_cell)[12] = new short[continent_ptr->ng][12];   // [100 deg Celsius]
		
		if (options_ptr->climate==0) {
			ng_wg22 = 70412;
			sprintf(filename,"%s/G_WG3_WG2WITH5MIN.UNF4",continent_ptr->hydro_input);
		} else if (options_ptr->climate==2) {
			ng_wg22= 67420;
			sprintf(filename,"%s/G_WG3_WATCH.UNF4",continent_ptr->hydro_input);
		}
		if (readInputFile(filename, 4, 1, continent_ptr->ng, &G_wg3_to_wg2[0], gcrc, options_ptr, continent_ptr)) exit(1);
		
		sprintf(filename, "%s/GTEMP_%d.12.UNF2", options_ptr->path_climate, actual_year);
		if (readInputFileWG2toWG3( filename, 2, 12, continent_ptr->ng, temperature_cell, gcrc, G_wg3_to_wg2, ng_wg22)) exit(1);
		
		for (int Cell_value=0; Cell_value<continent_ptr->ng; Cell_value++) {
			for (int month=1; month<=12; month++) {
				if (Cell_value==10 || Cell_value==1000) cout << "calculate water temperature:"<<endl;
				double adjustmentFactor = (G_elev_mean[Cell_value]-G_elev_range[Cell_value][0])*0.006;
				water_temp[Cell_value][month-1] = temperature_cell[Cell_value][month-1]/100. + adjustmentFactor; // Lufttemperatur korrigiert mit den Zellenhöhe
				water_temp[Cell_value][month-1]=ConstTemp[0]/(1.0+exp(ConstTemp[1]*water_temp[Cell_value][month-1]+ConstTemp[2])); //Wassertemperatur
				if (Cell_value==10 || Cell_value==1000) {
					cout << Cell_value << '\t' << month << '\t' << water_temp[Cell_value][month-1] << '\t' 
						<< temperature_cell[Cell_value][month-1] << '\t' 
						<< G_elev_mean[Cell_value]<< '\t' 
						<< G_elev_range[Cell_value][0]<< '\t' 
						<< adjustmentFactor << '\t'
						<< ConstTemp[0] << '\t'
						<< ConstTemp[1] << '\t'
						<< ConstTemp[2] << '\t'
						<< ConstTemp[0] << endl;
						;
				}
			}
		}
		
		delete[] G_wg3_to_wg2; G_wg3_to_wg2 = NULL;
		delete[] G_elev_mean; G_elev_mean = NULL;
		delete[] temperature_cell; temperature_cell = NULL;


	} else {
		short  (*temperature_cell)[12] = new short[continent_ptr->ng][12];   // [100 deg Celsius]
		sprintf(filename, "%s/GTEMP_%d.12.UNF2", options_ptr->path_climate, actual_year);
		if (readInputFile(filename, 2, 12, continent_ptr->ng, &temperature_cell[0][0], gcrc, options_ptr, continent_ptr)) exit(1);

		for (int Cell_value=0; Cell_value<continent_ptr->ng; Cell_value++) {
			for (int month=1; month<=12; month++) {
				water_temp[Cell_value][month-1]=ConstTemp[0]/(1.0+exp(ConstTemp[1]*temperature_cell[Cell_value][month-1]+ConstTemp[2])); // Wassertemperatur
			}
		}
		
		delete[] temperature_cell; temperature_cell = NULL;
	}
	return 0;
} // get_waterTempAir()

/*
int get_Q(int actual_year, double (*Q_out)[12], int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr) {

	char filename[maxcharlength];
	sprintf(filename,"%s/G_RIVER_AVAIL_%d.12.UNF0", options_ptr->path_watergap_output, actual_year);

	if (readInputFile(filename, 4, 12, continent_ptr->ng, &Q_out[0][0], gcrc, options_ptr, continent_ptr, 0.)) return 1;

	return 0;
} // end of get_Q()
*/
