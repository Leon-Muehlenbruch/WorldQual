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
#include "options_wqstat.h"
using namespace std;

//options_wqstatClass::options_wqstatClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//}

//options_wqstatClass(int option_c, char* option_v[]) : optionsClass(option_c, option_v)  {
//
//}

options_wqstatClass::~options_wqstatClass() {
	// TODO Auto-generated destructor stub
//	cout << "destructor options_wqstatClass\n";
}

short options_wqstatClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	flowVelocityVariant = 0;
	ConstVelocity = 0.0;
	climate = 0;
	watertype = 1;

	//#############
	// read OPTIONS.DAT
	int parent_number=5; // ab diesem Nummer sind Optionen von der Klasse optionsClass

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
				if (2 == i) {
					if (flowVelocityVariant == 2)
						ConstVelocity = atof(&string[7]);
					else ConstVelocity = 0.;
				}
				if (3 == i) {
					if (n == 0 || n == 1)
						climate = n;
					else {
						cerr << "OPTIONS ERROR: 10. Option 'Klimainput' darf 0 oder 1 sein!\n";
						cerr << "  0: Klimainput in 0.5�\n";
						cerr << "  1: Klimainput 5 min\n";
						return 1;
					}
				}
				if (4 == i) {
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
	switch (flowVelocityVariant) {
		case 0:
			if (!InputType) cout << "mit variablen Fliessgeschwindigkeit  aus der Tabelle\n";
			else cout << "mit variablen Fliessgeschwindigkeit  aus UNF-Dateien\n";
			break;
		case 1: cout << "mit variablen Fliessgeschwindigkeit (berechnet im Programm)\n"; break;
		case 2: cout << "mit konstanten Fliessgeschwindigkeit = "<< ConstVelocity <<'\n'; break;
	}

	switch (climate) {
	case 0: cout << "Climate Input: 0.5�\n"; break;
	case 1: cout << "Climate Input: 5min\n"; break;
	}

	if (!InputType) cout << "q und runoff  aus der Tabelle\n";
	else cout << "q und runoff  aus UNF-Dateien\n";

	cout << "watertype: " << watertype      << endl;

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt

	return 0;
 } // init()

void options_wqstatClass::Help(int num) {
	num = 1;
	cout << programme << ' ' << commandLine << ' ';
//	cout << "\n./wq_stat IDrun startYear endYear ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". Fließgeschwindigkeit\n"
		 << "      0: mit variablen Fließgeschwindigkeit  aus der Tabelle oder UNF-Datei  als Standart\n"
		 << "      1: mit variablen Fließgeschwindigkeit (berechnet) ###\n"
		 << "      2: mit konstanten Fließgeschwindigkeit (ein Wert für alle Zellen)\n";
	cout << std::setw(3) << num++ << ". Wert für konstante Fließgeschwindigkeit (nur bei 1. == 2: mit konstanten Flie�geschwindigkeit)\n";
	cout << std::setw(3) << num++ << ". Klimainput\n"
		<< "      0: in 0.5° ###\n"
		<< "      1. in 5 min\n";
	cout << std::setw(3) << num++ << ". water type, Tabelle wq_general._watertype\n"
		 << "      1: River\n";
	optionsClass::Help(num);
} // Help()
