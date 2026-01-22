// wq_instream_daily Abfrage wq_calib.wq_instream_daily
// volle Jahre
// für jede Station eine Spalte
// 16-01-2008 Ellen Teichert


#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>  //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "../general_function/timestring.h"

#define MY_DATABASE "wq_calib"
#define MY_HOST "MySQL"
#define MY_USER "xxx"
#define MY_PASSWORD ""

#define DEBUG
//#define DEBUG_queries
//#undef DEBUG_queries

using namespace std;
using namespace mysqlpp;

const short maxcharlength    = 255;   // maximum length of strings


sql_create_2(wq_tab, 1, 0, 
			 sql_int_unsigned, date,
			 sql_double,       value);

mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;

int nextDay(int &day, int &month, int &year);

int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram world started at: " << getTimeString() << endl << endl;
	
	char  MyDatabase[maxcharlength]; sprintf(MyDatabase,"%s",MY_DATABASE);
	char  MyHost[maxcharlength];     sprintf(MyHost,"%s",MY_HOST);
	char  MyUser[maxcharlength];     sprintf(MyUser,"%s",MY_USER);
	char  MyPassword[maxcharlength]; sprintf(MyPassword,"%s",MY_PASSWORD);
	
	if (argc<4) {
		cerr << "ERROR: Wrong parameter" << endl;
		cerr << "usage: ./wq_instream_daily "<<endl;
		cerr << "   or: ./wq_instream_daily IDParam start_year end_year [-h hostname] [-d database] [-u user] [-p password]" << endl;
		exit(1);
	}
	int j;
	int IDParam   = atoi(argv[1]);
	int start     = atoi(argv[2]);
	int end       = atoi(argv[3]);
	
	for (int num=4; num<argc; num++) {
		if (!strcmp(argv[num],"-h"))       sprintf(MyHost,     "%s", argv[++num]); // hostname
		else if (!strcmp(argv[num],"-d"))  sprintf(MyDatabase, "%s", argv[++num]); // database
		else if (!strcmp(argv[num],"-u"))  sprintf(MyUser,     "%s", argv[++num]); // user
		else if (!strcmp(argv[num],"-p"))  sprintf(MyPassword, "%s", argv[++num]); // password
		else IDParam = atoi(argv[num]);
	} // for(i)
	
	cout << "MyDatabase: " << MyDatabase << endl; 
	cout << "MyHost:     " << MyHost     << endl;     
	cout << "MyUser:     " << MyUser     << endl;     
	cout << "MyPassword: " << MyPassword << endl;
	cout << "IDParam:    " << IDParam    << endl;
	cout << "years:      " << start << " - " << end << endl;
	
	try {  
		char sql[2000]; // variable for SQL-query

		cout << "start:" << endl;
        
		if (!con.connect(MyDatabase, MyHost, MyUser, MyPassword,3306)) {
			cout << "database " << MyDatabase << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << MyDatabase << " connection ok..." << endl; //return 0;
		////////////////////////////////
		
		int Days=(end-start+1)*366;
		//================================================================================
		int nStations = 3; 
		int *StationID = new int[nStations];
		StationID[0] = 43;
		StationID[1] = 44;
		StationID[2] = 45;
		//================================================================================
		
		double** result;
		result = new double*[Days];
		for (int Nday=0; Nday<Days; Nday++) result[Nday] =  new double[nStations];
		
		for (int Nday=0; Nday<Days; Nday++) 
		  for (int station=0; station<nStations; station++) result[Nday][station] = 0.0;
		
		
		set <wq_tab> resWq_tab;
		set <wq_tab>::iterator itWq_tab;
		
		for (int station=0; station<nStations; station++) {
		
			j=sprintf(sql,"SELECT DATEDIFF(w.date, %d) as date, w.Value FROM wq_instream_daily w ", start*10000+101);
			j+=sprintf(sql+j, " WHERE w.parameter_id =%d AND w.station_id =%d ", IDParam, StationID[station]);
			j+=sprintf(sql+j, " AND w.date>%d AND w.date<%d; ", start*10000, (end+1)*10000);
            
			query.reset();
			query << sql;
			resWq_tab.clear();
			query.storein(resWq_tab);
            
			for (itWq_tab=resWq_tab.begin(); itWq_tab!=resWq_tab.end(); itWq_tab++) 
			{
				result[itWq_tab->date][station]=itWq_tab->value;
			}
			
		} // end of for(station)
		
		// OUTPUT
		char filename[maxcharlength];           // 
		// open output file 
		sprintf(filename,"wq_instream_%d_%d_%d.txt",IDParam, start, end);
		ofstream outputfile (filename);
		if (!outputfile) {
			cerr << "\nCould not open " << filename << " for output" << endl;
			exit (0);
		}
		cout << "output_file name: " <<filename<< endl;
		
		outputfile << "day \t month \t year \t";
		for (int station=0; station<nStations; station++) outputfile << StationID[station] << "\t";
		outputfile << endl;
		
		int day=1; int month=1; int year=start;
		for (int Nday=0; Nday<Days; Nday++) {
			outputfile << day << "\t" << month << "\t" << year << "\t";
			for (int station=0; station<nStations; station++) {
				outputfile << result[Nday][station] << "\t";
			}
			outputfile << endl;
			nextDay(day, month, year);
			if (year>end) break;
		}
		
	
	
	}// end try
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
		




	

	// print current date
	cout <<"\nProgram worldqual endet at: " << getTimeString() << endl << endl;
	return 0;
} // end of main()


int nextDay(int &day, int &month, int &year) {
	const short nDaysPerMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };	
	if (day<nDaysPerMonth[month-1]) { day++; return 0;}
	if (month<12) { day=1; month++; return 0;}
	day=1; month=1; year++; return 0;
} // end of nextDay()
