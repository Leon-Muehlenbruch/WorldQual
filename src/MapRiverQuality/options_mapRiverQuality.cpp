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
#include "options_mapRiverQuality.h"
using namespace std;

//options_mapRiverQualityClass::options_mapRiverQualityClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//	flagTH = 0;
//}

options_mapRiverQualityClass::~options_mapRiverQualityClass() {
	// TODO Auto-generated destructor stub
	cout << "destructor options_mapRiverQualityClass\n";
}

short options_mapRiverQualityClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	flagTH = 0;

	//#############
	// read OPTIONS.DAT
	int parent_number=2; // ab diesem Nummer sind Optionen von der Klasse optionsClass

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
					if (n == 0 || n == 1)
						flagTH = n;
					else {
						cerr << "OPTIONS ERROR: 1. Option 'Thresholds used' darf 0 oder 1 sein!\n";
						cerr << "0 (general), 1 (european bathing directive thresholds - only apply for FC)\n"<<endl;
						return 1;
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

	// Ausgabe worldqual-Optionen
	if (!flagTH)
		cout << "general thresholds used" <<endl;
	else
		cout << "european bathing directive thresholds - only apply for FC" <<endl;

	if (!InputType) cout << "q und runoff  aus der Tabelle\n";
	else cout << "q und runoff  aus UNF-Dateien\n";

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt

	return 0;
 } // init()

void options_mapRiverQualityClass::Help(int num) {
	num = 1;
	cout << programme << ' ' << commandLine << ' ';
//	cout << "\n./mapRiverQuality IDRun year ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". Thresholds used\n"
		 << "      0: general ###\n"
		 << "      1: european bathing directive thresholds - only apply for FC\n";

	optionsClass::Help(num);
} // Help()

