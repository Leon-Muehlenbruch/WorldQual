/*
 * rtf_input_corr_factor.cpp
 *
 *This class reads the rtf inputs which are corrected by river availability,
 *hence the sum of rtf can not exceed the river availability
 *
 *  Created on: 01.12.2014
 *      Author: reder
 */

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>  //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "timestring.h"
#include "country.h"
#include "general_functions.h"
//#include "../options/options_scen.h"
#include "options_rtf_input.h"

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
	cout <<"\nProgram rtf_input_corr_factor started at: " << getTimeString() << endl << endl;
	options_rtfinput options_scen(argc, &argv[0], "startYear endYear");

	if (argc<3) {
		options_scen.Help();
//		cout << "\n./rtf_input_corr_fact startYear endYear [-ooptions_file]\n\n";
		return 0;
	}

	char filename[maxcharlength];

	if(!IsNumber(argv[1]) || !IsNumber(argv[2]) ) {
		cerr << "ERROR: Wrong parameter" << endl;
		options_scen.Help();
//		cerr << "usage:./rtf_input_corr_fact startYear endYear [-ooptions_file]"<<endl;
		exit(1);
	}
	int start     = atoi(argv[1]);
	int end       = atoi(argv[2]);

	if (start>end) {
		cerr << "ERROR: Wrong parameter. start year> end year!!!" << endl;
		options_scen.Help();
//		cerr << "usage: ./rtf_input_corr_fact startYear endYear [-ooptions_file] "<<endl;
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

		char start_db[maxcharlength]; //sprintf(start_db, "%swq_load_%s", MyDatabase, options_scen.continent_abb);
		sprintf(start_db, "wq_general");

		if (!con.connect(&start_db[0], options_scen.MyHost, options_scen.MyUser, options_scen.MyPassword,3306))
		{
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl << endl; //return 0;
		////////////////////////////////

		// checking project_id and project- and database name, fill MyDatabase
		if (getProject(options_scen.project_id, &options_scen, con, "wq_load")) exit(1);


		cout<<"ATTENTION: if rft_input is used just for one continent:\n"
			<<"For countries, which are part of more than one continent, the return flow will only be calculated for the used  \n"
			<<"continent. This will lead to an overestimation in wq_load.cpp where the proportion of each cell according to \n"
		    <<"the return flow of each country is calculated. This problem can only be solved if data for all continents,  \n"
		    <<"where the country is part of, is supplied.\n"
		    <<"If the rtf is calculated for all continents, than the correct value will only be inserted into \n"
		    <<"wq_load.country_input with the run for the last continent! (for further reference see comments in the code)\n"<<endl;


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
			if(rowNum>(unsigned int)countries.number){cerr<<"Error: row number can not be larger than number of countries!"<<endl;}
			else count_cont[rowNum][0]  = res[rowNum]["isonum"];
		}

		//select the continents the countries is part of
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


	/*	for(int i=0;i<countries.number;i++)
			for(int j=0;j<4;j++)
				cout<<"count_cont["<<i<<"]["<<j<<"]: "<<count_cont[i][j]<<endl;
	 */

		///=================================
		// Parameter Landmaske einlesen
		if(getCELLS(&options_scen, &continent)) exit(1); cout << "\nParameter Landmaske wurden eingelesen\n"<<endl;

		// bei UNF-Dateien wird Umwandlung gcrc -> ArcID benoetigt
		int *gcrc;  // gcrc[gcrc-1]=ArcID
		int *grow;  // grow[gcrc-1]=row
		gcrc = new int[continent.ng];
		grow = new int[continent.ng];
		if(getGCRC(gcrc, grow, &options_scen, &continent)) {cerr << "error: getGCRC\n"; exit(1);}

		double (*man_ww) = new double[continent.ng];  // [m3]
		if (!man_ww) {cerr << "not enough memory!\n"; exit(1);}
		double (*man_wc) = new double[continent.ng];  // [m3]
		if (!man_wc) {cerr << "not enough memory!\n"; exit(1);}
		double (*man_wcool) = new double[continent.ng];  // [m3]
		if (!man_wcool) {cerr << "not enough memory!\n"; exit(1);}

		double (*irr_rtf)[12] = new double[continent.ng][12];  // [m3]
		if (!irr_rtf) {cerr << "not enough memory!\n"; exit(1);}

		double (*corr_factor)[12] = new double[continent.ng][12];

		int dataTypeSize, valuesPerCell;
		for (int actual_year=start; actual_year<=end; actual_year++) {
			cout << actual_year<<endl;
			dataTypeSize = 4;
			valuesPerCell = 1;


			// Daten aus UNF-Dateien einlesen
			sprintf(filename,"%s/G_MAN_WW_m3_%d.UNF0", options_scen.input_dir_griddi, actual_year);
			if (readInputFile(filename, dataTypeSize, valuesPerCell, continent.ng, man_ww, gcrc, &options_scen, &continent, 0.)) return 1;
			cout << "year " << actual_year << " manufacturing ww done\n";

			sprintf(filename,"%s/G_MAN_WC_m3_%d.UNF0", options_scen.input_dir_griddi, actual_year);
			if (readInputFile(filename, dataTypeSize, valuesPerCell, continent.ng, man_wc, gcrc, &options_scen, &continent, 0.)) return 1;
			cout << "year " << actual_year << " manufacturing wc done\n";

			sprintf(filename,"%s/G_MAN_WCOOL_m3_%d.UNF0", options_scen.input_dir_griddi, actual_year);
			if (readInputFile(filename, dataTypeSize, valuesPerCell, continent.ng, man_wcool, gcrc, &options_scen, &continent, 0.)) return 1;
			cout << "year " << actual_year << " manufacturing wcool done\n";

			valuesPerCell = 12;

			if(strcmp(MyDatabase,"sen_anlys_")!=0){ //not read for sensitivity analysis

				sprintf(filename,"%s/G_IRRIG_RTF_SW_m3_%d.12.UNF0", options_scen.input_dir_gwsws, actual_year);
				if (readInputFile(filename, dataTypeSize, valuesPerCell, continent.ng, &irr_rtf[0][0], gcrc, &options_scen, &continent, 0.)) return 1;
				cout << "year " << actual_year << " irrigation ww done\n";

			}

			sprintf(filename,"%s/G_CORR_FACT_RTF_%d.12.UNF0", options_scen.input_dir_corrfactor, actual_year);
			if (readInputFile(filename, 4, 12, continent.ng, &corr_factor[0][0], gcrc, &options_scen, &continent)) exit(1);

			cout<<"corr_factor[15553][0]="<<corr_factor[15553][0]<<endl;
			
			char TableName_wu[maxcharlength];
			char TableName_cellInput[maxcharlength];
			char TableName_cellInput_rtf_irr[maxcharlength];
			if (!options_scen.IDInTableName) {
				sprintf(TableName_wu, "%swq_load_%s.`wateruse`", MyDatabase, options_scen.continent_abb);
				sprintf(TableName_cellInput_rtf_irr, "%swq_load_%s.`cell_input_rtf_irr`", MyDatabase, options_scen.continent_abb);
				sprintf(TableName_cellInput, "%swq_load_%s.`cell_input`", MyDatabase, options_scen.continent_abb);
			} else {
				sprintf(TableName_wu, "%swq_load_%s.`wateruse_%d`", MyDatabase, options_scen.continent_abb, options_scen.IDScen);
				sprintf(TableName_cellInput_rtf_irr, "%swq_load_%s.`cell_input_rtf_irr_%d`", MyDatabase, options_scen.continent_abb, options_scen.IDScen);
				sprintf(TableName_cellInput, "%swq_load_%s.`cell_input_%d`", MyDatabase, options_scen.continent_abb, options_scen.IDScen);
			}
			{
				// es könnte passieren, dass Tabelle nicht existiert
				char sql[10000]; // variable for SQL-query
				bool TableExists;
				int j;
				ofstream mySQLfile; // dummy, CreateTable erwartet eine Datei, um den Code zu schreiben. Wenn aber Datei nicht geöffnet ist, wird cout benutzt

				
				j=sprintf(sql,    "CREATE TABLE  %s (", TableName_wu);
				if (!options_scen.IDInTableName) {
					j+=sprintf(sql+j, "  `IDScen` int(10) unsigned NOT NULL default '1',\n");
				}
				j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL COMMENT 'year',\n");
				j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
				
				j+=sprintf(sql+j, "  `dom_wwd` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'domestic waterwithdrawal [m³/a]',\n");
				j+=sprintf(sql+j, "  `dom_con` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'domestic water consum [m³/a]',\n");
				j+=sprintf(sql+j, "  `man_wwd` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'manufacturing waterwithdrawal [m³/a]',\n");
				j+=sprintf(sql+j, "  `man_con` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'manufacturing water consum [m³/a]',\n");
				j+=sprintf(sql+j, "  `man_wcool` DOUBLE NULL DEFAULT NULL COMMENT 'manufacturing water used for cooling [m³/a]',\n");
				j+=sprintf(sql+j, "  `irr_rtf` DOUBLE NULL DEFAULT NULL COMMENT 'irrigation return flow [m³/a]',\n");
				j+=sprintf(sql+j, "  `man_rtf_1` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_2` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_3` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_4` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_5` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_6` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_7` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_8` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_9` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_10` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_11` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf_12` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  `man_rtf` DOUBLE NULL DEFAULT NULL,\n");
				if (!options_scen.IDInTableName) {
					j+=sprintf(sql+j, "  PRIMARY KEY (`IDScen`, `time`, `cell`) USING BTREE\n");
				} else {
					j+=sprintf(sql+j, "  PRIMARY KEY (`time`, `cell`) USING BTREE\n");
				}
				j+=sprintf(sql+j, "  ) COLLATE='latin1_german1_ci' ENGINE=MyISAM;\n");


				if (CreateTable(TableName_wu, sql, 1, mySQLfile, TableExists)) exit (1);

				j=sprintf(sql,    "CREATE TABLE %s (", TableName_cellInput_rtf_irr);
				if (!options_scen.IDInTableName) {
					j+=sprintf(sql+j, "  	`IDScen` INT(10) UNSIGNED NOT NULL DEFAULT '1',\n");
				}
				j+=sprintf(sql+j, "  	`cell` INT(10) UNSIGNED NOT NULL COMMENT 'ArcID',\n");
				j+=sprintf(sql+j, "  	`time` INT(10) UNSIGNED NOT NULL COMMENT 'year',\n");
				j+=sprintf(sql+j, "  	`v1` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v2` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v3` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v4` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v5` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v6` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v7` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v8` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v9` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v10` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v11` DOUBLE NULL DEFAULT NULL,\n");
				j+=sprintf(sql+j, "  	`v12` DOUBLE NULL DEFAULT NULL,\n");
				if (!options_scen.IDInTableName) {
					j+=sprintf(sql+j, "  	PRIMARY KEY (`IDScen`, `time`, `cell`) USING BTREE\n");
				} else {
					j+=sprintf(sql+j, "  	PRIMARY KEY (`time`, `cell`) USING BTREE\n");
				}
				j+=sprintf(sql+j, "  ) COMMENT='irrigation returnflow (m3/month]'\n");
				j+=sprintf(sql+j, "  COLLATE='latin1_swedish_ci' ENGINE=MyISAM;\n");
				
				if (CreateTable(TableName_cellInput_rtf_irr, sql, 1, mySQLfile, TableExists)) exit (1);

				// cell_input Tabellemuss schon existieren und für die ganze Zeitreihe Zeilen geben. Hier wird nur update für rtf geben, kein insert!
				// j=sprintf(sql,    "CREATE TABLE %s (", TableName_cellInput);
				// if (!options_scen.IDInTableName) {
				// 	j+=sprintf(sql+j, "  	`IDScen` INT(10) UNSIGNED NOT NULL DEFAULT '1',\n");
				// }
				// j+=sprintf(sql+j, "  	`cell` INT(10) UNSIGNED NOT NULL,\n");
				// j+=sprintf(sql+j, "  	`time` INT(10) UNSIGNED NOT NULL DEFAULT '0',\n");
				// j+=sprintf(sql+j, "  	`country_id` INT(11) NOT NULL DEFAULT '-9999',\n");
				// j+=sprintf(sql+j, "  	`pop_urb` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'urban cell population [number]',\n");
				// j+=sprintf(sql+j, "  	`pop_rur` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'rural cell population [number]',\n");
				// j+=sprintf(sql+j, "  	`pop_tot` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'total cell population [number]',\n");
				// j+=sprintf(sql+j, "  	`rtf_man` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'manufacturing returnflow (m3/a]',\n");
				// j+=sprintf(sql+j, "  	`rtf_dom` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'domestic returnflow (m3/a]',\n");
				// j+=sprintf(sql+j, "  	`rtf_irr` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'irrigation returnflow (m3/a]',\n");
				// j+=sprintf(sql+j, "  	`gdp` DOUBLE UNSIGNED NULL DEFAULT NULL COMMENT 'gdp per capita [US$]',\n");
				// j+=sprintf(sql+j, "  	`salinity` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT 'primary salinity of soil 1 no, 2 yes',\n");
				// j+=sprintf(sql+j, "  	`humidity` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT 'humid: 1, arid: 2',\n");
				// j+=sprintf(sql+j, "  	`lu` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT 'land use class [-]',\n");
				// j+=sprintf(sql+j, "  	PRIMARY KEY (`IDScen`, `time`, `cell`) USING BTREE,\n");
				// j+=sprintf(sql+j, "  	INDEX `Index_country_id` (`country_id`) USING BTREE\n");
				// j+=sprintf(sql+j, "  ) COLLATE='latin1_german1_ci' ENGINE=MyISAM;\n");
				// 
				// if (CreateTable(TableName_cellInput, sql, 1, mySQLfile, TableExists)) exit (1);

			}

			// wenn in der Tabelle schon Daten fuer das Jahr gab, loeschen
			query.reset();
			if (!options_scen.IDInTableName) {
				query << "DELETE FROM "<< TableName_wu << " WHERE `IDScen`="<<options_scen.IDScen<<" AND `time`= " << actual_year << ";";
			} else {
				query << "DELETE FROM "<< TableName_wu << " WHERE `time`= " << actual_year << ";";
			}
			resExec = query.execute();
			cout<<"delete from wateruse: "<<query<<endl;
			cout << resExec.rows()<<endl;
			//cout << resExec.success<<endl;
			cout << resExec.insert_id()<<endl;
			cout << resExec.info()<<endl;

			if(strcmp(MyDatabase,"sen_anlys_")!=0){ //not read for sensitivity analysis
				query.reset();
				if (!options_scen.IDInTableName) {
					query << "DELETE FROM "<< TableName_cellInput_rtf_irr << " WHERE `IDScen`="<<options_scen.IDScen<<" AND `time`= " << actual_year << ";";
				} else {
					query << "DELETE FROM "<< TableName_cellInput_rtf_irr << " WHERE `time`= " << actual_year << ";";
				}
				resExec = query.execute();
				cout<<"delete from cel_input_rtf_irr: "<<query<<endl;
				cout << resExec.rows()<<endl;
				//cout << resExec.success<<endl;
				cout << resExec.insert_id()<<endl;
				cout << resExec.info()<<endl;
			}

			// Daten in die Tabelle einfuegen
			for (int cell=0; cell<continent.ng; cell++) {

				//if error in input, e.g. wrong value from statistics
				if(man_ww[cell]<(man_wc[cell]+man_wcool[cell])){
					man_wc[cell]=man_ww[cell];
					man_wcool[cell]=0.0;
				}

				query.reset();
				query << setprecision(30);
				if (!options_scen.IDInTableName) {
					query << "insert into "<< TableName_wu << " (`IDScen`, `time`, `cell`, "
					  << "`dom_wwd`, `dom_con`, `man_wwd`, `man_con`, `man_wcool`) "
					  << "values ( " <<options_scen.IDScen<< ", "<< actual_year << ", " << cell+1<<", ";
				} else {
					query << "insert into "<< TableName_wu << " (`time`, `cell`, "
					  << "`dom_wwd`, `dom_con`, `man_wwd`, `man_con`, `man_wcool`) "
					  << "values ( " << actual_year << ", " << cell+1<<", ";
				}
				query <<"NULL, "; //used to be dom_ww

				query <<"NULL, ";//used to be dom_wc

				if (man_ww[cell]==NODATA) query <<"NULL, ";
				else query <<man_ww[cell]<<", ";

				if (man_wc[cell]==NODATA) query <<"NULL, ";
				else query <<man_wc[cell]<<", ";

				if (man_wcool[cell]==NODATA) query <<"NULL);";
				else query <<man_wcool[cell]<<");";

				if(cell==51515)
					cout<<"insert into wateruse: "<<query<<endl;

				resExec = query.execute();

				if(strcmp(MyDatabase,"sen_anlys_")!=0){ //not used for sensitivity analysis
					query.reset();
					query << setprecision(30);
					if (!options_scen.IDInTableName) {
						query << "insert into "<< TableName_cellInput_rtf_irr << " (`IDScen`, `time`, `cell`, "
							<< "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) "
							<< "values ( " <<options_scen.IDScen<< ", "<< actual_year << ", " << cell+1<<", ";
					} else {
						query << "insert into "<< TableName_cellInput_rtf_irr << " (`time`, `cell`, "
							<< "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`) "
							<< "values ( " << actual_year << ", " << cell+1<<", ";
					}

					for (int month=0; month<12; month++) {
						if (irr_rtf[cell][month]==NODATA) query <<"NULL";
						else query <<irr_rtf[cell][month]*corr_factor[cell][month];

						//if(cell==145968)
						//	cout<<"irr_rtf["<<cell<<"]["<<month<<"]"<<irr_rtf[cell][month]
						//	     <<"corr_factor["<<cell<<"]["<<month<<"]="<<corr_factor[cell][month]<<query<<endl;

						if(month<11) query << ", ";
						else query<<");";
					} //for(month)


					if(cell==51515)
						cout<<"insert into cell input rtf irr: "<<query<<endl;

					resExec = query.execute();
				}//if (not used for sensitivity analysis)

			} //for(cell)

			if(strcmp(MyDatabase,"sen_anlys_")!=0){ //not used for sensitivity analysis
				query.reset();
				if (!options_scen.IDInTableName) {
					query <<"UPDATE "<< TableName_wu << " w, "<< TableName_cellInput_rtf_irr <<" m "
					 <<"SET w.`irr_rtf`=m.`v1`+m.`v2`+m.`v3`+m.`v4`+m.`v5`+m.`v6`+m.`v7`+m.`v8`+m.`v9`+m.`v10`+m.`v11`+m.`v12` "
					 <<"WHERE w.`IDScen`="<<options_scen.IDScen<<" AND w.`time`=" << actual_year<<" AND m.IDScen=w.IDScen AND m.`time`=w.`time` AND m.cell=w.cell;";
				} else {
					query <<"UPDATE "<< TableName_wu << " w, "<< TableName_cellInput_rtf_irr <<" m "
					 <<"SET w.`irr_rtf`=m.`v1`+m.`v2`+m.`v3`+m.`v4`+m.`v5`+m.`v6`+m.`v7`+m.`v8`+m.`v9`+m.`v10`+m.`v11`+m.`v12` "
					 <<"WHERE w.`time`=" << actual_year<<" AND m.`time`=w.`time` AND m.cell=w.cell;";
				}

				cout<<"update wateruse und celll input rtf irr: "<<query<<endl;
				resExec = query.execute();
			}

			if(strcmp(MyDatabase,"sen_anlys_")!=0){ //not used for sensitivity analysis
				query.reset();
				if (!options_scen.IDInTableName) {
					query <<"UPDATE "<< TableName_wu <<" w, "
						<< TableName_cellInput << " c "
						<< "SET c.`rtf_dom`=NULL, "
						<< "c.`rtf_man`= (w.`man_wwd`- w.`man_con` - `man_wcool`), "
						<< "c.`rtf_irr`= w.`irr_rtf` "
						<< "WHERE w.`IDScen`="<<options_scen.IDScen<<" AND w.`time`="<< actual_year
						<< " AND c.`IDScen`=w.`IDScen` AND c.`time`=w.`time` AND c.cell=w.cell;";
				} else {
					query <<"UPDATE "<< TableName_wu <<" w, "
						<< TableName_cellInput << " c "
						<< "SET c.`rtf_dom`=NULL, "
						<< "c.`rtf_man`= (w.`man_wwd`- w.`man_con` - `man_wcool`), "
						<< "c.`rtf_irr`= w.`irr_rtf` "
						<< "WHERE w.`time`="<< actual_year
						<< " AND c.`time`=w.`time` AND c.cell=w.cell;";
				}

				cout<<"update wateruse und celll input: "<<query<<endl;
				resExec = query.execute();
			} else { //for sensitvity analysis
				query.reset();
				if (!options_scen.IDInTableName) {
					query <<"UPDATE "<< TableName_wu <<" w, "
						<< TableName_cellInput << " c "
						<< "SET c.`rtf_dom`=NULL, "
						<< "c.`rtf_man`= (w.`man_wwd`- w.`man_con` - `man_wcool`) "
						<< "WHERE w.`IDScen`="<<options_scen.IDScen<<" AND w.`time`="<< actual_year
						<< " AND c.`IDScen`=w.`IDScen` AND c.`time`=w.`time` AND c.cell=w.cell;";
				} else {
					query <<"UPDATE "<< TableName_wu <<" w, "
						<< TableName_cellInput << " c "
						<< "SET c.`rtf_dom`=NULL, "
						<< "c.`rtf_man`= (w.`man_wwd`- w.`man_con` - `man_wcool`) "
						<< "WHERE w.`time`="<< actual_year
						<< " AND c.`time`=w.`time` AND c.cell=w.cell;";
				}

				cout<<"update wateruse und celll input: "<<query<<endl;
				resExec = query.execute();
			}
			//cout << "rows     : "<< resExec.rows<<endl;
			//cout << "success  : "<< resExec.success<<endl;
			//cout << "insert_id: "<< resExec.insert_id<<endl;
			//cout << "info     : "<< resExec.info<<endl;

			//If this if condition applies, this is usually the case that some cells already contain
			//the same value as to be updated, in this case MySQL does not do an update on those cells
			if (resExec.rows()!=(unsigned int)continent.ng)
			  cout << "ACHTUNG!!! Jahr "<<actual_year<< ": es wurden nicht alle Daten in die Tabelle cell_input uebertragen!\n"
			   << " es wurde erwartet "<< continent.ng<< " wurde in die Tabelle uebertragen: "<<resExec.rows()<<endl;


			//update wq_load.country_input; at this point the value for countries which are part
			//of more than one continent is not jet correct
			query.reset();
			if (!options_scen.IDInTableName) {
				query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
					  <<"JOIN (SELECT sum(c1.`rtf_man`) as 'rtf_man', sum(c1.`rtf_dom`) as 'rtf_dom', "
					  <<"sum(c1.`rtf_irr`) as 'rtf_irr', c1.`country_id`, c1.`IDScen`,  c1.`time` "
					  <<"FROM "<< TableName_cellInput <<" c1 "
					  <<"WHERE c1.`IDScen`="<<options_scen.IDScen<<" AND c1.`time`="<<actual_year
					  <<" GROUP BY c1.`country_id`) as sub "
					  <<"ON c.`country_id`=sub.`country_id` "
					  <<"SET c.`rtf_man`= sub.`rtf_man`, c.`rtf_dom`= sub.`rtf_dom`, c.`rtf_irr`= sub.`rtf_irr` "
					  <<"WHERE c.`IDScen`="<<options_scen.IDScen<<" AND c.`time`="<<actual_year<<";";
			} else {
				query <<"UPDATE "<<MyDatabase<<"wq_load.country_input c "
					  <<"JOIN (SELECT sum(c1.`rtf_man`) as 'rtf_man', sum(c1.`rtf_dom`) as 'rtf_dom', "
					  <<"sum(c1.`rtf_irr`) as 'rtf_irr', c1.`country_id`, " << options_scen.IDScen << ",  c1.`time` "
					  <<"FROM "<< TableName_cellInput <<" c1 "
					  <<"WHERE c1.`time`="<<actual_year
					  <<" GROUP BY c1.`country_id`) as sub "
					  <<"ON c.`country_id`=sub.`country_id` "
					  <<"SET c.`rtf_man`= sub.`rtf_man`, c.`rtf_dom`= sub.`rtf_dom`, c.`rtf_irr`= sub.`rtf_irr` "
					  <<"WHERE c.`IDScen`="<<options_scen.IDScen<<" AND c.`time`="<<actual_year<<";";
			}

			cout<<"update country_input: "<<query<<endl;
			resExec = query.execute();


//			if(options_scen.IDVersion==3 && strcmp(MyDatabase,"sen_anlys_")!=0){
//				setCountryInput(countries.number, count_cont, "rtf_man", "rtf_dom", "rtf_irr",  options_scen.IDScen,  actual_year);
//			}
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
	cout <<"\nProgram rtf_input_corr_factor ended at: " << getTimeString() << endl << endl;
	return 0;
}// main()

