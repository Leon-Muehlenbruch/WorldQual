/*
 * optionsfindRiverCells.h
 *
 *  Created on: 17.08.2016
 *      Author: kynast
 */

#ifndef OPTIONSFINDRIVERCELLS_H_
#define OPTIONSFINDRIVERCELLS_H_

//#include <options.h>
#include "../options/options.h"
#include "general_functions.h"


class options_findRiverCellsClass: public optionsClass {
public:
	options_findRiverCellsClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL)
	{
		inputfile = new char[maxcharlength];
		useInputFile = true;

		strcpy(inputfile,"STAT.DAT");

		startGlobal = NODATA;
		startArcID = NODATA;
		endGlobal = NODATA;
		endArcID = NODATA;
	}
	virtual ~options_findRiverCellsClass();

	char* inputfile;
	bool useInputFile;
	// start- globalID und ArcID, falls keine inputdatei verwendet
	int startGlobal, startArcID;
	// end- globalID und ArcID, falls keine inputdatei verwendet
	int endGlobal, endArcID;

	short init(int optionc, char* optionv[]);
	void printOptions();
	void Help(int num=1);

};

#endif /* OPTIONSFINDRIVERCELLS_H_ */
