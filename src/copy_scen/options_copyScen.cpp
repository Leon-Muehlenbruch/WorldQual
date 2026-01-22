/*
 * options.cpp
 *
 *  Created on: 22.05.2015
 *      Author: kynast
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>      // std::setw

//#include "../general_function/general_functions.h"
#include "../options/options.h"
#include "options_copyScen.h"
using namespace std;

//options_copyScenClass::options_copyScenClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//}

options_copyScenClass::~options_copyScenClass() {
	// TODO Auto-generated destructor stub
}

short options_copyScenClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	SQLexecute = false;
	refProject_id = 4;
	refScen = 9;
	project_id = 4;
	IDScenBeg = 91;
	IDScenEnd = 91;
	parameter_id = 310;

	//#############
	// read OPTIONS.DAT
	int parent_number=8; // ab diesem Nummer sind Optionen von der Klasse optionsClass

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
					SQLexecute = n;
				}
				if (2 == i) {
					refProject_id = n;
				}
				if (3 == i) {
					refScen = n;
				}
				if (4 == i) {
					project_id = n;
				}
				if (5 == i) {
					IDScenBeg = n;
				}
				if (6 == i) {
					if (n >= IDScenBeg) {
						IDScenEnd = n;
					} else {
						cerr << "IDScenEnd darf nicht kleiner als IDScenBegin sein!\n"
								<< "IDScenBeg: \t"<< IDScenBeg <<endl
								<< "IDScenEnd: \t"<< IDScenEnd << endl;
						return 1;
					}
				}
				if (7 == i) {
					parameter_id = n;
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
	cout << "############################################################"<<endl;

	// Ausgabe create scenario-Optionen
	if (SQLexecute) cout << "SQL Anweisungen werden erzeugt, in einer Datei gespeichert und ausgef�hrt." << endl;
	else cout << "SQL Anweisungen werden erzeugt, in einer Datei gespeichert aber nicht ausgef�hrt." << endl
			<< "Vielleicht zuerst anpassen (Falls Kopie vom Referenzszenario sich unterscheiden soll) und dann selbst ausf�hren." << endl;

	cout << "project_id: \t" << project_id<< endl;
	cout << "reference scenario: \t" << refScen << endl;
	if (IDScenBeg != IDScenEnd)
		cout << "create scenarios: \t" << IDScenBeg << "-" << IDScenEnd << endl;
	else
		cout << "create scenario: \t" << IDScenBeg << endl;
	cout << "parameter_id: \t" << parameter_id<< endl;

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt
	cout << "############################################################"<<endl;

	return 0;
 } // init()

void options_copyScenClass::Help(int num) {
	num = 1;
	cout << programme << ' ' << commandLine << ' ';
//	cout << "\n./copy_scen StartYear EndYear ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". execute type\n"
		 << "      0: nur sql-erzeugen, wird nicht ausgeführt (muss man per Hand machen, evtl. anpassen)\n"
		 << "      1: sql erzeugen und ausführen\n";

	cout << std::setw(3) << num++ << ". Referenz project_id, Tabelle wq_general._project\n"
		 << "      1: CESR\n"
		 << "      2: WWQA\n"
		 << "      3: CESR Sensitivity Analysis\n"
		 << "      4: test, für Entwickler\n";

	cout << std::setw(3) << num++ << ". Referenz IDScen, wq_general._szenario\n";

	cout << std::setw(3) << num++ << ". Ziel project_id , Tabelle wq_general._project\n"
		 << "   Falls neu: muss  in die Tabelle händisch eingetragen werden und Zieldatenbank erzeugt und Rechte vergeben werden!\n"
		 << "      1: CESR\n"
		 << "      2: WWQA\n"
		 << "      3: CESR Sensitivity Analysis\n"
		 << "      4: test, für Entwickler\n";

	cout << std::setw(3) << num++ << ". Ziel IDScenBegin, wq_general._szenario\n"
		 << "      Falls in wq_general._szenario IDScen schon gibt, es wird nicht geändert, falls noch nicht gibt, es wird eine Zeile eingefügt\n"
		 << "      mit der description: \"copy from idscen ... project ... [description von Referenzszenario]\"\n";

	cout << std::setw(3) << num++ << ". Ziel IDScenEnd, wq_general._szenario\n"
		 << "      darf größer oder gleich IDScenBegin sein. Falls größer, es werden mehrere Szenarien erzeugt als Kopie von Referenzszenario\n";

	cout << std::setw(3) << num++ << ". parameter_id aus der Tabelle wq_general.wq_parameter. Z.B.\n"
		 << "      80:  BOD\n"
		 << "      260: TDS\n"
		 << "      310: FC\n";

	optionsClass::Help(num);
}
