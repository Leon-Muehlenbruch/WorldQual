/*
 * copy_scen.cpp
 * Kopie von einem Szenario erzeugen
 *
 *  Created on: 29.06.2015
 *      Author: kynast
 */

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include "../general_function/timestring.h"
#include "../general_function/country.h"
#include "../general_function/general_functions.h"
#include "options_copyScen.h"

using namespace std;
using namespace mysqlpp;

char  refMyDatabase[maxcharlength];
char  MyDatabase[maxcharlength];

mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::Query      query_ins = con.query(); // for INSERT into 'statistik'
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;

// überprüfen, ob Daten in Ziel schon gibt
int checkData_wqLoad(const char* TableName, int IDScen, int parameter_id, int start, int end);

// insert Daten in die Tabelle
bool insertData_wq_load(const char* TableName, const char* sql_insert, short Way, int parameter_id, int IDScen, int StartYear, int EndYear
		, ofstream& mySQLfile, options_copyScenClass* options_copyScen_ptr);


int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram copy_scen started at: " << getTimeString() << endl << endl;
	options_copyScenClass options_copyScen(argc, &argv[0], "StartYear EndYear");

	if (argc<3) {
		options_copyScen.Help();
//		cout << "\n./copy_scen StartYear EndYear [-ooptions_file]\n";
		return 0;
	}

	if(!IsNumber(argv[1]) || !IsNumber(argv[2])) {
		cerr << "ERROR: Wrong parameter" << endl;
		cerr << "usage: ./copy_scen StartYear EndYear [-ooptions_file]"<<endl;
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
	if (options_copyScen.init(argc-3, &argv[3])) exit(1);

//	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input

	int toCalc; // 0 - BOD, 1 - salt, 2 - FC (Fecal coliforms), 3 - TN, 4 - TP

	char filename_sql[maxcharlength];
	sprintf(filename_sql,"copy_project_%d_scen_%d.SQL", options_copyScen.refProject_id, options_copyScen.refScen);
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

		if (!con.connect(&start_db[0], options_copyScen.MyHost, options_copyScen.MyUser, options_copyScen.MyPassword,3306)) {
			cout << "database connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl<<endl; //return 0;
		////////////////////////////////

		// checking parameter_id and parameter name
		mysqlpp::String ParameterName;
		if (checkParameter(options_copyScen.parameter_id, toCalc, ParameterName)) exit(1);

		// checking project_id and project- and database name, fill MyDatabase
		cout << "reference project: ";
		if (getProject(options_copyScen.refProject_id, &options_copyScen, con, "wq_load")) exit(1);

		if (options_copyScen.refProject_id == options_copyScen.project_id) { // Szenarien innerhalb eines Projektes kopieren
			CopyInOneProject = true;
			sprintf(refMyDatabase,"%s",MyDatabase); // refMyDatabase und MyDatabase sind gleich
			cout << "reference and destination projects are the same\n";
		} else { // Szenario in anderen Project kopieren
			CopyInOneProject = false;
			sprintf(refMyDatabase,"%s",MyDatabase);
			// checking project_id and project- and database name, fill MyDatabase with destination database
			cout << "destination project: ";
			if (getProject(options_copyScen.project_id, &options_copyScen, con, "wq_load")) exit(1);

			if (strcmp(refMyDatabase, MyDatabase) == 0) {
				cerr << "Verschiedene Projekte d�rfen nicht gleiche Datenbank nutzen!\n";
				exit(1);
			}
		}

		// checking reference IDScen and scenario name
		cout << "checking reference IDScen: ";
		if(checkIDScen(options_copyScen.refProject_id, options_copyScen.refScen)) exit(1);

		mySQLfile << "# Copy scenario: project_id " << options_copyScen.refProject_id << " IDScen " << options_copyScen.refScen << endl;
		mySQLfile << "# into scenarios: project_id " << options_copyScen.project_id << " IDScen: " << options_copyScen.IDScenBeg << " - " << options_copyScen.IDScenEnd<< endl;
		mySQLfile << "# parameter_id: " << options_copyScen.parameter_id << endl;
		mySQLfile << "# years: " << StartYear << "-" << EndYear << endl;

		bool TableExists;

		for (int IDScen = options_copyScen.IDScenBeg; IDScen<=options_copyScen.IDScenEnd; IDScen++) {
			cout << "####### IDScen : " << IDScen << " ##########################\n";
			DatenExistsTables.clear();
			mySQLfile << "#########################################################" << endl;
			mySQLfile << "# project_id "<< options_copyScen.project_id << " IDScen " << IDScen << endl;
			cout << "checking destination IDScen: ";
			if(checkIDScen(options_copyScen.project_id, IDScen)) {
				// Falls Szenario noch nicht existiert, eif�gen in wq_general._szenario
				cout << "Szenario "<< IDScen << " f�r Projekt "<< options_copyScen.project_id<<" wird zu Tabelle wq_general._szenario hinzugef�gt.\n";
				query.reset();
				j=sprintf(sql,    "INSERT INTO wq_general._szenario \n");
				j+=sprintf(sql+j, "SELECT %d, %d, s.ScenName, ", IDScen, options_copyScen.project_id);
				j+=sprintf(sql+j, "concat('copy from idscen ', %d, ' project ', %d , ' ', ifnull(s.description, '')) as description \n", options_copyScen.refScen, options_copyScen.refProject_id);
				j+=sprintf(sql+j, "FROM wq_general._szenario s \n");
				j+=sprintf(sql+j, "WHERE s.`idScen` = %d AND s.project_id= %d; ", options_copyScen.refScen, options_copyScen.refProject_id);
				query << sql;
				mySQLfile << sql << endl << endl;
				if (options_copyScen.SQLexecute) query.execute();
			}

			// wq_load Datenbank
			// country_input
			char TableName[maxcharlength];
			sprintf(TableName, "%swq_load.`country_input`", MyDatabase);
			TableExists = true;
			if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
				j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
				j+=sprintf(sql+j, "  `country_id` int(10) unsigned NOT NULL,\n");
				j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
				j+=sprintf(sql+j, "  `rtf_man` double unsigned default NULL COMMENT 'manufacturing returnflow (m3/a]',\n");
				j+=sprintf(sql+j, "  `fra_man_f` double default NULL COMMENT 'food, drink, tobacco fraction of manufacturing returnflow []',\n");
				j+=sprintf(sql+j, "  `fra_man_t` double default NULL COMMENT 'textiles fraction of manufacturing returnflow []',\n");
				j+=sprintf(sql+j, "  `fra_man_p` double default NULL COMMENT 'paper, palp, printing fraction of manufacturing returnflow []',\n");
				j+=sprintf(sql+j, "  `fra_man_c` double default NULL COMMENT 'chemical industry fraction of manufacturing returnflow []',\n");
				j+=sprintf(sql+j, "  `fra_man_g` double default NULL COMMENT 'glas, ceramics, cement fraction of manufacturing returnflow []',\n");
				j+=sprintf(sql+j, "  `fra_man_m` double default NULL COMMENT 'metals fraction of manufacturing returnflow []',\n");
				j+=sprintf(sql+j, "  `fra_man_nd` double default NULL COMMENT 'other fraction of manufacturing returnflow []',\n");
				j+=sprintf(sql+j, "  `rtf_dom` double unsigned default NULL COMMENT 'domestic returnflow (m3/a]',\n");
				j+=sprintf(sql+j, "  `rtf_irr` double unsigned default NULL COMMENT 'irrigation returnflow (m3/a]',\n");
				j+=sprintf(sql+j, "  `pop_tot` double unsigned default NULL COMMENT 'total population [number]',\n");
				j+=sprintf(sql+j, "  `pop_urb` double unsigned default NULL COMMENT 'urban population [number]',\n");
				j+=sprintf(sql+j, "  `pop_rur` double unsigned default NULL COMMENT 'rural population [number]',\n");
				j+=sprintf(sql+j, "  `con_prim` double unsigned default NULL COMMENT 'primary connectivity [%%]',\n");
				j+=sprintf(sql+j, "  `con_sec` double unsigned default NULL COMMENT 'secondary connectivity [%%]',\n");
				j+=sprintf(sql+j, "  `con_tert` double unsigned default NULL COMMENT 'tertiary connectivity [%%]',\n");
				j+=sprintf(sql+j, "  `con_quat` double unsigned default NULL COMMENT 'quaternary connectivity [%%]',\n");
				j+=sprintf(sql+j, "  `con_untr` double unsigned default NULL COMMENT 'no treatment [%%]',\n");
				j+=sprintf(sql+j, "  `stp_failure` double unsigned default NULL COMMENT 'percentage of sewage treatment plants (STP) not working or in a bad condition',\n");
				j+=sprintf(sql+j, "  `to_treat_and_unknown` double unsigned default NULL COMMENT 'percent of total pop connected to septic tanks, pit latrines, unknown place, bucket latrine and composting toilet -> some treatmet but much is unknown',\n");
				j+=sprintf(sql+j, "  `to_hanging_t` double unsigned default NULL COMMENT 'percent of total population connected to hanging latrines',\n");
				j+=sprintf(sql+j, "  `to_open_def` double unsigned default NULL COMMENT 'percent of total population practicing open defecation',\n");
				j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`time`,`country_id`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 ROW_FORMAT=FIXED;\n");

				if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);
			}

			// �berpr�fen, ob Daten im Ziel schon gibt
			if (TableExists) {
				int counter = checkData_wqLoad(TableName, IDScen, -1, StartYear, EndYear);
				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

			// country_parameter_input
			sprintf(TableName, "%swq_load.`country_parameter_input`", MyDatabase);
			TableExists = true;
			if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
				j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
				j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
				j+=sprintf(sql+j, "  `country_id` int(10) unsigned NOT NULL,\n");
				j+=sprintf(sql+j, "  `time` int(11) NOT NULL COMMENT 'as YearFrom',\n");
				j+=sprintf(sql+j, "  `ef` double default NULL COMMENT 'emission factor from population [kg/cap/year], FC: [10^10cfu/(cap*a)]',\n");
				j+=sprintf(sql+j, "  `conc_man_f` double default NULL COMMENT 'manufacturing concentration (food, drink, tobacco) of determinand in return flow [mg/l] resp. 10^10cfu/L for FC',\n");
				j+=sprintf(sql+j, "  `conc_man_t` double default NULL COMMENT 'manufacturing concentration (textiles) of determinand in return flow [mg/l] resp. 10^10cfu/L for FC',\n");
				j+=sprintf(sql+j, "  `conc_man_p` double default NULL COMMENT 'manufacturing concentration (paper, palp, printing) of determinand in return flow [mg/l] resp.  10^10cfu/L for FC',\n");
				j+=sprintf(sql+j, "  `conc_man_c` double default NULL COMMENT 'manufacturing concentration (chemical industry) of determinand in return flow [mg/l] resp.  10^10cfu/L for FC',\n");
				j+=sprintf(sql+j, "  `conc_man_g` double default NULL COMMENT 'manufacturing concentration (glas, ceramics, cement) of determinand in return flow [mg/l] resp. 10^10cfu/L for FC',\n");
				j+=sprintf(sql+j, "  `conc_man_m` double default NULL COMMENT 'manufacturing concentration (metals) of determinand in return flow [mg/l] resp.  10^10cfu/L for FC',\n");
				j+=sprintf(sql+j, "  `conc_man_nd` double default NULL COMMENT 'manufacturing concentration (other) of determinand in return flow [mg/l] resp.  10^10cfu/L for FC (f�r FC ist other der Mittelwert aus allen Industrieformen - siehe Dokumentation)',\n");
				j+=sprintf(sql+j, "  `conc_tds1` double default NULL COMMENT 'tds concentration in return flow for sepc class 1 [mg/l]',\n");
				j+=sprintf(sql+j, "  `conc_tds2` double default NULL COMMENT 'tds concentration in return flow for sepc class 2 [mg/l]',\n");
				j+=sprintf(sql+j, "  `conc_tds3` double default NULL COMMENT 'tds concentration in return flow for sepc class 3 [mg/l]',\n");
				j+=sprintf(sql+j, "  `conc_tds4` double default NULL COMMENT 'tds concentration in return flow for sepc class 4 [mg/l]',\n");
				j+=sprintf(sql+j, "  `c_geogen` double default NULL COMMENT 'geogen background concentration [t/km3]',\n");
				j+=sprintf(sql+j, "  `conc_urb` double default NULL COMMENT 'concentration of urban runoff [mg/l] resp. [cfu/100ml] for FC',\n");
				j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`,`country_id`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci ROW_FORMAT=DYNAMIC;\n");

				if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);

			}

			// �berpr�fen, ob Daten im Ziel schon gibt
			if (TableExists) {
				int counter = checkData_wqLoad(TableName, IDScen, options_copyScen.parameter_id, StartYear, EndYear);
				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

			// parameter_input
			sprintf(TableName, "%swq_load.`parameter_input`", MyDatabase);
			TableExists = true;
			if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
				j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
				j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL default '0',\n");
				j+=sprintf(sql+j, "  `time` int(11) NOT NULL,\n");
				j+=sprintf(sql+j, "  `rem_prim` double NOT NULL default '-9999' COMMENT 'Removal rate primary treatment [%%]',\n");
				j+=sprintf(sql+j, "  `rem_sec` double NOT NULL default '-9999' COMMENT 'Removal rate secondary treatment [%%]',\n");
				j+=sprintf(sql+j, "  `rem_tert` double NOT NULL default '-9999' COMMENT 'Removal rate tertiary treatment [%%]',\n");
				j+=sprintf(sql+j, "  `rem_untr` double NOT NULL default '-9999' COMMENT 'Removal rate untreated [%%]',\n");
				j+=sprintf(sql+j, "  `treat_failure` double default NULL COMMENT 'percentage of failure of septic tanks, pit latrines, composting toilets and to take the unknown of bucket and unknown place into account;',\n");
				j+=sprintf(sql+j, "  `rem_soil` double default NULL COMMENT 'Removal rate in soil [%%]',\n");
				j+=sprintf(sql+j, "  `red_fac_org` double default '-9999' COMMENT 'Reduction factor from organic fertilizer - percentage of initial load which can reach the river',\n");
				j+=sprintf(sql+j, "  `red_fac_inor` double default '-9999' COMMENT 'Reduction factor from inorganic fertilizer - percentage of initial load which can reach the river',\n");
				j+=sprintf(sql+j, "  `k_storage` double default NULL COMMENT 'k= die off rate constant in storage; only applies for fecal coliforms',\n");
				j+=sprintf(sql+j, "  `k_soil` double default NULL COMMENT 'k= die off rate constant in soil; only applies for fecal coliforms',\n");
				j+=sprintf(sql+j, "  `ks` double default NULL COMMENT 'FC inactivation due to sunlight cm�/cal',\n");
				j+=sprintf(sql+j, "  `sed_veloc` double default NULL COMMENT 'sedimentation velocity for fc [m/d]',\n");
				j+=sprintf(sql+j, "  `ke_tss_reg_alpha` double default NULL COMMENT 'alpha from the regression:\r\nke=alpha*TSS+beta',\n");
				j+=sprintf(sql+j, "  `ke_tss_reg_beta` double default NULL COMMENT 'beta from the regression:\r\nke=alpha*TSS+beta',\n");
				j+=sprintf(sql+j, "  `teta` double default NULL COMMENT 'variable zur Wassertemperaturberechnung. neu: nicht mehr in _runlist!!!',\n");
				j+=sprintf(sql+j, "  `teta_lake` double default NULL COMMENT 'variable zur Wassertemperaturberechnung in den Seen; neu: nicht mehr in _runlist!!!',\n");
				j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`parameter_id`,`time`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci ROW_FORMAT=FIXED;\n");

				if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);
			}

			// �berpr�fen, ob Daten im Ziel schon gibt
			if (TableExists) {
				int counter = checkData_wqLoad(TableName, IDScen, options_copyScen.parameter_id, StartYear, EndYear);
				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

			// wq_load_[continent] Datenbank
			// cell_input
			sprintf(TableName, "%swq_load_%s.`cell_input`", MyDatabase, options_copyScen.continent_abb);
			TableExists = true;
			if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
				j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
				j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
				j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL default '0',\n");
				j+=sprintf(sql+j, "  `country_id` int(11) NOT NULL default '-9999',\n");
				j+=sprintf(sql+j, "  `pop_urb` double unsigned default NULL COMMENT 'urban cell population [number]',\n");
				j+=sprintf(sql+j, "  `pop_rur` double unsigned default NULL COMMENT 'rural cell population [number]',\n");
				j+=sprintf(sql+j, "  `pop_tot` double unsigned default NULL COMMENT 'total cell population [number]',\n");
				j+=sprintf(sql+j, "  `rtf_man` double unsigned default NULL COMMENT 'manufacturing returnflow (m3/a]',\n");
				j+=sprintf(sql+j, "  `rtf_dom` double unsigned default NULL COMMENT 'domestic returnflow (m3/a]',\n");
				j+=sprintf(sql+j, "  `rtf_irr` double unsigned default NULL COMMENT 'irrigation returnflow (m3/a]',\n");
				j+=sprintf(sql+j, "  `gdp` double unsigned default NULL COMMENT 'gdp per capita [US$]',\n");
				j+=sprintf(sql+j, "  `salinity` int(10) unsigned default NULL COMMENT 'primary salinity of soil 1 no, 2 yes',\n");
				j+=sprintf(sql+j, "  `humidity` int(10) unsigned default NULL COMMENT 'humid: 1, arid: 2',\n");
				j+=sprintf(sql+j, "  `lu` int(10) unsigned default NULL COMMENT 'land use class [-]',\n");
				j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`time`,`cell`),\n");
				j+=sprintf(sql+j, "  KEY `Index_country_id` (`country_id`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci ROW_FORMAT=FIXED;\n");

				if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);
			}

			// �berpr�fen, ob Daten im Ziel schon gibt
			if (TableExists) {
				int counter = checkData_wqLoad(TableName, IDScen, -1, StartYear, EndYear);
				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

			// wq_load_[continent] Datenbank
			// cell_input_pesticide
			sprintf(TableName, "%swq_load_%s.`cell_input_pesticide`", MyDatabase, options_copyScen.continent_abb);
			TableExists = true;
			if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
				j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
				j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
				j+=sprintf(sql+j, "  `organic_carbon` double default NULL COMMENT `Gehalt an organischen Kohlenstoff im Boden [%%]`,\n");
				j+=sprintf(sql+j, "  PRIMARY KEY (`IDScen`,`cell`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n");

				if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);
			}

			// Ueberpruefen, ob Daten im Ziel schon gibt
			if (TableExists) {
				int counter = checkData_wqLoad(TableName, IDScen, -1, -1, -1);
				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}


			// wq_load_[continent] Datenbank
			// cell_input_pesticide_maps
			sprintf(TableName, "%swq_load_%s.`cell_input_pesticide_maps`", MyDatabase, options_copyScen.continent_abb);
			TableExists = true;
			if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
				j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
				j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL COMMENT 'year',\n");
				j+=sprintf(sql+j, "  `month` int(10) unsigned NOT NULL,\n");
				j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
				j+=sprintf(sql+j, "  `ld-input` double default NULL COMMENT 'Masse des in Zelle applizierten Pestidids [t]',\n");
				j+=sprintf(sql+j, "  PRIMARY KEY (`IDScen`, `year`, `month`,`cell`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1;\n");

				if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);
			}

			// Ueberpruefen, ob Daten im Ziel schon gibt
			if (TableExists) {
				int counter = checkData_wqLoad(TableName, IDScen, -1, StartYear, EndYear);
				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

			// cell_input_rtf_irr
			sprintf(TableName, "%swq_load_%s.`cell_input_rtf_irr`", MyDatabase, options_copyScen.continent_abb);
			TableExists = true;
			if (!CopyInOneProject) { // wenn es nicht gleiche Projekt ist, es k�nnte sein, Tabelle nicht existiert
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
				j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
				j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL COMMENT 'ArcID',\n");
				j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL COMMENT 'year',\n");
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
				j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDScen`,`time`,`cell`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='irrigation returnflow (m3/month]';\n");

				if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);
			}

			// �berpr�fen, ob Daten im Ziel schon gibt
			if (TableExists) {
				int counter = checkData_wqLoad(TableName, IDScen, -1, StartYear, EndYear);
				if (counter>0) { // Daten existieren schon
					DatenExistsTables.push_back(TableName);
				}
			}

//			cout << "######################## DatenExistsTables.size() = " << DatenExistsTables.size() << endl;
			cout << "########################################################################" << endl;
			// Way : 1: abbrechen 2: nur neue hinzuf�gen 3: l�schen und hinzuf�gen
			short Way=3; // 3, weil wenn keine Daten gibt, wird nicht abgefragt, muss man alles hinzuf�gen
			if (DatenExistsTables.size()>0) {
				cout << "IDScen " << IDScen << " von project " << options_copyScen.project_id << " hat schon Daten in den Tabellen: \n";
				for (unsigned i=0; i<DatenExistsTables.size(); i++) {
					cout << DatenExistsTables[i] << endl;
				}
				bool isntOK = false;
				cout << endl<< "####################"<<endl;
				cout << "1: break\n";
				cout << "2: add only new data\n";
				cout << "3: delete and add \n\n";
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
					cout << "add only new data\n";
					break;
				case 3:
					cout << "delete and add \n";
					break;
				default:
					cout << "please only 1, 2 or 3!\n";
					isntOK = true;
					break;
				}
				} while (isntOK);
			} // Wenn Daten existieren

			// ########################## Daten einf�gen ############################
			// hier kommen wir an nur Way == 2 oder 3 ist
			char sql_insert[10000];

			// wq_load Datenbank
			// country_input
			sprintf(TableName, "%swq_load.`country_input`", MyDatabase);
			j=sprintf(sql_insert,    "INSERT INTO %s \n", TableName);
			j+=sprintf(sql_insert+j, "SELECT %d, c.country_id, c.time, c.rtf_man, c.fra_man_f, c.fra_man_t, \n", IDScen);
			j+=sprintf(sql_insert+j, "c.fra_man_p, c.fra_man_c, c.fra_man_g, c.fra_man_m, c.fra_man_nd, \n");
			j+=sprintf(sql_insert+j, "c.rtf_dom, c.rtf_irr, c.pop_tot, c.pop_urb, c.pop_rur, \n");
			j+=sprintf(sql_insert+j, "c.con_prim, c.con_sec, c.con_tert, c.con_quat, c.con_untr, c.stp_failure, c.to_treat_and_unknown, \n");
			j+=sprintf(sql_insert+j, "c.to_hanging_t, c.to_open_def FROM %swq_load.country_input c \n", refMyDatabase);
			j+=sprintf(sql_insert+j, "WHERE c.IDScen=%d ", options_copyScen.refScen);

			if (insertData_wq_load(TableName, sql_insert, Way, -1, IDScen, StartYear, EndYear,
					mySQLfile, &options_copyScen)) exit(1);

			// country_parameter_input
			sprintf(TableName, "%swq_load.`country_parameter_input`", MyDatabase);
			j=sprintf(sql_insert,    "INSERT INTO %s \n", TableName);
			j+=sprintf(sql_insert+j, "SELECT %d, c.parameter_id, c.country_id, c.time, c.ef,", IDScen);
			j+=sprintf(sql_insert+j, "c.conc_man_f, c.conc_man_t, c.conc_man_p, c.conc_man_c, c.conc_man_g, c.conc_man_m, c.conc_man_nd, \n");
			j+=sprintf(sql_insert+j, "c.conc_tds1, c.conc_tds2, c.conc_tds3, c.conc_tds4, c.c_geogen, c.conc_urb \n");
			j+=sprintf(sql_insert+j, "FROM %swq_load.country_parameter_input c ", refMyDatabase);
			j+=sprintf(sql_insert+j, "WHERE c.IDScen=%d AND c.parameter_id=%d ", options_copyScen.refScen, options_copyScen.parameter_id);

			if (insertData_wq_load(TableName, sql_insert, Way, options_copyScen.parameter_id, IDScen, StartYear, EndYear,
					mySQLfile, &options_copyScen)) exit(1);

			// parameter_input
			sprintf(TableName, "%swq_load.`parameter_input`", MyDatabase);
			j=sprintf(sql_insert,    "INSERT INTO %swq_load.parameter_input \n", MyDatabase);
			j+=sprintf(sql_insert+j, "SELECT %d, p.parameter_id, p.time, p.rem_prim, p.rem_sec, p.rem_tert, p.rem_untr, \n", IDScen);
			j+=sprintf(sql_insert+j, "p.treat_failure, p.rem_soil, p.red_fac_org, p.red_fac_inor, p.k_storage, p.k_soil, p.ks, \n");
			j+=sprintf(sql_insert+j, "p.sed_veloc, p.ke_tss_reg_alpha, p.ke_tss_reg_beta, p.teta, p.teta_lake \n");
			j+=sprintf(sql_insert+j, "FROM %swq_load.parameter_input p ", refMyDatabase);
			j+=sprintf(sql_insert+j, "WHERE p.IDScen=%d AND p.parameter_id=%d ",options_copyScen.refScen, options_copyScen.parameter_id);

			if (insertData_wq_load(TableName, sql_insert, Way, options_copyScen.parameter_id, IDScen, StartYear, EndYear,
					mySQLfile, &options_copyScen)) exit(1);

			// wq_load_[continent] Datenbank
			// cell_input
			sprintf(TableName, "%swq_load_%s.`cell_input`", MyDatabase, options_copyScen.continent_abb);
			j=sprintf(sql_insert,    "INSERT INTO %s \n", TableName);
			j+=sprintf(sql_insert+j, "SELECT %d, c.cell, c.time, c.country_id, c.pop_urb, c.pop_rur, c.pop_tot, \n", IDScen);
			j+=sprintf(sql_insert+j, "c.rtf_man, c.rtf_dom, c.rtf_irr, c.gdp, c.salinity, c.humidity, c.lu \n");
			j+=sprintf(sql_insert+j, "FROM %swq_load_%s.cell_input c ", refMyDatabase, options_copyScen.continent_abb);
			j+=sprintf(sql_insert+j, "WHERE c.IDScen=%d ",options_copyScen.refScen);

			if (insertData_wq_load(TableName, sql_insert, Way, -1, IDScen, StartYear, EndYear,
					mySQLfile, &options_copyScen)) exit(1);

			// cell_input_pesticide
			sprintf(TableName, "%swq_load_%s.`cell_input_pesticide`", MyDatabase, options_copyScen.continent_abb);
			j=sprintf(sql_insert,    "INSERT INTO %s \n", TableName);
			j+=sprintf(sql_insert+j, "SELECT %d, c.cell, c.organic_carbon \n", IDScen);
			j+=sprintf(sql_insert+j, "FROM %swq_load_%s.cell_input_pesticide c ", refMyDatabase, options_copyScen.continent_abb);
			j+=sprintf(sql_insert+j, "WHERE c.IDScen=%d ",options_copyScen.refScen);

			if (insertData_wq_load(TableName, sql_insert, Way, -1, IDScen, StartYear, EndYear,
					mySQLfile, &options_copyScen)) exit(1);

			// cell_input_pesticide_maps
			sprintf(TableName, "%swq_load_%s.`cell_input_pesticide_maps`", MyDatabase, options_copyScen.continent_abb);
			j=sprintf(sql_insert,    "INSERT INTO %s \n", TableName);
			j+=sprintf(sql_insert+j, "SELECT %d, c.time, c.month, c.cell, c.ld_input \n", IDScen);
			j+=sprintf(sql_insert+j, "FROM %swq_load_%s.cell_input_pesticide_maps c ", refMyDatabase, options_copyScen.continent_abb);
			j+=sprintf(sql_insert+j, "WHERE c.IDScen=%d ",options_copyScen.refScen);

			if (insertData_wq_load(TableName, sql_insert, Way, -1, IDScen, StartYear, EndYear,
					mySQLfile, &options_copyScen)) exit(1);

			// cell_input_rtf_irr
			sprintf(TableName, "%swq_load_%s.`cell_input_rtf_irr`", MyDatabase, options_copyScen.continent_abb);
			j=sprintf(sql_insert,    "INSERT INTO %s \n", TableName);
			j+=sprintf(sql_insert+j, "SELECT %d, c.cell, c.time, ", IDScen);
			j+=sprintf(sql_insert+j, "c.v1, c.v2, c.v3, c.v4, c.v5, c.v6, c.v7, c.v8, c.v9, c.v10, c.v11, c.v12 \n");
			j+=sprintf(sql_insert+j, "FROM %swq_load_%s.cell_input_rtf_irr c ", refMyDatabase, options_copyScen.continent_abb);
			j+=sprintf(sql_insert+j, "WHERE c.IDScen=%d ",options_copyScen.refScen);

			if (insertData_wq_load(TableName, sql_insert, Way, -1, IDScen, StartYear, EndYear,
					mySQLfile, &options_copyScen)) exit(1);

		} // for(IDScen)

		// unabh�ngige von IDScen Daten nur dann kopieren, wenn Datenbanken MyDatabase und refMyDatabase nicht gleich sind
		if (!CopyInOneProject) {
			// hier MyDatabase und refMyDatabase unterschiedlich
			mySQLfile << "#########################################################" << endl;
			mySQLfile << "# independent from IDScen tables" << endl;
			DatenExistsTables.clear();

			char TableName[maxcharlength];

			if(toCalc==0 ||toCalc==3 || toCalc==4) {
				// geogenic_background_input
				sprintf(TableName, "`%swq_load`.`geogenic_background_input`", MyDatabase);
				j=sprintf(sql,    "CREATE TABLE  %s (\n", TableName);
				j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL default '0',\n");
				j+=sprintf(sql+j, "  `LU` int(11) NOT NULL,\n");
				j+=sprintf(sql+j, "  `geo_back` double default '-9999.99' COMMENT 'geogenic background: deposition rate t/(ha*a) for TN and TP,\r\nsurface runoff conc [t/L] fpr BOD; not applied for FC',\n");
				j+=sprintf(sql+j, "  `BibTeX_Key` varchar(70) character set latin1 NOT NULL COMMENT 'Literature ID',\n");
				j+=sprintf(sql+j, "  `comment` text collate latin1_german1_ci COMMENT 'space for special comments',\n");
				j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`parameter_id`,`LU`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci ROW_FORMAT=FIXED;\n");
				if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);

				if (TableExists) {
					j=sprintf(sql,    "SELECT count(*) as counter FROM %s \n", TableName);
					j+=sprintf(sql+j, "WHERE parameter_id=%d;", options_copyScen.parameter_id);
					query.reset();
					query << sql;
					res = query.store();
					int  counter=res.at(0)["counter"];
					cout << TableName << " Zeilen: "<< counter << endl;

					if (counter>0) { // Daten existieren schon
						DatenExistsTables.push_back(TableName);
					}
				}

			}
			if(toCalc==3 || toCalc==4){ //only for TN und TP
				// ind_fert_use_input
				sprintf(TableName, "`%swq_load`.`ind_fert_use_input`", MyDatabase);
				j=sprintf(sql,    "CREATE TABLE  %s (\n", TableName);
				j+=sprintf(sql+j, "  `country_id` int(10) unsigned NOT NULL default '0' COMMENT 'ISOnr country',\n");
				j+=sprintf(sql+j, "  `crop_type_id` int(10) unsigned NOT NULL COMMENT 'crop type',\n");
				j+=sprintf(sql+j, "  `YearFrom` int(10) unsigned NOT NULL default '0' COMMENT 'values apply from this year',\n");
				j+=sprintf(sql+j, "  `rate_N` double default NULL COMMENT 'N application [t/km^2] per crop & country',\n");
				j+=sprintf(sql+j, "  `rate_P` double default NULL COMMENT 'P application [t/km^2] per crop & country',\n");
				j+=sprintf(sql+j, "  `bibTex_key` varchar(100) character set latin1 default NULL,\n");
				j+=sprintf(sql+j, "  `comment` text collate latin1_german1_ci,\n");
				j+=sprintf(sql+j, "  PRIMARY KEY  (`country_id`,`crop_type_id`,`YearFrom`)\n");
				j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='ind. fertililizer application rate by crop and country and p';\n");
				switch (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) {
				case 1: exit(1); break;  // Fehler passiert
				case 2: // Create Table - Anweisung muss dokumentiert werden
					mySQLfile << sql << endl << endl; break;
				case 0: break;
				}

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

			}

			// ls_exr_input
			sprintf(TableName, "`%swq_load`.`ls_exr_input`", MyDatabase);
			j=sprintf(sql,    "CREATE TABLE %s (\n", TableName);
			j+=sprintf(sql+j, "  `parameter_id` int(10) unsigned NOT NULL,\n");
			j+=sprintf(sql+j, "  `LS` int(10) unsigned NOT NULL COMMENT 'livestock ID',\n");
			j+=sprintf(sql+j, "  `IDFAOReg` int(10) unsigned NOT NULL COMMENT 'ID FAO Region',\n");
			j+=sprintf(sql+j, "  `ls_exr_rate` double default NULL COMMENT 'excreation of one animal [t/a*head], FC: [10^10 cfu/head*a] in manure calculated from _ls_fao_conv and _lsu_exr',\n");
			j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`parameter_id`,`LS`,`IDFAOReg`)\n");
			j+=sprintf(sql+j, ") ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_german1_ci COMMENT='excretion rate of substance in manure by region';\n");
			if (CreateTable(TableName, sql, options_copyScen.SQLexecute, mySQLfile, TableExists)) exit (1);

			if (TableExists) {
				j=sprintf(sql,    "SELECT count(*) as counter FROM %s \n", TableName);
				j+=sprintf(sql+j, "WHERE parameter_id=%d;", options_copyScen.parameter_id);
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
				cout << " Project " << options_copyScen.project_id << " hat schon Daten in den Tabellen: \n";
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
			if(toCalc==0 ||toCalc==3 || toCalc==4) {
				// geogenic_background_input
				sprintf(TableName, "`%swq_load`.`geogenic_background_input`", MyDatabase);
				query.reset();
				j=sprintf(sql,    "DELETE FROM %swq_load.geogenic_background_input \n", MyDatabase);
				j+=sprintf(sql+j, "WHERE parameter_id=%d;", options_copyScen.parameter_id);
				query << sql;
				mySQLfile << sql << endl << endl;
				if (options_copyScen.SQLexecute) query.execute();

				query.reset();
				j=sprintf(sql,    "INSERT INTO %swq_load.geogenic_background_input \n", MyDatabase);
				j+=sprintf(sql+j, "SELECT g.parameter_id, g.LU, g.geo_back, g.BibTeX_Key, g.comment \n");
				j+=sprintf(sql+j, "FROM %swq_load.geogenic_background_input g ", refMyDatabase);
				j+=sprintf(sql+j, "WHERE g.parameter_id=%d;", options_copyScen.parameter_id);
				query << sql;
				mySQLfile << sql << endl << endl;
				if (options_copyScen.SQLexecute) query.execute();
			}
			if(toCalc==3 || toCalc==4){ //only for TN und TP
				// ind_fert_use_input
				sprintf(TableName, "`%swq_load`.`ind_fert_use_input`", MyDatabase);
				query.reset();
				j=sprintf(sql,    "DELETE FROM %swq_load.ind_fert_use_input;", MyDatabase);
				query << sql;
				mySQLfile << sql << endl << endl;
				if (options_copyScen.SQLexecute) query.execute();

				query.reset();
				j=sprintf(sql,    "INSERT INTO %swq_load.ind_fert_use_input \n", MyDatabase);
				j+=sprintf(sql+j, "SELECT * FROM %swq_load.ind_fert_use_input g;", refMyDatabase);
				query << sql;
				mySQLfile << sql << endl << endl;
				if (options_copyScen.SQLexecute) query.execute();
			}

			// ls_exr_input
			sprintf(TableName, "`%swq_load`.`ls_exr_input`", MyDatabase);
			query.reset();
			j=sprintf(sql,    "DELETE FROM %swq_load.ls_exr_input ", MyDatabase);
			j+=sprintf(sql+j, "WHERE parameter_id=%d;", options_copyScen.parameter_id);
			query << sql;
			mySQLfile << sql << endl << endl;
			if (options_copyScen.SQLexecute) query.execute();

			query.reset();
			j=sprintf(sql,    "INSERT INTO %swq_load.ls_exr_input \n", MyDatabase);
			j+=sprintf(sql+j, "SELECT * FROM %swq_load.ls_exr_input l ", refMyDatabase);
			j+=sprintf(sql+j, "WHERE l.parameter_id=%d;", options_copyScen.parameter_id);
			query << sql;
			mySQLfile << sql << endl << endl;
			if (options_copyScen.SQLexecute) query.execute();

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
	cout <<"\nProgram copy_scen ended at: " << getTimeString() << endl << endl;
	return 0;
} // main()

int checkData_wqLoad(const char* TableName, int IDScen, int parameter_id, int start, int end) {
	// wenn in der Tabelle kein Feld parameter_id gibt, parameter_id soll negativ sein
	// wenn abfrage soll 'time between start and end' sein, beide Jahre ausf�llen
	// wenn end <=0, wird time = start abgefragt
	// wenn start und end <=0, time wird nicht abgefragt
	char sql[10000]; // variable for SQL-query
	int j;

	j=sprintf(sql,    "SELECT count(*) as counter FROM %s \n", TableName);
	j+=sprintf(sql+j, "WHERE IDScen=%d ", IDScen); //AND time between %d and %d LIMIT 1;", IDScen, StartYear, EndYear);
	if (parameter_id>=0)
		j+=sprintf(sql+j, "AND parameter_id=%d ", parameter_id);
	if (start>0 && end>0)
		j+=sprintf(sql+j, "AND time between %d and %d ", start, end);
	else if (start>0)
		j+=sprintf(sql+j, "AND time = %d ", start);

	query.reset();
	query << sql;
//	cout << query << endl;
	res = query.store();
	int  counter=res.at(0)["counter"];
	cout << TableName << " Zeilen: "<< counter << endl;

	return counter;
} // checkData_wqLoad()

bool insertData_wq_load(const char* TableName, const char* sql_insert, short Way, int parameter_id, int IDScen, int StartYear, int EndYear
		, ofstream& mySQLfile, options_copyScenClass* options_copyScen_ptr) {
	// wenn in der Tabelle kein Feld parameter_id gibt, parameter_id soll negativ sein
	// wenn in der Tabelle kein Feld time gibt, StartYear und EndYear sollen negativ sein
	char sql[10000]; int j;

	if (isTableExists(TableName) && Way==2) {
		if (StartYear>0 && EndYear>0) {
			for (int year = StartYear; year <= EndYear; year++) {
				int counter = checkData_wqLoad(TableName, IDScen, parameter_id, year, -1);
				if (counter==0) { // Daten fuer das Jahr nicht gibt, einfuegen
					query.reset();
					j=sprintf(sql, "%s AND time = %d;", sql_insert, year);
					query << sql;
					mySQLfile << sql << endl << endl;
					if (options_copyScen_ptr->SQLexecute) query.execute();
				}

			}

		} else {
			int counter = checkData_wqLoad(TableName, IDScen, parameter_id, -1, -1);
			if (counter==0) { // Daten fuer das Jahr nicht gibt, einfuegen
				query.reset();
				query << sql;
				mySQLfile << sql << endl << endl;
				if (options_copyScen_ptr->SQLexecute) query.execute();
			}

		}

	} else {
		query.reset();
		j=sprintf(sql,    "DELETE FROM %s \n", TableName);
		j+=sprintf(sql+j, "WHERE IDScen=%d ", IDScen);
		if (parameter_id>=0)
			j+=sprintf(sql+j, "AND parameter_id=%d ", parameter_id);
		if (StartYear>0 && EndYear>0) {
			j+=sprintf(sql+j, "AND time between %d and %d;", StartYear, EndYear);
		} else {
			j+=sprintf(sql+j, ";");
		}
		query << sql;
		mySQLfile << sql << '\n' << '\n';
		if (options_copyScen_ptr->SQLexecute) query.execute();

		query.reset();
		if (StartYear>0 && EndYear>0) {
			j=sprintf(sql, "%s AND time between %d and %d;", sql_insert, StartYear, EndYear);
		} else {
			j=sprintf(sql, "%s;", sql_insert);
		}
		query << sql;
		mySQLfile << sql << endl << endl;
		if (options_copyScen_ptr->SQLexecute) query.execute();

	}
	return 0;
}
