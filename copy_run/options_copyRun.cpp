/*
 * options.cpp
 *
 *  Created on: 06.07.2015
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
#include "options_copyRun.h"
using namespace std;

//options_copyRunClass::options_copyRunClass() :optionsClass() {
//	// TODO Auto-generated constructor stub
//}

options_copyRunClass::~options_copyRunClass() {
	// TODO Auto-generated destructor stub
}

short options_copyRunClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	SQLexecute = false;
	refIDRun = 203100009;
	IDRun = 403100091;
	CopyLoad = false;
	project_id = 4;
	IDScen = 91;

	//#############
	// read OPTIONS.DAT
	int parent_number=7; // ab diesem Nummer sind Optionen von der Klasse optionsClass

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
					refIDRun = n;
				}
				if (3 == i) {
					IDRun = n;
				}
				if (4 == i) {
					if (n == 0 || n == 1) {
						CopyLoad = n;
					} else {
						cerr << "CopyLoad darf nur 0 oder 1 sein!\n";
						return 1;
					}
				}
				if (5 == i) {
					project_id = n;
				}
				if (6 == i) {
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
	cout << "############################################################"<<endl;

	// Ausgabe create scenario-Optionen
	if (SQLexecute) cout << "SQL Anweisungen werden erzeugt, in einer Datei gespeichert und ausgef�hrt." << endl;
	else cout << "SQL Anweisungen werden erzeugt, in einer Datei gespeichert aber nicht ausgef�hrt." << endl
			<< "Vielleicht zuerst anpassen (Falls Kopie vom Referenzszenario sich unterscheiden soll) und dann selbst ausf�hren." << endl;

	cout << "reference run: \t" << refIDRun<< endl;
	cout << "create run   : \t" << IDRun<< endl;

	if (CopyLoad) cout << "Tabelle load wird mitkopiert." << endl;
	else cout << "Tabelle load wird nicht mitkopiert" << endl;

	cout << "create scenario: \t" << IDScen << "in project with project_id: \t" << project_id<< endl;

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt
	cout << "############################################################"<<endl;

	return 0;
 } // init()

void options_copyRunClass::Help(int num) {
	num = 1;
	cout << programme << ' ' << commandLine << ' ';
//	cout << "\n./copy_run StartYear EndYear ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". execute type\n"
		 << "      0: nur sql-erzeugen, wird nicht ausgeführt (muss man per Hand machen, evtl. anpassen)\n"
		 << "      1: sql erzeugen und ausführen\n";

	cout << std::setw(3) << num++ << ". Referenz IDRun, Tabelle wq_general._runlist\n";

	cout << std::setw(3) << num++ << ". Ziel IDRun, Tabelle wq_general._runlist\n"
		 << "      Falls in der Tabelle wq_general._runlist IDRun schon gibt, wird überprüft, ob es gleiche Werte sind, Falls nicht Fehlermeldung und Abbruch,\n"
		 << "      Falls noch nicht gibt, es wird eine Zeile eingefügt\n"
		 << "      mit der comment: \"copy from idrun  ... [comment von Referenzrun]\"\n";

	cout << std::setw(3) << num++ << ". load:\n"
		 << "      0: kein [project]_worldqual_[continent].`load` kopieren, sondern eigener mit fill_worldqual_load selbst rechnen. \n"
		 << "      Falls wq_load keine reine Kopie von der Referenz, sondern angepasste, man sollte loading selbst rechnen\n"
		 << "      1: [project]_worldqual_[continent].`load` mitkopieren\n";

	cout << std::setw(3) << num++ << ". Ziel project_id, Tabelle wq_general._project\n"
		 << "      Falls neu: muss  in die Tabelle händisch eingetragen werden und Zieldatenbank erzeugt und Rechte vergeben werden!\n"
		 << "      -1: Referenz project_id übernehmen\n"
		 << "      1:  CESR\n"
		 << "      2:  WWQA\n"
		 << "      3:  CESR Sensitivity Analysis\n"
		 << "      4:  test, für Entwickler\n";

	cout << std::setw(3) << num++ << ". Ziel IDScen, wq_general._szenario\n"
		 << "      Falls in wq_general._szenario IDScen schon gibt, es wird nicht geändert, falls noch nicht gibt, es wird eine Zeile eingefügt\n"
		 << "      mit der description: \"copy from idscen ... project ... [description von Referenzszenario]\"\n";

	optionsClass::Help(num);
}
