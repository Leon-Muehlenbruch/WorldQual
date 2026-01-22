/*
 * general_functions.h
 *
 *  Created on: 08.01.2013
 *      Author: reder
 */

#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <fstream>
#include "../options/options.h"
#include "../worldqual/options_wq.h"
#include "country.h"

using namespace std;

#ifndef GENERAL_FUNCTIONS_H_
#define GENERAL_FUNCTIONS_H_

using namespace mysqlpp;

#ifndef DEBUG_queries
#define DEBUG_queries
//#undef DEBUG_queries
#endif
// is_c_geogen_country ist fuer Kodeuebertragbarkeit da: nur f�r Salz relevant!
//      1 - c_geogen als L�nderwerte in der Tabelle %swq_load.`country_parameter_input (bis 3.2015 benutzt)
//      0 - c_geogen_cell - Zellwerte aus der Tabelle %swq_load.`parameter_cell_input soll in den neuen Projekten (ab 3.2015) benutzt werden
//if (is_c_geogen_country == 1) {
//	c_geogen als L�nderwerte in der Tabelle wq_load.country_parameter_input
//	c_geogen darf entweder nur in der Tabelle wq_load.country_parameter_input geben (wq_load input)
//	oder in der worldqual.factors.c_geogen (worldqual input). Man darf es nicht doppelt rechnen!!
//} else {
//	c_geogen_cell - Zellwerte aus der Tabelle wq_load.`parameter_cell_input (wq_load input)
//	c_geogen aus der Tabelle worldqual.factors wird nicht benutzt.
//}
const bool is_c_geogen_country = 1;

#define BYTESWAP_NECESSARY

const short maxcharlength          = 25500;

struct continentStruct {
	int ng;      // number of cells
	int ng_land; // number of land cells
	int nrows;   // number of rows
	int ncols;   // number of columns
	int cellFraction;
	char hydro_input[maxcharlength];              // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Inputpath
};

const int loadTypes = 11;
struct loadStruct {
		bool loadType[loadTypes];  // true - Tabelle wird f�r load gebraucht; false - wird nicht gebraucht
		char loadTab[loadTypes][maxcharlength]; // Tabellenname: *wq_load_*.calc_cell_month_load_[loadTab[i]]
		char loadTabAlias[loadTypes][loadTypes];       // Alias der Tabelle in SQL-Anweisung
		char loadTabLongName[loadTypes][maxcharlength]; // Beschreibung, wird nur zur Information ausgegeben
};
void fillLoadStruct (loadStruct *loadStruct_ptr, int toCalc); // 0 - BOD, 1 - salt, 2 - FC (Fecal coliforms), 3 - TN, 4 - TP

extern const int      NODATA;
//extern const int   	  IDVersion_const;     	// in watergap_unf.watergap_region(IDVersion, IDRegion)
//extern const int   	  IDReg_const;    		// 2 version watergap2, region=world   3 version watergap3, region=1 Europa

//extern int continent[5];  // 0 = ng (number of cells); 1 - ng_land (number of land cells); 2 - nrows (number of rows); 3 - ncols (number of columns); 4 - cellFraction
//extern char continent_abb[10];
//extern optionsClass options;

//extern char hydro_input[maxcharlength];              // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Inputpath

extern mysqlpp::Connection con;
extern mysqlpp::Query query;
extern mysqlpp::StoreQueryResult     res;


//hydology methods
short  checkIDTemp(int IDTemp, double* ConstTemp, char* climateTemp);
int    get_outflc(int *g_outflc, const optionsClass* options_ptr, const continentStruct* continent_ptr);
int    get_outflowCell(int cell, const optionsClass* options_ptr);
int    get_inflow(int (*inflow)[8], const optionsClass* options_ptr, const continentStruct* continent_ptr);
int    get_factor(int IDrun, double *factor, double *c_geogen, double *factor_lake, int toCalc, const optionsClass* options_ptr, const continentStruct* continent_ptr);
int    get_cell_runoff(int actual_year, double (*cell_runoff)[12], int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr);
int    get_gw_runoff(int actual_year, double (*cell_runoff)[12], int *gcrc, int *grow, const options_wqClass* options_ptr, const continentStruct* continent_ptr);
int    get_Q(int actual_year, double (*Q_out)[12], int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr);
int    get_waterTemp(int actual_year, double (*water_temp)[12], short UseWaterTemp, int IDTemp, const optionsClass* options_ptr, const continentStruct* continent_ptr, int IDScen=-9999);
double get_flowVelocity(double Q_out, double g_slope);
bool getTeta(double &teta, double &teta_lake, const int IDScen, const int parameter_id, const int year);
bool getYear_parameter_input(int &MaxYear, const int IDScen, const int parameter_id, const int year);

//Methods to calculate water concentration
int   calculateAttenuation(double *ke, double *tss_cell, int IDScen, int parameter_id, int year
		, double (*Q_out)[12], int *gcrc, int *grow, const optionsClass* options_ptr, const continentStruct* continent_ptr);
int   getKeRelations(double *keRel, const int IDScen, const int  parameter_id, const int  year);
int   getTSSBasinValues(double *tss_cell, const int IDReg, const continentStruct* continent_ptr);
int   getDecayRates(double *decRate, int IDScen, int parameter_id, int year);
//int get_s_cell(int IDScen, int parameter_id, int actual_year, double (*s_cell)[12], double *geogen_info);
int get_s_cell(int IDScen, int parameter_id, int actual_year, double (*s_cell)[12]
	, const optionsClass* options_ptr, const continentStruct* continent_ptr, short loadingType, int toCalc);
int get_s_cell_direct(int IDScen, int parameter_id, int actual_year, double (*s_cell)[12]
	, const optionsClass* options_ptr, const continentStruct* continent_ptr, short loadingType, int toCalc);

//general methods
int    getCELLS(const optionsClass* options_ptr, continentStruct* continent);
int    getGCRC(int *gcrc, int *grow, const optionsClass* options_ptr, const continentStruct* continent_ptr);
int    getArcGlobId(int *arc_glob_id, const optionsClass* options_ptr, const continentStruct* continent_ptr);
bool   IsNumber(char* str);
bool getProject(int project_id, const optionsClass* options_ptr, mysqlpp::Connection con, const char* application);
bool checkParameter(int parameter_id, int& toCalc, mysqlpp::String& ParameterName, bool ReadToCalc=true);
bool checkIDScen(int project_id, int IDScen);
short checkIDRun(int IDrun, mysqlpp::String& runName, int& parameter_id_input, int& parameter_id, int& parameter_id_load
		, int& project_id, int& IDScen, int& IDScen_wq_load
		, int& IDTemp, double& Q_low_limit, short& Q_low_limit_type, short& FlgCons, short& FlgLake, short& UseWaterTemp);
short checkIDRun(int IDrun, mysqlpp::String& runName, int& parameter_id_input, int& parameter_id, int& parameter_id_load
		, int& project_id, int& IDScen, int& IDScen_wq_load);
bool isTableExists(const char* TableName);
short CreateTable(const char* TableName, const char* sql_createTable, bool QueryExec, ofstream &mySQLfile, bool &TableExists);

// get_CellParam() kann mit NULL statt RoutOrder ausgerufen werden, Falls kein RoutOrder gebraucht wird (in wq_stat  wird nicht gesamte Kontinent berechnet, sondern Flussabschnitt, RoutOrder wird intern berechnet)
int    get_CellParam(double *riverLength, int *RoutOrder, double *g_slope, double *flowVelocity_const, int *gcrc, const options_wqClass* options_ptr, const continentStruct* continent_ptr);
int    get_ArcID(const optionsClass* options_ptr, int arcid_global);
int    setCountryInput(int number_countries, int (*count_cont)[4], std::string var_one, std::string var_two, std::string var_three, int IDScen, int actual_year);
void getRiverName(char* riverName, ifstream &inputfile);

void SimMemorySwap(char* sourceArray,
           char* destArray,
           const unsigned short dataTypeSize,
           const unsigned long  arrayByteSize);


int readArea(float *G_area, char *G_land_area, int *gcrc, const optionsClass* options_ptr, const continentStruct* continent_ptr);

template <class T> int readUnf(char *input_file, T *output_grid, int valuesPerCell, int cells);
template <class T> int read_file(char *file, int n_values, T *grid);


int readInputFile(char *input_file, int  type_size, int  valuesPerCell, long number_of_cells
		, double *G_array, int *gcrc, const optionsClass* options_ptr, const continentStruct* continent_ptr, float nodata=(float)NODATA);

template <class T> int readInputFile(char *input_file, int  type_size, int  valuesPerCell, long number_of_cells
		, T *G_array, int *gcrc, const optionsClass* options_ptr, const continentStruct* continent_ptr, float nodata=(T)NODATA)
{
	cout <<"readInputFile for template; sizeof = " <<sizeof(T)<< "\n";
	T nodata_value;
	if(sizeof(nodata_value)==1) nodata_value=0;
	else nodata_value=nodata;
		ifstream ifile(input_file);
	if (!ifile) { cerr << "Can't open input file "<< input_file<<endl; return 1;}
	T* G_tmp_array = new T[number_of_cells*valuesPerCell];
	long  bytesRead;
	ifile.read((char*)G_tmp_array, number_of_cells*valuesPerCell*type_size);
	bytesRead = ifile.gcount();
	cout << "file "<< input_file << ' '<< bytesRead <<" bytesRead\n"
			<<"number_of_cells: " <<number_of_cells<<" valuesPerCell: "<<valuesPerCell
			<<" type_size: "<<type_size<<' ';
	if (type_size==1 ) cout <<"G_tmp_array "<< (int)G_tmp_array[0]<<endl;
	else  cout <<"G_tmp_array "<<G_tmp_array[5157]<<endl;
	ifile.close();
	if (bytesRead!=number_of_cells*valuesPerCell*type_size) {
		cerr << "Input file size mismatch "<< input_file<<" bytesRead: "<<bytesRead<<" number_of_cells*valuesPerCell*type_size: "<<number_of_cells*valuesPerCell*type_size<<endl;
		return 1;
	}
		// Aendern der Reihenfolge der Bytes (SimMemorySwap)
	#ifdef BYTESWAP_NECESSARY
	SimMemorySwap((char*) G_tmp_array,
	(char*) G_tmp_array,
	type_size,
	number_of_cells*valuesPerCell*type_size);
	#endif
	cout << "file "<< input_file << ' '<< bytesRead <<" bytesRead\n"
		<<"number_of_cells: " <<number_of_cells<<" valuesPerCell: "<<valuesPerCell
		<<" type_size: "<<type_size<<' ';
	if (type_size==1 ) cout << (int)G_tmp_array[0]<<endl;
	else  cout << G_tmp_array[5157]<<endl;

	bool order;
	//WaterGAP2 has rows and columns in an different order than WG3
	//This only applies to UNF files in the row format
	if(strcmp(options_ptr->continent_abb,"wg2")==0 && (number_of_cells==continent_ptr->nrows)){
		order=false; //cout<<"order false"<<endl;
	}else{
		order=true; //cout<<"order true"<<endl;
	}

	// gcrc -> arcid
    int index;
    for (int cell=0; cell<number_of_cells; cell++) {
        for (int month=0; month<valuesPerCell; month++) {
        	if (number_of_cells == continent_ptr->ng || number_of_cells== continent_ptr->ng_land)
        		index = (gcrc[cell]-1)*valuesPerCell+month;
        	else index = cell*valuesPerCell+month;
        	// gcrc[cell] == arcid
            //G_array[index] = G_tmp_array[cell*valuesPerCell+month];
        	if (order) G_array[index] = G_tmp_array[cell*valuesPerCell+month];
        	else  G_array[index] = G_tmp_array[month*number_of_cells+cell];
        	if (G_array[index]!=G_array[index]) {
        		cerr << "file\t"<<input_file<<"\tcell\t"<<cell<<"\tgcrc\t"<<gcrc[cell]
        			<<"\tmonth\t"<<month+1<<'\t'<<G_array[index]<<"\tvalue ="<<NODATA<<endl;
                G_array[index]=nodata_value;
        	}
        }
    }// for(cell)


	//cout<<"before delete "<<endl;
	delete[] G_tmp_array; G_tmp_array = NULL;
	//cout<<"before return"<<endl;
	return 0;
}



template<class T>
int readInputFileWG2toWG3( char *input_file, int  type_size, int  valuesPerCell, int number_of_cells, T (*G_array)[12], int *gcrc, int *G_wg3_to_wg2, int ng_wg22=70412)
{
	T nodata_value;
	if(sizeof(nodata_value)==1) nodata_value=0;
	else nodata_value=(T)NODATA;
		ifstream ifile(input_file);
	if (!ifile) { cerr << "Can't open input file "<< input_file<<endl; return 1;}
	cout<<"G_* Input on 0.5�"<<endl;
	
	T* G_tmp_array = new T[ng_wg22*valuesPerCell];
	long  bytesRead;
	ifile.read((char*)G_tmp_array, ng_wg22*valuesPerCell*type_size);
	bytesRead = ifile.gcount();
	cout << "file "<< input_file << ' '<< bytesRead <<" bytesRead\n"
			<<"ng_wg22: " <<ng_wg22<<" valuesPerCell: "<<valuesPerCell
			<<" type_size: "<<type_size<<' ';
	if (type_size==1 ) cout <<"G_tmp_array "<< (int)G_tmp_array[33988]<<endl;
	else  cout <<"G_tmp_array "<<G_tmp_array[33988]<<endl;
		ifile.close();
		if (bytesRead!=ng_wg22*valuesPerCell*type_size) {
		cerr << "Input file size mismatch "<< input_file<<" bytesRead: "<<bytesRead<<" ng_wg22*valuesPerCell*type_size: "<<ng_wg22*valuesPerCell*type_size<<endl;
		return 1;
	}
		// Aendern der Reihenfolge der Bytes (SimMemorySwap)
	#ifdef BYTESWAP_NECESSARY
	SimMemorySwap((char*) G_tmp_array,
	(char*) G_tmp_array,
	type_size,
	ng_wg22*valuesPerCell*type_size);
	#endif
	cout << "file "<< input_file << ' '<< bytesRead <<" bytesRead\n"
		<<"number_of_cells: " <<number_of_cells<<" valuesPerCell: "<<valuesPerCell
		<<" type_size: "<<type_size<<' ';
	if (type_size==1 ) cout << (int)G_tmp_array[33988]<<endl;
	else  cout << G_tmp_array[33988]<<endl;

	//for(int i=0;i<ng_wg22*valuesPerCell; i++)
	//	cout<<"G_tmp_array["<<i<<"]"<<G_tmp_array[i]<<endl;

	// gcrc -> arcid
	for (int cell=0; cell<number_of_cells; cell++) {
		for (int month=0; month<valuesPerCell; month++) {
		// gcrc[cell] == arcid
			G_array[cell][month] = G_tmp_array[(G_wg3_to_wg2[cell]-1)*valuesPerCell+month];
			/*if(cell==144248)
				cout<<"G_wg3_to_wg2[cell]: "<<G_wg3_to_wg2[cell]<<" G_tmp_array[(G_wg3_to_wg2[cell]-1)*valuesPerCell+month]: "<<G_tmp_array[(G_wg3_to_wg2[cell]-1)*valuesPerCell+month]
				       <<"G_tmp_array[(33989-1)*12+month]"<<endl;
			 */
			if (G_array[gcrc[cell]-1][month]!=G_array[gcrc[cell]-1][month]) {
				cerr << "file\t"<<input_file<<"\tcell\t"<<cell<<"\tgcrc\t"<<gcrc[cell]
				    <<"\tmonth\t"<<month+1<<'\t'<<G_array[gcrc[cell]-1][month]<<"\tvalue ="<<NODATA<<endl;
				G_array[gcrc[cell]-1][month]=nodata_value;
			}
		}
	}// for(cell)

	delete[] G_tmp_array; G_tmp_array = NULL;
	return 0;
}

#endif /* GENERAL_FUNCTIONS_H_ */
