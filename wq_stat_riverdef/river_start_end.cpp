/*
 * river_start_end.cpp
 *
 *  Created on: 23.08.2012
 *      Author: reder
 */

#include "river_start_end.h"
#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include "../general_function/timestring.h"
#include <string>
#include "../general_function/general_functions.h"
// bis jetzt brauchen wq_stat und wq_stat_stations identische Optionen, deswegen nutze hier gleiche Klasse
#include "../wq_stat/options_wqstat.h"

//#define MY_DATABASE ""
//#define MY_HOST "MySQL"
//#define MY_USER "worldqual"
//#define MY_PASSWORD "1qay"
//#define INPUT_DIR "INPUT"

//#define DEBUG
#define DEBUG_queries
#undef DEBUG_queries

//int continent[5];  // 0 = ng (Anzahlt Zellen); 1 - ng_land (Anzahl Landzellen); 2 - nrows (Anzahl Zeile); 3 - ncols (Anzahl Spalte); 4 - cellFraction


//char hydro_input[maxcharlength];           	 // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Input-directory
char  MyDatabase[maxcharlength];
//char continent_abb[10];			// Abbreviation of the continent name; used for detection of corresponding database


mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::Query      query_ins = con.query(); // for INSERT into 'statistik'
mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
mysqlpp::Row        row;


int getNrStations(const int parameter_id, const string years, const options_wqstatClass* options_wqstat_ptr);
void getStationIds(int (*station_id)[3], const int parameter_id, string *rivername, const string years, const options_wqstatClass* options_wqstat_ptr);
int getRiverPath(const int Cell_value, int stat, const int nr_stations, int &end_id, int &count,
		string &all_station_ids, int (*stations_ds)[3], int (*station_id)[3], int (*arc_glob_id)[2],
		const options_wqstatClass* options_wqstat_ptr, const continentStruct* continent_ptr);
void getArgGlobId( int (*arc_glob_id)[2], const options_wqstatClass* options_wqstat_ptr);
int getStartId(int cell, int (*inflow)[9], double *dis, int (*arc_glob_id)[2], const continentStruct* continent_ptr);
int getInflowCell(int cell, int (*inflow)[9], double *dis);
void getInflow(int (*inflow)[9], const options_wqstatClass* options_wqstat_ptr);
int arcid2globid(int cell, int (*arc_glob_id)[2], const continentStruct* continent_ptr);
void getNrOfValues(int (*countValues)[2], const int parameter_id, const string years, const options_wqstatClass* options_wqstat_ptr);
int getHiDis(int (*inflow)[9], int cell, double *dis);
int calcNrValues(int (*countValues)[2], const string all_station_ids, const int nr_stations);

int main(int argc, char *argv[])
{
	//////////////////////////////////routine to read data in////////////////////////

	// print current date
	cout <<"\nProgram world started at: " << getTimeString() << endl << endl;
	options_wqstatClass optionsWQstat(argc, &argv[0], "parameterID startYear endYear");

	if (argc<4) {
		optionsWQstat.Help();
//	    cout << "\n./wq_stat_riverdef parameterID startYear endYear [-ooptions_file]\n";
//	    cout << "default options file is OPTIONS.DAT\n";
//    cout << "\n./wq_stat_riverdef parameterID startYear endYear [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region] [-i path] [-wt watertype]"
//         << "\n\nparameters apart from IDrun, startYear, endYear must not be in order. "
//         << "\nif one (or more) parameters is missing the following default values are used:  "
//         << "\n\nDATABASE      \"empty string\""
//         << "\nHOST          \"MySQL\"    "
//         << "\nUSER          \"worldqual\""
//         << "\nPASSWORD      \"*******\"  "
//         << "\nIDVersion     3 (WaterGAP3)"
//         << "\nIDReg         1 (Europa) \n\n"
//         << "\npath          "<<INPUT_DIR
//    	 << "\n watertype River=1 is defualt";
		return 0;
	}

	if(!IsNumber(argv[1]) || !IsNumber(argv[2]) || !IsNumber(argv[3])) {
		cerr << "ERROR: Wrong parameter" << endl;
//		cerr << "usage: ./wq_stat_riverdef parameterID startYear endYear [-ooptions_file]\n";// [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region] [-i path]"<<endl;
		optionsWQstat.Help();
		exit(1);
	}
	int parameter_id     = atoi(argv[1]);
	int start     = atoi(argv[2]);
	int end       = atoi(argv[3]);


	cout << "parameter_id:      " << parameter_id      << endl;
	cout << "years:      " << start << " - " << end << endl;

	if (start>end) {
		cerr << "ERROR: Wrong parameter. start year> end year!!!" << endl;
		cerr << "usage: ./worldqual parameter_id start end [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region]"<<endl;
		cerr << "usage: ./wq_stat_riverdef parameterID startYear endYear [-ooptions_file]\n";// [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region] [-i path]"<<endl;
		exit(1);
	}

	cout << "_options einlesen"<<endl;
	///=================================
	// _options einlesen
	// 4 = 1 + 3 = programmname + 3 optionen aus der Befehlszeile werden nicht in options bearbeitet, sondern hier im Programm
	if(optionsWQstat.init(argc-4, &argv[4])) exit(1);

	continentStruct continent; // ng, ng_land, nrows, ncols, cellFraction und hydro_input

////////////////////////////////////checking read in data////////////////////////

	int j;
	try {
		char sql[20000]; // variable for SQL-query

		cout << "start:" << endl;

		char start_db[maxcharlength]; sprintf(start_db, "wq_general"); sprintf(MyDatabase, "wq_general");

		if (!con.connect(&start_db[0], optionsWQstat.MyHost, optionsWQstat.MyUser, optionsWQstat.MyPassword,3306)) {
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl; //return 0;
		////////////////////////////////

		// checking parameter_id
		query.reset();
		j=sprintf(sql,    "SELECT w.`parameter_id`, w.`parameter` FROM wq_general.wq_parameter w ");
		j+=sprintf(sql+j, "WHERE w.`parameter_id`=%d;", parameter_id);
		query << sql;
		res = query.store();
		if (res.empty()) {cerr << "ERROR: Wrong parameter: parameter_id" << endl; exit(1);}  // falsche IDSubst
		cout << "parameter_id = " <<parameter_id<<endl;

		if(getCELLS(&optionsWQstat, &continent)) exit(1); cout << "Parameter Landmaske wurden eingelesen\n";

		////////////////////////////////Initialization of variables//////////////////


		std::stringstream yearsstream;
		string years ="";

		for(int i=0;i<=end-start;i++){
			yearsstream<<years<<start+i;
			if(i!=end-start)
				yearsstream<<",";
		}
		years=yearsstream.str();
		cout<<"years: "<<years<<endl;

		int nr_stations;	//nr of stations available for substance x and years xxxx to xxxx
		nr_stations=getNrStations(parameter_id, years, &optionsWQstat);
		cout<<"nr_stations: "<<nr_stations<<endl;

		string *rivername=new string[nr_stations];	//rivername

		int (*station_id)[3] = new int[nr_stations][3];	//station id
		int (*stat_id_help)[3] = new int[nr_stations][3];	//help variable to store station ids

		cout<<"before getStationIds"<<endl;
		getStationIds(station_id, parameter_id, rivername, years, &optionsWQstat);
		cout<<"after1 getStationIds"<<endl;
		getStationIds(stat_id_help, parameter_id, rivername, years, &optionsWQstat);

		int (*stations_ds)[3] = new int[nr_stations][3];	//Matrix, station id, worked off 0-not done, 1= done, number of stations downstream

		int (*arc_glob_id)[2] = new int[continent.ng][2];	//matrix with arcid and global id for continent
		getArgGlobId(arc_glob_id, &optionsWQstat);

		int (*inflow)[9] = new int[continent.ng][9];	//matrix with the possible neighbors from witch inflows my occur
		getInflow(inflow, &optionsWQstat);

		int *gcrc              = new int[continent.ng];        // gcrc[gcrc-1]=ArcID
		int *grow   		   = new int[continent.ng];   // grow[gcrc-1]=row


		double *discharge = new double[continent.ng];		//contains discharge of every cell
		if(getGCRC(gcrc, grow, &optionsWQstat, &continent)) exit(1);   // read gcrc

		char filename[maxcharlength];
		sprintf(filename, "%s/riverAvailMean.UNF0", optionsWQstat.input_dir);
		if (readInputFile(filename, 4 , 1, continent.ng, &discharge[0], gcrc, &optionsWQstat, &continent)) exit(1);


		int (*countValues)[2]=new int[nr_stations][2];		//number of measurements per station
		getNrOfValues(countValues, parameter_id, years, &optionsWQstat);

		//Initialization of stations_ds
		for(int i=0; i<nr_stations; i++){
			stations_ds[i][0]=station_id[i][0];
			stations_ds[i][1]=0;
			stations_ds[i][2]=0;
		}


		/*for(int i =0; i<continent.ng;i++){
			cout<<"discharge: "<<discharge[i]<<" i: "<<i<<endl;
		}

		for(int i=0; i<continent.ng; i++){
			for(int j=0; j<9; j++){
				cout<<"inflow: "<<inflow[i][j]<<"\ti: "<<i<<endl;
			}
		}*/


		int start_id=continent.ng;
		int end_id=0;
		int stat;
		int count;
		string all_station_ids;
		int Cell_value=0;
		int nrValues;

		/////////////////////////start of the routine to get river start and end///////


		cout<<"\nrivername\tstart global id\tend global id\tnumber of stations\tAnzahlMesswerte\tstation ids\n";

		for(int i=0; i<nr_stations; i++){
			count=0;
			stat=0;
			all_station_ids="$";
			if(stations_ds[i][1]==0){	//if the station has not been worked of jet

				Cell_value = station_id[i][1];	//read in arc id
				//start_id=station_id[i][2];	//old way has to be changed

				//Initialization of help matrix
				for(int m=0; m<nr_stations; m++){
					for(int n=0;n<3;n++){
						stat_id_help[m][n]=station_id[m][n];
					}
				}


				//finds all stations downstream and global id of river mouth
				stat=getRiverPath(Cell_value, stat, nr_stations, end_id, count, all_station_ids, stations_ds, stat_id_help, arc_glob_id, &optionsWQstat, &continent);


				//finds start id and all stations upstream, where upstream is the path with the highest discharge
				start_id=getStartId(Cell_value, inflow, discharge, arc_glob_id, &continent);


				//in case the station is on a cell with has no outflow cell
				if(stat==0){
					count=1;
					std::stringstream help;
					help<<all_station_ids<<station_id[i][0]<<"$";
					all_station_ids=help.str();
				}

				//calculated the number of measurements in the path
				nrValues=calcNrValues(countValues, all_station_ids, nr_stations);


				//writes in the file the:
				//		rivername	station_id			start_global_id  end_gloabl_id  nr_of_stations nr_of_measurements all_stations_on_path
				cout<<rivername[i]<<station_id[i][0]<<"\t"<<start_id<<"\t"<<end_id<<"\t"<<count<<"\t"<<nrValues<<"\t"<<all_station_ids<<endl;



				/*for(int l=0; l<nr_stations; l++){
						for(int k=0; k<3; k++){
							cout<<stat_id_help[l][k]<<"\t";
						}
						cout<<""<<endl;
					}*/

			}

		}//for - calc nr of stations and measurements






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
 * method to get the number of stations which are available for substance x
 */
int getNrStations(const int parameter_id, const string years, const options_wqstatClass* options_wqstat_ptr){
	int number;

	mysqlpp::Query      query = con.query();
	query.reset();
	query <<"SELECT count(distinct w1.`station_id`) as 'number' FROM wq_general.wq_station w, "
		<<"wq_general.valid_data_"<<options_wqstat_ptr->continent_abb<<" w1, watergap_unf.gcrc g "
		<<"WHERE w.`station_id`=w1.`station_id` AND w1.`parameter_id`="<<parameter_id
		<<" AND year(w1.`date`) in ("<<years<<") AND w1.`watertype`="<<options_wqstat_ptr->watertype
		<<" AND g.`GLOBALID` = w.`global_id` AND w1.`data_used` is null AND w.`data_used` is null"
		<<" AND g.`IDVersion`="<< options_wqstat_ptr->IDVersion <<" AND g.`IDReg`= "<< options_wqstat_ptr->IDReg
		<<" ORDER BY w.`rivername`,w1.`station_id`;";

	cout<<"query stationId: "<<query<<endl;
	res = query.store();
	if (res.empty()) {cerr << "river_start_end: error no stations available for this parameter.\n"; }

	number = res.at(0)["number"];


	return number;
}

/*
 * method to get station id, arc id, global id and river name for each station
 */
void getStationIds(int (*station_id)[3], const int parameter_id,  string *rivername, const string years, const options_wqstatClass* options_wqstat_ptr){

	mysqlpp::Query      query = con.query();
	query.reset();

	query <<"SELECT w1.`station_id`, g.`cell`, w.`rivername` as 'river', w.`global_id`"
		  <<"FROM wq_general.wq_station w, wq_general.valid_data_"<<options_wqstat_ptr->continent_abb<<" w1, watergap_unf.gcrc g "
		  <<"WHERE w.`station_id`=w1.`station_id` AND w1.`parameter_id`="<<parameter_id
		  <<" AND g.`GLOBALID` = w.`global_id` AND w1.`data_used` is null AND w.`data_used` is null"
		  <<" AND year(w1.`date`) in ("<<years<<") AND w1.`watertype`="<<options_wqstat_ptr->watertype
		  <<" AND g.`IDVersion`="<< options_wqstat_ptr->IDVersion <<" AND g.`IDReg`= "<< options_wqstat_ptr->IDReg
		  <<" group by w1.`station_id`"
		  <<" ORDER BY w1.`station_id`;";


	cout<<"query stationId: "<<query<<endl;
	res = query.store();
	if (res.empty()) {cerr << "river_start_end: error no stations available for this parameter.\n"; }

	{
		int i=0;
		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){
			station_id[i][0] 	= res[rowNum]["station_id"];
			station_id[i][1] 	= res[rowNum]["cell"];
			station_id[i][2]	= res[rowNum]["global_id"];
			rivername[i] 		= (string) res[rowNum]["river"];
			//cout<<"rivername[i]: "<<rivername[i]<<" i: "<<i<<endl;
			i++;
		}
	}

	cout<<"get Station_ids done"<<endl;
}




/*
 * method to find all stations downstream and global id of river mouth
 */
int getRiverPath(const int Cell_value, int stat, const int nr_stations, int &end_id, int &count,
		string &all_station_ids, int (*stations_ds)[3], int (*station_id)[3], int (*arc_glob_id)[2],
		const options_wqstatClass* options_wqstat_ptr, const continentStruct* continent_ptr){
	int outflowCell = get_outflowCell(Cell_value, options_wqstat_ptr);


	//if the cell is not a sink
	if(outflowCell!=0){
		for(int j=0; j<nr_stations;j++){
			//looks for all stations with the same arc id
			if(station_id[j][1]==Cell_value){
				stat++;
				//end id is set
				end_id=station_id[j][2];
				//number of stations are counted
				count++;
				//stations on path are stored in a string seperated by $
				std::stringstream help;
				help<<all_station_ids<<station_id[j][0]<<"$";
				all_station_ids=help.str();
				//station is worked off in ds and help
				stations_ds[j][1]=1;
				station_id[j][1]=-9999;
			}
		}

		//cout<<"in getRiverPath end_id"<<end_id<<" count: "<<count<<" all_station_ids: "<<all_station_ids<<" stat: "<<stat<<endl;

		//start recursion
		stat=getRiverPath(outflowCell, stat, nr_stations, end_id, count, all_station_ids, stations_ds, station_id,arc_glob_id, options_wqstat_ptr, continent_ptr);
	}else {


		end_id=arcid2globid(Cell_value, arc_glob_id, continent_ptr);

	}
	return stat;

}

/*
 *method to get arcid and global id
 */
void getArgGlobId( int (*arc_glob_id)[2], const options_wqstatClass* options_wqstat_ptr){
	int j;
	char sql[20000];                        // variable for SQL-query

	mysqlpp::Query      query = con.query();
	query.reset();
	j=sprintf(sql, "SELECT g.`cell` as 'cell', g.`GLOBALID` as 'global_id' FROM watergap_unf.gcrc g ");
	j=sprintf(sql+j, "WHERE g.`IDVersion`=%d AND g.`IDReg`=%d ", options_wqstat_ptr->IDVersion, options_wqstat_ptr->IDReg);
	j=sprintf(sql+j, "GROUP BY g.`GLOBALID` HAVING count(*) =1; ");
	query << sql;
	res = query.store();
	cout<<query<<endl;
	if (res.empty()) {cerr << "arc_glob_id: error in operation to get arcid and globalid.\n"; }

	{
		int i=0;
		//cout<<"res.size(): "<<res.size()<<endl;
		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){
			arc_glob_id[i][0] 	= res[rowNum]["cell"];
			arc_glob_id[i][1] 	= res[rowNum]["global_id"];
			//cout<<"arc_glob_id[i][0]: "<<arc_glob_id[i][0]<<" i: "<<i<<endl;
			//cout<<"arc_glob_id[i][1]: "<<arc_glob_id[i][1]<<" i: "<<i<<endl;
			i++;
		}
	}
	cout<<"getArgGlobId done"<<endl;
}

/*
 * Method to convert the arc_id in a global_id
 */

int arcid2globid(int cell, int (*arc_glob_id)[2], const continentStruct* continent_ptr){
	int globalid=NODATA;

	for(int m=0; m<continent_ptr->ng;m++){
	//finding the global id for the arc id
		if(cell==arc_glob_id[m][0]){
			globalid=arc_glob_id[m][1];
			if(globalid==NODATA)
				cerr<<"Error: for this arcid no global id was found!\n"
				<<"arc_glob_id[m][1]: "<<arc_glob_id[m][1]<<endl;
		}
		//cout<<"end_id"<<end_id<<endl;
	}

	return globalid;

}

/*
 * Method to get out of the database every station with its number of measurements
 */
void getNrOfValues(int (*countValues)[2], const int parameter_id, const string years, const options_wqstatClass* options_wqstat_ptr){

	mysqlpp::Query      query = con.query();
	query.reset();
	query <<"SELECT count(*) as 'number', w1.`station_id` "
		<<" FROM wq_general.wq_station w, wq_general.valid_data_"<<options_wqstat_ptr->continent_abb<<" w1, watergap_unf.gcrc g "
		<<" WHERE w.`station_id`=w1.`station_id` AND w1.`parameter_id`= "<< parameter_id
		<<" AND year(w1.`date`) in ( "<< years <<") AND w1.`data_used` is null AND w.`data_used` is null"
		<<" AND g.`IDVersion`="<< options_wqstat_ptr->IDVersion <<" AND g.`IDReg`= "<< options_wqstat_ptr->IDReg
		<<" AND w1.`watertype`="<<options_wqstat_ptr->watertype
		<<" AND g.`GLOBALID` = w.`global_id` GROUP BY w1.`station_id`;";



	res = query.store();
	cout<<query<<endl;
	if (res.empty()) {cerr << "nrOfValues: error to get number of values.\n"; }
	{
		int i=0;

		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){
			countValues[i][0] 	= res[rowNum]["station_id"];
			countValues[i][1] 	= res[rowNum]["number"];
			//cout<<"countValues[i][0]: "<<countValues[i][0]<<" i: "<<i<<endl;
			//cout<<"countValues[i][1]: "<<countValues[i][1]<<" i: "<<i<<endl;
			i++;
		}
	}
	cout<<"getNrOfValues done"<<endl;
}





/*
 * method to get a matrix with the neighbouring cells from which inflow might occur
 */
void getInflow(int (*inflow)[9], const options_wqstatClass* options_wqstat_ptr){
	int j;
	char sql[20000];                        // variable for SQL-query


	mysqlpp::Query      query = con.query();
	query.reset();
	query << "LOCK TABLES watergap_unf.g_inflc_arc_id  as `g` READ;";
	query.execute();
	cout << "LOCK TABLE `getInflow`...\n";


	query.reset();
	j=sprintf(sql, "SELECT g.`cell`, g.`SW`, g.`S`, g.`SE`, g.`W`, g.`E`, g.`NW`, g.`N`, g.`NE` FROM watergap_unf.g_inflc_arc_id  `g` ");
	j+=sprintf(sql+j, " WHERE g.IDVersion=%d AND g.IDReg=%d;", options_wqstat_ptr->IDVersion, options_wqstat_ptr->IDReg );
	query << sql;
	res = query.store();
	cout<<query<<endl;
	if (res.empty()) {cerr << "arc_glob_id: error in operation to get arcid and globalid.\n"; }

	{
		int i=0;
		//cout<<"res.size(): "<<res.size()<<endl;
		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){
			inflow[i][0] 	= res[rowNum]["SW"];
			inflow[i][1] 	= res[rowNum]["SW"];
			inflow[i][2] 	= res[rowNum]["S"];
			inflow[i][3] 	= res[rowNum]["SE"];
			inflow[i][4] 	= res[rowNum]["W"];
			inflow[i][5] 	= res[rowNum]["E"];
			inflow[i][6] 	= res[rowNum]["NW"];
			inflow[i][7] 	= res[rowNum]["N"];
			inflow[i][8] 	= res[rowNum]["NE"];

			i++;
		}
	}
	query.reset();
	query << "UNLOCK TABLES;";
	query.execute();

/*	for(int j =0; j<8;j++){
		cout<<"inflowCell: "<<inflowCell[79456][j]<<endl;
	}*/

cout<<"getInflow done"<<endl;

}

/*
 * Method to return inflow cell, which is eighter
 * 1.) the cell itself, if the actual cell has no inflow
 * 2.) the inflow cell, if there exists only one inflow cell
 * 3.) the inflow cell with the highest discharge
 */
int getInflowCell(int cell, int (*inflow)[9], double *dis){
	int count, one;
	count=0;	//counts the number of inflow cells
	one=0;		//saves the indice in case of just one inflow cell

	//looks at all neighbouring cell if they are inflow to the actual cell
	for(int j =0; j<9;j++){
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
int getStartId(int cell, int (*inflow)[9], double *dis, int (*arc_glob_id)[2], const continentStruct* continent_ptr){
	int number, stop, globalid;
	stop=-1;

	//gets arc_id of the inflow cell with the highest discharge
	number=getInflowCell(cell, inflow, dis);

	//if the cell is not at spring
	if(number!=cell){
		//do untial a spring is found -> number == getInflowCell()
		while(stop!=number){
			stop=number;
			number=getInflowCell(stop, inflow, dis);
			//cout<<"number while: "<<number<<endl;
			//cout<<"stop while: "<<stop<<endl;

		}
	}
	//cout<<"number from getStartId: "<<number<<endl;
	//get global_id from arc_id
	globalid=arcid2globid(number, arc_glob_id, continent_ptr);
	return globalid;

}

/*
 * Method to find the hightest discharge of all neighbouring cells
 */
int getHiDis(int (*inflow)[9], int cell, double *dis){
	double (*discharge)[2] = new double[9][2];	//for all neighboring cells saves the arc_id and the amount of discharge
	double max=0.0;
	int maxcell =0;

	//cout<<"hidis, cell: "<<cell<<endl;

	//looks at all neighbors
	for(int j =0; j<9;j++){
		//saves arc_id of neighboring cell
		discharge[j][0]= inflow[cell-1][j];
		//if the inflow is a sink set discharge zero
		if(discharge[j][0]==0){
			discharge[j][1]=0;
		}else{
			//otherwise set the amount of discharge
			discharge[j][1]=dis[(int)discharge[j][0]-1];
			//cout<<"int)discharge[j][0]: "<<(int)discharge[j][0]<<" dis[(int)discharge[j][0]-1]: "<<dis[(int)discharge[j][0]-1]<<endl;
		}
	}

	//looks at all neighbors
	for(int j =0; j<9;j++){
		//finds the neighbor with the highest discharge
		if(discharge[j][1]>max){
			max=discharge[j][1];
			maxcell=(int)discharge[j][0];
			//cout<<"max: "<<max<<endl;
			//cout<<"maxcell: "<<maxcell<<endl;
		}

		//cout<<"discharge[j][0]: "<<discharge[j][0]<<endl;
		//cout<<"discharge[j][1]: "<<discharge[j][1]<<endl;

	}


	if(maxcell==0.0){
		//cout<<"in equals 0"<<endl;
		maxcell=cell;	//if the discharge of all inflow cells is zero than, the acutal cell is the treated like a sink
	}


	//returns the arc_id of the inflow cell with the highest discharge
	return maxcell;
}

/*
 * Method to return the number of measurements in the path
 */
int calcNrValues(int (*countValues)[2], const string all_station_ids, const int nr_stations){

	std::stringstream station_id;
	int count=0;
	size_t found;

	//for every station
	for(int i=0; i<nr_stations;i++){
		//the string of stations in the path is split
		station_id.str("");
		station_id<<"$"<<countValues[i][0]<<"$";
		found=all_station_ids.find(station_id.str());
		//for all stations the number of measurements are summed up
		if(found!=string::npos)
			count +=countValues[i][1];
		//cout<<"count: "<<count<<" countValues[i][0]: "<<countValues[i][0]<<" countValues[i][1]: "<<countValues[i][1]<<endl;
		//cout<<"station_id: "<<station_id.str()<<" all_station_ids: "<<all_station_ids<<endl;
	}

	return count;
}


