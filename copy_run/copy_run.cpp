/*
 * copy_run.cpp
 * Kopie von einem worldqual-run erzeugen
 *
 *  Created on: 06.07.2015
 *      Author: kynast
 */

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include "../general_function/timestring.h"
#include "../general_function/country.h"
#include "../general_function/general_functions.h"
#include "options_copyRun.h"

using namespace std;
using namespace mysqlpp;

char  refMyDatabase[maxcharlength];
char  MyDatabase[maxcharlength];

mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::Query      query_ins = con.query(); // for INSERT into 'statistik'
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;

// �berpr�fen, ob Daten in Ziel schon gibt
int checkData_wq(const char* TableName, int IDRun, int IDScen, int parameter_id, int start, int end);

int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram copy_run started at: " << getTimeString() << endl << endl;
	options_copyRunClass options_copyRun(argc, &argv[0], "StartYear EndYear");

	if (argc<3) {
		options_copyRun.Help();
//		cout << "\n./copy_run StartYear EndYear [-ooptions_file]\n";
		return 0;
	}

	if(!IsNumber(argv[1]) || !IsNumber(argv[2])) {
		cerr << "ERROR: Wrong parameter" << endl;
		options_copyRun.Help();
//		cerr << "usage: ./copy_run StartYear EndYear [-ooptions_file]"<<endl;
		exit(1);
	}
	int StartYear       = atoi(argv[1]);
	int EndYear         = atoi(argv[2]);

	if (StartYear > EndYear) {
		cerr << "ERROR: " << StartYear <<" > " << EndYear <<"!\n";
		exit(1);
	}

	bool CopyInOneProject = true;

	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 3 = 1 + 2 = programmname + 2 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if (options_copyRun.init(argc-3, &argv[3])) exit(1);

//	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input

	mysqlpp::String ref_runName, runName;
	int parameter_id_input, parameter_id, parameter_id_load;
	int ref_project_id, ref_IDScen, ref_IDScen_wq_load;
	int toCalc; // 0 - BOD, 1 - salt, 2 - FC (Fecal coliforms), 3 - TN, 4 - TP

	char filename_sql[maxcharlength];
	sprintf(filename_sql,"copy_idRun_%d_into_%d.SQL", options_copyRun.refIDRun, options_copyRun.IDRun);
	ofstream mySQLfile;
	mySQLfile.open(filename_sql, ios::out);
	if (!mySQLfile.is_open()) {
		cerr << "\nError while opening file " << filename_sql << endl;
		exit(1);
	}

	int j;
	try {
		char sql[10000]; // variable for SQL-query
		// Wenn vectorl�nge >0 wird, zuerst entscheiden: a: abbrechen b: nur neue hinzuf�gen c: l�schen und hinzuf�gen
		vector<string> DatenExistsTables;

		cout << "start:" << endl;
		char start_db[maxcharlength]; //sprintf(start_db, "%swq_load_%s", MyDatabase, optionsWQ_load.continent_abb);
		sprintf(start_db, "wq_general");

		if (!con.connect(&start_db[0], options_copyRun.MyHost, options_copyRun.MyUser, options_copyRun.MyPassword,3306)) {
			cout << "database connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl<<endl; //return 0;
		////////////////////////////////

		cout << "####################################################\n";
		cout << "Pruefen Referenz-IDrun:\t" << options_copyRun.refIDRun << endl;
		if(checkIDRun(options_copyRun.refIDRun, ref_runName, parameter_id_input, parameter_id, parameter_id_load
				, ref_project_id, ref_IDScen, ref_IDScen_wq_load)) exit(1);

		// wq_general._runlist fuer neuen IDRun erstellen
		cout << "####################################################\n";
		cout << "Pruefen Ziel-IDrun:\t" << options_copyRun.IDRun << endl;
		{
			int dest_project_id, dest_IDScen, dest_parameter_id_input, dest_parameter_id, dest_parameter_id_load, dest_IDScen_wq_load;
			switch (checkIDRun(options_copyRun.IDRun, runName, dest_parameter_id_input, dest_parameter_id, dest_parameter_id_load
					, dest_project_id, dest_IDScen, dest_IDScen_wq_load)) {
			case 1: exit(1); break;
			case 2: // neue Zeile einf�gen
				j=sprintf(sql,    "INSERT INTO wq_general._runlist \n");
				j+=sprintf(sql+j, "SELECT %d, r.Name, r.conservative, r.lake, ", options_copyRun.IDRun);
				j+=sprintf(sql+j, "%d, %d, %d, \n", options_copyRun.IDScen, ref_IDScen_wq_load, options_copyRun.project_id);
				j+=sprintf(sql+j, "%d, %d, ", parameter_id_input, parameter_id);
				j+=sprintf(sql+j, "r.IDTemp, r.teta, r.teta_lake, r.parameter_id_load, r.svn_version_id, ");
				j+=sprintf(sql+j, "r.Q_low_limit, r.Q_low_limit_type, \n");
				j+=sprintf(sql+j, "concat('copy from idrun ', %d , ' ', ifnull(r.comment, '')) as comment\n", options_copyRun.refIDRun);
				j+=sprintf(sql+j, "FROM wq_general._runlist r ");
				j+=sprintf(sql+j, "WHERE r.IDrun = %d;", options_copyRun.refIDRun);
				query.reset();
				query << sql;
				mySQLfile << sql << endl << endl;
				if (options_copyRun.SQLexecute) query.execute();

				break;
			case 0: // Zeile schon gibt, �berpr�fen ob es zu OPTIONS.DAT passt, Falls nicht, abbrechen mit Fehlermeldung
				if (dest_IDScen != options_copyRun.IDScen || dest_IDScen_wq_load != ref_IDScen_wq_load
						|| dest_parameter_id != parameter_id
						|| dest_parameter_id_input != parameter_id_input
						|| dest_parameter_id_load != parameter_id_load
						|| dest_project_id != options_copyRun.project_id
					) {
					cerr << "Zeile in der Tabelle wq_general._runlist f�r IDrun= " << options_copyRun.IDRun << "existiert, \n"
						<< "�bereinstimmt aber mit deinen Parametern nicht. Bitte anpassen entweder die Tabelle oder OPTIONS.DAT\n\n";
					cerr << "dest_IDScen            : " << dest_IDScen             << "\toptions.IDScen    : " << options_copyRun.IDScen     << endl;
					cerr << "dest_IDScen_wq_load    : " << dest_IDScen_wq_load     << "\tref_IDScen_wq_load: " << ref_IDScen_wq_load         << endl;
					cerr << "dest_parameter_id      : " << dest_parameter_id       << "\tparameter_id      : " << parameter_id               << endl;
					cerr << "dest_parameter_id_input: " << dest_parameter_id_input << "\tparameter_id_input: " << parameter_id_input         << endl;
					cerr << "dest_parameter_id_load : " << dest_parameter_id_load  << "\tparameter_id_load : " << parameter_id_load          << endl;
					cerr << "dest_project_id        : " << dest_project_id         << "\toptions.project_id: " << options_copyRun.project_id << endl;
					exit(1);
				}
				break;
			default: exit(1); break;
			}
		}

		// checking project_id and project- and database name, fill MyDatabase
		if (getProject(ref_project_id, &options_copyRun, con, "worldqual")) exit(1);

		if (options_copyRun.project_id == -1 || options_copyRun.project_id == ref_project_id) { // Szenarien innerhalb eines Projektes kopieren
			CopyInOneProject = true;
			sprintf(refMyDatabase,"%s",MyDatabase); // refMyDatabase und MyDatabase sind gleich
			options_copyRun.project_id = ref_project_id;
			cout << "reference and destination projects are the same\n";
		} else { // Szenario in anderen Project kopieren
			CopyInOneProject = false;
			sprintf(refMyDatabase,"%s",MyDatabase);
			// checking project_id and project- and database name, fill MyDatabase with destination database
			cout << "destination project: ";
			if (getProject(options_copyRun.project_id, &options_copyRun, con, "worldqual")) {
				cerr << "wq_general._project muss f�r Ziel-projeckt ausgef�llt sein und Datenbanken erstellt und Rechte vergeben werden\n";
				cerr << "befor man Kopieren startet. Bitte an den DB_Administrator wenden!\n";
				exit(1);
			}

			if (strcmp(refMyDatabase, MyDatabase) == 0) {
				cerr << "Verschiedene Projekte d�rfen nicht gleiche Datenbank nutzen!\n";
				exit(1);
			}
		}

		// checking reference IDScen and scenario name
		cout << "checking reference IDScen: ";
		if(checkIDScen(ref_project_id, ref_IDScen)) exit(1);
		if (ref_IDScen != ref_IDScen_wq_load) {
			cout << "ref_IDScen_wq_load: ";
			if(checkIDScen(ref_project_id, ref_IDScen_wq_load)) exit(1);
		}
		cout << "checking destination IDScen: ";
		if(checkIDScen(options_copyRun.project_id, options_copyRun.IDScen)) {
			// Falls Szenario noch nicht existiert, eif�gen in wq_general._szenario
			cout << "Szenario "<< options_copyRun.IDScen << " f�r Projekt "<< options_copyRun.project_id<<" wird zu Tabelle wq_general._szenario hinzugef�gt.\n";
			query.reset();
			j=sprintf(sql,    "INSERT INTO wq_general._szenario \n");
			j+=sprintf(sql+j, "SELECT %d, %d, s.ScenName, ", options_copyRun.IDScen, options_copyRun.project_id);
			j+=sprintf(sql+j, "concat('copy from idscen ', %d, ' project ', %d , ' ', ifnull(s.description, '')) as description \n", ref_IDScen, ref_project_id);
			j+=sprintf(sql+j, "FROM wq_general._szenario s \n");
			j+=sprintf(sql+j, "WHERE s.`idScen` = %d AND s.project_id= %d; ", ref_IDScen, ref_project_id);
			query << sql;
			mySQLfile << sql << endl << endl;
			if (options_copyRun.SQLexecute) query.execute();
		}

		mysqlpp::String ParameterName;
		// checking parameter_id and parameter name
		if (checkParameter(parameter_id, toCalc, ParameterName, false)) exit(1);

		mySQLfile << "# Copy worldqual run: IDrun " << options_copyRun.refIDRun << endl;
		mySQLfile << "# into run: IDrun " << options_copyRun.IDRun << endl;
		mySQLfile << "# parameter_id: " << parameter_id << endl;
		mySQLfile << "# Referenzproject: " << ref_project_id << " Szenario: "<< ref_IDScen << endl;
		mySQLfile << "# Zielproject    : " << options_copyRun.project_id << " Szenario: "<< options_copyRun.IDScen << endl;

		char TableName[maxcharlength];
		sprintf(TableName, "%sworldqual_%s.`factors`", MyDatabase, options_copyRun.continent_abb);
		bool TableExists = true;

		if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
			j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
			j+=sprintf(sql+j, "  `IDrun` int(10) unsigned NOT NULL auto_increment,\n");
			j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
			j+=sprintf(sql+j, "  `decomposition` double NOT NULL COMMENT 'Abbaufaktor [1/sec]',\n");
			j+=sprintf(sql+j, "  `c_geogen` double NOT NULL COMMENT 'Hintergrundkonzentration [Tonne/km3]',\n");
			j+=sprintf(sql+j, "  `decay_rate_lake` double default NULL COMMENT 'Abbaufaktor in den Seen [1/sec]',\n");
			j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDrun`,`cell`)\n");
			j+=sprintf(sql+j, ") ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=latin1 COMMENT='input for WorldQual.cpp';\n");

			if (CreateTable(TableName, sql, options_copyRun.SQLexecute, mySQLfile, TableExists)) exit (1);
		}

		// �berpr�fen, ob Daten im Ziel schon gibt
		if (TableExists) {
			int counter = checkData_wq(TableName, options_copyRun.IDRun, -1, -1, -1, -1);
			if (counter>0) { // Daten existieren schon
				DatenExistsTables.push_back(TableName);
			}
		}

		// load
		if (options_copyRun.CopyLoad) {
			sprintf(TableName, "%sworldqual_%s.`load`", MyDatabase, options_copyRun.continent_abb);
			TableExists = true;
			if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
				j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '0',\n");
				j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
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
				j+=sprintf(sql+j, "  `geogen` int(10) unsigned default '1' COMMENT '=1 geogene Konzentraton f�r Zelle-Jahr aus factors nutzen\r\n=0 geogene Konzentraton f�r Zelle-Jahr=0',\n");
				j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`year`,`cell`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='montly grid cell loading (t/month); input for WorldQual.cpp';\n");

				if (CreateTable(TableName, sql, options_copyRun.SQLexecute, mySQLfile, TableExists)) exit (1);
			}

			// �berpr�fen, ob Daten im Ziel schon gibt
			if (TableExists) {
				int counter = checkData_wq(TableName, -1, options_copyRun.IDScen, parameter_id, StartYear, EndYear);
				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

		} // if (options_copyRun.CopyLoad)

//			cout << "######################## DatenExistsTables.size() = " << DatenExistsTables.size() << endl;
		cout << "########################################################################" << endl;
		// Way : 1: abbrechen 2: l�schen und hinzuf�gen
		short Way=2; // 2, weil wenn keine Daten gibt, wird nicht abgefragt, muss man alles hinzuf�gen
		if (DatenExistsTables.size()>0) {
			cout << "IDRun " << options_copyRun.IDRun << " hat schon Daten in den Tabellen: \n";
			for (unsigned i=0; i<DatenExistsTables.size(); i++) {
				cout << DatenExistsTables[i] << endl;
			}
			bool isntOK = false;
			cout << endl<< "####################"<<endl;
			cout << "1: break\n";
//			cout << "2: add only new data\n";
			cout << "2: delete and add \n\n";
			do {
			//cin >> Way; std::cout << " * " << Way << std::endl;
		    cout << "select please: ";
		    int counter=0;
		    // nur short-Werte werden abgefragt und nicht mehr als 10 Versuche
		    // Falls Programm mit nohup gestartet wurde, counter wird zu gro� und es wird abgebrochen
		    while(!(cin >> Way) && counter<10){
		    	counter++;
		        cin.clear();
		        cin.ignore(numeric_limits<streamsize>::max(), '\n');
		        cout << "Invalid input.  Try again: ";
		    }
		    if (counter == 10) {
				cout << "\n\nBitte copy_scen nicht mit nohup starten! Es wird abgefragt, was man mit existierenden Daten machen soll.\n";
				exit(1);
		    }
		    cout << "You enterd: " << Way << endl;
		    isntOK = false;

			switch(Way) {
			case 0:
			case 1:
				cout << "break\n";
				exit(1);
				break;
			case 2:
//				cout << "add only new data\n";
//				break;
//			case 3:
				cout << "delete and add \n";
				break;
			default:
//				cout << "please only 1, 2 or 3!\n";
				cout << "please only 1 or 2!\n";
				isntOK = true;
				break;
			} // switch(Way)
			} while (isntOK);
		} // Wenn Daten existieren

		// ########################## Daten einf�gen ############################
		// hier kommen wir an nur Way == 2

		sprintf(TableName, "%sworldqual_%s.`factors`", MyDatabase, options_copyRun.continent_abb);
		query.reset();
		j=sprintf(sql,    "DELETE FROM %s \n", TableName);
		j+=sprintf(sql+j, "WHERE IDRun=%d;", options_copyRun.IDRun);
		query << sql;
		mySQLfile << sql << '\n' << '\n';
		if (options_copyRun.SQLexecute) query.execute();

		query.reset();
		j=sprintf(sql,    "INSERT INTO %s \n", TableName);
		j+=sprintf(sql+j, "SELECT %d, f.cell, ", options_copyRun.IDRun);
		j+=sprintf(sql+j, "f.decomposition, f.c_geogen, f.decay_rate_lake \n");
		j+=sprintf(sql+j, "FROM %sworldqual_%s.factors f ", refMyDatabase, options_copyRun.continent_abb);
		j+=sprintf(sql+j, "WHERE f.IDRun=%d;",options_copyRun.refIDRun);
		query << sql;
		mySQLfile << sql << endl << endl;
		if (options_copyRun.SQLexecute) query.execute();

		// load
		if (options_copyRun.CopyLoad) {
			sprintf(TableName, "%sworldqual_%s.`load`", MyDatabase, options_copyRun.continent_abb);
			query.reset();
			j=sprintf(sql,    "DELETE FROM %s \n", TableName);
			j+=sprintf(sql+j, "WHERE IDScen=%d AND parameter_id=%d ", options_copyRun.IDScen, parameter_id);
			j+=sprintf(sql+j, "AND year between %d and %d;", StartYear, EndYear);
			query << sql;
			mySQLfile << sql << '\n' << '\n';
			if (options_copyRun.SQLexecute) query.execute();

			query.reset();
			j=sprintf(sql,    "INSERT INTO %s\n", TableName);
			j+=sprintf(sql+j, "SELECT %d, l.parameter_id, l.cell, l.year, ", options_copyRun.IDScen);
			j+=sprintf(sql+j, "l.v1, l.v2, l.v3, l.v4, l.v5, l.v6, l.v7, l.v8, l.v9, l.v10, l.v11, l.v12, l.geogen \n");
			j+=sprintf(sql+j, "FROM %sworldqual_%s.load l ", refMyDatabase, options_copyRun.continent_abb);
			j+=sprintf(sql+j, "WHERE l.IDScen=%d AND l.parameter_id=%d ", ref_IDScen, parameter_id);
			j+=sprintf(sql+j, "AND l.year between %d and %d;", StartYear, EndYear);
			query << sql;
			mySQLfile << sql << endl << endl;
			if (options_copyRun.SQLexecute) query.execute();

		} // if (options_copyRun.CopyLoad)

		//##########################################################
		// unabh�ngige von IDScen Daten nur dann kopieren, wenn Datenbanken MyDatabase und refMyDatabase nicht gleich sind
		if (!CopyInOneProject) {
			// hier MyDatabase und refMyDatabase unterschiedlich
			mySQLfile << "#########################################################" << endl;
			mySQLfile << "# independent from IDScen tables" << endl;
			DatenExistsTables.clear();

			char TableName[maxcharlength];

			// flow_velocity
			sprintf(TableName, "%sworldqual_%s.`flow_velocity`", MyDatabase, options_copyRun.continent_abb);
			j=sprintf(sql,    "CREATE TABLE  %s (\n", TableName);
			j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
			j+=sprintf(sql+j, "  `u` double COMMENT 'flow_velocity [m/s] (const flow_velocity - variante)',\n");
			j+=sprintf(sql+j, "  `river_length` double default NULL COMMENT '[km] calculated from cell distance*sinuosity_eu_point',\n");
			j+=sprintf(sql+j, "  `rout_order` double default NULL,\n");
			j+=sprintf(sql+j, "  PRIMARY KEY  (`cell`)\n");
			j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='Flie�geschwindigkeit [m/s]';\n");

			if (CreateTable(TableName, sql, options_copyRun.SQLexecute, mySQLfile, TableExists)) exit (1);

			if (TableExists) {
				j=sprintf(sql,    "SELECT count(*) as counter FROM %s;", TableName);
				query.reset();
				query << sql;
				res = query.store();
				int  counter=res.at(0)["counter"];
				cout << TableName << " Zeilen: "<< counter << endl;

				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

			// routing
			sprintf(TableName, "%sworldqual_%s.`routing`", MyDatabase, options_copyRun.continent_abb);
			j=sprintf(sql,    "CREATE TABLE  %s (\n", TableName);
			j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
			j+=sprintf(sql+j, "  `inflow_count` int(10) unsigned default NULL,\n");
			j+=sprintf(sql+j, "  `inflow_done` int(11) default NULL,\n");
			j+=sprintf(sql+j, "  PRIMARY KEY  (`cell`)\n");
			j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='tabelle fuer berechnungen';\n");

			if (CreateTable(TableName, sql, options_copyRun.SQLexecute, mySQLfile, TableExists)) exit(1);

			if (TableExists) {
				j=sprintf(sql,    "SELECT count(*) as counter FROM %s;", TableName);
				query.reset();
				query << sql;
				res = query.store();
				int  counter=res.at(0)["counter"];
				cout << TableName << " Zeilen: "<< counter << endl;

				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

//			cout << "######################## DatenExistsTables.size() = " << DatenExistsTables.size() << endl;
			cout << "########################################################################" << endl;
			// Way : 1: abbrechen 2: �berschreiben
			short Way=2; // 1, weil wenn keine Daten gibt, wird nicht abgefragt, muss man alles hinzuf�gen
			if (DatenExistsTables.size()>0) {
				cout << " run " << options_copyRun.IDRun << " hat schon Daten in den Tabellen: \n";
				for (unsigned i=0; i<DatenExistsTables.size(); i++) {
					cout << DatenExistsTables[i] << endl;
				}
				bool isntOK = false;
				cout << endl<< "####################"<<endl;
				cout << "1: break\n";
				cout << "2: delete and add \n\n";
				do {
			    cout << "select please: ";
			    int counter=0;
			    // nur short-Werte werden abgefragt und nicht mehr als 10 Versuche
			    // Falls Programm mit nohup gestartet wurde, counter wird zu gro� und es wird abgebrochen

			    while(!(cin >> Way) && counter<10){
			    	counter++;
			        cin.clear();
			        cin.ignore(numeric_limits<streamsize>::max(), '\n');
			        cout << "Invalid input.  Try again: ";
			    }
			    if (counter == 10) {
					cout << "\n\nBitte copy_scen nicht mit nohup starten! Es wird abgefragt, was man mit existierenden Daten machen soll.\n";
					exit(1);
			    }
			    cout << "You enterd: " << Way << endl;
			    isntOK = false;

				switch(Way) {
				case 0:
				case 1:
					cout << "break\n";
					exit(1);
					break;
				case 2:
					cout << "delete and add \n";
					break;
				default:
					cout << "please only 1 or 2!\n";
					isntOK = true;
					break;
				}
				} while (isntOK);

			}
			// ########################## Daten einf�gen ############################
			// hier kommen wir an nur Way == 2 ist

			// flow_velocity
			sprintf(TableName, "%sworldqual_%s.`flow_velocity`", MyDatabase, options_copyRun.continent_abb);
			query.reset();
			j=sprintf(sql,    "DELETE FROM %s;", TableName);
			query << sql;
			mySQLfile << sql << endl << endl;
			if (options_copyRun.SQLexecute) query.execute();

			query.reset();
			j=sprintf(sql,    "INSERT INTO %s \n", TableName);
			j+=sprintf(sql+j, "SELECT * FROM %sworldqual_%s.`flow_velocity`;", refMyDatabase, options_copyRun.continent_abb);
			query << sql;
			mySQLfile << sql << endl << endl;
			if (options_copyRun.SQLexecute) query.execute();

			// routing
			sprintf(TableName, "%sworldqual_%s.`routing`", MyDatabase, options_copyRun.continent_abb);
			query.reset();
			j=sprintf(sql,    "DELETE FROM %s;", TableName);
			query << sql;
			mySQLfile << sql << endl << endl;
			if (options_copyRun.SQLexecute) query.execute();

			query.reset();
			j=sprintf(sql,    "INSERT INTO %s \n", TableName);
			j+=sprintf(sql+j, "SELECT * FROM %sworldqual_%s.`routing`;", refMyDatabase, options_copyRun.continent_abb);
			query << sql;
			mySQLfile << sql << endl << endl;
			if (options_copyRun.SQLexecute) query.execute();
		} // if (!CopyInOneProject)

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
	// print current date
	cout <<"\nProgram copy_run ended at: " << getTimeString() << endl << endl;
	return 0;
} // main()

int checkData_wq(const char* TableName, int IDRun, int IDScen, int parameter_id, int start, int end) {
	// wenn in der Tabelle kein Feld IDRun, IDScen oder parameter_id gibt, soll entsprechende Argument negativ sein
	// wenn abfrage soll 'time between start and end' sein, beine Jahre ausf�llen
	// wenn end <=0, wird time = start abgefragt
	// wenn start und end <=0, Tabelle hat keinen time-Feld
	char sql[10000]; // variable for SQL-query
	int j;

	j=sprintf(sql,    "SELECT count(*) as counter FROM %s WHERE\n", TableName);

	bool fieldsAfterWhere=0; // braucht man schon "AND"?

	if(IDRun>=0) {
		if (fieldsAfterWhere) j+=sprintf(sql+j, "AND ");
		j+=sprintf(sql+j, "IDrun=%d ", IDRun);
		fieldsAfterWhere = 1;
	}
	if (IDScen>=0) {
		if (fieldsAfterWhere) j+=sprintf(sql+j, "AND ");
		j+=sprintf(sql+j, "IDScen=%d ", IDScen);
		fieldsAfterWhere = 1;
	}
	if (parameter_id>=0) {
		if (fieldsAfterWhere) j+=sprintf(sql+j, "AND ");
		j+=sprintf(sql+j, "parameter_id=%d ", parameter_id);
		fieldsAfterWhere = 1;
	}
	if (start>0) {
		if (end>0) {
			if (fieldsAfterWhere) j+=sprintf(sql+j, "AND ");
			j+=sprintf(sql+j, "year between %d and %d;", start, end);
			fieldsAfterWhere = 1;
		}  else {
			if (fieldsAfterWhere) j+=sprintf(sql+j, "AND ");
			j+=sprintf(sql+j, "AND year = %d;", start);
			fieldsAfterWhere = 1;
		}
	} else {
		j+=sprintf(sql+j, ";");
	}

	query.reset();
	query << sql;
	cout << query << endl;
	res = query.store();
	int  counter=res.at(0)["counter"];
	cout << TableName << " Zeilen: "<< counter << endl;

	return counter;
}

