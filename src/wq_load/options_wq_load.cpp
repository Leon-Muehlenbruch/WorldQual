/*
 * options.cpp
 *
 *  Created on: 22.05.2015
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
#include "options_wq_load.h"
using namespace std;

//options_wq_loadClass::options_wq_loadClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//}

options_wq_loadClass::~options_wq_loadClass() {
	// TODO Auto-generated destructor stub
	cout << "destructor options_wqClass\n";
}

short options_wq_loadClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	parameter_id = 310;
	following_year = 0;
	project_id = 4;
	IDScen = 91;
	manure_timing = 0;
	climate = 0;
	gridded_pop_from_file = 0;
	gridded_pop_path      = new char[maxcharlength];
	path_watergap_output  = new char[maxcharlength];
	path_livestock_output = new char[maxcharlength];
	path_corr_factor      = new char[maxcharlength];
	path_climate          = new char[maxcharlength];
	path_gnrd             = new char[maxcharlength];
	path_tp_input         = new char[maxcharlength];
	path_tp_input2        = new char[maxcharlength];
	path_tp_input3        = new char[maxcharlength];
	IDInTableName = 0;


	//#############
	// read OPTIONS.DAT
	int parent_number=18; // ab diesem Nummer sind Optionen von der Klasse optionsClass

	FILE *file_ptr;
	file_ptr = fopen(filename_opt, "r");
	if (file_ptr != NULL) {
		printf("Reading: %s\n\n", filename_opt);
		int i = 0, n;

		// so lange options.dat noch nicht beendet und Zeilen von der optionsClass nicht angefangen
		while (fgets(string, maxcharlength, file_ptr) && i<parent_number-1) {
			cout << string << endl;
			if (0 == strncmp(string, "Value:", 6)) {
				i++;
				n = atoi(&string[7]);
				if (1 == i) {
					parameter_id = n;
				}
				if (2 == i) {
					if (n == 0 || n == 1)
						following_year = n;
					else {
						cerr << "OPTIONS ERROR: 2. Option 'following_year timing' darf 0 oder 1 sein!\n";
						return 1;
					}
				}
				if (3 == i) {
					project_id = n;
				}
				if (4 == i) {
					IDScen = n;
				}
				if (5 == i) {
					if (n == 0 || n == 1)
						manure_timing = n;
					else {
						cerr << "OPTIONS ERROR: 5. Option 'manure timing' darf 0 oder 1 sein!\n";
						return 1;
					}
				}
				if (6 == i) {
					if (n == 0 || n == 1 || n==2)
						climate = n;
					else {
						cerr << "OPTIONS ERROR: 6. Option 'Klimainput' darf 0 oder 1 sein!\n";
						cerr << "  0: Klimainput in 0.5�\n";
						cerr << "  1: Klimainput 5 min\n";
						cerr << "  2: Klimainput in 0.5 CRU/WATCH\n";
						return 1;
					}
				}
				if (7 == i) {
					gridded_pop_from_file = n;
				}
				if (8 == i) {
					StringCheck(&string[7]);
					strcpy(gridded_pop_path, &string[7]);
				}

				if (9 == i) {
					StringCheck(&string[7]);
					strcpy(path_watergap_output, &string[7]);
				}
				if (10 == i) {
					StringCheck(&string[7]);
					strcpy(path_livestock_output, &string[7]);
				}
				if (11 == i) {
					StringCheck(&string[7]);
					strcpy(path_corr_factor, &string[7]);
				}
				if (12 == i) {
					StringCheck(&string[7]);
					strcpy(path_climate, &string[7]);
				}
				if (13 == i) {
					StringCheck(&string[7]);
					strcpy(path_gnrd, &string[7]);
				}
				if (14 == i) {
					StringCheck(&string[7]);
					strcpy(path_tp_input, &string[7]);
				}
				if (15 == i) {
					StringCheck(&string[7]);
					strcpy(path_tp_input2, &string[7]);
				}
				if (16 == i) {
					StringCheck(&string[7]);
					strcpy(path_tp_input3, &string[7]);
				}
				if (17 == i) {
					IDInTableName = n;
				}
			} // if(Value)
		}
cout << "read_parentOptions:" << endl;
		if (read_parentOptions(parent_number, file_ptr)) return 1;
		fclose(file_ptr);
	} else {
		printf("\nError while opening file '%s'.\n", filename_opt);
		//printf("Using default options.\n");
		return 1;
	}

	//#############
	// Options Ausgabe

	// Ausgabe wq_load-Optionen
	cout << "parameter_id: \t" << parameter_id<< endl;
	cout << "project_id: \t" << project_id<< endl;
	cout << "IDScen: \t" << IDScen<< endl;
	switch(manure_timing){
		case 0: cout << "manure is applied all year round\n"<< endl;break;
		case 1: cout << "manure is applied from March to October\n"<< endl;break;
		default:cerr << "Error: Please enter 0 or 1"<< endl; return 1; break;
	}

	switch (climate) {
	case 0: cout << "Climate Input: 0.5°\n"; break;
	case 1: cout << "Climate Input: 5min\n"; break;
	case 2: cout << "Climate Input: 0.5° CRU/WATCH\n"; break;
	}

	switch (gridded_pop_from_file) {
	case 0: cout << "gridded population from the table wq_load_[continent].cell_input.°\n"; break;
	case 1: 
		cout << "gridded population from UNF-files\n"; 
		cout << "path to griddet population: "<< gridded_pop_path << "\n"; 
		break;
	}

	//cout << "path to hydro_input     : "<< path_hydro_input << "\n"; 
	cout << "path to watergap_output : "<< path_watergap_output << "\n"; 
	cout << "path to livestock_output: "<< path_livestock_output << "\n"; 
	cout << "path to corr_factor     : "<< path_corr_factor << "\n"; 
	cout << "path to climate         : "<< path_climate << "\n"; 
	cout << "path to GNRD            : "<< path_gnrd << "\n"; 
	cout << "path to tp/tn input     : "<< path_tp_input << "\n"; 
	cout << "path to tp/tn input 2   : "<< path_tp_input2 << "\n"; 

	if (!IDInTableName) 
		cout << "IDScen und parameterID als Spalten in den Tabellen\n";
	else 
		cout << "IDScen und parameterID in table name\n";

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gefällt

	return 0;
 } // init()

void options_wq_loadClass::Help(int num) {
	num = 1;
//	cout << "\n./wq_load year ";
	cout << programme << ' ' << commandLine << ' ';
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". parameter_id aus der Tabelle wq_general.wq_parameter. Z.B.\n"
		 << "      80:  BOD\n"
		 << "      260: TDS\n"
		 << "      310: FC\n"
		 << "      390: pesticide\n";
	cout << std::setw(3) << num++ << ". only for pesticide: is not first year?\n"
		 << "      0: first year to calculate\n"
		 << "      1: following year. there are a loading from december last year\n";
	cout << std::setw(3) << num++ << ". project_id, Tabelle wq_general._project\n"
		 << "      1: CESR\n"
		 << "      2: WWQA\n"
		 << "      3: CESR Sensitivity Analysis\n"
		 << "      4: test, für Entwickler\n";

	cout << std::setw(3) << num++ << ". IDScen, wq_general._szenario\n";

	cout << std::setw(3) << num++ << ". manure timing \n"
		 << "      0:  all year round application ###\n"
		 << "      1:  application between March and October\n";

	cout << std::setw(3) << num++ << ". Klimainput\n"
		 << "      0: in 0.5° ###\n"
		 << "      1. in 5 min\n";
	optionsClass::Help(num);
}
