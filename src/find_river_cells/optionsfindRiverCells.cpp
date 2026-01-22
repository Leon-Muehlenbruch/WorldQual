/*
 * optionsfindRiverCells.cpp
 *
 *  Created on: 17.08.2016
 *      Author: kynast
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "options.h"
#include "optionsfindRiverCells.h"
//#include "general_functions.h"
using namespace std;

//options_findRiverCellsClass::options_findRiverCellsClass() {
//	inputfile = new char[maxcharlength];
//	useInputFile = true;
//
//	strcpy(inputfile,"STAT.DAT");
//
//	startGlobal = NODATA;
//	startArcID = NODATA;
//	endGlobal = NODATA;
//	endArcID = NODATA;
//}
//options_findRiverCellsClass::options_findRiverCellsClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
//		inputfile = new char[maxcharlength];
//		useInputFile = true;
//
//		strcpy(inputfile,"STAT.DAT");
//
//		startGlobal = NODATA;
//		startArcID = NODATA;
//		endGlobal = NODATA;
//		endArcID = NODATA;
//};

options_findRiverCellsClass::~options_findRiverCellsClass() {
	// TODO Auto-generated destructor stub
	delete[] inputfile;
}

short options_findRiverCellsClass::init(int option_c, char* option_v[])
{
	char string[maxcharlength];

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	optionsClass::init(option_c, option_v); // initiiren allgemeine Optionen
	cout << "optionsClass::init done\n";
	strcpy(inputfile,"STAT.DAT");

	startGlobal = NODATA;
	startArcID = NODATA;
	endGlobal = NODATA;
	endArcID = NODATA;

	//#############
	// wenn in der Befehlszeile start- und/oder end-GlobalID eingegeben, lesen
	char option[5];      // to process options, e.g. "-d", "-t", "-o", "-r", "-s"
	char optionValue[maxcharlength]; // option value (filenames)


	for (int num=0; num<option_c; num++) {
		cout << num << endl;
		separateCommandLineOption(option_v[num], option, optionValue);
		cout << "---\n";
		if (!strcmp(option,"-s")) {
			startGlobal     = atoi(optionValue);
			useInputFile = false;
		}
		else if (!strcmp(option,"-e")) {
			endGlobal     = atoi(optionValue);
			useInputFile = false;
		}
	} // for(i)
	cout << "options done\n";

	//#############
	// read OPTIONS.DAT
	int parent_number=2; // ab diesem Nummer sind Optionen von der Klasse optionsClass

	FILE *file_ptr;
	file_ptr = fopen(filename_opt, "r");
	if (file_ptr != NULL) {
		printf("Reading: %s\n\n", filename_opt);
		int i = 0;

		while (fgets(string, maxcharlength, file_ptr) && i<parent_number-1) {
			if (0 == strncmp(string, "Value:", 6)) {
				i++;
				StringCheck(&string[7]);
				strcpy(inputfile, &string[7]);
			} // if(Value)
		}
		if (read_parentOptions(parent_number, file_ptr)) return 1;
		fclose(file_ptr);
	} else {
		printf("\nError while opening file '%s'.\n", filename_opt);
		//printf("Using default options.\n");
		Help();
		return 1;
	}

	return 0;
 } // init()

void options_findRiverCellsClass::printOptions() {
	//#############
	// Options Ausgabe

	// Ausgabe worldqual-Optionen
	if (useInputFile) {
		cout << "inputfile: " << inputfile << endl;
	} else if (startGlobal != NODATA && endGlobal != NODATA) {
			cout << "finden river ab " << startGlobal << "(" << startArcID
				<< ") bis " << endGlobal << "(" << endArcID << ")\n";
	} else if (startGlobal != NODATA) {
		cout << "finden river ab " << startGlobal << "(" << startArcID
				<< ") bis Flüssmündung \n";
	} else {
		cout << "finden river ab Flussquelle bis " << endGlobal << "(" << endArcID << ")\n";;
	}

	optionsClass::printOptions(); // Ausgabe allgemeinen Optionen, continent_abb wird hier gef�llt

}

void options_findRiverCellsClass::Help(int num) {
	num = 1;
	cout << programme << ' ' << commandLine << ' ';
//	cout << "\n./find_river_cells [-s startGlobal_id] [-e endGlobal_id] ";
	cout << "[-ooptions_file (default: OPTIONS.DAT)]\n";
	cout << "\nStruktur von  OPTIONS.DAT:\n";
	cout << std::setw(3) << num++ << ". Datei mit Flüssen.\n"
			<< "     wird nur benutzt, wenn in der Befehlszeile keine Start- und/oder End-GlobalID eingegeben wurden\n"
			<< "     Datei soll aus drei Spalten bestehen: Rivername, Start-GlobalID, End-GlobalID getrennt mit tab\n";
	optionsClass::Help(num);
}
