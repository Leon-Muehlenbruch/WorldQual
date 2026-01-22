/*
 * options.cpp
 *
 *  Created on: 11.05.2015
 *      Author: kynast
 *      based on optionClass from WaterGAP hydro model
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>      // std::setw

//#include "../general_function/general_functions.h"
#include "../options/options.h"
//#include "options.h"
#include "options_wqstat_scen.h"
using namespace std;

//options_wqstat_scenClass::options_wqstat_scenClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//}

options_wqstat_scenClass::~options_wqstat_scenClass() {
	// TODO Auto-generated destructor stub
	//cout << "destructor options_wqstat_scenClass\n";
}

short options_wqstat_scenClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	IDrun = 0;
	IDrun_scen1 = 0;
	IDrun_scen2 = 0;
	IDrun_scen3 = 0;
	DiffYears = 0;
	watertype = 1;

	//#############
	// read OPTIONS.DAT
	int parent_number=7; // ab diesem Nummer sind Optionen von der Klasse optionsClass

	FILE *file_ptr;
	file_ptr = fopen(filename_opt, "r");
	if (file_ptr != NULL) {
		printf("Reading: %s\n\n", filename_opt);
		int i = 0, n;

		while (fgets(string, maxcharlength, file_ptr) && i<parent_number-1) {
			if (0 == strncmp(string, "Value:", 6)) {
				i++;
				n = atoi(&string[7]);
				if (1 == i) {
					IDrun = n;
				}
				if (2 == i) {
					DiffYears = n;
				}
				if (3 == i) {
					IDrun_scen1 = n;
				}
				if (4 == i) {
					IDrun_scen2 = n;
				}
				if (5 == i) {
					IDrun_scen3 = n;
				}
				if (6 == i) {
					watertype = n;
				}
			} // if(Value)
		}
		if (read_parentOptions(parent_number, file_ptr)) return 1;
		fclose(file_ptr);
	} else {
		printf("\nError while opening file '%s'.\n", filename_opt);
		//printf("Using default options.\n");
		return 1;
	}

	//#############
	// Options Ausgabe

	// Ausgabe worldqual-Optionen
	cout << "IDrun      : " << IDrun       << endl;
	cout << "DiffYears  : " << DiffYears   << endl;
	cout << "IDrun_scen1: " << IDrun_scen1 << endl;
	cout << "IDrun_scen2: " << IDrun_scen2 << endl;
	cout << "IDrun_scen3: " << IDrun_scen3 << endl;
	cout << "watertype: " << watertype      << endl;

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt

	return 0;
 } // init()

void options_wqstat_scenClass::Help(int num) {
	num = 1;
	cout << programme << ' ' << commandLine << ' ';
//	cout << "\n./wq_stat IDrun startYear endYear ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". IDrun\n";
	cout << std::setw(3) << num++ << ". Jahresunterschied zwischen baseline und Szenarien.\n";
	cout << std::setw(3) << num++ << ". IDrun Szenario 1\n";
	cout << std::setw(3) << num++ << ". IDrun Szenario 1\n";
	cout << std::setw(3) << num++ << ". IDrun Szenario 1\n";
	cout << std::setw(3) << num++ << ". water type, Tabelle wq_general._watertype\n"
		 << "      1: River\n";
	optionsClass::Help(num);
} // Help()
