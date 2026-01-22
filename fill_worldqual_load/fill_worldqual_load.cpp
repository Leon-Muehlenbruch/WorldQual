/*
 * fill_worldqual_load.cpp
 *
 *  Created on: 09.05.2014
 *      Author: reder
 */

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>  //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include <math.h>
#include <list>
#include <iostream>
#include "country.h"
#include "timestring.h"
#include "general_functions.h"
#include "options_fill_wq_load.h"
//#include "def.h"

#define DEBUG
//#define DEBUG_queries
//#undef DEBUG_queries


char  hydro_input[maxcharlength];              // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Inputpath
char  MyDatabase[maxcharlength];


mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::StoreQueryResult     res;
//mysqlpp::UseQueryResult     Use_res;
mysqlpp::Row        row;


sql_create_1(Liste_X, 1, 0,
			 sql_int_unsigned, cell);
sql_create_2(Routing_upd, 1, 0,
			 sql_int_unsigned, cell,
			 sql_int_unsigned, counter);


int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram fill_worldqual_load started at: " << getTimeString() << endl << endl;
	options_fill_wq_loadClass optionsFill_WQ_load(argc, &argv[0], "yearStart yearEnd");

	if (argc<3) {
		optionsFill_WQ_load.Help();
		return 0;
	}

	cout << "_options einlesen"<<endl;

	///=================================
	// _options einlesen
	// 3 = 1 + 2 = programmname + 2 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if (optionsFill_WQ_load.init(argc-3, &argv[3])) exit(1);;

	int toCalc; // 0 - BOD, 1 - salt, 2 - FC (Fecal coliforms), 3 - TN, 4 - TP

	if(!IsNumber(argv[1]) || !IsNumber(argv[2])) {
		cerr << "ERROR: Wrong parameter" << endl;
		cerr << "usage: ./fill_worldqual_load yearStart yearEnd "<<endl;
		exit(1);
	}


	int yearStart     = atoi(argv[1]);
	int yearEnd       = atoi(argv[2]);

	if (yearStart>yearEnd) {
		cerr << "ERROR: Wrong parameter. yearStart>yearEnd!!!" << endl;
		cerr << "usage: ./fill_worldqual_load yearStart yearEnd "<<endl;
		exit(1);
	}


	try {
		char sql[10000]; // variable for SQL-query
		char sql_sel[10000]; // variable for SQL-query

		cout << "start:" << endl;
        char start_db[maxcharlength]; //sprintf(start_db, "%sworldqual_%s", MyDatabase, continent_abb);
		sprintf(start_db, "wq_general");

		if (!con.connect(&start_db[0], &optionsFill_WQ_load.MyHost[0], &optionsFill_WQ_load.MyUser[0], &optionsFill_WQ_load.MyPassword[0],3306)) {
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok...\n" << endl; //return 0;
		////////////////////////////////
		mysqlpp::String ParameterName;
		// checking parameter_id and parameter name
		if (checkParameter(optionsFill_WQ_load.parameter_id, toCalc, ParameterName)) exit(1);

		const int loadTypes = 11;
		bool loadType[loadTypes];  // true - Tabelle wird f�r load gebraucht; false - wird nicht gebraucht
		char loadTab[loadTypes][maxcharlength]; // Tabellenname: *wq_load_*.calc_cell_month_load_[loadTab[i]]
		char loadTabAlias[loadTypes][loadTypes];       // Alias der Tabelle in SQL-Anweisung
		char loadTabLongName[loadTypes][maxcharlength]; // Beschreibung, wird nur zur Information ausgegeben

		strcpy(loadTab[0], "dom");        strcpy(loadTabAlias[0], "cd");      strcpy(loadTabLongName[0], "domestic");
		strcpy(loadTab[1], "urb");        strcpy(loadTabAlias[1], "cu");      strcpy(loadTabLongName[1], "urban surface runoff");
		strcpy(loadTab[2], "man");        strcpy(loadTabAlias[2], "cm");      strcpy(loadTabLongName[2], "manufacturing");
		strcpy(loadTab[3], "sc");         strcpy(loadTabAlias[3], "cs");      strcpy(loadTabLongName[3], "scattered settlements");
		strcpy(loadTab[4], "open_defec"); strcpy(loadTabAlias[4], "cdsc");    strcpy(loadTabLongName[4], "open defecation");
		strcpy(loadTab[5], "hanging_l");  strcpy(loadTabAlias[5], "hl");      strcpy(loadTabLongName[5], "hanging latrines");
		strcpy(loadTab[6], "manure");     strcpy(loadTabAlias[6], "cmanure"); strcpy(loadTabLongName[6], "manure application");
		strcpy(loadTab[7], "irr");        strcpy(loadTabAlias[7], "cirr");    strcpy(loadTabLongName[7], "irrigation");
		strcpy(loadTab[8], "geog_back");  strcpy(loadTabAlias[8], "cg");      strcpy(loadTabLongName[8], "geogen background");
		strcpy(loadTab[9], "ind_fert");   strcpy(loadTabAlias[9], "cif");     strcpy(loadTabLongName[9], "inorganic fertilizer");
		strcpy(loadTab[10], "mining");    strcpy(loadTabAlias[10],"cmining"); strcpy(loadTabLongName[9], "mining");

		if(toCalc==0||toCalc==2) { //fo FC and BOD
			loadType[0] = 1; // dom
			loadType[1] = 1; // urb
			loadType[2] = 1; // man
			loadType[3] = 1; // sc
			loadType[4] = 1; // open_defec
			loadType[5] = 1; // hanging_l
			loadType[6] = 1; // manure
			loadType[7] = 0; // irr
			loadType[8] = 0; // geog_back
			loadType[9] = 0; // ind_fert
			loadType[10]= 0; // mining
		} else if(toCalc==1) { //for TDS
			loadType[0] = 1; // dom
			loadType[1] = 1; // urb
			loadType[2] = 1; // man
			loadType[3] = 1; // sc
			loadType[4] = 1; // open_defec
			loadType[5] = 1; // hanging_l
			loadType[6] = 1; // manure
			loadType[7] = 1; // irr
			loadType[8] = 1; // geog_back
			loadType[9] = 0; // ind_fert
			loadType[10]= 1; // mining
		} else if(toCalc==3||toCalc==4) { //for TN an TP
			loadType[0] = 1; // dom
			loadType[1] = 1; // urb
			loadType[2] = 1; // man
			loadType[3] = 1; // sc
			loadType[4] = 1; // open_defec
			loadType[5] = 1; // hanging_l
			loadType[6] = 1; // manure
			loadType[7] = 0; // irr
			loadType[8] = 1; // geog_back
			loadType[9] = 1; // ind_fert
			loadType[10]= 0; // mining
		} else if(toCalc==5) { //for pesticide
			loadType[0] = 0; // dom
			loadType[1] = 0; // urb
			loadType[2] = 0; // man
			loadType[3] = 0; // sc
			loadType[4] = 0; // open_defec
			loadType[5] = 0; // hanging_l
			loadType[6] = 0; // manure
			loadType[7] = 1; // irr
			loadType[8] = 0; // geog_back
			loadType[9] = 0; // ind_fert
			loadType[10]= 0; // mining
		}
		//cout of sectors which are calculated
		cout<<"\nSectors added in load are:\n";
		for (int type=0; type<loadTypes; type++) {
			if (loadType[type]) cout << loadTabLongName[type] << endl;
		}
		cout << endl;

		int j;
		int year;

		// checking project_id and project- and database name, fill MyDatabase
		if (getProject(optionsFill_WQ_load.project_id, &optionsFill_WQ_load, con, "worldqual")) exit(1);

		for(int IDScen=optionsFill_WQ_load.IDScenBeg; IDScen<=optionsFill_WQ_load.IDScenEnd; IDScen++){
			
			char LoadTableName[maxcharlength];
			sprintf(LoadTableName, "%sworldqual_%s.`load_%d_%d`", MyDatabase, optionsFill_WQ_load.continent_abb, IDScen, optionsFill_WQ_load.parameter_id);
			{ // Falls Tabelle load nicht existiert, erstellen; Tabelle mit IDScen und parameter_id in den Tabellennamen
				if (!isTableExists(LoadTableName)) {
					j=sprintf(sql,    "CREATE TABLE  %s (", LoadTableName);
					//j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '0',\n");
					//j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
					j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
					j+=sprintf(sql+j, "  `year` int(10) unsigned NOT NULL,\n");
					j+=sprintf(sql+j, "  `v1` double default NULL,\n");
					j+=sprintf(sql+j, "  `v2` double default NULL,\n");
					j+=sprintf(sql+j, "  `v3` double default NULL,\n");
					j+=sprintf(sql+j, "  `v4` double default NULL,\n");
					j+=sprintf(sql+j, "  `v5` double default NULL,\n");
					j+=sprintf(sql+j, "  `v6` double default NULL,\n");
					j+=sprintf(sql+j, "  `v7` double default NULL,\n");
					j+=sprintf(sql+j, "  `v8` double default NULL,\n");
					j+=sprintf(sql+j, "  `v9` double default NULL,\n");
					j+=sprintf(sql+j, "  `v10` double default NULL,\n");
					j+=sprintf(sql+j, "  `v11` double default NULL,\n");
					j+=sprintf(sql+j, "  `v12` double default NULL,\n");
					//j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`year`,`cell`)\n");
					j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`year`,`cell`)\n");
					j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='montly grid cell loading (t/month); input for WorldQual.cpp';\n");

					query.reset();
					query << sql;
					query.execute();
				}

			}

			//for year!
			year=yearStart;
			cout<<"scen_nr="<<IDScen<<endl;


			do{
				cout<<"year="<<year<<endl;

				query.reset();

				j=sprintf(sql,"DELETE FROM %s ", LoadTableName);
				j+=sprintf(sql+j,"WHERE `year`=%d;", year);
				//j+=sprintf(sql+j,"WHERE `IDScen`=%d AND `parameter_id`=%d  AND `year`=%d;", IDScen, optionsFill_WQ_load.parameter_id, year);

				query << sql;
				cout << "Query: " << query << endl << endl; //cin >> j;
				query.execute();

				query.reset();

				// erste Tabelle in der Liste wird in WHERE-Anweisung f�r primary key Werte eingegeben
				// restliche Tabellen werden an die erste gejoint
				int firstTab=0;
				for (int type=0; type<loadTypes; type++)
					if (loadType[type]) {firstTab = type; break;}

				// Select Anweisung zusammenstellen
				//j=sprintf(sql,"SELECT %s.`IDScen`, %s.`parameter_id`, %s.`cell`, %s.`time`, \n", loadTabAlias[firstTab], loadTabAlias[firstTab], loadTabAlias[firstTab], loadTabAlias[firstTab]);
				j=sprintf(sql,"SELECT %s.`cell`, %s.`time`, \n", loadTabAlias[firstTab], loadTabAlias[firstTab]);
				for (int month=1; month<=12; month++) {
					char month_beg[2]=" ";
					for (int type=0; type<loadTypes; type++) {
						if (loadType[type]) {
							j+=sprintf(sql+j,"%s IFNULL(%s.`v%d`,0)", month_beg, loadTabAlias[type], month);
							strcpy(month_beg, "+");
						}
					}
					if (month<12) j+=sprintf(sql+j,",\n");
				} // for(month)

				{ // Tabellenliste
					j+=sprintf(sql+j,"\nFROM ");
					char tabList_beg[2]=" ";
					for (int type=0; type<loadTypes; type++) {
						if (loadType[type]) {
							j+=sprintf(sql+j,"%s %swq_load_%s.calc_cell_month_load_%s_%d_%d %s\n"
								, tabList_beg, MyDatabase, optionsFill_WQ_load.continent_abb, loadTab[type], IDScen, optionsFill_WQ_load.parameter_id, loadTabAlias[type]);
							strcpy(tabList_beg, ",");
						}
					}
				}

				// IDScen und parameter_id jetzt in den Tabellennamen
				//j+=sprintf(sql+j,"WHERE %s.`IDScen`=%d ", loadTabAlias[firstTab], IDScen);
				//j+=sprintf(sql+j,"AND %s.`parameter_id`=%d AND ", loadTabAlias[firstTab], optionsFill_WQ_load.parameter_id);
				j+=sprintf(sql+j,"WHERE  ");
				j+=sprintf(sql+j,"%s.`time`=%d ", loadTabAlias[firstTab], year);

				for (int type=firstTab+1; type<loadTypes; type++) {
					if (loadType[type]) {
						//j+=sprintf(sql+j,"\nAND %s.`IDScen`=%s.`IDScen` ", loadTabAlias[type], loadTabAlias[firstTab]);
						//j+=sprintf(sql+j,"AND %s.`parameter_id`=%s.`parameter_id` ", loadTabAlias[type], loadTabAlias[firstTab]);
						j+=sprintf(sql+j,"AND %s.`time`=%s.`time` ", loadTabAlias[type], loadTabAlias[firstTab]);
						j+=sprintf(sql+j,"AND %s.`cell`=%s.`cell` ", loadTabAlias[type], loadTabAlias[firstTab]);
					}
				}
				j+=sprintf(sql+j,";");

				cout << sql << endl;

				query << sql;
				//if (year==yearStart) { // nur ein MAl testen, sonst zu viel Zeit
				//	res = query.store();
				//	if (res.empty()) {cerr << "Error: table is empty -> check parametrization "<< endl; exit(1);}
				//}

				query.reset();

				// INSERT Anweisung aus Select zusammenstellen
				sprintf(sql_sel, "%s", sql); // select-Abfrage wird in insert gebraucht
				//j=sprintf(sql,"INSERT INTO %sworldqual_%s.`load` \n", MyDatabase, optionsFill_WQ_load.continent_abb);
				j=sprintf(sql,"INSERT INTO %s \n", LoadTableName);
				//j+=sprintf(sql+j,"(`IDScen`, `parameter_id`, `cell`, `year`, `v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) \n");
				j+=sprintf(sql+j,"(`cell`, `year`, `v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) \n");
				j+=sprintf(sql+j,"%s", sql_sel);

				query << sql;
				cout << "Query: " << query << endl << endl; //cin >> j
				cout << "========================================================================\n";
				query.execute();

				year++;


				//cout<<"year="<<year<<endl;


			} while(year<=yearEnd);
	}//for(Scen_nr)



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


