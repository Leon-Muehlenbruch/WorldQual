/*
 * pop_dens_input.cpp
 *
 *  Created on: 23.01.2013
 *      Author: reder
 */

#include "pop_dens_input.h"
#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>  //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "../general_function/country.h"
#include "../general_function/timestring.h"
#include "../general_function/general_functions.h"
#include "../options/options_scen.h"

//#define BYTESWAP_NECESSARY

//#define MY_DATABASE ""
//#define MY_HOST "MySQL"
//#define MY_USER "worldqual"
//#define MY_PASSWORD "1qay"
//#define INPUT_DIR "INPUT"

//#define DEBUG
#define DEBUG_queries
//#undef DEBUG_queries



//int continent[5];  // 0 = ng; 1 - ng_land; 2 - nrows; 3 - ncols
//char hydro_input[maxcharlength];              // in watergap_unf.watergap_region(hydro_input) Pfad zu waterGAP-Inputverzeichniss
char  MyDatabase[maxcharlength];
//char continent_abb[10];			// Abbreviation of the continent name; used for detection of corresponding database


mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::Query      query_ins = con.query(); // fuer INSERT into 'statistik'
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;
mysqlpp::SimpleResult resExec; // mysqlpp::ResNSel    resExec;


int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram pop_dens_input started at: " << getTimeString() << endl << endl;
	options_scenClass options_scen(argc, &argv[0], "startYear endYear");

	if (argc<3) {
		options_scen.Help();
//    cout << "\n./rtf_input startYear endYear  [-ooptions_file]\n"; //IDScen [-h hostname] [-d database] [-u user] [-p password] [-v IDVersion] [-r IDReg] [-i path]"
//         << "\nif one (or more) parameters is missing the following default values are used:  "
//         << "\n\nDATABASE      \"empty string\""
//         << "\nHOST          \""<< MY_HOST<<"\""
//         << "\nUSER          \""<< MY_USER<<"\""
//         << "\nPASSWORD      \"*******\"  "
//         << "\nIDVersion     3 (WaterGAP3)"
//         << "\nIDReg         1 (Europe)"
//         << "\nNoData        "<< NODATA<<"\n"
//         << "\npath          \""<< INPUT_DIR<<"\"\n\n";
		return 0;
	}

	char filename[maxcharlength];

	if(!IsNumber(argv[1]) || !IsNumber(argv[2])) {
		cerr << "ERROR: Wrong parameter" << endl;
		options_scen.Help();
//		cerr << "usage: ./rtf_input startYear endYear [-ooptions_file]"<<endl;
		exit(1);
	}
	int start     = atoi(argv[1]);
	int end       = atoi(argv[2]);
//	int IDScen    = atoi(argv[3]);

	if (start>end) {
		cerr << "ERROR: Wrong parameter. start year> end year!!!" << endl;
		options_scen.Help();
//		cerr << "usage: ./rtf_input startYear endYear  [-ooptions_file]"<<endl;
		exit(1);
	}

	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 3 = 1 + 2 = programmname + 3 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(options_scen.init(argc-3, &argv[3])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input


	try {
		cout << "start:" << endl;

		char start_db[maxcharlength]; //sprintf(start_db, "%swq_load_%s", MyDatabase, continent_abb);
		sprintf(start_db, "wq_general");

		if (!con.connect(&start_db[0], options_scen.MyHost, options_scen.MyUser, options_scen.MyPassword,3306))
		{
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl << endl; //return 0;
		////////////////////////////////

		cout<<"ATTENTION: if pop_dens_input is used just for one continent:\n"
			<<"For countries, which are part of more than one continent, the population density will only be calculated for the used  \n"
			<<"continent. This will lead to an overestimation in wq_load.cpp where the proportion of each cell according to \n"
			<<"the population of each country is calculated. This problem can only be solved if data for all continents,  \n"
			<<"where the country is part of, is supplied.\n"
			<<"If the pop_dens is calculated for all continents, than the correct value will only be inserted into \n"
			<<"wq_load.country_input with the run for the last continent! (for further reference see comments in the code)\n"<<endl;

		// checking project_id and project- and database name, fill MyDatabase
		if (getProject(options_scen.project_id, &options_scen, con, "wq_load")) exit(1);

		//=========================================
		//get country_ids of countries which are part of more than one continent
		// class containing information about the countries
				COUNTRY_CLASS countries(options_scen.IDScen, start);

				int (*count_cont)[4] = new int[countries.number][4];	//0 - country_id; 1 - first continent nr; 2 - second continent nr; 3 - thrid continent number;
				cout<<"countries.number: "<<countries.number<<endl;

				//delfault setting
				for(int i=0;i<countries.number;i++)
					for(int j=0;j<4;j++)
						count_cont[i][j]=NODATA;

				query.reset();
				//select only countries which are in more than one continent
				query<<"SELECT c.`isonum` FROM watergap_unf.country_continent c "
					 <<"GROUP BY c.`isonum` having count(*)>1;";
				res = query.store();

				if (res.empty()) {cerr << "Error in table watergap_unf.country_continent.\n"; return 1;}

				for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){
					if((int)rowNum>countries.number){cerr<<"Error: row number can not be larger than number of countries!"<<endl;}
						count_cont[rowNum][0]  = res[rowNum]["isonum"];
				}

				//select the continents the countries are part of
				for(int i=0;i<countries.number;i++)
				{
					if(count_cont[i][0]!=NODATA){
						query.reset();
						query<<"SELECT c.`idreg` FROM watergap_unf.country_continent c "
								<<"WHERE c.`isonum`="<<count_cont[i][0]<<";";

						res = query.store();

						if (res.empty()) {cerr << "Error in table watergap_unf.country_continent for idreg.\n"; return 1;}

						for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
							count_cont[i][rowNum+1]  = res[rowNum]["idreg"];
					}

				}


				for(int i=0;i<countries.number;i++)
					for(int j=0;j<4;j++)
						cout<<"count_cont["<<i<<"]["<<j<<"]: "<<count_cont[i][j]<<endl;



		///=================================
		// Parameter Landmaske einlesen
		if(getCELLS(&options_scen, &continent)) exit(1); cout << "\nParameter Landmaske wurden eingelesen\n"<<endl;

		// bei UNF-Dateien wird Umwandlung gcrc -> ArcID benoetigt
		int *gcrc;  // gcrc[gcrc-1]=ArcID
		int *grow;  // grow[gcrc-1]=row
		gcrc = new int[continent.ng];
		grow = new int[continent.ng];
		if(getGCRC(gcrc, grow, &options_scen, &continent)) {cerr << "error: getGCRC\n"; exit(1);}


		double (*pop_urb) = new double[continent.ng];
		if (!pop_urb) {cerr << "not enough memory!\n"; exit(1);}
		double (*pop_rur) = new double[continent.ng];
		if (!pop_rur) {cerr << "not enough memory!\n"; exit(1);}
		double (*pop_tot) = new double[continent.ng];
		if (!pop_tot) {cerr << "not enough memory!\n"; exit(1);}


		for (int actual_year=start; actual_year<=end; actual_year++) {
			cout << actual_year<<endl;

			// read UNF files
			sprintf(filename,"%s/GURBPOP%d.UNF0", options_scen.input_dir, actual_year);
			if (readInputFile(filename, 4, 1, continent.ng, &pop_urb[0], gcrc, &options_scen, &continent)) exit(1);
			cout << "year " << actual_year << " urban population done\n";

			sprintf(filename,"%s/GRURPOP%d.UNF0", options_scen.input_dir, actual_year);
			if (readInputFile(filename, 4, 1, continent.ng, &pop_rur[0], gcrc, &options_scen, &continent)) exit(1);
			cout << "year " << actual_year << " rural population done\n";

			for (int cell=0; cell<continent.ng; cell++) {
				query.reset();
				query <<"UPDATE "<<MyDatabase<<"wq_load_"<<options_scen.continent_abb<<".`cell_input` c "
					  <<"SET c.`pop_urb`="<<pop_urb[cell]
				      <<" , c.`pop_rur`="<<pop_rur[cell]
				      <<" , c.`pop_tot`="<<pop_urb[cell]+pop_rur[cell]
				      <<" WHERE c.`IDScen`="<<options_scen.IDScen<<" AND c.`time`="<<actual_year<<" AND c.`cell`="<<cell+1<<";";

				if(cell==51515||cell==175351||cell==140751){
					cout<<"update cell_input: "<<query<<endl;
					cout<<"\npop_urb[cell]: "<<pop_urb[cell]<<" pop_rur[cell]: "<<pop_rur[cell]<<" cell: "<<cell<<endl<<endl;
				}
				resExec = query.execute();

			}//end cell

			//update wq_load.country_input; at this point the value for countries which are part
			//of more than one continent is not jet correct
			query.reset();
			query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
				  <<"JOIN (SELECT sum(c1.`pop_urb`) as 'pop_urb', sum(c1.`pop_rur`) as 'pop_rur', "
				  <<"sum(c1.`pop_tot`) as 'pop_tot', c1.`country_id`, c1.`IDScen`,  c1.`time` "
				  <<"FROM "<<MyDatabase<<"wq_load_"<<options_scen.continent_abb<<".cell_input c1 "
				  <<"WHERE c1.`IDScen`="<<options_scen.IDScen<<" AND c1.`time`="<<actual_year
				  <<" GROUP BY c1.`country_id`) as sub "
				  <<"ON c.`country_id`=sub.`country_id` "
				  <<"SET c.`pop_urb`= sub.`pop_urb`, c.`pop_rur`= sub.`pop_rur`, c.`pop_tot`= sub.`pop_tot` "
				  <<"WHERE c.`IDScen`="<<options_scen.IDScen<<" AND c.`time`="<<actual_year<<";";


			cout<<"update country_input: "<<query<<endl;
			resExec = query.execute();

//			if(options_scen.IDVersion==3 && strcmp(MyDatabase,"sen_anlys_")!=0){
//				//to set the countries which are in more than one continent
//				setCountryInput(countries.number, count_cont, "pop_urb", "pop_rur", "pop_tot",  options_scen.IDScen,  actual_year);
//			}


		}//end actual_year








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
		cout <<"\nProgram pop_dens_input endet at: " << getTimeString() << endl << endl;
		return 0;
}
