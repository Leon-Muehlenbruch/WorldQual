// 05-09-2007
// Ellen Teichert
//

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
//#include <strstream> 
#include <math.h>
#include "../general_function/timestring.h"
#include "../general_function/general_functions.h"
#include "options_wqstat.h"
#include "statistic.h"

//#define DEBUG
//#define DEBUG_queries
//#undef DEBUG_queries


const int   initYears              = 0; // Initialisierungsjahre
char  MyDatabase[maxcharlength];

//======================================================
// statistik output
//======================================================
// Donau - Teil
//int arcid_begin = 28815;   // = -1 - keine Statistik, =0 - Statistik wurde ausgegeben, Ende des Flussabschnittes erreicht
//int arcid_end   = 30044;
//======================================================

mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::Query      query_ins = con.query(); // fuer INSERT into 'statistic'
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;

int    get_C(int cell, int IDrun, char  *MyDatabase, int actual_year, int month, double &con, double &con2, const options_wqstatClass* options_wqstat_ptr);
void   get_Output(vector<statisticClass>& statistic, int riverSection, int toCalc,
		int parameter_id, int parameter_id_load, int parameter_id_discharge, int parameter_id_conductivity,
		int start, int end, const options_wqstatClass* options_wqstat_ptr);
int    getRoutArea(const options_wqstatClass* options_wqstat_ptr, int Cell_value);



int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram wq_stat started at: " << getTimeString() << endl << endl;
	options_wqstatClass optionsWQstat(argc, &argv[0], "IDrun startYear endYear");
	
	if (argc<4) {
		optionsWQstat.Help();
//	    cout << "\n./wq_stat IDrun startYear endYear [-ooptions_file]\n";
//	    cout << "default options file is OPTIONS.DAT\n";
//    cout << "\n./wq_stat IDrun startYear endYear  [-const velocity] [-h hostname] [-u user] [-p password] [-v version] [-r region] [-i path] [-c climate] [-wt watertype]"
//         << "\n\nparameters apart from IDrun, startYear, endYear must not be in order. "
//         << "\nif one (or more) parameters is missing the following default values are used:  "
////         << "\n\nDATABASE      \"empty string\""
//         << "\nHOST          \"MySQL\"    "
//         << "\nUSER          \"worldqual\""
//         << "\nPASSWORD      \"*******\"  "
//         << "\nIDVersion     3 (WaterGAP3)"
//         << "\nIDReg         1 (Europa) \n\n"
//         << "\n-const bedeutet, dass es mit konstanten Fliessgeschwindigkeit gerechnet wird. Wert soll eingegeben werden [m/sec]."
//         << "\nwenn nicht -const eingegeben wurde, Fliessgeschwindigkeit wird aus der Tabelle oder UNF-Datei entnommen.\n"
//         << "\n-i            bedeuet Daten nicht aus der Tabellen, sondern aus UNF-Dateien einzulesen.\n"
//    	 << "\n-c climate input on 0.5�=> 0\n\n"
//    	 << "\nwatertype River=1 is defualt";
    return 0;
	}

	if(!IsNumber(argv[1]) || !IsNumber(argv[2]) || !IsNumber(argv[3])) {
		cerr << "ERROR: Wrong parameter" << endl;
		optionsWQstat.Help();
//		cerr << "usage: ./wq_stat IDrun startYear endYear [-ooptions_file]"<<endl;
		//[-const velocity] [-h hostname] [-u user] [-p password] [-v version] [-r region] [-i path] [-c climate]"<<endl;
		exit(1);
	}
	int IDrun     = atoi(argv[1]);
	int parameter_id_input, parameter_id, parameter_id_load, parameter_id_discharge=10;
	int parameter_id_conductivity = 120;	double multiplier_conductivity = 0.64; // f�r TDS
	int start     = atoi(argv[2]);
	int end       = atoi(argv[3]);
	int project_id, IDScen, IDScen_wq_load;
	int IDTemp; double TETA,TETA_lake;      // Parameter zum Wassertemperatur
	double *riverFlowTime = new double[(end-start+initYears+1)*12];       // Fliesszeit fuer Fluss, s
	double Q_low_limit, Q_low_limit_km3month; // Wenn Q_out in (0; Q_low_limit] Kozentration = 0. setzen; [m3/s] und in [km3/month]
	short Q_low_limit_type; // bei Q_low_limit != NULL: 0 Konzentration auf 0 setzen; 1: Konzentration C0 weiter geben
	short FlgCons; // ==1 conservative substance; ==0 non-conservative substance
	short FlgLake; // ==1 mit Abbau in den Seen/Reservoirs ; ==0 keine Abbau in den Seen/Reservoirs
	short UseWaterTemp; // == 0 keine Wassertemperatur wird benutzt; == 1 mit Wassertemperatur


	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 4 = 1 + 3 = programmname + 3 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(optionsWQstat.init(argc-4, &argv[4])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input

	int toCalc;
	mysqlpp::String runName;

	const short nDaysPerMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };	
	
	char headerline[maxcharlength]; // variable for line input of header

	//gridIO.setFileEndianType(2);  // fileEndianType = 2 (sparc system)
	if (is_c_geogen_country) {
		cout << "=========================================\n";
		cout << "gilt nur fuer salz:\n";
		cout << "c_geogen als Länderwerte in der Tabelle wq_load.country_parameter_input\n";
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

	int j;
	try {
		char sql[2000]; // variable for SQL-query

		cout << "start:" << endl;
		char start_db[maxcharlength]; //sprintf(start_db, "%sworldqual_%s", MyDatabase, continent_abb);
		sprintf(start_db, "wq_general");


		if (!con.connect(&start_db[0], optionsWQstat.MyHost, optionsWQstat.MyUser, optionsWQstat.MyPassword,3306)) {
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
//		query.reset();
//		j=sprintf(sql,    "SELECT s.`Name`, project_id, ");
//		j+=sprintf(sql+j, "IFNULL(s.`IDScen`,-1) as `IDScen`, IFNULL(s.`IDScen_wq_load`,IFNULL(s.`IDScen`,-1)) as `IDScen_wq_load`,  ");
//		j+=sprintf(sql+j, "IFNULL(s.`conservative`,0) as `conservative`, ");
//		j+=sprintf(sql+j, "s.`parameter_id_input`, s.`parameter_id`, ");
//		j+=sprintf(sql+j, "w.`name` as parameter, IFNULL(w.`typeAlgoID`, %d) as `typeAlgoID`, ", NODATA);
//		j+=sprintf(sql+j, "IFNULL(s.`IDTemp`,-1) as `IDTemp`, "); //IFNULL(s.`teta`,-1) as `teta`, ");
//		j+=sprintf(sql+j, "IFNULL(s.`parameter_id_load`,-9999) as parameter_id_load, ");
//		j+=sprintf(sql+j, "IFNULL(s.`Q_low_limit`,%d) as `Q_low_limit` ", NODATA);
//		j+=sprintf(sql+j, "FROM wq_general._runlist s, wq_general.wq_parameter w ");
//		j+=sprintf(sql+j, "WHERE s.IDrun=%d AND w.`parameter_id`=s.`parameter_id`;", IDrun);
//		query << sql;
//		cout << "Query: " << query << endl << endl; //cin >> j;
//		res = query.store();
//		if (res.empty()) {cerr << "ERROR: Wrong parameter: IDrun" << endl; exit(1);}  // falsche IDrun
//		mysqlpp::String Name=res.at(0)["Name"];
//		mysqlpp::String ParameterName=res.at(0)["parameter"];
//		parameter_id_input=res.at(0)["parameter_id_input"];
//		parameter_id=res.at(0)["parameter_id"];
//		parameter_id_load=res.at(0)["parameter_id_load"];
//		project_id=res.at(0)["project_id"];
//		IDScen=res.at(0)["IDScen"];
//		IDScen_wq_load=res.at(0)["IDScen_wq_load"];
//		cout << "IDrun = " <<IDrun<<" (" << Name <<")"<<endl;
//		IDTemp = res.at(0)["IDTemp"];
////		TETA   = res.at(0)["TETA"];
//		Q_low_limit = res.at(0)["Q_low_limit"];
//		cout << "IDTemp: "<<IDTemp<<endl; //"\tTETA: "<<TETA<<endl;
//		cout << "Q_low_limit: "<< Q_low_limit << endl;
//		if(Q_low_limit==NODATA) cout<<"Es wird ohne Q_low_limit berechnet."<<endl;
//		else {
//			if (Q_low_limit_type) cout << "bei Q_out<= " << Q_low_limit << " Konzentration wird = C0\n";
//			else cout << "bei Q_out<= " << Q_low_limit << " Konzentration wird = 0\n";
//		}
//
//		FlgCons   = res.at(0)["conservative"];
//
//		toCalc=res.at(0)["typeAlgoID"];
//		cout<<"toCalc = "<<toCalc<<endl;
//		if(toCalc==NODATA)
//			cerr<<"Error: typeAlgoID must not be null in table wq_general.wq_parameter!"<<endl;
//
//		switch (toCalc) {
//		case 0: cout << "BOD\n"; break;
//		case 1: cout << "salt\n"; break;
//		case 2: cout << "Fecal coliforms\n"; break;
//		case 3: cout << "TN\n"; break;
//		case 4: cout << "TP\n"; break;
//		default:
//			cerr << "ERROR: Wrong parameter type" << endl;
//			cerr << "usage:  0 (BOD), 1 (salt), 2 (Fecal coliforms), 3 TN, 4 TP\n"; exit(1);
//			break;
//		}
//
//		if (FlgCons==1) cout << "konservativer Stoff wird berechnet.\n";
//		else  if (FlgCons == 0){
//			cout << "nicht-konservativer Stoff wird berechnet.\n";
//		}
//		else {
//			cerr << "ERROR: Error in _runlist.conservative. Can be ==1 (conservative substance) or ==0/NULL (non-conservative substance)\n";
//			exit(1);
//		}
//
//		short UseWaterTemp;
//		if (IDTemp<0) {
//			cout << "keine Wassertemperatur wird benutzt.\n";
//			UseWaterTemp=0;
//		}
//		else {
//			UseWaterTemp=1;
//			query.reset();
//			j=sprintf(sql,  "SELECT * FROM wq_general._water_temperature_list w WHERE w.`IDTemp`=%d;", IDTemp);
//			query << sql;
//			res = query.store();
//			if (res.empty()) {cerr << "ERROR: Wrong parameter: IDTemp" << endl; exit(1);}  // falsche IDTemp
//			mysqlpp::String NameTemp=res.at(0)["Name"];
//			mysqlpp::String commentTemp=res.at(0)["comment"];
//			cout << "IDTemp = " <<IDTemp<<" (" << NameTemp <<" "<< commentTemp<<")"<<endl;
//		}

		// checking project_id and project- and database name, fill MyDatabase
		if (getProject(project_id, &optionsWQstat, con, "worldqual")) exit(1);

		// checking IDScen and scenario name
		if(checkIDScen(project_id, IDScen)) exit(1);
		if (IDScen != IDScen_wq_load) {
			cout << "IDScen_wq_load: ";
			if(checkIDScen(project_id, IDScen_wq_load)) exit(1);
		}

		mysqlpp::String ParameterName;
		// checking parameter_id and parameter name
		if (checkParameter(parameter_id, toCalc, ParameterName)) exit(1);

		int dummy; // toCalc nur fuer parameter_id
		mysqlpp::String LoadParameterName= (mysqlpp::String)" ";
		if (parameter_id_load>0) { // wenn berechnete Fracht aus gemessenen Abfluss und Konzentration eingegeben ist
			if (checkParameter(parameter_id_load, dummy, LoadParameterName, false)) exit(1);
		}

		mysqlpp::String DischargeParameterName= (mysqlpp::String)" ";
		if (parameter_id_discharge>0) { // wenn Discharge eingegeben ist
			if (checkParameter(parameter_id_discharge, dummy, DischargeParameterName, false)) exit(1);
		}

		mysqlpp::String conductivityParameterName= (mysqlpp::String)" ";  // f�r TDS
		if (toCalc==1 && parameter_id_conductivity>0) { // nur f�r Salz
			if (checkParameter(parameter_id_conductivity, dummy, conductivityParameterName, false)) exit(1);
		}

		// Pruefen, ob concentration schon berechnet wurde
		query.reset();
		j=sprintf(sql,    "SELECT count(*) as cells FROM `%swq_out_%s`.`concentration_%d` WHERE date between %d and %d;"
				, MyDatabase, optionsWQstat.continent_abb, IDrun, start*100+1, end*100+12);
		query << sql;
		res = query.store();
		int cells=res.at(0)["cells"];

		if (cells==0) {cerr << "ERROR: Kein INPUT vorhanden\n"; exit(1);}
		cout << "cells = " << cells <<endl;
		cout <<"----------------\n";

		///=================================
		// Parameter Landmaske einlesen
		if(getCELLS(&optionsWQstat, &continent)) exit(1); cout << "Parameter Landmaske wurden eingelesen\n";
		
		int *gcrc;  // gcrc[gcrc-1]=ArcID
		int *grow;  // grow[gcrc-1]=row
		//if (InputType)    // einlesen nicht nur wenn Input aus UNF-Dateien, weil immer GFREQ.UNF1 eingelesen wird
		{  // bei UNF-Dateien wird Umwandlung gcrc -> ArcID benoetigt
			gcrc = new int[continent.ng];
			grow = new int[continent.ng];
			if(getGCRC(gcrc, grow, &optionsWQstat, &continent)) exit(1);
		}
		
		double X_cell;             // Position innerhalb der Zelle
		double flowVelocity;       // [m/sec]  // u=86.4 km/day = 1 m/s  Fliessgeschwindigkeit   
		double flowVelocity_month; // [km/Monat]
		double dec_T;              // Abbaufaktor bei wassertemperatur in der Zelle [1/Monat]
		double length ; // = 55.0;            // Fliesslaenge
		double K; // = L/(86.4*31); // 
		int Time;                   // Time-Feld in Datenbank = year+100+month
		double FlowTime;            // Fliesszeit fur Zelle, s
		

		double Q_in[8][12], C_in[8], C_in_2[8]; // Durchfluss und Konzentration (2 Varianten) von Zuflusszellen
		double (*Q_out)[12]             = new double[continent.ng][12];  // river availability [km3/month]
		double (*cell_runoff_total)[12] = new double[continent.ng][12];           // cell runoff total fuer jede Zelle, Monatswerte
		double load_total, load_total_2;        // Frachtinput zum Zellenabschnitt 
		double load_up, load_up_2;              // Frachtinput von oben
		double load_acc, load_acc_2;            // Frachtinput accumuliert
		double C0, C0_2, Q_in_sum, Q_in_max, Qd, Ac_1, Ac_2, Cd_1, Cd_2;
		double Qx;                              // Durchfluss an jeden Kilometer in der Zelle (zum Ausgeben)
		double Qx_in, Qx_out;                   // temp value for calculate Qd
		double factor_month;  // Abbaufaktor(1/sec, 1/Monat), Hintergrundkonzentration
		double Konz_1, Konz_2;
		

		double *g_slope_array            = new double[continent.ng];    // zur Berechnung variablen Fliessgeschwindigkeit for all cells
		double *flowVelocity_const_array = new double[continent.ng];	// zur Berechnung variablen Fliessgeschwindigkeit for all cells
		double *riverLength = new double[continent.ng];					// Fliesslaenge for all cells



		double (*flowVelocity_var)[12] = new double[continent.ng][12];            // Fliessgeschwindigkeit aus UNF-Datei
		
		double (*water_temp)[12]= new double[continent.ng][12];                  // Wassertemperatur

		
		double krad;							//only for FC: die-off rate due to solar radiation
		double depth;							//depth of a waterbody
		double sed_veloc;						//sedimentation velocity [m/d]
		double ksd;											//sedimentation rate [1/month]
		double *ke				   = new double[continent.ng];		//Calculated attenuation coefficient [1/m]
		double *tss_cell		   = new double[continent.ng];		//TSS yield Mg/(a*km^2) -> (Megagram/(year*km^2))
		double ks;													//sunlight inactivation [cm�/cal]; only used for FC
		double *decRate			   = new double[2];					//0 - sedimentation velocity [m/d]; 1 -  sunlight inactivation [cm�/cal]; only used for FC
		short (*radiation)[12]    = new short[continent.ng][12];	//solar radiation in W/m^2
		double *decomposition    = new double[continent.ng];    // Abbaufaktor(1/sec, 1/Monat)
		double *c_geogen         = NULL; // Hintergrundkonzentration, geogen background concentration [t/km3]
		if (is_c_geogen_country && toCalc==1)
			c_geogen         = new double[continent.ng];
		double *decay_rate_lake  = new double[continent.ng];    // Abbaufaktor in den Seen [1/sec] (1/sec, 1/Monat)
		// Stoffzufluss in der Zelle (s_cell) und von oben liegenden Zellen (s_ups)
		double (*s_cell)[12] = new double[continent.ng][12];
//		double (*geogen_info)           = new double[continent.ng];      // wenn == 0 kein geogener Hintergrund beruecksichtigen

		double *river_routing = new double[continent.ng];
		int *g_outflc       = new int[continent.ng];       // Abflusszelle
		int (*inflow)[8]    = new int[continent.ng][8];    // Zuflusszellen



		for (int i=0; i<8; i++) {

		  C_in[i] = 0; C_in_2[i] = 0;
		  for (int month=1; month<=12; month++) {		  
		    Q_in[i][month-1] = 0.0;
		  }
		}

		//------------------------------------------------------
		//query.reset();
		//query << "DELETE FROM wq_general.`statistic`;";
		//query.execute();
		//------------------------------------------------------
		vector <statisticClass> statistic;
		statisticClass statistic_line;
		statistic_line.setValues();
		//======================================================
		// statistik output
		//======================================================

		ofstream outputfile_stat;

		char filename[maxcharlength];       //
		char rivername[maxcharlength];
		int arcid_begin_global, arcid_end_global;
		int arcid_begin, arcid_end;
		double stat_FlowLenght;
		
		sprintf(filename,"STAT.DAT"); {
			ifstream inputfile(filename,ios::in);
			
			if (!inputfile) { // Keine Statistik
				arcid_begin = -1;	arcid_end   = -1;
			}
			else {
				// read commentlines indicated by # 
				while (inputfile && inputfile.peek()=='#') {
					inputfile.getline(headerline,maxcharlength);
				}  

				int   riverSection=0;   // Nummerierung von Statistiken f�r verschiedene Flussabschnitte
				
				do {
					//----------------------------------
					// Flussname einlesen 
					getRiverName(rivername, inputfile);
//					{
//						char ch; int count=0;
//						inputfile.get(ch);
//						do {
//							if (ch=='\n') {cout << '*'; inputfile.get(ch); continue;}
//							if (isspace(ch)) ch=' ';
//							rivername[count++]=ch;
//							inputfile.get(ch);
//						} while (inputfile && (ch!='\t') && count<maxcharlength);
//						rivername[count]='\0';
//						if (ch!='\t') inputfile.ignore(256,'\t');
//					}
					//----------------------------------
					if (!inputfile) continue;

					inputfile >> arcid_begin_global >> arcid_end_global;
					arcid_begin = get_ArcID(&optionsWQstat, arcid_begin_global);
					arcid_end   = get_ArcID(&optionsWQstat, arcid_end_global);

					//F�r WG2 watch id f�r den Rhine
					//ATTENTION: arcid_begin and arcid_end are hard coded for WG2 for the Rhine!
					if(strcmp(optionsWQstat.continent_abb,"wg2")==0){
						cout<<"\nATTENTION: arcid_begin and arcid_end are hard coded for WG2 for the Rhine!!!\n"<<endl;
						arcid_begin=28711;  //27871;
						arcid_end=23919;	//per Hand aus GIS gesucht: basin34=23919 und aus mother watch outlet geholt
					}

					riverSection++;
					stat_FlowLenght = 0.0;
					int TimeStep;
					for (TimeStep=0; TimeStep<((end-start+initYears+1)*12); TimeStep++)
						riverFlowTime[TimeStep] = 0.0; 
					
					cout <<"rivername: " << rivername << " arcid_begin(global): "<< arcid_begin << "(" << arcid_begin_global 
						<< ") arcid_end(global): "<<arcid_end<<"(" << arcid_end_global<<")"<<endl;



					int cell=arcid_begin;
					int count=1;

					if (get_inflow(inflow, &optionsWQstat, &continent))   exit(1);
					if (get_outflc(g_outflc, &optionsWQstat, &continent)) exit(1);


					for(int i=0;i<continent.ng;i++)
						river_routing[i]=NODATA;

					river_routing[0]=arcid_begin;

					while(g_outflc[cell-1]!=0){
						river_routing[count]=g_outflc[cell-1];
						count++;
						cell=g_outflc[cell-1];
					}


					//------------------------------------------------------
					int Cell_value = arcid_begin;
					int rout_area=getRoutArea(&optionsWQstat, Cell_value);
					int Cell_value_inflow = 0;    // Vorzelle im Fluss
					length = 0.0;
					
					get_CellParam(riverLength, NULL, g_slope_array, flowVelocity_const_array, gcrc, &optionsWQstat, &continent);

					if (get_factor(IDrun, decomposition, c_geogen, decay_rate_lake, toCalc, &optionsWQstat, &continent)) exit(1);
					cout << "get_factor done\n";

					//NumberInflow = get_inflow(Cell_value, inflow, IDVersion, IDReg);
					//if (NumberInflow==-1) continue;  // Zelle nicht in berechnetem Region
						
					for (int actual_year=start-initYears; actual_year<=end; actual_year++) {
						if (getTeta(TETA, TETA_lake, IDScen, parameter_id, actual_year)) exit(1);
						if (IDTemp<0 || TETA<0) {
							cout << "keine Wassertemperatur f�r das Jahr " << actual_year << " wird benutzt.\n";
							UseWaterTemp=0;
						} else {
							UseWaterTemp=1;
							cout << "Wassertemperatur f�r das Jahr " << actual_year << " wird benutzt.\n";
						}

						if (get_Q(actual_year, Q_out, gcrc, &optionsWQstat, &continent)) exit(1);      cout << "get_Q done\n";

						if(toCalc==1){
							if (get_cell_runoff(actual_year, cell_runoff_total, gcrc, &optionsWQstat, &continent)){
								cerr<<"error in get_cell_runoff"<<endl; exit(1);
							}
							cout << "get_cell_runoff done\n";
						}
						if (get_waterTemp(actual_year, water_temp, UseWaterTemp, IDTemp, &optionsWQstat, &continent)) {
							cerr<<"error in get_waterTemp"<<endl; exit(1);
						}

						if (optionsWQstat.flowVelocityVariant==0 && optionsWQstat.InputType) { // Fliessgeschwindigkein aus UNF-Datei
							char filename[maxcharlength];
							sprintf(filename,"%s/G_RIVER_VELOCITY_%d.12.UNF0", optionsWQstat.input_dir, actual_year);
							if (readInputFile(filename, 4, 12, continent.ng, &flowVelocity_var[0][0], gcrc, &optionsWQstat, &continent)) exit(1);
						}


						if (get_s_cell(IDScen, parameter_id_input, actual_year, s_cell, &optionsWQstat, &continent)) exit(1);
						cout << "get_s_cell done\n";


						if(toCalc==2){

							//**********Initialization of decay rate parameters***********/

							if (optionsWQstat.climate==0) {

								char filename[maxcharlength];
								int *G_wg3_to_wg2 = new int[continent.ng];
								sprintf(filename,"%s/G_WG3_WG2WITH5MIN.UNF4", optionsWQstat.input_dir);
								if (readInputFile(filename, 4, 1, continent.ng, &G_wg3_to_wg2[0], gcrc, &optionsWQstat, &continent)) exit(1);

								sprintf(filename, "%s/GSHORTWAVE_%d.12.UNF2", optionsWQstat.input_dir, actual_year);
								if (readInputFileWG2toWG3( filename, 2, 12, continent.ng, radiation, gcrc, G_wg3_to_wg2)) exit(1);

								cout<<"radiation[144598][0]"<<radiation[144598][0]<<endl;

								cout << "get GSHORTWAVE_ done\n";

								delete[] G_wg3_to_wg2; G_wg3_to_wg2 = NULL;

							} else {
								sprintf(filename, "%s/GSHORTWAVE_%d.12.UNF2", optionsWQstat.input_dir, actual_year);
								if (readInputFile(filename, 2, 12, continent.ng, &radiation[0][0], gcrc, &optionsWQstat, &continent)) exit(1);

								cout << "get GSHORTWAVE_ done\n";
							}

							getDecayRates(decRate, IDScen, parameter_id, actual_year);
							sed_veloc=decRate[0];
							ks=decRate[1];
							cout<<"\nsedimenation rate: sed_veloc: "<<sed_veloc<<endl;
							cout<<"inactivation rate at sunlight: ks: "<<ks<<endl;
							//************Calculation of the attenuation coefficient***********//

							if (getTSSBasinValues(tss_cell, optionsWQstat.IDReg, &continent)) exit(1);
							//calculates the attenuation coefficient
							if (calculateAttenuation(ke, tss_cell,  IDScen,  parameter_id,  actual_year, Q_out , gcrc, grow, &optionsWQstat, &continent)) exit(1);

						}

						int counter=0;

						while (river_routing[counter]!=NODATA) {

							if(counter==0)
								stat_FlowLenght=0.0;
							else
								stat_FlowLenght += length;
							X_cell = length;  // default - Position am Ende der Zelle
							


							Cell_value=river_routing[counter];
							length=riverLength[Cell_value-1];


							for (int month=1; month<=12; month++) {
								if(Q_low_limit==NODATA) Q_low_limit_km3month=NODATA;
								else Q_low_limit_km3month = Q_low_limit * 3600.*24.*nDaysPerMonth[month-1] / 10e9;

								for (int i=0; i<8; i++) {
									if (inflow[Cell_value-1][i]==0) {
									Q_in[i][month-1] = 0.0;
									}
									else {
									/*inflow ist die Zellennummer, aus der der Inflow kommt; im vorherigen Schritt wurde der
									Inflow fuer die aktuelle Zelle bereits berechnet. Die erste Zelle hat keinen Inflow,
									sondern nur das was in der Zelle selbst produziert wird.*/
									Q_in[i][month-1] = Q_out[inflow[Cell_value-1][i]-1][month-1];
									}
								}

								Time = actual_year*100+month;
								TimeStep=(actual_year-(start-initYears))*12+month-1;
                
								/*
								if (flowVelocityVariant)             // variante 2: variable Fliessgeschwindigkeit
									flowVelocity = get_flowVelocity(Q_out[month-1]*(1000000000./(3600.*24.*nDaysPerMonth[month-1])), // [m3/sec]
										g_slope, Cell_value);          // [m/sec]
								*/

								//=======================================================
								// Fliessgeschwindigkeit bestimmen
								//=======================================================
								switch (optionsWQstat.flowVelocityVariant) {
									case 0:                                 // Tabelle oder UNF
										if (!optionsWQstat.InputType) flowVelocity = flowVelocity_const_array[Cell_value-1];
										else            flowVelocity = flowVelocity_var[Cell_value-1][month-1];
										break;
									case 1:                                 // rechnen
										flowVelocity = get_flowVelocity(Q_out[Cell_value-1][month-1]*(1000000000./(3600.*24.*nDaysPerMonth[month-1])), // [m3/sec]
												g_slope_array[Cell_value-1]);                    // [m/sec]

										break;
									case 2:                                 // const fur jede Zelle
										flowVelocity = optionsWQstat.ConstVelocity;    // [m/s]
										break;
								};
								
								FlowTime = (flowVelocity==0.0 ? 0.0 :length*1000.0/flowVelocity);
								
								flowVelocity_month=flowVelocity*3600*24*nDaysPerMonth[month-1]/1000;   // m/sec -> km/Monat
								factor_month = decomposition[Cell_value-1]* (3600*24*nDaysPerMonth[month-1]);               // 1/sec -> 1/Monat
								K = length/(flowVelocity_month);  //  [Monat]


								//if (Cell_value==122503 || Cell_value==122993)
								// cout << Cell_value<< " : Q_out,flowVelocity, factor_month: "
								//  << Q_out[month-1] << '\t'<< flowVelocity<<'\t'<<factor_month<<endl;

								for (int i=0; i<8; i++) {
									if (inflow[Cell_value-1][i]==0) C_in[i] = 0;
									else if(get_C(inflow[Cell_value-1][i], IDrun, MyDatabase, actual_year, month, C_in[i], C_in_2[i], &optionsWQstat)) {
										cerr << "irgendwas falsch mit get_C("<<inflow[i]<<", "<<IDrun<<", "<<actual_year<<", "<<month<<").\n"; exit(1);
										}
								}

								C0 = 0.0; C0_2 = 0.0; Q_in_sum = 0.0; Q_in_max = 0.0; 
								load_total=0.0;  load_total_2=0.0;
								load_up=0.0;     load_up_2=0.0;
								
								for (int i=0; i<8; i++) {
								  C0       += Q_in[i][month-1]*C_in[i];
								  C0_2     += Q_in[i][month-1]*C_in_2[i];
								  Q_in_sum += Q_in[i][month-1];
								  if (Q_in_max<Q_in[i][month-1]) Q_in_max = Q_in[i][month-1];
								  
								  if (Cell_value==129067) cout << "++++\t"<<Cell_value<<'\t'<<Cell_value_inflow<<'\t'<<inflow[i] <<'\t'
								  <<month<<'\t'<<Q_in[i][month-1] <<'\t'<< C_in[i]<<endl;
								  
								  if (Cell_value_inflow>0) {  // Fracht am Anfang zer Zelle, bestehend aus seitlichen Zuflussen
								  	if ( inflow[Cell_value-1][i]!=Cell_value_inflow) {
								  		load_total   += Q_in[i][month-1]*C_in[i];
								  		load_total_2 += Q_in[i][month-1]*C_in_2[i];
								  	}
								  	else {
								  		load_up   += Q_in[i][month-1]*C_in[i];
								  		load_up_2 += Q_in[i][month-1]*C_in_2[i];
								  	}
								  }
								}

								load_acc   = load_total + load_up;
								load_acc_2 = load_total_2 + load_up_2;
								
								if (Q_in_sum==0) Qx_in  = 10e-12; else Qx_in  = Q_in_sum;
								if (Q_out[Cell_value-1][month-1]==0)    Qx_out = 10e-12; else Qx_out = Q_out[Cell_value-1][month-1];
								Qd = log(Qx_out/Qx_in)/length;
								
								//double cell_runoff_total = get_cell_runoff(Cell_value, actual_year, month);
								double s_geogen;
								//if (cell_runoff_total[month-1]<0 || geogen_info==0) s_geogen = 0.0;
								//else s_geogen= c_geogen * cell_runoff_total[month-1];
								

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

								//L�schen, wenn es l�uft
								/*//s_geogen= c_geogen * cell_runoff_total[month-1];
								if (cell_runoff_total[month-1]<0) s_geogen = 0.0;
								else s_geogen= c_geogen * cell_runoff_total[month-1];*/

								double S_input  = s_cell[Cell_value-1][month-1]+s_geogen;
								
								if (Cell_value==124296)
									cout << Cell_value //<< " geogen_info=" <<geogen_info
									  <<" s_geogen=" <<s_geogen
									  <<" S_input=" << S_input
									  <<endl;

								//==================================================================
								if (FlgCons) { // conservative substance

									//ATTENTION: This calculation is similar to conservative in worldqual.cpp
									//changes here and there have to be consistent
//									C0     = (Q_out[Cell_value-1][month-1]==0? 0.0 :C0/ Q_out[Cell_value-1][month-1]);
//									C0_2   = (Q_out[Cell_value-1][month-1]==0? 0.0 :C0_2/ Q_out[Cell_value-1][month-1]);
									C0     = (Q_in_sum==0? 0.0 :C0/ Q_in_sum);
									C0_2   = (Q_in_sum==0? 0.0 :C0_2/ Q_in_sum);

									Cd_2 = 0.0;  //only for non conservative substance



									//04-02-2009
									// C0 = sum(Q_in*Cin)/Q_out
									// Cd = S_input/Q_out
									if ( Q_out[Cell_value-1][month-1]<= 0.0) Cd_1 = 0;
									else Cd_1 = S_input/Q_out[Cell_value-1][month-1];

									// Die Schleife soll f�r X_cell ab km 0 f�r jeden Kilometer und am Ende (falls length keine Ganzzahl ist)
									// noch f�r X_cell == length durchlaufen werden. Wenn X_cell >= length zum ersten Mal eintritt:  (!isLength && X_cell>=length)
									// ist der boolean noch false. Anschlie�end soll X_cell , falls es groesser oder gleich length geworden ist, mit length ersetzt werden (isLength = true, length).
									// Hier wird der boolen auf true gesetzt und X_cell=length gesetzt. Das ganze soll nur genau einmal passieren, daf�r gibt es die boolische Variable isLength.
									// Dieser Ausdruck ist etwas verklausuliert geschrieben, damit man nicht den Code dublizieren muss (bei �nderungen w�rde schnell was vergessen werden).
									// Wichtigste ungew�hnliche Schreibweisen (http://www.cplusplus.com/doc/tutorial/operators/):
									// - conditional ternary operator (?):      conditional ? result1 : result2
									// c = (a>b) ? a : b;  // hier wird wenn a>b c=a gemacht, sonst c=b
									// - Comma operator (, ) The comma operator (,) is used to separate two or more expressions that are included where only one expression is expected.
									// 		When the set of expressions has to be evaluated for a value, only the right-most expression is considered.
									// 		a = (b=3, b+2); // hier wird b=3 und a=5
									// - For loop:   for (initialization; condition; increase) statement;
									bool isLength = false; // X_cell hat den Wert length noch nicht erreicht
									for (X_cell=0.0; !isLength; X_cell++)
									{
										if (X_cell>length) {
											X_cell = length;
											isLength = true;
										}
										if (Q_low_limit_km3month!=NODATA && Q_out[Cell_value-1][month-1]<= Q_low_limit_km3month) {
											if (!Q_low_limit_type) { // Konzentration auf 0 setzen
											  Konz_1 = 0.0;	Konz_2 = 0.0;
											  load_total   = 0.;
											  load_total_2 = 0.;
											  load_acc = 0.;
											  load_acc_2 = 0.;
											  load_up = 0.;
											  load_up_2 = 0.;
											} else { // 1: Konzentration C0 weiter geben
												if (Q_out[Cell_value-1][month-1]== 0.0) {
													Konz_1 = 0.0;	Konz_2 = 0.0;
												} else {
												  Konz_1 = C0;	Konz_2 = C0_2;
												}
											}
										} else {
											if (Q_out[Cell_value-1][month-1]== 0.0) {
												Konz_1 = 0.0;	Konz_2 = 0.0;
											} else {
												//if ( (1.0 - exp(-Qd * X_cell))<0 ) Cd_1=0.0;
												double exponent=exp(-Qd * X_cell);
												if (exponent>1.0) exponent = 1.0;
												Konz_1 = C0   * exponent + Cd_1 * (1.0 - exponent);
												Konz_2 = 0.;
											}
											if (X_cell>0) { // Fracht in der Zelle
												load_total   = S_input *X_cell/length;
												load_total_2 = S_input *X_cell/length;
												//load_up      = 0;
												//load_up_2    = 0;
											}
										} // if (Q_out > Q_low_limit_km3month)

										Qx = Qx_in*exp(Qd*X_cell);  // Durchfluss an diesem Kilometer

										statistic_line.riverSection = riverSection;
										statistic_line.cell = Cell_value;
										statistic_line.date = actual_year*100+month;
										statistic_line.lengthCell = X_cell;
										statistic_line.lengthSection = stat_FlowLenght+X_cell;
										statistic_line.rout_area = rout_area;
										statistic_line.con = Konz_1;
										statistic_line.con2 = Konz_2;
										statistic_line.flowVelocity = flowVelocity;
										statistic_line.flowTime = riverFlowTime[TimeStep]+FlowTime;
										statistic_line.load_total = load_total;
										statistic_line.load_total_2 = load_total_2;
										statistic_line.load_up = load_up;
										statistic_line.load_up_2 = load_up_2;

										// accumulierte Fracht
										if (X_cell==0) {
											statistic_line.load_acc = load_acc;
											statistic_line.load_acc_2 =load_acc_2;
										} else{
											statistic_line.load_acc = load_acc+load_total;
											statistic_line.load_acc_2 =load_acc_2+load_total_2;
										}
										statistic_line.Q_out = Q_out[Cell_value-1][month-1];
										statistic_line.Q_d = Qd;
										statistic_line.Q_in = Q_in_sum;
										statistic_line.Qx = Qx;

										statistic.push_back(statistic_line);

										if (riverFlowTime[TimeStep]!=riverFlowTime[TimeStep]) cerr << "ERROR2= FlowTime="<<FlowTime<<" length="<<length<<" flowVelocity="<<flowVelocity<<" riverFlowTime[TimeStep]="<<riverFlowTime[TimeStep]<<" TimeStep="<<TimeStep<<endl;
									} // for X_cell

									riverFlowTime[TimeStep]+=FlowTime;

								}  // if(FlgCons)
								else { // non-conservative substance
								//ATTENTION: This calculation is similar to non-conservative in worldqual.cpp
								//changes here and there have to be consistent
								//==================================================================
									C0     = (Q_out[Cell_value-1][month-1]==0? 0.0 :C0/ Q_out[Cell_value-1][month-1]);
									C0_2   = (Q_out[Cell_value-1][month-1]==0? 0.0 :C0_2/ Q_out[Cell_value-1][month-1]);


									if(toCalc==2){
										//Ansatz von Thomann und M�ller
										//depth is in m because the formula from Verzano 2012 enters Q_out in m^3/s and returns m
										depth=0.349*pow(Q_out[Cell_value-1][month-1]*(1000000000./(3600.*24.*nDaysPerMonth[month-1])),0.341); //depth in m
										if(sed_veloc==0.0) ksd=0.0;
											else
											ksd=(sed_veloc*nDaysPerMonth[month-1])/depth;

										//Q_out[month-1]*((3600.*24.*nDaysPerMonth[month-1])/1000000000.0);

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
											//cout<<"((ks*radiation[Cell_value-1][month-1]*((860*24*nDaysPerMonth[month-1])/10000))/(depth*ke[Cell_value-1]))*(1-exp(-ke[Cell_value-1]*depth))="<<((ks*radiation[Cell_value-1][month-1]*((860*24*nDaysPerMonth[month-1])/10000))/(depth*ke[Cell_value-1]))*(1-exp(-ke[Cell_value-1]*depth))
												//			<<"\nks="<<ks<<" radiation[Cell_value-1][month-1]="<<radiation[Cell_value-1][month-1]<<" nDaysPerMonth[month-1]="<<nDaysPerMonth[month-1]<<"\ndepth="<<depth<<" ke[Cell_value-1])="<<ke[Cell_value-1]
													//		<<" Cell_value="<<Cell_value<< " month="<<month<<endl;

										}



									}else{
										dec_T = factor_month; //Abbaurate je nach Substanz
										krad=0.0;
										ksd=0.0;
									}

									if (UseWaterTemp) {

										//dec_T = factor_month * pow(TETA,water_temp[month-1]-20.0);
										dec_T =   (factor_month * pow(TETA,water_temp[Cell_value-1][month-1]-20.0)) + krad +ksd;

										//cout<<"dec_T:"<<dec_T<<" factor_month:"<<factor_month<<" TETA:"<<TETA<<" water_temp[Cell_value-1][month-1]-20.0:"<<water_temp[Cell_value-1][month-1]-20.0<<" krad:"<<krad<<" ksd:"<<ksd<<endl;
										//     Guadiana 			- Mosel/Rhine 			- Po 			- Vistula 			- Meuse 				- Tweed
										if(Cell_value==156606 || Cell_value==79743|| Cell_value==124333 || Cell_value==91924 || Cell_value==90671 || Cell_value==59540)
											cout<<"radiation[month][cell]: "<<radiation[Cell_value-1][month-1]
												<<" cell-1: "<<Cell_value-1<<" month-1: "<<month-1<< " year: "<<actual_year
												<<"\nkrad: "<<krad<<" dec_t: "<<dec_T<<" factor_month: "<<factor_month
												<<" ks: "<<ks<<" ke[Cell_value-1]="<<ke[Cell_value-1]
												<<" Q_out[month-1]: "<<Q_out[Cell_value-1][month-1]
												<<" flowVelocity_month: "<<flowVelocity_month
												<<"\nksd: "<<ksd<<" sed_veloc: "<<sed_veloc<<" depth: "<<depth<<" nDaysPerMonth[month-1]: "<<nDaysPerMonth[month-1]<<endl;

									}

									if (Q_out[Cell_value-1][month-1]==0.0) {if (Q_in_sum == 0.0) Ac_1 = 10e-7; else Ac_1=Q_in_sum/flowVelocity_month;}
									else               Ac_1   = Q_out[Cell_value-1][month-1]/flowVelocity_month;

									Ac_2   = (Q_in_sum+Q_out[Cell_value-1][month-1])/(2*flowVelocity_month);
									if ((Q_out[Cell_value-1][month-1]==0.0) && (Q_in_sum==0.0)) Ac_2=10e-7;
									else {
										if   (Q_in_sum == 0.0)  Ac_2 = Q_out[Cell_value-1][month-1]/flowVelocity_month;
										else if (Q_out[Cell_value-1][month-1] == 0.0)   Ac_2 = Q_in_sum/flowVelocity_month;
									}

									Cd_1 = (Ac_1==0? 0.0 : (s_cell[Cell_value-1][month-1]+s_geogen)/length/Ac_1/dec_T);
									Cd_2 = (Ac_2==0? 0.0 : (s_cell[Cell_value-1][month-1]+s_geogen)/length/Ac_2/dec_T);

									// Schleife soll f�r X_cell ab 0 f�r jeden Kilometer und am Ende (Falls length keine Ganzzahl ist)
									// noch f�r X_cell == length durchlaufen
									// daf�r ersetze ich ein Mal X_cell, falls er gr��er oder gleich length geworden ist mit length
									// damit es nur ein Mal passiert, nutze boolische Variable isLength
									// bei den conservative substance gibt es ausfuehrlichere Erkl�rung f�r die for-Schleife
									bool isLength = false;
									for (X_cell=0.0; X_cell<length; X_cell++, X_cell = (!isLength && X_cell>=length) ?  (isLength = true, length) : X_cell) {
										if (Q_low_limit_km3month!=NODATA && Q_out[Cell_value-1][month-1]<= Q_low_limit_km3month) {
											if (!Q_low_limit_type) { // Konzentration auf 0 setzen
												Konz_1 = 0.0;	Konz_2 = 0.0;
												load_total   = 0.;
												load_total_2 = 0.;
												load_acc = 0.;
												load_acc_2 = 0.;
												load_up = 0.;
												load_up_2 = 0.;
											} else { // 1: Konzentration C0 weiter geben
												Konz_1 = C0;	Konz_2 = C0_2;
											}
										} else {
											Konz_1 = C0   * exp(-dec_T * X_cell/flowVelocity_month) + Cd_1 * (1.0 - exp(-dec_T * X_cell/flowVelocity_month));
											Konz_2 = C0_2 * exp(-dec_T * X_cell/flowVelocity_month) + Cd_2 * (1.0 - exp(-dec_T * X_cell/flowVelocity_month));

											if (X_cell>0) {
												load_total   = S_input *X_cell/length;
												load_total_2 = S_input *X_cell/length;
												//load_up      = 0;
												//load_up_2    = 0;
											}
										}

										//cout<<"Konz_1:"<<Konz_1<<" C0:"<<C0<<" dec_T:"<<dec_T<<" X_cell:"<<X_cell<<"stat_FlowLenght:"<<stat_FlowLenght<<" flowVelocity_month:"<<flowVelocity_month
										//												<<" Cd_1:"<<Cd_1<<endl;

										Qx = Qx_in*exp(Qd*X_cell);  // Durchfluss an diesem Kilometer

										double x=riverFlowTime[TimeStep]+FlowTime;

										statistic_line.riverSection = riverSection;
										statistic_line.cell = Cell_value;
										statistic_line.date = actual_year*100+month;
										statistic_line.lengthCell = X_cell;
										statistic_line.lengthSection = stat_FlowLenght+X_cell;
										statistic_line.rout_area = rout_area;
										statistic_line.con = Konz_1;
										statistic_line.con2 = Konz_2;
										statistic_line.flowVelocity = flowVelocity;
										statistic_line.flowTime = x;
										statistic_line.load_total = load_total;
										statistic_line.load_total_2 = load_total_2;
										statistic_line.load_up = load_up;
										statistic_line.load_up_2 = load_up_2;

										// accumulierte Fracht
										if (X_cell==0) {
											statistic_line.load_acc = load_acc;
											statistic_line.load_acc_2 =load_acc_2;
										} else{
											statistic_line.load_acc = load_acc+load_total;
											statistic_line.load_acc_2 =load_acc_2+load_total_2;
										}
										statistic_line.Q_out = Q_out[Cell_value-1][month-1];
										statistic_line.Q_d = Qd;
										statistic_line.Q_in = Q_in_sum;
										statistic_line.Qx = Qx;

										statistic.push_back(statistic_line);

										if (riverFlowTime[TimeStep]!=riverFlowTime[TimeStep]) cerr << "ERROR3: FlowTime="<<FlowTime<<" length="<<length<<" flowVelocity="<<flowVelocity<<" riverFlowTime[TimeStep]="<<riverFlowTime[TimeStep]<<" TimeStep="<<TimeStep<<endl;
									} // for (X_cell)

									riverFlowTime[TimeStep]+=FlowTime;

								//==================================================================
								}// non-conservative substance

							
								if (Cell_value==26136||Cell_value==25709||Cell_value==26574
										||Cell_value==26137||Cell_value==105959 ||Cell_value==80888||Cell_value==156606||Cell_value==157028||Cell_value==158375||Cell_value==79743 ||Cell_value==150925 || Cell_value==59540)
								{
									cout << "Cell_value   =\t" <<Cell_value <<endl;
									cout <<"month         =\t"<<month<<endl;
									//cout << "NumberInflow =\t" <<NumberInflow << endl;
									//cout << "Time         =\t" <<Time << endl;
									cout << "factor_month =\t" <<factor_month <<endl;
									cout << "Q_out        =\t" <<Q_out[Cell_value-1][month-1] <<endl;
									cout << "Q_in_sum     =\t" <<Q_in_sum <<endl;
									cout << "s_cell       =\t" <<s_cell[Cell_value-1][month-1] <<endl;
									cout << "s_geogen     =\t" <<s_geogen <<endl;
									cout << "C0 =\t" <<C0   << "\t" << C0_2 << endl;
									cout << "Cd =\t" <<Cd_1 << "\t" << Cd_2 << endl;
									cout << "Qd =\t" <<Qd   << "\nlength =\t" << length << "\nflowVelocity = "  << flowVelocity << endl;
									cout << "Ac =\t" <<Ac_1 << "\t" << Ac_2 << endl;
									cout << "Konz_1 =\t" <<Konz_1   << "\t" << Konz_2 << endl;
									cout << "cell_runoff_total[month-1]=\t"<<cell_runoff_total[Cell_value-1][month-1]<<endl;


									// bei nicht konservativen Substanzen, wenn Wassertemperatrur benutzt wurde
									if (factor_month!=0.0 && UseWaterTemp) {
										cout << "water_temp =\t" <<water_temp[Cell_value-1][month-1] << endl;
										cout << "dec_T      =\t" <<dec_T << endl;
									}
									cout << "================================================\n";
								}
							
							} // for (month)
							
						counter++;
						Cell_value_inflow=Cell_value;
						rout_area=getRoutArea(&optionsWQstat, Cell_value);

						} // while(river_routing[] != NODATA)
						if (Cell_value!=arcid_end) {
							cerr<<"Error in while river routing -> arcidend != g_outflc=0; Cell_value="<<Cell_value<<" arcid_end="<<arcid_end<<endl;
							for(int i=0; i< continent.ng;i++)
								cerr<<"river_routing[i]="<<river_routing[i]<<" i= "<<i<<endl;
						}
						cout<<"Cell_value="<<Cell_value<<endl;

						//==============================================
						cout<<"TEST: FlowTime="<<FlowTime<<" flowVelocity="<<flowVelocity<<" length="<<length<<endl;

						if (riverFlowTime[TimeStep]!=riverFlowTime[TimeStep]) cerr << "ERROR4: FlowTime="<<FlowTime<<" length="<<length<<" flowVelocity="<<flowVelocity<<" riverFlowTime[TimeStep]="<<riverFlowTime[TimeStep]<<" TimeStep="<<TimeStep<<endl;

						//#############################
						cout<<"after execute"<<endl;

						//==============================================
					} //end for(actual_year)

					cout << "############# make statistic done, size= " << statistic.size() << endl << "########################" << endl;

					//=========================================
					// OUTPUT in die Datei
					//=========================================
					//ofstream outputfile_stat;
                
					sprintf(filename,"statistic_param_%d_%s_%d_%d.txt", IDrun, rivername ,start, end);
					cout << "output file: " << filename << endl;
					outputfile_stat.open(filename);
					if (!outputfile_stat) {
						cerr << "\nCould not open " << filename << " for output" << endl;
						exit (0);
					}
					outputfile_stat << getTimeString() << endl;
					outputfile_stat << "IDVersion:\t" << optionsWQstat.IDVersion << "\tIDReg:\t" << optionsWQstat.IDReg <<endl;
					outputfile_stat << "IDrun:\t"<<IDrun << "\t ( \t" << runName << "\t)"<< endl;
					outputfile_stat << "Parameter          :\t" << parameter_id_input   << "\toriginal: " << parameter_id << "\t ( \t" << ParameterName << "\t )" << endl;
					outputfile_stat << "berechnete Fracht  :\t" << parameter_id_load   << "\t ( \t" << LoadParameterName << "\t)" << endl;
					outputfile_stat << ".......... Fracht  :\t" << parameter_id_conductivity   << "\t ( \t" << conductivityParameterName << "\t)" << endl;
					outputfile_stat << "years:\t" << start << "\t - \t" << end << endl;
					outputfile_stat << rivername << "\tarcid_begin: \t" << arcid_begin << "\t(\t"<<arcid_begin_global<<"\t)\t" 
					  <<"arcid_end: \t" << arcid_end<< "\t(\t"<<arcid_end_global<<"\t)\t" <<endl;
					outputfile_stat << "riverSection\tArcId\tdate\tkm in cell\tkm from begin\tKonz_1, mg/l \tKonz_2, mg/l "
					  << "\tmeasured \tflag_uncert \tmeasured_conductivity, mg/l \tflag_uncert_conductivity \tflowVelocity, m/s \ttimeOfFlow, day \triver availability, m3/s "
					  << "\tmontly grid cell loading plus tributaries (t/month)"
					  << "\tmontly grid cell loading plus tributaries(2) (t/month)"
					  << "\tmontly upstream loads (t/month)"
					  << "\tmontly upstream loads(2) (t/month)"
					  << "\tmonthly grid cell loading accumulated (t/month)"
					  << "\tmonthly grid cell loading accumulated(2) (t/month)"
					  << "\tQ_d\tQin\tQx\tGRDC\tmeasured flow, m3/s "
					  << "\tmeasured monthly grid cell loading accumulated (t/month) \tflag_uncert loading "
					  << "\tDischarge - single value (m�/s)\t flag_uncert discharge"
					  << "\trout area"<< endl;
					
					get_Output(statistic, riverSection, toCalc
							, parameter_id, parameter_id_load, parameter_id_discharge, parameter_id_conductivity
							, start, end, &optionsWQstat);
					
					for (unsigned int num=0; num<statistic.size(); num++)
					{
						outputfile_stat << statistic[num].riverSection << '\t'
						  << statistic[num].cell << '\t'
						  << statistic[num].date << '\t'
						  << statistic[num].lengthCell << '\t'
						  << statistic[num].lengthSection << '\t'
						  << statistic[num].con/1000. << '\t'   // mg_l
						  << statistic[num].con2/1000. << '\t'; // mg_l
						
						if (statistic[num].measured!=-9999)
							outputfile_stat << statistic[num].measured;
						outputfile_stat << '\t';
 						
						if (statistic[num].flag_uncert!=-9999)
							outputfile_stat << statistic[num].flag_uncert;
						outputfile_stat << '\t';

						if (statistic[num].measured_conductivity!=-9999)
							outputfile_stat << statistic[num].measured_conductivity * multiplier_conductivity;
						outputfile_stat << '\t';

						if (statistic[num].flag_uncert_conductivity!=-9999)
							outputfile_stat << statistic[num].flag_uncert_conductivity;
						outputfile_stat << '\t';

//						if (statistic[num].station_id!=-9999)
// 							outputfile_stat << statistic[num].station_id;
// 						outputfile_stat << '\t';

						if (statistic[num].flowVelocity!=-9999)
 							outputfile_stat << statistic[num].flowVelocity;
 						outputfile_stat << '\t';

 						if (statistic[num].flowTime!=-9999)
 							outputfile_stat << statistic[num].flowTime/3600./24.;
 						//outputfile_stat << '\t' << QValue;
 						//outputfile_stat << '\t' << loadValue;
 						int month = statistic[num].date % 100;
 						outputfile_stat << '\t' << statistic[num].Q_out/3600./24./nDaysPerMonth[month-1]*1000000000.;
 						outputfile_stat << '\t' << statistic[num].load_total;
 						outputfile_stat << '\t' << statistic[num].load_total_2;
 						outputfile_stat << '\t' << statistic[num].load_up;
 						outputfile_stat << '\t' << statistic[num].load_up_2;
 						outputfile_stat << '\t' << statistic[num].load_acc;
 						outputfile_stat << '\t' << statistic[num].load_acc_2;
 						outputfile_stat << '\t' << statistic[num].Q_d;
 						outputfile_stat << '\t' << statistic[num].Q_in;
 						outputfile_stat << '\t' << statistic[num].Qx << '\t' ;
						if (statistic[num].GRDC!=-9999)
							outputfile_stat << statistic[num].GRDC;
 						outputfile_stat << '\t';
						if (statistic[num].calculated!=-9999)
							outputfile_stat << statistic[num].calculated;
 						outputfile_stat << '\t';
						if (statistic[num].measured_load!=-9999)
							outputfile_stat << statistic[num].measured_load;
 						outputfile_stat << '\t';
						if (statistic[num].flag_uncert_load!=-9999)
							outputfile_stat << statistic[num].flag_uncert_load;
 						outputfile_stat << '\t';
						if (statistic[num].measured_discharge!=-9999)
							outputfile_stat << statistic[num].measured_discharge;
 						outputfile_stat << '\t';
						if (statistic[num].flag_uncert_discharge!=-9999)
							outputfile_stat << statistic[num].flag_uncert_discharge;
 						outputfile_stat << '\t';
						if (statistic[num].rout_area!=-9999)
							outputfile_stat << statistic[num].rout_area;
 						//outputfile_stat << '\t';
						//outputfile_stat << statistic[num].date%100<<'-'
						//	<<statistic[num].date/100;
 						outputfile_stat << '\n';

					}
						
					//======================================================
					
					outputfile_stat.close();
					statistic.clear();
                
					//=========================================

				} while (inputfile);

			} // else



		} // end of block "STAT.DAT"
		//------------------------------------------------------
		delete[] riverFlowTime; riverFlowTime=NULL;


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
	cout <<"\nProgram wq_stat endet at: " << getTimeString() << endl << endl;
	return 0;
} // end of main()







int get_C(int cell, int IDrun, char *MyDatabase, int actual_year, int month, double &con, double &con2, const options_wqstatClass* options_wqstat_ptr) {
	int j;
	char sql[2000]; // variable for SQL-query

	query.reset();
	j=sprintf(sql,    "SELECT c.con, c.con2 FROM `%swq_out_%s`.concentration_%d c  ", MyDatabase, options_wqstat_ptr->continent_abb, IDrun);
	j+=sprintf(sql+j, "WHERE c.`date`=%d and c.`cell`=%d; ", actual_year*100 + month, cell);
	query << sql;
	#ifdef DEBUG_queries
	  cout << "Query get_C: " << query << endl << endl; //cin >> j;
	#endif
	res = query.store();
	if (res.empty()) {cout << "get_C: cell " << cell << " is not in calculated region\n"; return 1;}  // Zelle nicht in berechnetem Region 

	con  = res.at(0)["con"];
	con2 = res.at(0)["con2"];
	return 0;
} // end of get_C()




void get_Output(vector<statisticClass>& statistic, int riverSection, int toCalc,
	int parameter_id, int parameter_id_load, int parameter_id_discharge, int parameter_id_conductivity,
	int start, int end, const options_wqstatClass* options_wqstat_ptr) {
	int j;
	char sql[2000]; // variable for SQL-query
	
//	query.reset();
//	query << "DELETE FROM wq_general.`statistic_grdc`;";
//	query.execute();

//	j=sprintf(sql, "insert into wq_general.statistic_grdc (`GRDC`, `date`, `calculated`, `cell`) ");
	j=sprintf(sql, " SELECT f.`GRDC`, f.`year`*100+f.`month` as `date`, ");
	j+=sprintf(sql+j, " f.`calculated`, ");
	j+=sprintf(sql+j, " s.`ArcID` as `cell` ");
	j+=sprintf(sql+j, " FROM grdc.flow f, grdc.station_02_2007 s ");
	j+=sprintf(sql+j, " WHERE s.GRDC=f.GRDC  AND f.year between  %d AND %d", start, end);
	j+=sprintf(sql+j, " AND s.`IDVersion`=%d AND s.`IDReg`=%d AND ", options_wqstat_ptr->IDVersion, options_wqstat_ptr->IDReg);
	j+=sprintf(sql+j, " f.`data_used` is null AND f.`calculated`>0 AND s.`ArcID` is not null;");
	query.reset();
	query << sql;
	mysqlpp::StoreQueryResult res_statistic_grdc = query.store();
//	query.execute();

//	query.reset();
//	query << "DELETE FROM wq_general.`statistic_instream`;";
//	query.execute();


	if(strcmp(options_wqstat_ptr->continent_abb,"wg2")!=0){


	//first the daily average is built and afterwords the monthly average
//	j=sprintf(sql, "INSERT INTO wq_general.statistic_instream (`parameter_id`, `station_id`, `cell`, `date`, `flag_uncert`, `measured`) ");
	j=sprintf(sql, "SELECT a.`parameter_id`, -9999, a.`cell`, a.`month` AS `date`, a.`flag_uncert`, avg(a.measured_day_avg) AS `measured` \n");
	j+=sprintf(sql+j, "FROM (SELECT w.`parameter_id`, w.`date`, ws.`global_id`, g.`cell`, max(ifnull(ws.`flag_uncert`,0)) as 'flag_uncert' \n");
	j+=sprintf(sql+j, ", avg(w.Value) as 'measured_day_avg', DATE_FORMAT(w.`date`,'%%Y%%m') as `month` ");
	j+=sprintf(sql+j, "\nFROM wq_general.valid_data_%s w, wq_general.wq_station ws, watergap_unf.gcrc g ", options_wqstat_ptr->continent_abb);
	j+=sprintf(sql+j, "\nWHERE w.`parameter_id` in (%d, %d, %d, %d) ", parameter_id, parameter_id_load, parameter_id_discharge, parameter_id_conductivity);
//	j+=sprintf(sql+j, "\n AND w.`data_used` is null ");
	j+=sprintf(sql+j, "\n AND (w.`data_used` is null OR w.`data_used`=-5) ");
	j+=sprintf(sql+j, "\n AND ws.`data_used` is null AND w.`watertype`=%d ", options_wqstat_ptr->watertype);
	j+=sprintf(sql+j, "\nAND ws.station_id=w.station_id AND g.`IDVersion`=%d AND g.`IDReg`=%d ", options_wqstat_ptr->IDVersion, options_wqstat_ptr->IDReg);
	j+=sprintf(sql+j, " AND g.`GLOBALID`=ws.`global_id` \nAND year(w.`date`) BETWEEN %d AND %d GROUP BY w.`parameter_id`, w.`date`, g.`cell`) AS a ", start, end);
	j+=sprintf(sql+j, "GROUP BY a.`parameter_id`, a.`month`, a.`cell`;");
	query.reset();
	query << sql;


	}else{//Attention - the WG2 part is hard coded for Europe for validation!!!

		cout<<"\nAttention - the WG2 part is hard coded for Europe for validation!!!\n"<<endl;




//		j=sprintf(sql, "INSERT INTO wq_general.statistic_instream (`parameter_id`, `station_id`, `cell`, `date`, `flag_uncert`, `measured`) ");
		j=sprintf(sql, "SELECT a.`parameter_id`, -9999, a.`cell`, a.`month` AS `date`, a.`flag_uncert`, avg(a.measured_day_avg) AS `measured` ");
		j+=sprintf(sql+j, "FROM (SELECT w.`parameter_id`, w.`date`, ws.`global_id`, a.`arcid_watch` as 'cell', max(ifnull(ws.`flag_uncert`,0)) as 'flag_uncert', avg(w.Value) as 'measured_day_avg', DATE_FORMAT(d.`date`,'%%Y%%m') as `month` ");
		j+=sprintf(sql+j, "FROM wq_general.valid_data_eu w, wq_general.wq_station ws, watergap_unf.arcid_wg3_wg2 a ");
		j+=sprintf(sql+j, "WHERE w.`parameter_id` in (%d, %d, %d, %d) ", parameter_id, parameter_id_load, parameter_id_discharge, parameter_id_conductivity);
//		j+=sprintf(sql+j, " AND w.`data_used` is null ");
		j+=sprintf(sql+j, " AND (w.`data_used` is null OR w.`data_used`=-5)");
		j+=sprintf(sql+j, " AND ws.`data_used` is null AND w.`watertype`=%d ", options_wqstat_ptr->watertype);
		j+=sprintf(sql+j, "AND ws.station_id=w.station_id ");
		j+=sprintf(sql+j, "AND a.`arcid_global`=ws.`global_id` AND year(w.`date`) BETWEEN %d AND %d GROUP BY w.`parameter_id`,w.`date`, a.`arcid_watch`) AS a ", start, end);
		j+=sprintf(sql+j, "GROUP BY a.`parameter_id`, a.`month`, a.`cell`;");

		query.reset();
		query << sql;


	}

	cout<<"\n\n Myquery: "<<query<<endl;

	mysqlpp::StoreQueryResult res_statistic_instream = query.store();
	cout << "##################################\n";
	cout << "res_statistic_grdc.size():\t"<<res_statistic_grdc.size()<<endl;
	cout << "res_statistic_instream.size():\t"<<res_statistic_instream.size()<<endl;
	cout << "##################################\n";

	vector <statisticClass> statistic_new_rows;

	for(unsigned int num=0; num<=statistic.size(); num++) {
		int date = statistic[num].date;
		int cell = statistic[num].cell;

		for (unsigned int rowNum=0; rowNum<res_statistic_grdc.size(); rowNum++)
		{
			int date_res = res_statistic_grdc[rowNum]["date"];
			int cell_res = res_statistic_grdc[rowNum]["cell"];
			if (cell == cell_res && date ==  date_res) {
				if (statistic[num].GRDC != -9999) { // mehrere Stationen in einer Zelle
					statisticClass statistic_line;
					statistic_line.copyValues(statistic[num]); // Copie erstellen
					statistic_line.setValues(); // Daten, die in dieser Funktion evtl. geaendert wurden, auf -9999 setzen
					statistic_line.GRDC = res_statistic_grdc[rowNum]["GRDC"];
					statistic_line.calculated = res_statistic_grdc[rowNum]["calculated"];
					statistic_new_rows.push_back(statistic_line);
				} else {
					statistic[num].GRDC = res_statistic_grdc[rowNum]["GRDC"];
					statistic[num].calculated = res_statistic_grdc[rowNum]["calculated"];
				}
			}
		}

	}
	cout << "statistic_new_rows.size():\t"<<statistic_new_rows.size()<<endl;
	cout << "statistic.size():\t"<<statistic.size()<<endl;
	cout << "##################################\n";
	for (unsigned int rowNum=0; rowNum<statistic_new_rows.size(); rowNum++) // wenn es Zellen gab mit mehr als eine GRDC Station
		statistic.push_back(statistic_new_rows[rowNum]);
	cout << "##################################\n";

	cout << "statistic.size():\t"<<statistic.size()<<endl;
	cout << "##################################\n";
	int count_measured=0, count_dis=0, count_load=0, count_cond=0;
	for(unsigned int num=0; num<=statistic.size(); num++) {
		int date = statistic[num].date;
		int cell = statistic[num].cell;
		//cout << "num=\t"<<num<<'\t'<<date<<'\t'<<cell<<endl;
		for (unsigned int rowNum=0; rowNum<res_statistic_instream.size(); rowNum++)
		{
			int date_res = res_statistic_instream[rowNum]["date"];
			int cell_res = res_statistic_instream[rowNum]["cell"];
			if (cell == cell_res && date ==  date_res) {
				int parameter_id_res = res_statistic_instream[rowNum]["parameter_id"];
				//cout << "rowNum=\t"<<rowNum<<'\t'<<date_res<<'\t'<<cell_res<<'\t'<< parameter_id_res <<endl;
				if (parameter_id == parameter_id_res) {
					statistic[num].measured = res_statistic_instream[rowNum]["measured"];
					statistic[num].flag_uncert = res_statistic_instream[rowNum]["flag_uncert"];
					count_measured++;
				}
				if (parameter_id_discharge == parameter_id_res) {
					statistic[num].measured_discharge = res_statistic_instream[rowNum]["measured"];
					statistic[num].flag_uncert_discharge = res_statistic_instream[rowNum]["flag_uncert"];
					count_dis++;
				}
				if (parameter_id_load == parameter_id_res) {
					statistic[num].measured_load = res_statistic_instream[rowNum]["measured"];
					statistic[num].flag_uncert_load = res_statistic_instream[rowNum]["flag_uncert"];
					count_load++;
				}
				if (parameter_id_conductivity == parameter_id_res) {
					statistic[num].measured_conductivity = res_statistic_instream[rowNum]["measured"];
					statistic[num].flag_uncert_conductivity = res_statistic_instream[rowNum]["flag_uncert"];
					count_cond++;
				}
			}
		}
	}
	cout << "statistic.size():\t"<<statistic.size()<<endl;
	cout << "##################################\n";
} // end of get_Output()

int    getRoutArea(const options_wqstatClass* options_wqstat_ptr, int Cell_value) {
	int j;
	char sql[2000]; // variable for SQL-query

	if (options_wqstat_ptr->IDVersion==2 )	{
		j=sprintf(sql, "SELECT `rout_area` FROM watergap_unf.mother_wg2 m WHERE m.`arcid`=%d;", Cell_value);
	} else if (options_wqstat_ptr->IDVersion==3)	{
		j=sprintf(sql, "SELECT `rout_area` FROM watergap_unf.mother_%s m WHERE m.`arcid`=%d;", options_wqstat_ptr->continent_abb, Cell_value);
	}
	else {
		return -9999; // kein bekannter Kontinent
	}
	
	query.reset();
	query << sql;
	//cout<<"get Rout Area query: "<<query<<endl;
	res = query.store();
	if (res.empty()) return -9999;  // arcid existiert nicht
	int rout_area = res.at(0)["rout_area"];
	return rout_area;
}


