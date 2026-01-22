/*
 * This program finds all arc_ids and global_ids from the spring to the river mouth
 * There are four options in how to use the program:
 * 1.) Enter -s global_start_id and -e global_end_id -> The arc_ids and global_ids
 * 	   from the spring towards the river mouth will be written in cout >*.out
 * 	   -> it will be checked if end of routing=global_end_id
 * 2.) Enter -s global_start_id -> The arc_ids and global_ids
 * 	   from the spring towards the river mouth will be written in cout >*.out
 * 3.) Enter -e global_end_id -> The arc_ids and global_ids
 * 	   from the spring towards the river mouth will be written in cout >*.out
 * 	   -> it will be checked if end of routing=global_end_id
 * 	   -> The start_id will be calculated following the highest discharge
 * 4.) Do not enter -s or -e -> a seperate file STAT.DAT has to be provided with
 * 	   the following format: rivername \t global_start_id \t global_end_id
 * 	   -> no spaces there are in here to keep it readable
 * 	   -> The arc_ids and global_ids from the spring towards the river mouth
 * 	   will be written in a separate file for each river entered in STAT.DAT
 *
 * 	How to use the program best:
 *
 * 	1.) If you want only one river quickly -> less input but not so comfortable
 * 		output
 * 	2.) If you want only one river quickly -> less input but not so comfortable
 * 		output
 * 	3.) If you do not know the global_start_id of a you can use 3.) to get this
 * 		information -> apart from that: if you want only one river quickly
 * 		-> less input but not so comfortable output
 * 	4.) If you want information for many rivers and you know start_id and end_id
 * 		-> bit more input but very comfortable output
 *
 *
 * find_river_cells.cpp
 *
 *  Created on: 09.09.2014
 *      Author: reder
 */
// *****************************
// Changed on: 08-2016
// Changed by: Kynast
// ./find_river_cells [-sStartGlobal_id] [-eEndGlobal_id] [-ooptions_file (default: OPTIONS.DAT)]
//
// 1. Nutzung OPTIONS.DAT wie in anderen Programmen
// 2. ob -s und/oder -e oder STAT.DAT benutzt wird, output hat jetzt immer gleichen Format
//    Unterschied ist nur in Dateinamen, in STAT.DAT gibt es Flussname, er wird in Outputdateinamen verwendet.
//    aus der befehlszeile verwendete Flussabschnitte werden mit GobalID's benannt
// 3. bitte beachten, dass kein Leerzeichen zwischen -s/-e und GlobalID's gibt! Einheitlich mit anderen WaterGAP-Anwendungen


#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "timestring.h"
#include <string>
#include "general_functions.h"
#include "optionsfindRiverCells.h"

#define MY_DATABASE ""
#define MY_HOST "MySQL"
#define MY_USER "worldqual"
#define MY_PASSWORD "1qay"
#define INPUT_DIR "INPUT"

//#define DEBUG
#define DEBUG_queries
#undef DEBUG_queries

//int continent[5];  // 0 = ng (Anzahl Zellen); 1 - ng_land (Anzahl Landzellen); 2 - nrows (Anzahl Zeile); 3 - ncols (Anzahl Spalte); 4 - cellFraction


//char hydro_input[maxcharlength];           	 // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Input-directory
char  MyDatabase[maxcharlength];
//char continent_abb[10];			// Abbreviation of the continent name; used for detection of corresponding database


mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::Query      query_ins = con.query(); // for INSERT into 'statistik'
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;


int getStartArcId(int cell, int (*inflow)[8], double *dis, int* arc_glob_id, const continentStruct* continent_ptr);
int getInflowCell(int cell, int (*inflow)[8], double *dis);
//void getInflow(int (*inflow)[9], const int IDVersion, const int IDReg);
int arcid2globid(int cell, int *arc_glob_id, const continentStruct* continent_ptr);
int getHiDis(int (*inflow)[8], int cell, double *dis);
void goDownstream(int start, int end, char* filename
		, int* g_outflc, int* arc_glob_id, const continentStruct* continent_ptr);


int main(int argc, char *argv[])
{
	//////////////////////////////////routine to read data in////////////////////////

	// print current date
	cout <<"\nProgram find_river_cells started at: " << getTimeString() << endl << endl;

	cout << "_options einlesen"<<endl;
	options_findRiverCellsClass optionsFindRiverCells(argc, &argv[0], "[-sStartGlobal_id] [-eEndGlobal_id]");
	///=================================
	// _options einlesen
	// 1 = 1 + 0 = programmname + 0 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(optionsFindRiverCells.init(argc-1, &argv[1])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input

//	if (argc<1) {
//
//		cout << "\n./find_river_cells [-s startGlobal_id] [-e endGlobal_id] [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region] [-i path]"
//				<< "eigther startGlobal or endGlobal or both have to be entered!"
//			 << "\nif one (or more) parameters is missing the following default values are used:  "
//			 << "\n\nDATABASE      \"empty string\""
//			 << "\nHOST          \"MySQL\"    "
//			 << "\nUSER          \"worldqual\""
//			 << "\nPASSWORD      \"*******\"  "
//			 << "\nIDVersion     3 (WaterGAP3)"
//			 << "\nIDReg         1 (Europa) \n\n"
//			 << "This program finds all arc_ids and global_ids from the spring to the river mouth \n"
//			 << "There are four options in how to use the program:\n"
//			 << " 1.) Enter -s global_start_id and -e global_end_id -> The arc_ids and global_ids\n"
//			 << " 	   from the spring towards the river mouth will be written in cout >*.out\n"
//			 << " 	   -> it will be checked if end of routing=global_end_id\n"
//			 << " 2.) Enter -s global_start_id -> The arc_ids and global_ids\n"
//			 << " 	   from the spring towards the river mouth will be written in cout >*.out\n"
//			 << " 3.) Enter -e global_end_id -> The arc_ids and global_ids\n"
//			 << " 	   from the spring towards the river mouth will be written in cout >*.out\n"
//			 << " 	   -> it will be checked if end of routing=global_end_id\n"
//			 << " 	   -> The start_id will be calculated following the highest discharge\n"
//			 << " 4.) Do not enter -s or -e -> a seperate file STAT.DAT has to be provided with\n"
//			 << " 	   the following format: rivername \t global_start_id \t global_end_id\n"
//			 << " 	   -> no spaces there are in here to keep it readable\n"
//			 << " 	   -> The arc_ids and global_ids from the spring towards the river mouth\n"
//			 << " 	   will be written in a separate file for each river entered in STAT.DAT\n\n"
//			 << " 	How to use the program best:\n\n"
//			 << " 	1.) If you want only one river quickly -> less input but not so comfortable output\n"
//			 << " 	2.) If you want only one river quickly -> less input but not so comfortable output\n"
//			 << " 	3.) If you do not know the global_start_id of a you can use 3.) to get this\n"
//			 << " 		information -> apart from that: if you want only one river quickly\n"
//			 << " 		-> less input but not so comfortable output\n"
//			 << " 	4.) If you want information for many rivers and you know start_id and end_id\n"
//			 << " 		-> bit more input but very comfortable output\n\n";
//		return 0;
//	}


////////////////////////////////////checking read in data////////////////////////
//
//
	char headerline[maxcharlength]; // variable for line input of header
	try {
		cout << "start:" << endl;

		char start_db[maxcharlength]; sprintf(start_db, "wq_general");
		MyDatabase[0]='\0';

		if (!con.connect(&start_db[0], &optionsFindRiverCells.MyHost[0], &optionsFindRiverCells.MyUser[0], &optionsFindRiverCells.MyPassword[0],3306)) {
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database connection ok..." << endl; //return 0;
		////////////////////////////////

		if(getCELLS(&optionsFindRiverCells, &continent)) exit(1); cout << "\nParameter Landmaske wurden eingelesen\n"<<endl;

		//#############
		// ArcID's für start- und end finden
		optionsFindRiverCells.startArcID = (optionsFindRiverCells.startGlobal == NODATA ? NODATA : get_ArcID(&optionsFindRiverCells, optionsFindRiverCells.startGlobal));
		cout << "startArcID done\n" << optionsFindRiverCells.startGlobal << '\t' << optionsFindRiverCells.startArcID << endl;
		optionsFindRiverCells.endArcID = (optionsFindRiverCells.endGlobal == NODATA ? NODATA : get_ArcID(&optionsFindRiverCells, optionsFindRiverCells.endGlobal));
		cout << "endArcID done\n" << optionsFindRiverCells.endGlobal << '\t' << optionsFindRiverCells.endArcID << endl;
		optionsFindRiverCells.printOptions();

		////////////////////////////////Initialization of variables//////////////////

		int *arc_glob_id = new int[continent.ng];	//arc_glob_id[arcid - 1] = globalID
		if(getArcGlobId(arc_glob_id, &optionsFindRiverCells, &continent)) exit(1);

		// (Zuflusszellen) matrix with the possible neighbors from witch inflows my occur
		int (*inflow)[8]    = new int[continent.ng][8];
		int *g_outflc       = new int[continent.ng];       // Abflusszelle

		cout << "get_outflc"<<endl;
		if (get_outflc(g_outflc, &optionsFindRiverCells, &continent)) exit(1);
		cout << "get_inflow"<<endl;
		if (get_inflow(inflow, &optionsFindRiverCells, &continent))   exit(1);


		int *gcrc              = new int[continent.ng];        // gcrc[gcrc-1]=ArcID
		int *grow   		   = new int[continent.ng];   // grow[gcrc-1]=row
		cout << "get_gcrc"<<endl;
		if(getGCRC(gcrc, grow, &optionsFindRiverCells, &continent)) exit(1);


		double *discharge = new double[continent.ng];		//contains discharge of every cell

		char filename[maxcharlength];
		sprintf(filename, "%s/riverAvailMean.UNF0", optionsFindRiverCells.input_dir);
		if (readInputFile(filename, 4, 1, continent.ng, &discharge[0], gcrc, &optionsFindRiverCells, &continent))  exit(1);


		/////////////////////////start of the routine to get river cells///////

		//if start and end id are given then a it can be checked if the input was correct
		//in case only the startid is given than all cells until the outlet are returned
		if(optionsFindRiverCells.startGlobal!=NODATA) {
			//output filename
			if (optionsFindRiverCells.endGlobal!=NODATA) {
				sprintf(filename,"%d_%d_cells_of_river.txt", optionsFindRiverCells.startGlobal, optionsFindRiverCells.endGlobal);
			} else {
				sprintf(filename,"%d_cells_of_river.txt", optionsFindRiverCells.startGlobal);
			}

			goDownstream(optionsFindRiverCells.startArcID, optionsFindRiverCells.endArcID, filename
					, g_outflc, arc_glob_id, &continent);

		} else if(optionsFindRiverCells.endGlobal!=NODATA) {
			// in case only the end id is given all cells upstream following the hightest discharge are given
			cout<<"Global end id entered"<<endl;

			// find start_id following the highest discharge
			int start_id=getStartArcId(optionsFindRiverCells.endArcID, inflow, discharge, arc_glob_id, &continent);
			int start_id_global = arcid2globid(start_id, arc_glob_id, &continent);
			cout<<"start_id="<<start_id<<endl;

			sprintf(filename,"%d_%d_cells_of_river.txt", start_id_global, optionsFindRiverCells.endGlobal);

			goDownstream(start_id, optionsFindRiverCells.endArcID, filename
					, g_outflc, arc_glob_id, &continent);

		} else {//in case nothing is given it will be read from a file STAT.DAT
			cout<<"Only Data from STAT.DAT is read in and will be written in separate files"<<endl;

			// Write data in a seperate file!
			ofstream outputfile_stat;

			char rivername[maxcharlength];
			int arcid_begin_global, arcid_end_global;
			int arcid_begin, arcid_end;

			//read from input file
			{
				cout << "read file " << optionsFindRiverCells.inputfile << endl;
				ifstream inputfile(optionsFindRiverCells.inputfile,ios::in);

				if (!inputfile) { // Keine Statistik
					cerr << "ERROR: inputfile " << optionsFindRiverCells.inputfile << " nicht gefunden!\n";
					exit(1);
				}

				// read commentlines indicated by #
				while (inputfile && inputfile.peek()=='#') {
					inputfile.getline(headerline,maxcharlength);
				}

				do {
					//----------------------------------
					// Flussname einlesen
					{
						char ch = ' '; int count=0;
						while (inputfile.get(ch) && ch != '\t' && count < 50) {
							if (!isspace(ch))
								rivername[count++]=ch;
						}
						rivername[count]='\0';

						// falls while() wegen count<50 beendet (zu Lange Flussname) Rest der Name ignorieren
						if (ch!='\t') inputfile.ignore(maxcharlength,'\t');
						cout << "count: "<< count << "rivername: "<< rivername << endl;
					}
					//----------------------------------
					if (!inputfile) continue;

					//save line from input file in corresponding variables
					inputfile >> arcid_begin_global >> arcid_end_global;
					arcid_begin = get_ArcID(&optionsFindRiverCells, arcid_begin_global);
					arcid_end   = get_ArcID(&optionsFindRiverCells, arcid_end_global);

					// if arcid_begin nodata, find start_id following the highest discharge
					if (arcid_begin == -9999) {
						arcid_begin = getStartArcId(arcid_end, inflow, discharge, arc_glob_id, &continent);
						arcid_begin_global = arcid2globid(arcid_begin, arc_glob_id, &continent);
						cout<<"river " << rivername << ": "<<arcid_begin << arcid_end << endl;
					}
					//======================================================

					//output filename
					sprintf(filename,"%s_cells_of_river.txt", rivername);
					cout << "output file: " << filename << endl;

					goDownstream(arcid_begin, arcid_end, filename
							, g_outflc, arc_glob_id, &continent);

					//=========================================
				} while (inputfile);
			} // end of block "STAT.DAT"

		}//end of else -> not -s or -e entered

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
		catch (const std::out_of_range& er) {
			cerr << "Error: " << er.what() << endl;
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




/*
 * Method to convert the arc_id in a global_id
 */

int arcid2globid(int cell, int* arc_glob_id, const continentStruct* continent_ptr){
	int globalid=NODATA;
	if (cell >= 1 && cell < continent_ptr->ng)
		globalid = arc_glob_id[cell-1];

	return globalid;
}


/*
 * method to get a matrix with the neighbouring cells from which inflow might occur
 */
//void getInflow(int (*inflow)[9], const int IDVersion, const int IDReg){
//	int j;
//	char sql[20000];                        // variable for SQL-query
//
//
//	mysqlpp::Query      query = con.query();
//	query.reset();
//	query << "LOCK TABLES watergap_unf.g_inflc_arc_id  as `g` READ;";
//	query.execute();
//	cout << "LOCK TABLE `getInflow`...\n";
//
//
//	query.reset();
//	j=sprintf(sql, "SELECT g.`cell`, g.`SW`, g.`S`, g.`SE`, g.`W`, g.`E`, g.`NW`, g.`N`, g.`NE` FROM watergap_unf.g_inflc_arc_id  `g` ");
//	j+=sprintf(sql+j, " WHERE g.IDVersion=%d AND g.IDReg=%d;", IDVersion, IDReg );
//	query << sql;
//	res = query.store();
//	cout<<query<<endl;
//	if (res.empty()) {cerr << "arc_glob_id: error in operation to get arcid and globalid.\n"; }
//
//	{
//		int i=0;
//		//cout<<"res.size(): "<<res.size()<<endl;
//		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){
//			inflow[i][0] 	= res[rowNum]["SW"];
//			inflow[i][1] 	= res[rowNum]["SW"];
//			inflow[i][2] 	= res[rowNum]["S"];
//			inflow[i][3] 	= res[rowNum]["SE"];
//			inflow[i][4] 	= res[rowNum]["W"];
//			inflow[i][5] 	= res[rowNum]["E"];
//			inflow[i][6] 	= res[rowNum]["NW"];
//			inflow[i][7] 	= res[rowNum]["N"];
//			inflow[i][8] 	= res[rowNum]["NE"];
//
//			i++;
//		}
//	}
//	query.reset();
//	query << "UNLOCK TABLES;";
//	query.execute();
//
///*	for(int j =0; j<8;j++){
//		cout<<"inflowCell: "<<inflowCell[79456][j]<<endl;
//	}*/
//
//
//
//}

/*
 * Method to return inflow cell, which is eighter
 * 1.) the cell itself, if the actual cell has no inflow
 * 2.) the inflow cell, if there exists only one inflow cell
 * 3.) the inflow cell with the highest discharge
 */
int getInflowCell(int cell, int (*inflow)[8], double *dis){
	int count, one;
	count=0;	//counts the number of inflow cells
	one=0;		//saves the indice in case of just one inflow cell

	//looks at all neighbouring cell if they are inflow to the actual cell
	for(int j =0; j<8;j++){
		if(inflow[cell-1][j]!=0){
			count++;
			one=j;
		}
	}
	//decides which of the three possibilities is apparent
	switch(count){
		case 0: return cell;
		case 1: return inflow[cell-1][one];
		default: return getHiDis(inflow, cell, dis);
	}
	//cout<<"getInflowCell done"<<endl;
	return 0;
}

/*
 * Method wich returns the global_id of the spring of the river - following the
 * path of highest discharge
 */
int getStartArcId(int cell, int (*inflow)[8], double *dis, int* arc_glob_id, const continentStruct* continent_ptr){
	int number, stop;
	stop=-1;

	number = cell;
	do {
		cell = number;
		//gets arc_id of the inflow cell with the highest discharge
		number=getInflowCell(cell, inflow, dis);
		stop++;

	} while (number != cell);

	//cout<<"number from getStartArcId: "<<number<<endl;
	//get global_id from arc_id
//	globalid=arcid2globid(number, arc_glob_id, continent_ptr);

	return number;
}

/*
 * Method to find the hightest discharge of all neighbouring cells
 */
int getHiDis(int (*inflow)[8], int cell, double *dis){
	double max=0.0;
	int maxcell = cell;

	for(int j =0; j<8; j++) {
		int neighbour = inflow[cell-1][j];
		if (neighbour>0 && dis[neighbour-1]>0 && dis[neighbour-1]> max) {
			max = dis[neighbour-1];
			maxcell = neighbour;
		}
	}


	//returns the arc_id of the inflow cell with the highest discharge
	// oder gleiche Zelle
	return maxcell;
}

// von globalID: start flussabwärts bis zu end oder Flussmündung gehen
// start und end sind ArcID's
void goDownstream(int start, int end, char* outputfilename
		, int* g_outflc, int* arc_glob_id, const continentStruct* continent_ptr) {
	cout << "===============================================\n";
	cout << "goDownstream: Flussabschnitt: " << start << " - " << end << endl;
	cout << "===============================================\n";
	if (start!=NODATA && end!=NODATA) {
		cout<<"\nGlobal start and end id entered"<<endl;
	} else if (start!=NODATA) {
		cout<<"Global start id entered"<<endl;
	} else {
		cerr << "ERROR: in der Funktion goDownstream() keine Startzelle definiert!";
		return;
	}

	cout << "output file: " << outputfilename << endl;
	ofstream outputfile_stat;
	outputfile_stat.open(outputfilename);
	if (!outputfile_stat) {
		cerr << "\nCould not open " << outputfilename << " for output" << endl;
		exit (0);
	}



	outputfile_stat << "arc_id\tglobal_id"<<endl;


	// wenn true, endGlobal wurde erreicht
	bool start_end=false;
	int cell_value = start;

	//go through all outflow cells
	do {
		//save arc_id and global_id of outflow cell in list
		outputfile_stat << cell_value <<"\t"<<arcid2globid(cell_value, arc_glob_id, continent_ptr)<<endl;

		//check if endGlobal is the same as the last cell
		if (end != NODATA && cell_value == end) {
			cell_value=0;
			start_end=true;
		} else {
			cell_value=g_outflc[cell_value-1];
		}

	} while(cell_value!=0 && !start_end);

	//check if endGlobal is the same as the last cell
	if(!start_end)
		cerr<<"start global id does not fit with end global id!"<<endl;

} // goDownstream()

