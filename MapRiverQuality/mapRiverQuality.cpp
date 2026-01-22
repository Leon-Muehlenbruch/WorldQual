// 10-12-2008
// Ellen Kynast
//
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
#include "options_mapRiverQuality.h"

//#define BYTESWAP_NECESSARY

//#define DEBUG
#define DEBUG_queries
//#undef DEBUG_queries


mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::StoreQueryResult     res;
mysqlpp::UseQueryResult     Use_res;
mysqlpp::Row        row;

char  MyDatabase[maxcharlength];


int    ClassDefine(double value, short toCalc, short flagTH);
// .12.UNF0 Dateien einlesen



int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram world started at: " << getTimeString() << endl << endl;
	options_mapRiverQualityClass options_mapRiverQuality(argc, &argv[0], "IDRun year");

	if (argc<3) {
		options_mapRiverQuality.Help();
//		cout << "\n./mapRiverQuality IDRun year  [-ooptions_file]"<< endl;
		return 0;
	}

	if(!IsNumber(argv[1]) || !IsNumber(argv[2])) {
		cerr << "ERROR: Wrong parameter" << endl;
//		cerr << "usage: ./mapRiverQuality IDRun year [-ooptions_file]"<<endl;
		options_mapRiverQuality.Help();
		exit(1);
	}
	int IDRun        = atoi(argv[1]);
	int year         = atoi(argv[2]);

	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 3 = 1 + 2 = programmname + 2 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(options_mapRiverQuality.init(argc-3, &argv[3])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input

	short toCalc; // 0 - BOD, 1 - Salz, 2 - TC (Total coliforms), 3 - TN, 4 - TP
//	short flagTH=0;	//Thresholds used -> 0 standard thresholds -> 1 e.g. thresholds of European bathing directive etc. depending on substance used

	double (*Q_out)[12];            Q_out = NULL;        // river availability [km3/month]
	double (*concentration)[12];    concentration = NULL; // monthly grid cell concentration [t/km3]
	int (*concentration_class)[12]; concentration_class = NULL; // class for  monthly grid cell concentration [-]
	int *gcrc;   gcrc = NULL; // gcrc[gcrc-1]=ArcID
	int *grow;   grow = NULL; // grow[gcrc-1]=row

	int j;
	try {
		char sql[20000]; // variable for SQL-query

		cout << "start:" << endl;
		char start_db[maxcharlength]; //sprintf(start_db, "%sworldqual_%s", MyDatabase, continent_abb);
		sprintf(start_db, "wq_general");

		if (!con.connect(&start_db[0], options_mapRiverQuality.MyHost, options_mapRiverQuality.MyUser, options_mapRiverQuality.MyPassword,3306)) {
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl<<endl; //return 0;
		////////////////////////////////

		///=================================

		// Parameter Landmaske einlesen
		if(getCELLS(&options_mapRiverQuality, &continent)) exit(1); cout << "Parameter Landmaske wurden eingelesen\n";

		Q_out         = new double[continent.ng][12];  // river availability [km3/month]
		concentration = new double[continent.ng][12];   // montly grid cell concentration [t/km3]
		concentration_class = new int[continent.ng][12];  // class for montly grid cell concentration [-]

		{  // bei UNF-Dateien wird Umwandlung gcrc -> ArcID benoetigt
			gcrc = new int[continent.ng];
			grow = new int[continent.ng];
			if(getGCRC(gcrc, grow, &options_mapRiverQuality, &continent)) exit(1);
		}
		cout << "gcrc[1]="<<gcrc[0]<<endl;
		cout << "gcrc[51]="<<gcrc[50]<<endl;
		//cout << "gcrc[180001]="<<gcrc[180000]<<endl;
		//cout << "gcrc[180721]="<<gcrc[180720]<<endl;

		query.reset();
		j=sprintf(sql, "SELECT IFNULL(w.`typeAlgoID`, %d) as `typeAlgoID` ", NODATA);
		j=sprintf(sql, "FROM wq_general._runlist r, wq_general.wq_parameter w ");
		j+=sprintf(sql+j, "WHERE r.`IDrun`=%d AND r.`parameter_id`=w.`parameter_id`;", IDRun);
		query << sql;
		cout<<query<<endl;
		res = query.store();
		toCalc=res.at(0)["typeAlgoID"];
		cout<<"toCalc = "<<toCalc<<endl;
		if(toCalc==NODATA)
			cerr<<"Error: typeAlgoID must not be null in table wq_general.wq_parameter!"<<endl;

		switch (toCalc) {
		case 0: cout << "BOD\n"; break;
		case 1: cout << "salt\n"; break;
		case 2: cout << "Fecal coliforms\n"; break;
		case 3: cout << "TN\n"; break;
		case 4: cout << "TP\n"; break;
		default:
			cerr << "ERROR: Wrong parameter type" << endl;
			cerr << "usage:  0 (BOD), 1 (salt), 2 (Fecal coliforms), 3 TN, 4 TP\n"; exit(1);
			break;
		}

		switch (options_mapRiverQuality.flagTH) {
			case 0: cout << "general thresholds used\n"; break;
			case 1: if(toCalc==2){ cout << "thresholds of european bathing directive used\n"; break;}
					else cerr << "ERROR: For this parameter only general thresholds are available (0)" << endl; exit(1);
			default:
				cerr << "ERROR: Wrong use of threshold flag" << endl;
				cerr << "usage:  0 (general), 1 (european bathing directive thresholds - only apply for FC)\n"; exit(1);
				break;
		}


		int cell, month;
		//#################################
		// concentrationen in conc_class aus wq_out_*.concentration_
		cout << "concentrationen in conc_class aus .concentration_"<<IDRun<<endl;
		for (cell=1; cell<continent.ng; cell++)
			for (int month=1; month<=12; month++) {
				concentration[cell-1][month-1]=NODATA;
				concentration_class[cell-1][month-1]=NODATA;
				Q_out[cell-1][month-1]=-9999;
			}

		mysqlpp::StoreQueryResult resQuery;
		mysqlpp::Row    rowQery;

		resQuery.clear();

		query.reset();
		j=sprintf(sql,    "SELECT mod(c.`date`,100) as month, c.`cell`, c.`con` ");
		j+=sprintf(sql+j, "FROM %swq_out_%s.concentration_%d c WHERE c.`date` between %d and %d;", MyDatabase, options_mapRiverQuality.continent_abb, IDRun, year*100+1, year*100+12);
		query << sql;

		#ifdef DEBUG_queries
		  	cout << "Query: " << query << endl << endl; //cin >> j;
		#endif
		resQuery = query.store();

		if (resQuery.empty()) {
			cerr << "error: .concentration_"<<IDRun<<", year " << year << " not exist!"<<endl;
			return 1;
		}

		cout << "resQuery.size(): "<<resQuery.size()<<endl;
		for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
		{
			month=resQuery[rowNum]["month"];  //rowQery["cell"];
			cell=resQuery[rowNum]["cell"];  //rowQery["cell"];
			if (cell>continent.ng) {cerr << "cell "<< cell << " not exist! error in g_waterTemp.\n"; return 1;}

			concentration[cell-1][month-1] = resQuery[rowNum]["con"]/1000.;	//FC same as the others because: value is in 10^10cfu/km^3 - to get cfu/(10^2*ml); 1km^3= 10^15ml => divide by 10^3
			concentration_class[cell-1][month-1] = ClassDefine(concentration[cell-1][month-1], toCalc, options_mapRiverQuality.flagTH);

		}// for(rowNum)
		resQuery.clear();

		//#############################################
		// fuer saisonale Werte  brauchen river availability
		char filename[maxcharlength];
		sprintf(filename,"%s/G_RIVER_AVAIL_%d.12.UNF0", options_mapRiverQuality.input_dir, year);

		cout << "read "<<filename<<endl;
		if (readInputFile(filename, 4, 12, continent.ng, &Q_out[0][0], gcrc, &options_mapRiverQuality, &continent, 0.))  exit(1);
		cout << "done."<<endl;

		//#######################################
		// concentrationen in conc_class schreiben
		cout << "concentrationen in conc_class schreiben\n";

		char TableName[maxcharlength];
		sprintf(TableName, "%sworldqual_%s.`calc_conc_class`", MyDatabase, options_mapRiverQuality.continent_abb);
		if (!isTableExists(TableName)) {
			j=sprintf(sql,    "CREATE TABLE  %s (", TableName);
			j+=sprintf(sql+j, "  `IDRun` int(10) unsigned NOT NULL auto_increment,\n");
			j+=sprintf(sql+j, "  `cell` int(10) unsigned NOT NULL,\n");
			j+=sprintf(sql+j, "  `time` int(10) unsigned NOT NULL,\n");
			j+=sprintf(sql+j, "  `v1` double default NULL COMMENT 'v1 - v12: Klassen',\n");
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
			j+=sprintf(sql+j, "  `spring` double default NULL,\n");
			j+=sprintf(sql+j, "  `summer` double default NULL,\n");
			j+=sprintf(sql+j, "  `autumn` double default NULL,\n");
			j+=sprintf(sql+j, "  `winter` double default NULL,\n");
			j+=sprintf(sql+j, "  `spring_class` double default NULL,\n");
			j+=sprintf(sql+j, "  `summer_class` double default NULL,\n");
			j+=sprintf(sql+j, "  `autumn_class` double default NULL,\n");
			j+=sprintf(sql+j, "  `winter_class` double default NULL,\n");
			j+=sprintf(sql+j, "  `bath_season` double default NULL COMMENT 'Month of bathing season. May vary by continent e.g. in Europe it is May to September',\n");
			j+=sprintf(sql+j, "  `bath_season_class` double default NULL COMMENT 'class of the bathing season',\n");
			j+=sprintf(sql+j, "  PRIMARY KEY  USING BTREE (`IDRun`,`time`,`cell`)\n");
			j+=sprintf(sql+j, ") ENGINE=MyISAM AUTO_INCREMENT=310000001 DEFAULT CHARSET=latin1 COMMENT='class changes [-] ; calc. by mapRiverQuality';\n");

			query.reset();
			query << sql;
			query.execute();
		}

		query.reset();
		query << "DELETE FROM "<<MyDatabase<<"worldqual_"<<options_mapRiverQuality.continent_abb<<".calc_conc_class WHERE `IDRun`=" << IDRun << " AND `time` = " << year<<";";
		query.execute();
		cout <<"delete done\n";

		short comma; int counter=0;
		query.reset();
		query << "INSERT INTO "<<MyDatabase<<"worldqual_"<<options_mapRiverQuality.continent_abb<<".calc_conc_class (`IDRun`, `cell`, `time`, "
		 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`, "
		 << "`spring`, `spring_class`, `summer`, `summer_class`, "
		 << "`autumn`, `autumn_class`, `winter`, `winter_class`, "
		 << " `bath_season`, `bath_season_class`) VALUES ";  comma = 0;

		for (int cell=0; cell<continent.ng; cell++) {
			if (comma) query << ", ";
			else comma = 1;

			query << "("<<IDRun<<", "<<cell+1<<", "<<year<<", ";

			// Monatswerte
			for  (int month=0; month<12; month++) {
				if (concentration_class[cell][month]==NODATA ) query << "NULL";
				else query << concentration_class[cell][month];
				//if (month<11) query << ", "; else query << ")";
				query << ", ";
			} // for(month)

			// saisonale Werte
			// spring
			double Value;
			double Value1  = (concentration[cell][2]*Q_out[cell][2]
				+ concentration[cell][3]*Q_out[cell][3] + concentration[cell][4]*Q_out[cell][4]);
			double Value2 = (Q_out[cell][2] + Q_out[cell][3] + Q_out[cell][4]);
			if (Value2==0) Value=0;
			else if (concentration[cell][2]==NODATA || concentration[cell][3]== NODATA || concentration[cell][4] == NODATA)
				Value = NODATA;
			else Value = Value1/Value2;

			if (Value == NODATA) query << "NULL, NULL, ";
			else query <<  Value << ", "<< ClassDefine(Value, toCalc, options_mapRiverQuality.flagTH)<< ", ";

			// summer
			Value1 = (concentration[cell][5]*Q_out[cell][5]
				+ concentration[cell][6]*Q_out[cell][6] + concentration[cell][7]*Q_out[cell][7]);
			Value2 = (Q_out[cell][5] + Q_out[cell][6] + Q_out[cell][7]);

			if (Value2==0) Value=0;
			else if (concentration[cell][5]==NODATA || concentration[cell][6]== NODATA || concentration[cell][7] == NODATA)
				Value = NODATA;
			else Value = Value1/Value2;

			if (Value == NODATA) query << "NULL, NULL, ";
			else query <<  Value << ", "<< ClassDefine(Value, toCalc, options_mapRiverQuality.flagTH)<< ", ";

			// autumn
			Value1 = (concentration[cell][8]*Q_out[cell][8]
				+ concentration[cell][9]*Q_out[cell][9] + concentration[cell][10]*Q_out[cell][10]);
			Value2 = (Q_out[cell][8] + Q_out[cell][9] + Q_out[cell][10]);


			if (Value2==0) Value=0;
			else if (concentration[cell][8]==NODATA || concentration[cell][9]== NODATA || concentration[cell][10] == NODATA)
				Value = NODATA;
			else Value = Value1/Value2;

			if (Value == NODATA) query << "NULL, NULL, ";
			else query <<  Value << ", "<< ClassDefine(Value, toCalc, options_mapRiverQuality.flagTH)<< ", ";

			// winter
			Value  = (concentration[cell][11]*Q_out[cell][11]
				+ concentration[cell][0]*Q_out[cell][0] + concentration[cell][1]*Q_out[cell][1]);
			Value2 = (Q_out[cell][11] + Q_out[cell][0] + Q_out[cell][1]);

			if (Value2==0) Value=0;
			else if (concentration[cell][11]==NODATA || concentration[cell][0]== NODATA || concentration[cell][1] == NODATA)
				Value = NODATA;
			else Value = Value1/Value2;

			if (Value == NODATA) query << "NULL, NULL, ";
			else query <<  Value << ", "<< ClassDefine(Value, toCalc, options_mapRiverQuality.flagTH)<< ", ";

			//bathing season
			if(options_mapRiverQuality.flagTH==1 && toCalc==2){
				Value  = (concentration[cell][4]*Q_out[cell][4]
				     + concentration[cell][5]*Q_out[cell][5] + concentration[cell][6]*Q_out[cell][6]
				     + concentration[cell][7]*Q_out[cell][7] + concentration[cell][8]*Q_out[cell][8]);
				Value2 = (Q_out[cell][4] + Q_out[cell][5] + Q_out[cell][6]+ Q_out[cell][7]+ Q_out[cell][8]);

				if (Value2==0) Value=0;
				else if (concentration[cell][4]==NODATA || concentration[cell][5]== NODATA || concentration[cell][6] == NODATA|| concentration[cell][7] == NODATA|| concentration[cell][8] == NODATA)
					Value = NODATA;
				else Value = Value1/Value2;

				if (Value == NODATA) query << "NULL, NULL)";
				else query <<  Value << ", "<< ClassDefine(Value, toCalc, options_mapRiverQuality.flagTH)<< ")";

			}else query << "NULL, NULL)";

			counter++;


			#ifdef DEBUG_queries
			  if (cell==0||cell==467||cell==129627||cell==129628)
			  cout << "Query: " << query << endl << endl; //cin >> j;
			#endif

			if (counter==10) {
				counter=0;
				query << ";";

				#ifdef DEBUG_queries
				  if (cell==0||cell==467||cell==129627||cell==129628)
				  cout << "Query: " << query << endl << endl; //cin >> j;
				#endif

				query.execute();

				query.reset();
				query << "INSERT INTO "<<MyDatabase<<"worldqual_"<<options_mapRiverQuality.continent_abb<<".calc_conc_class (`IDRun`, `cell`, `time`, "
				 << "`v1`, `v2`, `v3`, `v4`, `v5`, `v6`, `v7`, `v8`, `v9`, `v10`, `v11`, `v12`, "
				 << "`spring`, `spring_class`, `summer`, `summer_class`, "
				 << "`autumn`, `autumn_class`, `winter`, `winter_class`, "
				 << " `bath_season`, `bath_season_class`) VALUES ";  comma = 0;
			}

		} // for(cell)

		if (counter>0) {
			#ifdef DEBUG_queries
			  cout << "Query: " << query << endl << endl; //cin >> j;
			#endif

			query << ";";
			query.execute();
		}







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

    delete[] Q_out              ;  Q_out                = NULL;
    delete[] concentration      ;  concentration        = NULL;
    delete[] concentration_class;  concentration_class  = NULL;
    delete[] gcrc               ;  gcrc                 = NULL;
    delete[] grow               ;  grow                 = NULL;


	cout <<"\nProgram worldqual endet at: " << getTimeString() << endl << endl;
	return 0;
} // end of main()








int ClassDefine(double value, short toCalc, short flagTH) {


switch(toCalc){

//BOD

case 0: if(flagTH==0){
		if (value> 0.  && value <= 4.) return 1; //no impact on fish
		else if (value> 4.  && value <= 8.) return 2; // low impact on fish
		else if (value> 8.) return 3; // high impact on fish
		else return 0;

}else if(flagTH==1){
		if (value> 0.  && value <= 1.) return 1;
		else if (value> 1.  && value <= 2.5) return 2;
		else if (value> 2.5 && value <= 5.0) return 3;
		else if (value> 5.0 && value <=10.0) return 4;
		else if (value>10.0 && value <=50.0) return 5;
		else if (value>50.0) return 6;
		else return 0;


}else
		cout<<"flag: "<<flagTH<<endl;
		cerr << "ERROR: Wrong flag" << endl;
		cerr << "usage:  only (0) impact on fish and (1) general (from SCENES) possible"; exit(1);
		return NODATA;

//TDS

case 1: if (value> 0.  && value <= 450.) return 1;// according to FAO Water Quality for Irrigation guideline none restriction
		else if (value> 450.  && value <= 2000.) return 2; // slight to moderate restriction
		else if (value> 2000.) return 3; // severe restriction
		else return 0;
//FC
case 2:
	if(flagTH==0){
		if (value == 0.0) return 1;	//drinking water
		else if (value> 0.0  && value <= 200.0) return 2;	//recreation primary contact - irrigation crops eaten raw
		else if (value> 200.0 && value <= 1000.0) return 3; //irrigation
		else if (value> 1000.0 && value <= 2000.0) return 4;	//maximal tolerable class
		else if (value>2000.0) return 5;	//to much
		else return 0;

	}else if(flagTH==1){ //European bathing directive -> CAUTION: the classes of the directive include more parameters than FC!!!
		if (value>= 0.0  && value <= 100.0) return 1;		//CG - compliant with the mandatory and the guide values of the Directive
		else if (value> 100.0 && value <= 2000.0) return 2; //CI - compliant with the mandatory values of the Directive
		else if (value>2000.0) return 3;					//NC - not compliant with the mandatory values of the Directive

	}else
		cout<<"flag: "<<flagTH<<endl;
		cerr << "ERROR: Wrong flag" << endl;
		cerr << "usage:  only general (0) and European bathing directive thresholds (1) can be used"; exit(1);
		return NODATA;



//TN
case 3: if (value> 0.  && value <= 1.) return 1;
		else if (value> 1.  && value <= 1.5) return 2;
		else if (value> 1.5 && value <= 3.0) return 3;
		else if (value> 3.0 && value <= 6.0) return 4;
		else if (value> 6.0 && value <= 12.0) return 5;
		else if (value> 12.0 && value <= 24.0) return 6;
		else if (value> 24.0) return 7;
		else return 0;

//TP
case 4: if (value> 0.  && value <= 0.05) return 1;
		else if (value> 0.05  && value <= 0.08) return 2;
		else if (value> 0.08 && value <= 0.15) return 3;
		else if (value> 0.15 && value <= 0.3) return 4;
		else if (value> 0.3 && value <= 0.6) return 5;
		else if (value> 0.6 && value <= 1.2) return 6;
		else if (value> 1.2) return 7;
		else return 0;

default:
		cerr << "ERROR: Wrong parameter type" << endl;
		cerr << "usage:  0 (BOD), 1 (salt), 2 (Total coliforms), 3 (TN), 4 (TP)\n so far only 0, 3, 4 are implemented"; exit(1);
		break;

}

} // ClassDefine()

