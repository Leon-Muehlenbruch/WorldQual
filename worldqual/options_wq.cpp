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
#include "options_wq.h"
using namespace std;

//options_wqClass::options_wqClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//}

options_wqClass::~options_wqClass() {
	// TODO Auto-generated destructor stub
	cout << "destructor options_wqClass\n";
}

short options_wqClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	riverLength = 0;
	routingOrder = 0;
	flowVelocityVariant = 0;
	ConstVelocity = 0.0;
	climate = 0;
	isWaterTempFromAir = 0;
	path_watergap_output  = new char[maxcharlength];
	path_climate  = new char[maxcharlength];
	path_hydro_input  = new char[maxcharlength];
	path_hydro_routing  = new char[maxcharlength];
	loadingType = 0;

	//#############
	// read OPTIONS.DAT
	int parent_number=12; // ab diesem Nummer sind Optionen von der Klasse optionsClass

	FILE *file_ptr;
	file_ptr = fopen(filename_opt, "r");
	if (file_ptr != NULL) {
		printf("Reading: %s\n\n", filename_opt);
		int i = 0, n;

		// so lange options.dat noch nicht beendet und Zeilen von der optionsClass nicht angefangen
		while (fgets(string, maxcharlength, file_ptr) && i<parent_number-1) {
			if (0 == strncmp(string, "Value:", 6)) {
				cout << i+1 << string << endl;
				i++;
				n = atoi(&string[7]);
				if (1 == i) {
					if (n == 0 || n == 1)
						riverLength = n;
					else {
						cerr << "OPTIONS ERROR: 1. Option 'river length' darf 0 oder 1 sein!\n";
						return 1;
					}
				}
				if (2 == i) {
					if (n == 0 || n == 1)
						routingOrder = n;
					else {
						cerr << "OPTIONS ERROR: 2. Option 'routing order' darf 0 oder 1 sein!\n";
						return 1;
					}
				}
				if (3 == i) {
					if (n == 0 || n == 1 || n == 2)
						flowVelocityVariant = n;
					else {
						cerr << "OPTIONS ERROR: 8. Option 'Flie�geschwindigkeit'.\n";
						cerr << "  0: mit variablen Flie�geschwindigkeit  aus der Tabelle oder UNF-Datei  als Standart ###\n";
						cerr << "  1: mit variablen Flie�geschwindigkeit (berechnet)\n";
						cerr << "  2: mit konstanten Flie�geschwindigkeit (ein Wert f�r alle Zellen)\n";
						return 1;
					}
				}
				if (4 == i) {
					if (flowVelocityVariant == 2)
						ConstVelocity = atof(&string[7]);
					else ConstVelocity = 0.;
				}
				if (5 == i) {
					if (n == 0 || n == 1 || 2)
						climate = n;
					else {
						cerr << "OPTIONS ERROR: 10. Option 'Klimainput' darf 0 oder 1 sein!\n";
						cerr << "  0: Klimainput in 0.5 \n";
						cerr << "  1: Klimainput 5 min\n";
						cerr << "  2: Klimainput in 0.5 CRU/WATCH\n";
						return 1;
					}
				}
				if (6 == i) {
					if (n == 0 || n == 1)
						isWaterTempFromAir = n;
					else {
						cerr << "OPTIONS ERROR: 6. water temperature darf 0 oder 1 sein!\n";
						return 1;
					}
				}
				if (7 == i) {
					StringCheck(&string[7]);
					strcpy(path_watergap_output, &string[7]);
				}
				if (8 == i) {
					StringCheck(&string[7]);
					strcpy(path_climate, &string[7]);
				}
				if (9 == i) {
					StringCheck(&string[7]);
					strcpy(path_hydro_input, &string[7]);
				}
				if (10 == i) {
					StringCheck(&string[7]);
					strcpy(path_hydro_routing, &string[7]);
				}
				if (11 == i) {
					loadingType = n;
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
	if (riverLength)
		cout << "calculate river length" <<endl;
	else
		cout << "no calculate river length" <<endl;
	if (routingOrder)
		cout << "calculate routing order" <<endl;
	else
		cout << "no calculate routing order" <<endl;

	switch (flowVelocityVariant) {
		case 0:
			if (!InputType) cout << "mit variablen Fliessgeschwindigkeit  aus der Tabelle\n";
			else cout << "mit variablen Fliessgeschwindigkeit  aus UNF-Dateien\n";
			break;
		case 1: cout << "mit variablen Fliessgeschwindigkeit (berechnet im Programm)\n"; break;
		case 2: cout << "mit konstanten Fliessgeschwindigkeit = "<< ConstVelocity <<'\n'; break;
	}

	switch (climate) {
	case 0: cout << "Climate Input: 0.5°\n"; break;
	case 1: cout << "Climate Input: 5min\n"; break;
	case 2: cout << "Climate Input: 0.5° CRU/WATCH\n"; break;
	}
	if (isWaterTempFromAir)
		cout << "Water temperature is from DB, IDTemp from wq_general._runlist" <<endl;
	else {
		cout << "Water temperature calculated from air temperature" <<endl;
		if (climate==0) cout << "   wird G_ELEV_MEAN_CLM.UNF0 benötigt." << endl;
		else if (climate==2) cout << "   wird G_ELEV_MEAN_WATCH.UNF0 benötigt." << endl;
	}
	cout << "path to watergap_output   : "<< path_watergap_output << "\n"; 
	cout << "path to climate           : "<< path_climate << "\n"; 
	cout << "path to hydro_input       : "<< path_hydro_input << "\n"; 
	cout << "path to path_hydro_routing: "<< path_hydro_routing << "\n"; 

	switch(loadingType){
		case 0: cout << "Loading is in the Table load\n"; break;
		case 1: cout << "Loading is in the Table load_[IDScen]_[parameter_id]\n"; break;
		case 2: cout << "Loading read from wq_load OUTPUT (Tables calc_cell_monthly_load_[type])\n"; break;
	}
	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt

	return 0;
 } // init()

void options_wqClass::Help(int num) {
	num = 1;
	cout << programme << ' ' << commandLine << ' ';
//	cout << "\n./wq_stat IDrun startYear endYear ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". river length\n"
		 << "      0: no calculate ###\n"
		 << "      1: calculate\n";

	cout << std::setw(3) << num++ << ". routing order\n"
		 << "      0: no calculate ###\n"
		 << "      1: calculate\n";

	cout << std::setw(3) << num++ << ". Fließgeschwindigkeit\n"
		 << "      0: mit variablen Fließgeschwindigkeit  aus der Tabelle oder UNF-Datei  als Standart\n"
		 << "      1: mit variablen Fließgeschwindigkeit (berechnet) ###\n"
		 << "      2: mit konstanten Fließgeschwindigkeit (ein Wert für alle Zellen)\n";

	cout << std::setw(3) << num++ << ". Wert für konstante Fließgeschwindigkeit (nur bei 3. == 2: mit konstanten Flie�geschwindigkeit)\n";

	cout << std::setw(3) << num++ << ". Klimainput\n"
		 << "      0: in 0.5° ###\n"
		 << "      1. in 5 min\n";
	
	cout << std::setw(3) << num++ << ". water temperature\n"
		 << "      0: from DB, IDTemp from wq_general._runlist ###\n"
		 << "      1: from air temperature\n";

	cout << std::setw(3) << num++ << ". path_watergap_output\n"
		 << "      OUTPUT von WaterGAP (hydrology): 	G_RIVER_VELOCITY_%d.12.UNF0 G_GLO_LAKE_STOR_MEAN_%d.12.UNF0 G_RES_STOR_MEAN_%d.12.UNF0\n"
		 << "      Value: /path/to/watergap-Output\n";

	cout << std::setw(3) << num++ << ". path_climate\n"
 		 << "      GSHORTWAVE_%d.12.UNF2; vor water temperature from air temperature: GTEMP_[year].12.UNF2\n";

	cout << std::setw(3) << num++ << ". path_hydro_input\n"
		 << "      G_WG3_WG2WITH5MIN.UNF4 G_MEANDERING_RATIO.UNF0; \n"
		 << "      if water temperature calculated fro air temp and climate in 0.5 or 0.5 CRU/WATCH : G_ELEV_MEAN_CLM.UNF0, G_ELEV_MEAN_WATCH.UNF0\n";

	cout << std::setw(3) << num++ << ". path_hydro_routing\n"
		 << "        GCELLDIST.9.UNF0\n";

	optionsClass::Help(num);
} // Help()
