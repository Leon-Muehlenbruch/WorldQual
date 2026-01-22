/*
 * options.h
 *
 *  Created on: 11.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_WQSTAT_SCEN_H_
#define OPTIONS_WQSTAT_SCEN_H_

#include "options.h"

class options_wqstat_scenClass: public optionsClass {
public:
	options_wqstat_scenClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		IDrun = 0;
		IDrun_scen1 = 0;
		IDrun_scen2 = 0;
		IDrun_scen3 = 0;
		DiffYears = 0;
		watertype = 1;
	}
	virtual ~options_wqstat_scenClass();

	// Optionen f�r wq_stat_scen
	int IDrun, 	IDrun_scen1, IDrun_scen2, IDrun_scen3; // IDRun f�r baseline und Szenarien
	int DiffYears; // Jahresunterschied zwischen baseline und Szenarien
	int watertype;                  // default: 1, set on river

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_WQSTAT_SCEN_H_ */
