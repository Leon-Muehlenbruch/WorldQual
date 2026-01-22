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
#include "../options/options.h"
#include "options_fill_wq_load.h"
using namespace std;

//options_fill_wq_loadClass::options_fill_wq_loadClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//}

options_fill_wq_loadClass::~options_fill_wq_loadClass() {
	// TODO Auto-generated destructor stub
	cout << "destructor options_wqClass\n";
}

short options_fill_wq_loadClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	parameter_id = 310;
	project_id = 4;
	IDScenBeg = 91;
	IDScenEnd = 91;

	//#############
	// read OPTIONS.DAT
	int parent_number=5; // ab diesem Nummer sind Optionen von der Klasse optionsClass

	FILE *file_ptr;
	file_ptr = fopen(filename_opt, "r");
	if (file_ptr != NULL) {
		printf("Reading: %s\n\n", filename_opt);
		int i = 0, n;

		// so lange options.dat noch nicht beendet und Zeilen von der optionsClass nicht angefangen
		while (fgets(string, maxcharlength, file_ptr) && i<parent_number-1) {
			if (0 == strncmp(string, "Value:", 6)) {
				i++;
				n = atoi(&string[7]);
				if (1 == i) {
					parameter_id = n;
				}
				if (2 == i) {
					project_id = n;
				}
				if (3 == i) {
					IDScenBeg = n;
				}
				if (4 == i) {
					if (n < IDScenBeg) {
						cerr << "OPTIONS ERROR: 4. Option 'IDScenEnd' darf nicht kleiner als IDScenBeg sein!\n";
						cerr << "scenarios: \t" << IDScenBeg << "\t-\t" << IDScenEnd<< endl;
						return 1;
					} else {
						IDScenEnd = n;
					}
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

	// Ausgabe wq_load-Optionen
	cout << "parameter_id: \t" << parameter_id<< endl;
	cout << "project_id: \t" << project_id<< endl;
	if (IDScenEnd!= IDScenBeg)
		cout << "scenarios: \t" << IDScenBeg << "\t-\t" << IDScenEnd<< endl;
	else
		cout << "scenario IDScen: \t" << IDScenBeg << endl;

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt

	return 0;
 } // init()


void options_fill_wq_loadClass::Help(int num) {
	num = 1;
	cout << "\n./fill_worldqual_load yearStart yearEnd ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". parameter_id aus der Tabelle wq_general.wq_parameter. Z.B.\n"
		 << "      80:  BOD\n"
		 << "      260: TDS\n"
		 << "      310: FC\n";

	cout << std::setw(3) << num++ << ". project_id, Tabelle wq_general._project\n"
		 << "      1: CESR\n"
		 << "      2: WWQA\n"
		 << "      3: CESR Sensitivity Analysis\n"
		 << "      4: test, für Entwickler\n";

	cout << std::setw(3) << num++ << ". IDScen, wq_general._szenario\n";

	cout << std::setw(3) << num++ << ". IDScen_end, wq_general._szenario\n"
	 << "      Wenn nicht gleich IDScen, es werden mehrere Szenarien nach einander bearbeitet: [IDScen; IDScen_end]\n";

	optionsClass::Help(num);
}
