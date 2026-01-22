// 14-05-2008
// Ellen Teichert
//
// last change 22.05.2012 - Klara Reder
//

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "timestring.h"
#include "country.h"
#include "options_wq_load.h"
#include "general_functions.h"
#include "ClassWQload.h"


using namespace std;
using namespace mysqlpp;

//int continent[5];  // 0 = ng; 1 - ng_land; 2 - nrows; 3 - ncols

char hydro_input[maxcharlength];           	 // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Input-directory
char  MyDatabase[maxcharlength];

mysqlpp::Connection con(use_exceptions);
//mysqlpp::Query      query = con.query();
//mysqlpp::Query      query_ins = con.query(); // for INSERT into 'statistik'
//mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
//mysqlpp::Row        row;

//#define DEBUG
#define DEBUG_queries
//#undef DEBUG_queries

int main(int argc, char *argv[])
{	//routine of reading variables
	// print current date
	cout <<"\nProgram world started at: " << getTimeString() << endl << endl;

	options_wq_loadClass optionsWQ_load(argc, &argv[0], "year");
	if (argc<2) {
		optionsWQ_load.Help();
//    cout << "\n./wq_load year [-ooptions_file]\n";
//   parameterID project_id IDScen [-h hostname] [-u user] [-p password] [-v IDVersion] [-r IDReg] [-c climate] [-m manureTiming]"
//    	 << " [-r IDReg] [-i path] "
//         << "\nif one (or more) parameters is missing the following default values are used: "
//         << "\n\nDATABASE      \"empty string\""
//         << "\nHOST          \""<<MY_HOST<<"\""
//         << "\nUSER          \"worldqual\""
//         << "\nPASSWORD      \"*******\"  "
//         << "\nIDVersion     3 (WaterGAP3)"
//         << "\nIDReg         1 (Europa)"
//         << "\npath          "<<INPUT_DIR
//         << "\ntype          0 - BOD; 1 - salt; 2 - Fecal coliforms (for export coefficient total coliforms); 3 - TN; 4 - TP"<<endl
//         //<< "\n-s            salt is calculated (total=dom+man+irr)\n"
//         << "\npath          path for cell_runoff_total, urban_runoff_cell from UNF files."
//    	 << "\n-c climate input on 0.5�=> 0\n"
//    	 << "\n-m manure timing default 0 - all year round application; 1 - application between March and October\n\n";
		return 0;
	}

	if(!IsNumber(argv[1])) {
		cerr << "ERROR: Wrong parameter" << endl;
//		cerr << "usage: ./wq_load year"<<endl;
		optionsWQ_load.Help();
		// parameterID project_id IDScen [-h hostname] [-u user] [-p password] [-v IDVersion] [-r IDReg] [-c climate] [-m manureTiming]"
		//    	 << " [-r IDReg] [-i path] "<<endl;
		exit(1);
	}
	int year         = atoi(argv[1]);
//	int parameter_id = atoi(argv[2]);
//	int project_id   = atoi(argv[3]);
//	int IDScen       = atoi(argv[4]);

	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 2 = 1 + 1 = programmname + 1 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(optionsWQ_load.init(argc-2, &argv[2])) exit(1);

	try {
		cout << "start:" << endl;
		char start_db[maxcharlength]; //sprintf(start_db, "%swq_load_%s", MyDatabase, optionsWQ_load.continent_abb);
		sprintf(start_db, "wq_general");

		if (!con.connect(&start_db[0], optionsWQ_load.MyHost, optionsWQ_load.MyUser, optionsWQ_load.MyPassword,3306)) {
			cout << "database connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl<<endl; //return 0;
		////////////////////////////////

		ClassWQ_load wq_load(&optionsWQ_load, year);

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
	cout <<"\nProgram worldqual ended at: " << getTimeString() << endl << endl;
	return 0;
} // end of main()



