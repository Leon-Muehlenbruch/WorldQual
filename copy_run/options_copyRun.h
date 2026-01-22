/*
 * options.h
 *
 *  Created on: 06.07.2015
 *      Author: kynast
 */

#ifndef OPTIONS_SCEN_H_
#define OPTIONS_SCEN_H_

class options_copyRunClass: public optionsClass {
public:
	options_copyRunClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		SQLexecute = false;
		refIDRun = 203100009;
		IDRun = 403100091;
		CopyLoad = false;
		project_id = 4;
		IDScen = 91;
	};
	virtual ~options_copyRunClass();

	// Optionen f�r create scenarios
	bool SQLexecute; // false: SQL's werden in einer Datei gespeichert aber nicht ausgef�hrt; true: sie werden gespeichert und ausgef�hrt
	int refIDRun;
	int IDRun;
	bool CopyLoad; // false: project]_worldqual_[continent].`load` nicht mitkopieren, sondern eigener mit fill_worldqual_load selbst rechnen.
	               // true:  project]_worldqual_[continent].`load` mitkopieren
	int project_id; // wenn -1: project_id aus der Referenz �bernehmen
	int IDScen;

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_SCEN_H_ */
