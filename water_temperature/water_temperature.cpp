// 10-12-2008
// Ellen Teichert
// Wassertemperetur rechnen
// y= C0/(1+exp(C1*t+C2))
// 

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>  //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "../general_function/timestring.h"
#include "../general_function/general_functions.h"
#include "options_waterTemp.h"

//#define BYTESWAP_NECESSARY

//#define DEBUG
#define DEBUG_queries
//#undef DEBUG_queries

char  MyDatabase[maxcharlength];

double WaterTemp(short int t, double* ConstTemp);

mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::Query      query_ins = con.query(); // fuer INSERT into 'statistik'
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;
mysqlpp::SimpleResult resExec; // mysqlpp::ResNSel    resExec;


int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram world started at: " << getTimeString() << endl << endl;
	options_waterTempClass options_waterTemp(argc, &argv[0], "startYear endYear");

	if (argc<3) {
		options_waterTemp.Help();
//		cout << "\n./water_temperature startYear endYear [-ooptions_file]\n\n";
//    cout << "\n./water_temperature startYear endYear IDTemp [-h hostname] [-d database] [-u user] [-p password] [-v IDVersion] [-r IDReg] [-i path] [-n nodata] [-c climate] [-s IDScen]"
//         << "\nif one (or more) parameters is missing the following default values are used: "
//         << "\n\nDATABASE      \"empty string\""
//         << "\nHOST          \""<< MY_HOST<<"\""
//         << "\nUSER          \""<< MY_USER<<"\""
//         << "\nPASSWORD      \"*******\"  "
//         << "\nIDVersion     3 (WaterGAP3)"
//         << "\nIDReg         1 (Europa)"
//         << "\nNoData        "<< NODATA<<"\n"
//         << "\npath          \""<< INPUT_DIR<<"\"\n"
//         << "\npath          path to read in UNF-files.\n"
//         << "\n-c climate input on 0.5�=> 0\n\n";
         return 0;
	}

	char filename[maxcharlength];

	if(!IsNumber(argv[1]) || !IsNumber(argv[2]) ) {
		cerr << "ERROR: Wrong parameter" << endl;
		options_waterTemp.Help();
//		cerr << "usage: ./water_temperature startYear endYear [-ooptions_file]"<<endl;
		exit(1);
	}
	int start     = atoi(argv[1]);
	int end       = atoi(argv[2]);

	if (start>end) {
		cerr << "ERROR: Wrong parameter. start year> end year!!!" << endl;
		options_waterTemp.Help();
//		cerr << "usage: ./water_temperature startYear endYear [-ooptions_file]"<<endl;
//		cerr << "usage: ./rtf_input startYear endYear IDTemp [-h hostname] [-d database] [-u user] [-p password] [-v IDVersion] [-r IDReg] [-i path] [-n nodata]"<<endl;
		exit(1);
	}

	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 3 = 1 + 2 = programmname + 2 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(options_waterTemp.init(argc-3, &argv[3])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input

	try {
		cout << "start:" << endl;
		char start_db[maxcharlength]; sprintf(start_db, "wq_general");


		if (!con.connect(&start_db[0], options_waterTemp.MyHost, options_waterTemp.MyUser, options_waterTemp.MyPassword,3306)) {
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl << endl; //return 0;
		////////////////////////////////

		// checking project_id and project- and database name, fill MyDatabase
		if (getProject(options_waterTemp.project_id, &options_waterTemp, con, "worldqual")) exit(1);

		char  climateTemp[maxcharlength];
		double ConstTemp[3];  // Konstanten f�r die Temperaturkurve y= C0/(1+exp(C1*t+C2))
		//checking IDTemp und f�llen ConstTemp
		if (checkIDTemp(options_waterTemp.IDTemp, ConstTemp, climateTemp)) exit(1);

		///=================================
		// Parameter Landmaske einlesen
		if(getCELLS(&options_waterTemp, &continent)) exit(1); cout << "\nParameter Landmaske wurden eingelesen\n"<<endl;
		
		// bei UNF-Dateien wird Umwandlung gcrc -> ArcID benoetigt
		int *gcrc;  // gcrc[gcrc-1]=ArcID
		int *grow;  // grow[gcrc-1]=row
		gcrc = new int[continent.ng];
		grow = new int[continent.ng];
		if(getGCRC(gcrc, grow, &options_waterTemp, &continent)) {cerr << "error: getGCRC\n"; exit(1);}   // read gcrc
	
		short int (*air_temp)[12] = new short int[continent.ng][12];  // [km3]
		if (!air_temp) {cerr << "not enough memory!\n"; exit(1);}
		
		int dataTypeSize, valuesPerCell;
		for (int actual_year=start; actual_year<=end; actual_year++) {
			cout << actual_year<<endl;
			dataTypeSize = 2;
			valuesPerCell = 12;

			// Daten aus UNF-Dateien einlesen

			if (options_waterTemp.climate==0) {

				int *G_wg3_to_wg2 = new int[continent.ng];
				sprintf(filename,"%s/G_WG3_WG2WITH5MIN.UNF4",options_waterTemp.input_dir);
				if (readInputFile(filename, 4, 1, continent.ng, &G_wg3_to_wg2[0], gcrc, &options_waterTemp, &continent)) exit(1);

				sprintf(filename, "%s/GTEMP_%d.12.UNF2", options_waterTemp.input_dir, actual_year);
				if (readInputFileWG2toWG3( filename, 2, 12, continent.ng, air_temp, gcrc, G_wg3_to_wg2)) exit(1);

				delete[] G_wg3_to_wg2; G_wg3_to_wg2 = NULL;

				//cout<<"air_temp[144598][0]: "<<air_temp[144598][0]<<endl;

			}else if(options_waterTemp.climate==1){

				sprintf(filename,"%s/GTEMP_%d.12.UNF2", options_waterTemp.input_dir, actual_year);
				if (readInputFile(filename, dataTypeSize, valuesPerCell, continent.ng, &air_temp[0][0], gcrc, &options_waterTemp, &continent, 0.)) return 1;
				cout << "year " << actual_year << " air temperatur read\n";
			}


			if(options_waterTemp.IDScen==NODATA){
				// wenn in der Tabelle schon Daten fuer das Jahr gab, loeschen
				query.reset();
//				query << "DELETE FROM "<<MyDatabase<<"worldqual_"<<options_waterTemp.continent_abb<<".`water_temperature` WHERE `IDTemp`="
				query << "DELETE FROM wq_climate."<<climateTemp<<"_water_temperature_"<<options_waterTemp.continent_abb
					<<" WHERE `IDTemp`=" << options_waterTemp.IDTemp << " AND `year`= " << actual_year << ";";
				cout<<"delete from water_temperature: "<<query<<endl;
				resExec = query.execute();
				cout << resExec.rows()<<endl;
				//cout << resExec.success<<endl;
				cout << resExec.insert_id()<<endl;
				cout << resExec.info()<<endl;
			}else{
				// wenn in der Tabelle schon Daten fuer das Jahr gab, loeschen
				query.reset();
//				query << "DELETE FROM "<<MyDatabase<<"worldqual_"<<options_waterTemp.continent_abb<<".`water_temperature` WHERE `IDTemp`="<<options_waterTemp.IDTemp
				query << "DELETE FROM wq_climate."<<climateTemp<<"_water_temperature_"<<options_waterTemp.continent_abb
					<< "WHERE `IDTemp`=" << options_waterTemp.IDTemp
					<<" AND `year`= " << actual_year<<" AND `IDScen`= " <<options_waterTemp.IDScen<< ";";
				cout<<"delete from water_temperature for IDScen: "<<query<<endl;
				resExec = query.execute();
				cout << resExec.rows()<<endl;
				//cout << resExec.success<<endl;
				cout << resExec.insert_id()<<endl;
				cout << resExec.info()<<endl;
			}




			// Daten in die Tabelle einfuegen
			for (int cell=0; cell<continent.ng; cell++) {
				query.reset();
				query << setprecision(30);
				if(options_waterTemp.IDScen==NODATA)
//					query << "insert into "<<MyDatabase<<"worldqual_"<<options_waterTemp.continent_abb<<".`water_temperature` (`IDTemp`, `year`, `cell`, "
					query << "insert into wq_climate."<< climateTemp <<"_water_temperature_"<<options_waterTemp.continent_abb<<" (`IDTemp`, `year`, `cell`, "
						  << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) "
						  << "values ( " <<options_waterTemp.IDTemp<< ", "<< actual_year << ", " << cell+1<<", ";
				else
//					query << "insert into "<<MyDatabase<<"worldqual_"<<options_waterTemp.continent_abb<<".`water_temperature` (`IDTemp`, `IDScen`, `year`, `cell`, "
					query << "insert into wq_climate."<< climateTemp <<"_water_temperature_"<<options_waterTemp.continent_abb<<" (`IDTemp`, `IDScen`, `year`, `cell`, "
					<< "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) "
					<< "values ( " <<options_waterTemp.IDTemp<< ", "<<options_waterTemp.IDScen<< ", "<< actual_year << ", " << cell+1<<", ";
				
				for (int month=0; month<12; month++) {
					if (air_temp[cell][month]==NODATA) query <<"NULL";
					else query <<WaterTemp(air_temp[cell][month], ConstTemp);
					
					if (cell==0) cout <<cell<<'\t'<<month<<" air:\t"<<air_temp[cell][month]<<"\twater:\t"<<WaterTemp(air_temp[cell][month], ConstTemp)<<endl;
					
					if(month<11) query << ", ";
					else query<<");";
				} //for(month)

				resExec = query.execute();
				
			} //for(cell)
			
			
		} //for(actual_year)




	
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
}// main()


