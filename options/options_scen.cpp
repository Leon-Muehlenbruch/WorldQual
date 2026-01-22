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
#include "options_scen.h"
using namespace std;

//options_scenClass::options_scenClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//}

options_scenClass::~options_scenClass() {
	// TODO Auto-generated destructor stub
	cout << "destructor options_scenClass\n";
}

short options_scenClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	project_id = 4;
	IDScen = 91;

	//#############
	// read OPTIONS.DAT
	int parent_number=3; // ab diesem Nummer sind Optionen von der Klasse optionsClass

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
					project_id = n;
				}
				if (2 == i) {
					IDScen = n;
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

	// Ausgabe scen-Optionen
	cout << "project_id: \t" << project_id<< endl;
	cout << "scenario IDScen: \t" << IDScen << endl;

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt

	return 0;
 } // init()

void options_scenClass::Help(int num) {
	num = 1;
	cout << programme << ' ' << commandLine << ' ';
//	cout << "\n./rtf_input startYear endYear ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". project_id, Tabelle wq_general._project\n"
		 << "      1: CESR\n"
		 << "      2: WWQA\n"
		 << "      3: CESR Sensitivity Analysis\n"
		 << "      4: test, für Entwickler\n";

	cout << std::setw(3) << num++ << ". IDScen, wq_general._szenario\n";

	optionsClass::Help(num);
}
