// 05-09-2007
// Ellen Teichert
//

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "../general_function/timestring.h"
#include "../general_function/general_functions.h"
#include "options_wqstat_scen.h"

//#define MY_DATABASE ""
//#define MY_HOST "MySQL"
//#define MY_USER "worldqual"
//#define MY_PASSWORD "1qay"
//#define BYTESWAP_NECESSARY


const int   initYears              = 0; // Initialisierungsjahre

int continent[5];  // 0 = ng; 1 - ng_land; 2 - nrows; 3 - ncols
char hydro_input[maxcharlength];           	 // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Input-directory
char  MyDatabase[maxcharlength];
char continent_abb[10];			// Abbreviation of the continent name; used for detection of corresponding database


sql_create_14(Output_Daten, 5, 0, 
			 sql_int_unsigned, riverSection,
			 sql_int, station_id,
			 sql_int_unsigned, cell,
			 sql_int_unsigned, date,
			 sql_int_unsigned, rout_area,
			 sql_double, con1_mg_l,
			 sql_double, con2_mg_l,
			 sql_double, measured,
			 sql_double, con1_mg_l_scen1,
			 sql_double, con2_mg_l_scen1,
			 sql_double, con1_mg_l_scen2,
			 sql_double, con2_mg_l_scen2,
			 sql_double, con1_mg_l_scen3,
			 sql_double, con2_mg_l_scen3
			 );

sql_create_1(Liste_X, 1, 0, 
			 sql_int_unsigned, cell);

sql_create_7(min_max_date, 4, 0, 
			 sql_int_unsigned, riverSection,
			 sql_int, parameter_id,
			 sql_int, station_id,
			 sql_int_unsigned, cell,
			 sql_int_unsigned, rout_area,
			 sql_int_unsigned, min_date,
			 sql_int_unsigned, max_date
			 );

mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::Query      query_ins = con.query(); // fuer INSERT into 'statistic'
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;

set <Liste_X> resListe_X;
set <Liste_X>::iterator itListe_X;

set <Output_Daten> resOutput_Daten;
set <Output_Daten>::iterator itOutput_Daten;

void   get_Output(int riverSection, int IDVersion, int IDReg, 
		int parameter_id, int parameter_id_load, int parameter_id_discharge, int start, int end, int IDrun, char *MyDatabase, 
		int DiffYears, int IDrun_scen1, int IDrun_scen2, int IDrun_scen3, int watertype);


int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram world started at: " << getTimeString() << endl << endl;
	options_wqstat_scenClass options_wqstat_scen(argc, &argv[0], "startYear endYear");
	
	if (argc<8) {
		options_wqstat_scen.Help();
//	    cout << "\n./wq_stat_scen startYear endYear [-ooptions_file]\n";
//	    cout << "\n./wq_stat_stations IDrun startYear endYear DiffYears IDrun_scen1 IDrun_scen2 IDrun_scen3 [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region] [-i path] [-wt watertype]";
//         << "\n\nparameters apart from IDrun, startYear, endYear DiffYears IDrun_scen1 IDrun_scen2 IDrun_scen3 must not be in order. "
//         << "\nDiffYears      Jahresunterschied zwischen baseline und Szenarien"
//         << "\nIDrun_scen1,2,3 IDruns f�r Szenarien"
//         << "\nWenn ein (oder mehrere) Parameter fehlt, werden folgende Werte als Vorgabe benutzt:                          "
//         << "\n\nDATABASE      \"empty string\""
//         << "\nHOST          \"MySQL\"    "
//         << "\nUSER          \"worldqual\""
//         << "\nPASSWORD      \"*******\"  "
//         << "\nIDVersion     3 (WaterGAP3)"
//         << "\nIDReg         1 (Europa) \n\n"
//         << "\nwenn weder -calc noch -const eingegeben wurden, Fliessgeschwindigkeit wird aus der Tabelle oder UNF-Datei entnommen.\n"
//         << "\n-i            bedeuet Daten nicht aus der Tabellen, sondern aus UNF-Dateien einzulesen.\n\n"
//         << "\nwatertype River=1 is defualt";
         return 0;
	}

	if(!IsNumber(argv[1]) || !IsNumber(argv[2])) {
		cerr << "ERROR: Wrong parameter" << endl;
		options_wqstat_scen.Help();
//	    cout << "usage: ./wq_stat_scen startYear endYear [-ooptions_file]\n";
		exit(1);
	}
	int start       = atoi(argv[1]);
	int end         = atoi(argv[2]);

	if (start>end) {
		cerr << "ERROR: Wrong parameter. start year> end year!!!" << endl;
		options_wqstat_scen.Help();
//	    cout << "usage: ./wq_stat_scen startYear endYear [-ooptions_file]\n";
		exit(1);
	}

	 // 0: baseline, 1-3: Szenarien
	int parameter_id_input[4], parameter_id[4], parameter_id_load[4], parameter_id_discharge=10;
	int project_id[4];
	int IDScen[4], IDScen_wq_load[4];

	double *riverFlowTime = new double[(end-start+initYears+1)*12];       // Fliesszeit fuer Fluss, s
	
	char headerline[maxcharlength]; // variable for line input of header

	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 3 = 1 + 2 = programmname + 2 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(options_wqstat_scen.init(argc-3, &argv[3])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input

	int j;
	try {
		char sql[2000]; // variable for SQL-query

		cout << "start:" << endl;
		char start_db[maxcharlength]; //sprintf(start_db, "%sworldqual_%s", MyDatabase, continent_abb);
		sprintf(start_db, "wq_general");

		if (!con.connect(&start_db[0], options_wqstat_scen.MyHost, options_wqstat_scen.MyUser, options_wqstat_scen.MyPassword,3306)) {
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl; //return 0;
		////////////////////////////////

		mysqlpp::String runName[4];

		cout << "####################################################\n";
		cout << "Pruefen baseline-IDrun:\t" << options_wqstat_scen.IDrun << endl;
		int scen=0;
		if(checkIDRun(options_wqstat_scen.IDrun, runName[scen], parameter_id_input[scen], parameter_id[scen], parameter_id_load[scen]
				, project_id[scen], IDScen[scen], IDScen_wq_load[scen])) exit(1);

		cout << "Pruefen scenario-IDrun:\t" << options_wqstat_scen.IDrun_scen1 << endl;
		scen=1;
		if(checkIDRun(options_wqstat_scen.IDrun_scen1, runName[scen], parameter_id_input[scen], parameter_id[scen], parameter_id_load[scen]
				, project_id[scen], IDScen[scen], IDScen_wq_load[scen])) exit(1);

		cout << "Pruefen scenario-IDrun:\t" << options_wqstat_scen.IDrun_scen2 << endl;
		scen=2;
		if(checkIDRun(options_wqstat_scen.IDrun_scen2, runName[scen], parameter_id_input[scen], parameter_id[scen], parameter_id_load[scen]
				, project_id[scen], IDScen[scen], IDScen_wq_load[scen])) exit(1);

		cout << "Pruefen scenario-IDrun:\t" << options_wqstat_scen.IDrun_scen3 << endl;
		scen=3;
		if(checkIDRun(options_wqstat_scen.IDrun_scen3, runName[scen], parameter_id_input[scen], parameter_id[scen], parameter_id_load[scen]
				, project_id[scen], IDScen[scen], IDScen_wq_load[scen])) exit(1);

		for (int scen=1; scen<4; scen++) {
			if (parameter_id[0] != parameter_id[scen]) {
				cerr << "Unterschiedliche parameter_id f�r baseline und Szenario!\n";
				exit(1);
			}
			if (parameter_id_input[0] != parameter_id_input[scen]) {
				cerr << "Unterschiedliche parameter_id_input f�r baseline und Szenario!\n";
				exit(1);
			}
			if (parameter_id_load[0] != parameter_id_load[scen]) {
				cerr << "Unterschiedliche parameter_id_load f�r baseline und Szenario!\n";
				exit(1);
			}
		}

		// checking IDScen and scenario name
		for (int scen=0; scen<4; scen++) {
			if(checkIDScen(project_id[scen], IDScen[scen])) exit(1);
			if (IDScen[scen] != IDScen_wq_load[scen]) {
				cout << "IDScen_wq_load: ";
				if(checkIDScen(project_id[scen], IDScen_wq_load[scen])) exit(1);
			}
		}


		mysqlpp::String ParameterName, LoadParameterName, DischargeParameterName;
		int toCalc;

		// checking parameter_id and parameter name
		if (checkParameter(parameter_id[0], toCalc, ParameterName)) exit(1);

		int dummy; // toCalc nur fuer parameter_id
		if (parameter_id_load[0]>0) { // wenn berechnete Fracht aus gemessenen Abfluss und Konzentration eingegeben ist
			if (checkParameter(parameter_id_load[0], dummy, LoadParameterName, false)) exit(1);
		}

		if (parameter_id_discharge>0) { // wenn Discharge eingegeben ist
			if (checkParameter(parameter_id_discharge, dummy, DischargeParameterName, false)) exit(1);
		}

		//############################################
		// Pruefen, ob concentration schon berechnet wurde
		query.reset();
		j=sprintf(sql,    "SELECT count(*) as cells FROM `%swq_out_%s`.`concentration_%d` WHERE date between %d and %d;"
				, MyDatabase, continent_abb, options_wqstat_scen.IDrun, start*100+1, end*100+12);
		query << sql;
		res = query.store();
		int cells=res.at(0)["cells"];

		if (cells==0) {cerr << "ERROR: Kein INPUT vorhanden\n"; exit(1);}
		cout << "cells = " << cells <<endl;
		cout <<"----------------\n";

		//############################################
		// Pruefen, ob concentration f�r Szenario 1 schon berechnet wurde
		{
		query.reset();
		j=sprintf(sql,    "SELECT count(*) as cells FROM `%swq_out_%s`.`concentration_%d` WHERE date between %d and %d;"
			, MyDatabase, continent_abb, options_wqstat_scen.IDrun_scen1, (start+options_wqstat_scen.DiffYears)*100+1, (end+options_wqstat_scen.DiffYears)*100+12);
		query << sql;
		res = query.store();
		cells=res.at(0)["cells"];

		if (cells==0) {cerr << "ERROR: Kein INPUT vorhanden f�r Szenario 1: IDrun=" <<options_wqstat_scen.IDrun_scen1<<"\n"; options_wqstat_scen.IDrun_scen1=-1;}
		cout << "cells Szenario1: " << cells <<endl;
		cout <<"----------------\n";

		//############################################
		// Pruefen, ob concentration f�r Szenario 2 schon berechnet wurde
		query.reset();
		j=sprintf(sql,    "SELECT count(*) as cells FROM `%swq_out_%s`.`concentration_%d` WHERE date between %d and %d;"
			, MyDatabase, continent_abb, options_wqstat_scen.IDrun_scen2, (start+options_wqstat_scen.DiffYears)*100+1, (end+options_wqstat_scen.DiffYears)*100+12);
		query << sql;
		res = query.store();
		cells=res.at(0)["cells"];

		if (cells==0) {cerr << "ERROR: Kein INPUT vorhanden f�r Szenario 2: IDrun=" <<options_wqstat_scen.IDrun_scen1<<"\n"; options_wqstat_scen.IDrun_scen2=-1;}
		cout << "cells Szenario2: " << cells <<endl;
		cout <<"----------------\n";
		
		//############################################
		// Pruefen, ob concentration f�r Szenario 3 schon berechnet wurde
		query.reset();
		j=sprintf(sql,    "SELECT count(*) as cells FROM `%swq_out_%s`.`concentration_%d` WHERE date between %d and %d;"
			, MyDatabase, continent_abb, options_wqstat_scen.IDrun_scen3, (start+options_wqstat_scen.DiffYears)*100+1, (end+options_wqstat_scen.DiffYears)*100+12);
		query << sql;
		res = query.store();
		cells=res.at(0)["cells"];

		if (cells==0) {cerr << "ERROR: Kein INPUT vorhanden f�r Szenario 3: IDrun=" <<options_wqstat_scen.IDrun_scen3<<"\n"; options_wqstat_scen.IDrun_scen3=-1;}
		cout << "cells Szenario3: " << cells <<endl;
		cout <<"----------------\n";
		}

		///=================================
		// Parameter Landmaske einlesen
		if(getCELLS(&options_wqstat_scen, &continent)) exit(1); cout << "Parameter Landmaske wurden eingelesen\n";
		
		int inflow[8];              // Zuflusszellen
		double Q_in[8][12], C_in[8], C_in_2[8]; // Durchfluss und Konzentration (2 Varianten) von Zuflusszellen
		double Q_out[12];                   // Durchfluss in der Zelle, Monatswerte
		
		for (int i=0; i<8; i++) {
		  inflow[i]   = 0;
		  C_in[i] = 0; C_in_2[i] = 0;
		  for (int month=1; month<=12; month++) {		  
		    Q_in[i][month-1] = 0.0;
		  }
		}
		for (int month=1; month<=12; month++) {		  
		  Q_out[month-1] = 0.0;
		}

		//------------------------------------------------------
//		query.reset();
//		query << "DELETE FROM wq_general.`statistic_rivercells`;";
//		query.execute();
		//------------------------------------------------------

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

				int   riverSection=0;   // Nummerierung von Statistiken fuer verschiedene Flussabschnitte
				
				do {
					//----------------------------------
					// Flussname einlesen 
					{
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
					//----------------------------------
					if (!inputfile) continue;

					inputfile >> arcid_begin_global >> arcid_end_global;
					arcid_begin = get_ArcID(&options_wqstat_scen, arcid_begin_global);
					arcid_end   = get_ArcID(&options_wqstat_scen, arcid_end_global);
					cout<<"arcid_begin="<<arcid_begin<<" arcid_end="<<arcid_end<<endl;

//					vector <statisticStationsClass> statistic;
					vector <int> statistic_rivercells;  // Liste von allen Zellen im Flussabschnitt
					//======================================================

					riverSection++;
					stat_FlowLenght = 0.0;
					int TimeStep;
					for (TimeStep=0; TimeStep<((end-start+initYears+1)*12); TimeStep++)
						riverFlowTime[TimeStep] = 0.0; 
					
					cout <<"rivername: " << rivername << " arcid_begin(global): "<< arcid_begin << "(" << arcid_begin_global 
						<< ") arcid_end(global): "<<arcid_end<<"(" << arcid_end_global<<")"<<endl;
					//------------------------------------------------------
					
					// Cellenliste aus def Fluss in die Tabelle statistic_rivercells
					int Cell_value = arcid_begin;
					
					while (Cell_value>0) { 
						int outflowCell = get_outflowCell(Cell_value, &options_wqstat_scen);  // Abflusszelle finden
						
						statistic_rivercells.push_back(Cell_value);
//						query_ins.reset();  // query fuer INSERT into 'statistic_rivercells'
//						query_ins << "INSERT INTO wq_general.statistic_rivercells (`riverSection`, `cell`) VALUES ( "
//						  << riverSection<<", " << Cell_value << ");";
//						query_ins.execute();

						
						if (Cell_value==arcid_end) Cell_value=0;	else Cell_value=outflowCell;
                
						//==============================================
					} //while (Cell_value>0)


//					query_ins.reset();  // query fuer rout_area in 'statistic_rivercells'
//					query_ins << "update wq_general.statistic_rivercells s, watergap_unf.mother_"<<continent_abb<<" m "
//					  << "set s.`rout_area`= m.`rout_area` WHERE m.`arcid`=s.`cell`;";
//					query_ins.execute();

					//=========================================
					// OUTPUT in die Datei
					//=========================================
					//ofstream outputfile_stat;
                
					sprintf(filename,"statistic_stat_scen_param_%d_%s_%d_%d.txt", options_wqstat_scen.IDrun, rivername ,start, end);
					cout << "output file: " << filename << endl;
					outputfile_stat.open(filename);
					if (!outputfile_stat) {
						cerr << "\nCould not open " << filename << " for output" << endl;
						exit (0);
					}
					outputfile_stat << getTimeString() << endl;
					outputfile_stat << "IDVersion:\t" << options_wqstat_scen.IDVersion << "\tIDReg:\t" << options_wqstat_scen.IDReg << endl;
					outputfile_stat << "IDrun base :\t"<<options_wqstat_scen.IDrun << "\t ( \t" << runName[0] << "\t)"<< endl;
					outputfile_stat << "IDrun scen1:\t"<<options_wqstat_scen.IDrun_scen1 << "\t ( \t" << runName[1] << "\t)"<< endl;
					outputfile_stat << "IDrun scen2:\t"<<options_wqstat_scen.IDrun_scen2 << "\t ( \t" << runName[2] << "\t)"<< endl;
					outputfile_stat << "IDrun scen3:\t"<<options_wqstat_scen.IDrun_scen3 << "\t ( \t" << runName[3] << "\t)"<< endl;
					outputfile_stat << "Parameter          : " << parameter_id_input[0]   << " original: " << parameter_id[0] << "\t ( \t" << ParameterName << "\t )\tmmm" << endl;
					outputfile_stat << "berechnete Fracht  :\t" << parameter_id_load   << "\t ( \t" << LoadParameterName << "\t)" << endl;
					outputfile_stat << "years:\t" << start << "\t - \t" << end << endl;
					outputfile_stat << rivername << "\tarcid_begin: \t" << arcid_begin << "\t(\t"<<arcid_begin_global<<"\t)\t" 
					  <<"arcid_end: \t" << arcid_end<< "\t(\t"<<arcid_end_global<<"\t)\t" <<endl;
					outputfile_stat << "riverSection\t ArcId\t date\t*\t*\t Konz_1, mg/l \t Konz_2, mg/l "
					  << "\tmeasured \tStationID \trout_area "
					  << "\t "<< options_wqstat_scen.IDrun_scen1<<" Konz_1 scen1, mg/l \t Konz_2 scen1, mg/l"
					  << "\t "<< options_wqstat_scen.IDrun_scen2<<" Konz_1 scen2, mg/l \t Konz_2 scen2, mg/l"
					  << "\t "<< options_wqstat_scen.IDrun_scen3<<" Konz_1 scen3, mg/l \t Konz_2 scen3, mg/l"<< endl;
					
					get_Output(riverSection, options_wqstat_scen.IDVersion, options_wqstat_scen.IDReg
						, parameter_id[0], parameter_id_load[0]
						, parameter_id_discharge, start, end, options_wqstat_scen.IDrun, MyDatabase
						, options_wqstat_scen.DiffYears
						, options_wqstat_scen.IDrun_scen1, options_wqstat_scen.IDrun_scen2, options_wqstat_scen.IDrun_scen3
						, options_wqstat_scen.watertype);
					
					unsigned int cellValue=0;	unsigned int dateValue=0;


					for (itOutput_Daten=resOutput_Daten.begin(); itOutput_Daten!=resOutput_Daten.end(); itOutput_Daten++) {
						if ((itOutput_Daten->cell!=cellValue) || (itOutput_Daten->date!=dateValue)) {
							cellValue=itOutput_Daten->cell;
							dateValue=itOutput_Daten->date;
						}

						outputfile_stat << itOutput_Daten->riverSection << '\t'
						  << itOutput_Daten->cell << '\t'
						  << itOutput_Daten->date%100<<'-'<<itOutput_Daten->date/100 << "\t\t\t"
						  << itOutput_Daten->con1_mg_l << '\t'
						  << itOutput_Daten->con2_mg_l << '\t';
						
						if (itOutput_Daten->measured!=-9999) outputfile_stat << itOutput_Daten->measured;
						outputfile_stat << '\t';
 						
 						if (itOutput_Daten->station_id!=-9999) outputfile_stat << itOutput_Daten->station_id;  
 						outputfile_stat << '\t';
						outputfile_stat << itOutput_Daten->rout_area << '\t';

						if (itOutput_Daten->con1_mg_l_scen1!=-9999) outputfile_stat << itOutput_Daten->con1_mg_l_scen1;
						outputfile_stat << '\t';
						if (itOutput_Daten->con2_mg_l_scen1!=-9999) outputfile_stat << itOutput_Daten->con2_mg_l_scen1;
						outputfile_stat << '\t';

						if (itOutput_Daten->con1_mg_l_scen2!=-9999) outputfile_stat << itOutput_Daten->con1_mg_l_scen2;
						outputfile_stat << '\t';
						if (itOutput_Daten->con2_mg_l_scen2!=-9999) outputfile_stat << itOutput_Daten->con2_mg_l_scen2;
						outputfile_stat << '\t';

						if (itOutput_Daten->con1_mg_l_scen3!=-9999) outputfile_stat << itOutput_Daten->con1_mg_l_scen3;
						outputfile_stat << '\t';
						if (itOutput_Daten->con2_mg_l_scen3!=-9999) outputfile_stat << itOutput_Daten->con2_mg_l_scen3;
						outputfile_stat << '\t';

 						outputfile_stat << '\n';

					}
						
					//======================================================
					
					outputfile_stat.close();
                
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
	cout <<"\nProgram worldqual endet at: " << getTimeString() << endl << endl;
	return 0;
} // end of main()



void get_Output(int riverSection, int IDVersion, int IDReg, int parameter_id, int parameter_id_load, 
	int parameter_id_discharge, int start, int end, int IDrun, char *MyDatabase,
	int DiffYears, int IDrun_scen1, int IDrun_scen2, int IDrun_scen3, int watertype) {
	int j;
	char sql[2000]; // variable for SQL-query
	cout << "get_Output():\n";
	
//	// lock tables
//	query.reset();
//	query << "LOCK TABLES wq_general.`statistic_instream` WRITE, wq_general.valid_data_"<<continent_abb<<" as w READ, ";
//	query <<"wq_general.wq_station as ws READ, watergap_unf.gcrc as g READ ";
//	query.execute();
//	cout << "LOCK TABLE wq_general.`statistic_instream`\n";

	
//	// eigentlich wird schneller, wenn statistic_instream nur ein mal pro programm-lauf gef�llt wird
//	// und dann f�r alle Fl�sse verwendet.
//	// z.Zeit bei jedem get_output - Aufruf, also f�r jede Fluss neu gleiche Daten in die Tabelle geschrieben werden
//	query.reset();
//	query << "DELETE FROM wq_general.`statistic_instream`;";
//	query.execute();


	//first the daily average is built and afterwords the monthly average
//	j=sprintf(sql, "INSERT INTO wq_general.statistic_instream (`parameter_id`, `station_id`, `cell`, `date`, `flag_uncert`, `measured`) ");
	j=sprintf(sql, "SELECT a.`parameter_id`, -9999, a.`cell`, a.`month` AS `date`, a.`flag_uncert`, avg(a.measured_day_avg) AS `measured` \n");
	j+=sprintf(sql+j, "FROM (SELECT w.`parameter_id`, ws.`global_id`, g.`cell`, max(ifnull(ws.`flag_uncert`,0)) as 'flag_uncert'");
	j+=sprintf(sql+j, ", avg(w.Value) as 'measured_day_avg', w.`date`, DATE_FORMAT(w.`date`,'%%Y%%m') as `month` \n");
	j+=sprintf(sql+j, "FROM wq_general.valid_data_%s w, wq_general.wq_station ws, watergap_unf.gcrc g ", continent_abb);
	j+=sprintf(sql+j, "WHERE w.`parameter_id` =%d AND ws.`data_used` is null ", parameter_id);
	j+=sprintf(sql+j, "AND ws.station_id=w.station_id AND g.`IDVersion`=%d AND g.`IDReg`=%d ", IDVersion, IDReg);
	j+=sprintf(sql+j, " AND w.`data_used` is null AND w.`watertype`=%d", watertype);
	j+=sprintf(sql+j, " AND g.`GLOBALID`=ws.`global_id` AND year(w.`date`) BETWEEN %d AND %d ", start, end);
	j+=sprintf(sql+j, "GROUP BY w.`date`, g.`cell`) AS a ");
	j+=sprintf(sql+j, "GROUP BY a.`month`, a.`cell`;");
	query.reset();
	query << sql;
	cout<<"avg query="<<query<<endl;

//	query.execute();
	mysqlpp::StoreQueryResult res_statistic_instream = query.store();
	cout << "##################################\n";
	cout << "res_statistic_instream.size():\t"<<res_statistic_instream.size()<<endl;
	cout << "##################################\n";

	if (res_statistic_instream.size() == 0) return;

//	query.reset();
//	query << "UNLOCK TABLES;";
//	query.execute();


//	// lock tables
//	query.reset();
//	query << "LOCK TABLES wq_general.`statistic_min_max_date` WRITE, wq_general.`statistic_stations` WRITE, wq_general.`statistic_instream` as s READ, ";
//	query <<"wq_general.`statistic_rivercells` as `sr` READ, `"<<MyDatabase<<"wq_out_"<<continent_abb<<"`.`concentration_"<<IDrun<<"` c READ,"
//	<<'`'<<MyDatabase<<"wq_out_"<<continent_abb<<"`.`concentration_"<<IDrun_scen1<<"` c1 READ,"
//	<<'`'<<MyDatabase<<"wq_out_"<<continent_abb<<"`.`concentration_"<<IDrun_scen2<<"` c2 READ,"
//	<<'`'<<MyDatabase<<"wq_out_"<<continent_abb<<"`.`concentration_"<<IDrun_scen3<<"` c3 READ, ";
//	query <<"wq_general. `statistic_min_max_date` as sm READ;";
//
//	cout << "Query: " << query << endl << endl; //cin >> j;
//
//	query.execute();
//	cout << "LOCK TABLES wq_general.`statistic_min_max_date`...\n";

//	// ==================================================
//	// so werden Daten calculated nur f�r c.date sein, wo auch s.measured gibt
//	j=sprintf(sql, "SELECT sr.`riverSection`, s.`station_id`, sr.`cell`, s.`date`, c.`con`/1000 as `con1_mg_l`, ");
//	j+=sprintf(sql+j, "c.`con2`/1000 as `con2_mg_l`, s.`measured` ");
//	j+=sprintf(sql+j, "FROM wq_general.`statistic_instream` s, `%swq_out_%s`.`concentration_%d` c, wq_general.`statistic_rivercells` sr ", MyDatabase, continent_abb, IDrun);
//	j+=sprintf(sql+j, "WHERE s.`parameter_id`= %d AND c.`date`=s.`date` ", parameter_id);
//	j+=sprintf(sql+j, "AND c.cell=s.cell and sr.riversection=%d and sr.cell=s.cell;", riverSection);
//	// ==================================================
	
//	// und so alle calculated
//	query.reset();
//	query << "DELETE FROM wq_general.`statistic_min_max_date` where riversection="<<riverSection<<';';
//	query.execute();
//	cout << "DELETE FROM wq_general.`statistic_min_max_date` done\n";



	//j=sprintf(sql, "insert into statistic_min_max_date ");
	//j+=sprintf(sql+j, "SELECT sr.`riverSection`, s.`parameter_id`, s.`station_id`, s.`cell`, ");
	j=sprintf(sql, "SELECT sr.`riverSection`, s.`parameter_id`, s.`station_id`, s.`cell`, sr.`rout_area`, ");
	
	// nur die mMonate, die es gibt
	//j+=sprintf(sql+j, "min(s.`date`), max(s.`date`) ");
	
	// Jahr immer ab Januar bis Dezember
	j+=sprintf(sql+j, "min(s.`date`)-mod(min(s.`date`),100)+1, max(s.`date`)-mod(max(s.`date`),100)+12 ");
	
	j+=sprintf(sql+j, "FROM wq_general.`statistic_instream` s, wq_general.`statistic_rivercells` sr ");
	j+=sprintf(sql+j, "WHERE sr.`riverSection`=%d AND sr.cell=s.cell ", riverSection);
	j+=sprintf(sql+j, "GROUP BY s.`parameter_id`, s.`cell`;");
	
	set <min_max_date> resMinMaxDate;
	set <min_max_date>::iterator itMinMaxDate;

	query.reset();
	query << sql;
	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	query.storein(resMinMaxDate);
	//query.execute();
	cout << "insert into statistic_min_max_date done\n";
	
	query.reset();
	query << "DELETE FROM wq_general.`statistic_stations` where riversection="<<riverSection<<';';
	query.execute();
	cout << "DELETE FROM `statistic_stations` done\n";

	int min_date, max_date, min_year, max_year, min_month, max_month, beg_mon, end_mon;

	for (itMinMaxDate=resMinMaxDate.begin(); itMinMaxDate!=resMinMaxDate.end(); itMinMaxDate++) {
		min_date=itMinMaxDate->min_date; min_year = min_date/100; min_month=min_date%100;
		max_date=itMinMaxDate->max_date; max_year = max_date/100; max_month=max_date%100;
		for (int year = min_year; year<=max_year; year++) {
			beg_mon=1; end_mon=12;
			if (year==min_year) beg_mon=min_month;
			else if (year==max_year) end_mon=max_month;
			
			for (int month=beg_mon; month<=end_mon; month++) {
				//cout << year<<" "month<<" "<<
				j=sprintf(sql, "insert into wq_general.statistic_stations (`riverSection`, `station_id`, `cell`, `date`, `con1_mg_l`, `con2_mg_l`, `rout_area`) ");
				j+=sprintf(sql+j, "SELECT %d, %d, ", itMinMaxDate->riverSection, NODATA);
				j+=sprintf(sql+j, "c.`cell`, c.`date`, c.`con`/1000 as `con1_mg_l`, c.`con2`/1000  as `con2_mg_l`, %d ", itMinMaxDate->rout_area);
				j+=sprintf(sql+j, "FROM `%swq_out_%s`.`concentration_%d` c ", MyDatabase, continent_abb, IDrun);
				j+=sprintf(sql+j, "WHERE c.`date`=%d ", year*100+month);
				j+=sprintf(sql+j, "AND c.cell=%d;", itMinMaxDate->cell);
				
				query.reset();
				query << sql;
				
				//#ifdef DEBUG_queries
				//  cout << "Query: " << query << endl << endl; cin >> j;
				//#endif
				cout<<"query insert into wq_general.statistic_stations: "<<query<<endl;
				query.execute();
				
				
			} // for(month)
		} // for(year)

	}
	
	
	
	cout << "insert into statistic_stations done\n";

	j=sprintf(sql, "update  wq_general.statistic_stations , wq_general.statistic_instream s ");
	j+=sprintf(sql+j, "set statistic_stations.`measured`= s.`measured` ");
	j+=sprintf(sql+j, "WHERE statistic_stations.`riverSection`=%d AND ", riverSection);
	j+=sprintf(sql+j, " s.cell=statistic_stations.cell AND ");
	j+=sprintf(sql+j, "s.`date`=statistic_stations.`date`;");
	query.reset();
	query << sql;
	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	query.execute();
	cout << "update  statistic_stations done\n";

	if (IDrun_scen1>0) {
		
		cout << "Szenario 1: IDrun = "<< IDrun_scen1<<"...";
		j=sprintf(sql, "update wq_general.statistic_stations, `%swq_out_%s`.`concentration_%d` c1 ", MyDatabase, continent_abb, IDrun_scen1);
		j+=sprintf(sql+j, "set statistic_stations.`con1_mg_l_scen1`=c1.`con`/1000, statistic_stations.`con2_mg_l_scen1`= c1.`con2`/1000 ");
		j+=sprintf(sql+j, "WHERE statistic_stations.`riverSection`=%d ", riverSection);
		j+=sprintf(sql+j, "and c1.`date`-%d=statistic_stations.`date` AND c1.`cell`=statistic_stations.`cell`;", DiffYears*100);
		
		query.reset();
		query << sql;
		query.execute();
		cout << "done\n";
	}

	if (IDrun_scen2>0) {
		
		cout << "Szenario 2: IDrun = "<< IDrun_scen2<<"...";
		j=sprintf(sql, "update wq_general.statistic_stations, `%swq_out_%s`.`concentration_%d` c2 ", MyDatabase, continent_abb, IDrun_scen2);
		j+=sprintf(sql+j, "set statistic_stations.`con1_mg_l_scen2`=c2.`con`/1000, statistic_stations.`con2_mg_l_scen2`= c2.`con2`/1000 ");
		j+=sprintf(sql+j, "WHERE statistic_stations.`riverSection`=%d ", riverSection);
		j+=sprintf(sql+j, "and c2.`date`-%d=statistic_stations.`date` AND c2.`cell`=statistic_stations.`cell`;", DiffYears*100);
		
		query.reset();
		query << sql;
		query.execute();
		cout << "done\n";
	}

	if (IDrun_scen3>0) {
		
		cout << "Szenario 3: IDrun = "<< IDrun_scen3<<"...";
		j=sprintf(sql, "update wq_general.statistic_stations, `%swq_out_%s`.`concentration_%d` c3 ", MyDatabase, continent_abb, IDrun_scen3);
		j+=sprintf(sql+j, "set statistic_stations.`con1_mg_l_scen3`=c3.`con`/1000, statistic_stations.`con2_mg_l_scen3`= c3.`con2`/1000 ");
		j+=sprintf(sql+j, "WHERE statistic_stations.`riverSection`=%d ", riverSection);
		j+=sprintf(sql+j, "and c3.`date`-%d=statistic_stations.`date` AND c3.`cell`=statistic_stations.`cell`;", DiffYears*100);
		
		query.reset();
		query << sql;
		query.execute();
		cout << "done\n";
	}

	j=sprintf(sql, "SELECT `riverSection`, `station_id`, `cell`, `date`, `rout_area`, ");
	j+=sprintf(sql+j, "IFNULL(`con1_mg_l`, -9999) as `con1_mg_l`, IFNULL(`con2_mg_l`, -9999) as `con2_mg_l`, ");
	j+=sprintf(sql+j, "IFNULL(`measured`, -9999) as `measured`, ");
	j+=sprintf(sql+j, "IFNULL(`con1_mg_l_scen1`, -9999) as `con1_mg_l_scen1`, IFNULL(`con2_mg_l_scen1`, -9999) as `con2_mg_l_scen1`, ");
	j+=sprintf(sql+j, "IFNULL(`con1_mg_l_scen2`, -9999) as `con1_mg_l_scen2`, IFNULL(`con2_mg_l_scen2`, -9999) as `con2_mg_l_scen2`, ");
	j+=sprintf(sql+j, "IFNULL(`con1_mg_l_scen3`, -9999) as `con1_mg_l_scen3`, IFNULL(`con2_mg_l_scen3`, -9999) as `con2_mg_l_scen3` ");
	j+=sprintf(sql+j, "FROM wq_general.statistic_stations ");
	j+=sprintf(sql+j, "WHERE `riverSection`=%d;",riverSection);

	//
	resOutput_Daten.clear();
	
	query.reset();
	query << sql;
	//#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	//#endif
	query.storein(resOutput_Daten);
	cout << "resOutput_Daten.size: " << resOutput_Daten.size() << endl;
	cout << "##################################\n";

	query.reset();
	query << "UNLOCK TABLES;";
	query.execute();

} // end of get_Output()



