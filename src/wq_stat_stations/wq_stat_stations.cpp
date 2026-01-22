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
// bis jetzt brauchen wq_stat und wq_stat_stations identische Optionen, deswegen nutze hier gleiche Klasse
#include "../wq_stat/options_wqstat.h"
#include "statistic_stations.h"

//#define BYTESWAP_NECESSARY

//#define DEBUG
//#define DEBUG_queries
//#undef DEBUG_queries

using namespace std;
using namespace mysqlpp;

const int   initYears              = 0; // Initialisierungsjahre

char  MyDatabase[maxcharlength];

sql_create_9(Output_Daten, 5, 0, 
			 sql_int_unsigned, riverSection,
			 sql_int, station_id,
			 sql_int_unsigned, cell,
			 sql_int_unsigned, date,
			 sql_int_unsigned, rout_area,
			 sql_double, con1_mg_l,
			 sql_double, con2_mg_l,
			 sql_double, measured,
			 sql_int, flag_uncert
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
mysqlpp::StoreQueryResult res;
mysqlpp::Row        row;

set <Liste_X> resListe_X;
set <Liste_X>::iterator itListe_X;

set <Output_Daten> resOutput_Daten;
set <Output_Daten>::iterator itOutput_Daten;


void   get_Output(vector<statisticStationsClass>& statistic, int Cell_value, int toCalc,
		int parameter_id, int parameter_id_load, int parameter_id_discharge, int parameter_id_conductivity,
		int start, int end, int IDrun,
		char *MyDatabase, const options_wqstatClass* options_wqstat_ptr);
void findMinMaxDate(int &min_date, int &max_date, mysqlpp::StoreQueryResult res_statistic);

int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram world started at: " << getTimeString() << endl << endl;
	options_wqstatClass optionsWQstat(argc, &argv[0], "IDrun startYear endYear");

	if (argc<4) {
		optionsWQstat.Help();
//	    cout << "\n./wq_stat_stations IDrun startYear endYear [-ooptions_file]\n";
//	    cout << "default options file is OPTIONS.DAT\n";
//    cout << "\n./wq_stat_stations IDrun startYear endYear [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region] [-wt watertype]"
//         << "\n\nparameters apart from IDrun, startYear, endYear must not be in order. "
//         << "\nif one (or more) parameters is missing the following default values are used:   "
//         << "\n\nDATABASE      \"empty string\""
//         << "\nHOST          \"MySQL\"    "
//         << "\nUSER          \"worldqual\""
//         << "\nPASSWORD      \"*******\"  "
//         << "\nIDVersion     3 (WaterGAP3)"
//         << "\nIDReg         1 (Europa) \n\n"
//         << "\nwenn weder -calc noch -const eingegeben wurden, Fliessgeschwindigkeit wird aus der Tabelle oder UNF-Datei entnommen.\n\n"
//         << "\n watertype River=1 is default";
		return 0;
	}

	if(!IsNumber(argv[1]) || !IsNumber(argv[2]) || !IsNumber(argv[3])) {
		cerr << "ERROR: Wrong parameter" << endl;
		optionsWQstat.Help();
//		cerr << "usage: ./wq_stat_stations IDrun startYear endYear [-ooptions_file]\n"; //[-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region] "<<endl;
		exit(1);
	}
	int IDrun     = atoi(argv[1]);
	int parameter_id_input, parameter_id, parameter_id_load, parameter_id_discharge=10;
	int parameter_id_conductivity = 120;	double multiplier_conductivity = 0.64; // f�r TDS
	mysqlpp::String runName;
	int start     = atoi(argv[2]);
	int end       = atoi(argv[3]);
	int project_id, IDScen, IDScen_wq_load;
	double *riverFlowTime = new double[(end-start+initYears+1)*12];       // Fliesszeit fuer Fluss, s

	if (start>end) {
		cerr << "ERROR: Wrong parameter. start year> end year!!!" << endl;
		cerr << "usage: ./worldqual IDrun start end [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region]"<<endl;
		exit(1);
	}
	//const short nDaysPerMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 4 = 1 + 3 = programmname + 3 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(optionsWQstat.init(argc-4, &argv[4])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input
	int toCalc;

	char headerline[maxcharlength]; // variable for line input of header

	int j;
	try {
		char sql[2000]; // variable for SQL-query

		cout << "start:" << endl;
		char start_db[maxcharlength]; //sprintf(start_db, "%sworldqual_%s", MyDatabase, optionsWQstat.continent_abb);
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
				, project_id, IDScen, IDScen_wq_load)) exit(1);

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

		mysqlpp::String conductivityParameterName= (mysqlpp::String)" ";  // für TDS
		if (toCalc==1 && parameter_id_conductivity>0) { // nur für Salz
			if (checkParameter(parameter_id_conductivity, dummy, conductivityParameterName, false)) exit(1);
		}

		// Pruefen, ob concentration schon berechnet wurde
		query.reset();
		j=sprintf(sql,    "SELECT count(*) as cells FROM `%swq_out_%s`.`concentration_%d` WHERE date between %d and %d;"
				, MyDatabase, optionsWQstat.continent_abb, IDrun, start*100+1, end*100+12);
		query << sql;
		cout<<"query cells= "<<query<<endl;
		res = query.store();
		int cells=res.at(0)["cells"];

		if (cells==0) {cerr << "ERROR: Kein INPUT vorhanden\n"; exit(1);}
		cout << "cells = " << cells <<endl;
		cout <<"----------------\n";

		///=================================
		// Parameter Landmaske einlesen
		if(getCELLS(&optionsWQstat, &continent)) exit(1); cout << "Parameter Landmaske wurden eingelesen\n";
		
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
					cout<<"arcid_begin="<<arcid_begin<<" arcid_end="<<arcid_end<<endl;

					vector <statisticStationsClass> statistic;
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
					
					// Zellenliste aus def Fluss in den Vector statistic_rivercells
					int Cell_value = arcid_begin;
					
					while (Cell_value>0) { 
						int outflowCell = get_outflowCell(Cell_value, &optionsWQstat);  // Abflusszelle finden
						statistic_rivercells.push_back(Cell_value);

						if (Cell_value==arcid_end) Cell_value=0;	else Cell_value=outflowCell;
						
						//==============================================
					} //while (Cell_value>0)

					//=========================================
					// bas0_id bestimmen
					query.reset();
					query << "SELECT bas0_id FROM watergap_unf.mother_"<<optionsWQstat.continent_abb<<" WHERE arcid in (";
					for (unsigned int num=0; num<statistic_rivercells.size(); num++) {
						query <<  statistic_rivercells[num]; //<< ";";
						if (num==statistic_rivercells.size()-1) query << ") GROUP BY bas0_id;";
						else query << ", ";
					}
					//cout<<"query="<<query<<endl;
					res = query.store();
					if (res.size()!=1) {cerr << "ERROR: cells have not unique bas0_id! "<< res.size()<< endl; exit(1);}
					int bas0_id=res.at(0)["bas0_id"];

					//=========================================
					// OUTPUT in die Datei
					//=========================================
					//ofstream outputfile_stat;
                
					sprintf(filename,"statistic_station_param_%d_%s_%d_%d.txt", IDrun, rivername ,start, end);
					cout << "output file: " << filename << endl;
					outputfile_stat.open(filename);
					if (!outputfile_stat) {
						cerr << "\nCould not open " << filename << " for output" << endl;
						exit (0);
					}
					outputfile_stat << getTimeString() << endl;
					outputfile_stat << "IDVersion:\t" << optionsWQstat.IDVersion << "\tIDReg:\t" << optionsWQstat.IDReg <<"\tbas0_id:\t"<< bas0_id<< endl;
					outputfile_stat << "IDrun:\t"<<IDrun << "\t ( \t" << runName << "\t)"<< endl;
					outputfile_stat << "Parameter          :\t" << parameter_id_input   << "\toriginal: " << parameter_id << "\t ( \t" << ParameterName << "\t )" << endl;
					outputfile_stat << "berechnete Fracht  :\t" << parameter_id_load   << "\t ( \t" << LoadParameterName << "\t)" << endl;
					outputfile_stat << ".......... Fracht  :\t" << parameter_id_conductivity   << "\t ( \t" << conductivityParameterName << "\t)" << endl;
					outputfile_stat << "years:\t" << start << "\t - \t" << end << endl;
					outputfile_stat << rivername << "\tarcid_begin: \t" << arcid_begin << "\t(\t"<<arcid_begin_global<<"\t)\t" 
					  <<"arcid_end: \t" << arcid_end<< "\t(\t"<<arcid_end_global<<"\t)\t" <<endl;
					outputfile_stat << "riverSection\t ArcId\t date\t Konz_1, mg/l \t Konz_2, mg/l "
					  << "\tmeasured \tflag_uncert \tmeasured_conductivity \tflag_uncert_conductivity \trout_area "<< endl;
					
					for (unsigned int num=0; num<statistic_rivercells.size(); num++) {
						Cell_value = statistic_rivercells[num];

						query.reset();
						query << "SELECT rout_area FROM watergap_unf.mother_"<<optionsWQstat.continent_abb<<" WHERE arcid=" << Cell_value<< ";";
						//cout<<"query rout area: "<<query<<endl;
						res = query.store();
						int rout_area=res.at(0)["rout_area"];

						get_Output(statistic, Cell_value, toCalc, parameter_id, parameter_id_load, parameter_id_discharge, parameter_id_conductivity, start, end, IDrun, MyDatabase, &optionsWQstat);
					
						for (unsigned int num=0; num<statistic.size(); num++)
						{
							outputfile_stat << riverSection << '\t'
							  << Cell_value << '\t'
							  << statistic[num].date << '\t'
							  << statistic[num].con << '\t'
							  << statistic[num].con2 << '\t';

							if (statistic[num].measured!=-9999) outputfile_stat << statistic[num].measured;
							outputfile_stat << '\t';

							if (statistic[num].flag_uncert!=-9999) outputfile_stat << statistic[num].flag_uncert;
							outputfile_stat << '\t';

							if (statistic[num].measured_conductivity!=-9999) outputfile_stat << statistic[num].measured_conductivity * multiplier_conductivity;
							outputfile_stat << '\t';

							if (statistic[num].flag_uncert_conductivity!=-9999) outputfile_stat << statistic[num].flag_uncert_conductivity;
							outputfile_stat << '\t';

//	 						if (statistic[num].station_id!=-9999) outputfile_stat << statistic[num].station_id;
//	 						outputfile_stat << '\t';
							outputfile_stat << rout_area;
	 						outputfile_stat << '\n';

						} // for(statistic)
						statistic.clear();
					} // for(statistic_rivercells)
						
					//======================================================
					
					outputfile_stat.close();
                
					//=========================================
					statistic_rivercells.clear();

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
                ", actual size: " << (int)er.actual_size <<
                "\ndata string: "<<er.data<<endl;
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



void get_Output(vector<statisticStationsClass>& statistic, int Cell_value, int toCalc
		, int parameter_id, int parameter_id_load
		, int parameter_id_discharge, int parameter_id_conductivity
		, int start, int end, int IDrun, char *MyDatabase, const options_wqstatClass* options_wqstat_ptr)
{
	int j;
	char sql[2000], sql1[2000], sql2[2000]; // variable for SQL-query
	//cout << "get_Output():\n";
	
	// monatliche Mittelwerte von Messwerten fuer die Zelle Cell_value
	j=sprintf(sql1, "SELECT a.`parameter_id`, -9999, a.`cell`, a.`month` AS `date`, a.`flag_uncert`, avg(a.measured_day_avg) AS `measured` ");
	j+=sprintf(sql1+j, "FROM (SELECT w.`parameter_id`, w.`date`, ws.`global_id`, g.`cell`, max(ifnull(ws.`flag_uncert`,0)) as 'flag_uncert' ");
	j+=sprintf(sql1+j, ", avg(w.Value) as 'measured_day_avg', DATE_FORMAT(w.`date`,'%%Y%%m') as `month` ");
	j+=sprintf(sql1+j, "FROM wq_general.valid_data_%s w, wq_general.wq_station ws, watergap_unf.gcrc g ", options_wqstat_ptr->continent_abb);
	j+=sprintf(sql+j, "WHERE w.`parameter_id` =%d ", parameter_id);
	j=sprintf(sql2, " AND w.`data_used` is null AND ws.`data_used` is null AND w.`watertype`=%d ", options_wqstat_ptr->watertype);
	j+=sprintf(sql2+j, "AND ws.station_id=w.station_id AND g.`IDVersion`=%d AND g.`IDReg`=%d ", options_wqstat_ptr->IDVersion, options_wqstat_ptr->IDReg);
	j+=sprintf(sql2+j, " AND g.`GLOBALID`=ws.`global_id` AND year(w.`date`) BETWEEN %d AND %d AND g.cell = %d", start, end, Cell_value);
	j+=sprintf(sql2+j, " GROUP BY w.`parameter_id`, w.`date`, g.`cell`) AS a ");
	j+=sprintf(sql2+j, "GROUP BY a.`parameter_id`, a.`month`, a.`cell`;");

	// Messwerte von parameter_id
	j=sprintf(sql, "%s\n WHERE w.`parameter_id` =%d \n %s", sql1, parameter_id, sql2);

	query.reset();
	query << sql;
	//cout<<"query statistic_instream: "<<query<<endl;
	mysqlpp::StoreQueryResult res_statistic_instream = query.store();
	//cout << "##################################\n";
	//cout << "res_statistic_instream.size():\t"<<res_statistic_instream.size()<<endl;
	//cout << "##################################\n";
	
	// Messwerte von parameter_id_conductivity
	mysqlpp::StoreQueryResult res_statistic_conductivity;
	if (toCalc==1) {
		j=sprintf(sql, "%s\n WHERE w.`parameter_id` =%d \n %s", sql1, parameter_id_conductivity, sql2);

		query.reset();
		query << sql;
		//cout<<"query statistic_instream: "<<query<<endl;
		res_statistic_conductivity = query.store();
		//cout << "##################################\n";
		//cout << "res_statistic_conductivity.size():\t"<<res_statistic_conductivity.size()<<endl;
		//cout << "##################################\n";
	}

	if ((toCalc!=1 && res_statistic_instream.size() == 0)
			|| (toCalc==1 && res_statistic_instream.size() == 0 && res_statistic_conductivity.size() == 0)) return;

	// Minimum und Maximum Datum von den Messwerten, um berechnete Werte in gleichen Zeitraum aber lueckenlos auszugeben
	int min_date, max_date, min_year, max_year, min_month, max_month, beg_mon, end_mon;
	if (res_statistic_instream.size()>0) {
		min_date = res_statistic_instream[0]["date"];
		max_date = res_statistic_instream[0]["date"];
	} else {
		min_date = res_statistic_conductivity[0]["date"];
		max_date = res_statistic_conductivity[0]["date"];
	}
	findMinMaxDate(min_date, max_date, res_statistic_instream);
	findMinMaxDate(min_date, max_date, res_statistic_conductivity);

	// wenn Zeitraum so anfangen und enden soll wie Messwerte gibt, min_month, max_month so berechnen
	min_year = min_date/100; min_month=min_date%100;
	max_year = max_date/100; max_month=max_date%100;
	// Jahr immer ab Januar bis Dezember
	min_month = 1; max_month = 12;

	statisticStationsClass statistic_line;
	statistic_line.setValues();

	for (int year = min_year; year<=max_year; year++) {
		beg_mon=1; end_mon=12;
		if (year==min_year) beg_mon=min_month;
		else if (year==max_year) end_mon=max_month;

		for (int month=beg_mon; month<=end_mon; month++) {
			j=sprintf(sql, "SELECT c.`date`, c.`con`/1000 as `con1_mg_l`, c.`con2`/1000  as `con2_mg_l` ");
			j+=sprintf(sql+j, "FROM `%swq_out_%s`.`concentration_%d` c ", MyDatabase, options_wqstat_ptr->continent_abb, IDrun);
			j+=sprintf(sql+j, "WHERE c.`date`=%d ", year*100+month);
			j+=sprintf(sql+j, "AND c.cell=%d;", Cell_value);

			query.reset();
			query << sql;
			//cout<<"query statistic_stations: "<<query<<endl;
			mysqlpp::StoreQueryResult res_statistic_stations = query.store();

			for (unsigned int rowNum=0; rowNum<res_statistic_stations.size(); rowNum++) {
				statistic_line.cell = Cell_value;
				statistic_line.date = year*100+month;
				statistic_line.con = res_statistic_stations[rowNum]["con1_mg_l"];
				statistic_line.con2 = res_statistic_stations[rowNum]["con2_mg_l"];
				statistic_line.measured = NODATA;
				statistic_line.flag_uncert = NODATA;
				statistic_line.measured_conductivity = NODATA;
				statistic_line.flag_uncert_conductivity = NODATA;

				for(unsigned int num=0; num<res_statistic_instream.size(); num++) {
					int date = res_statistic_instream[num]["date"];
					if (date==year*100+month) {
						statistic_line.measured = res_statistic_instream[num]["measured"];
						statistic_line.flag_uncert = res_statistic_instream[num]["flag_uncert"];
					}
				}
				for(unsigned int num=0; num<res_statistic_conductivity.size(); num++) {
					int date = res_statistic_conductivity[num]["date"];
					if (date==year*100+month) {
						statistic_line.measured_conductivity = res_statistic_conductivity[num]["measured"];
						statistic_line.flag_uncert_conductivity = res_statistic_conductivity[num]["flag_uncert"];
					}
				}
				statistic.push_back(statistic_line);
			}
			
		} // for(month)
	} // for(year)

	//cout << "statistic.size: " << statistic.size() << endl;
	//cout << "##################################\n";
} // end of get_Output()

void findMinMaxDate(int &min_date, int &max_date, mysqlpp::StoreQueryResult res_statistic) {
	for(unsigned int num=1; num<res_statistic.size(); num++) {
		int date = res_statistic[num]["date"];
		if (date < min_date) {
			min_date = date;
		}

		if (date > max_date) {
			max_date = date;
		}
	}
}
