/*
 * options.h
 *
 *  Created on: 22.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_SCEN_H_
#define OPTIONS_SCEN_H_

class options_copyScenClass: public optionsClass {
public:
	options_copyScenClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		SQLexecute = false;
		refProject_id = 4;
		refScen = 9;
		project_id = 4;
		IDScenBeg = 91;
		IDScenEnd = 91;
		parameter_id = 310;
	}
	virtual ~options_copyScenClass();

	// Optionen f�r create scenarios
	bool SQLexecute; // false: SQL's werden in einer Datei gespeichert aber nicht ausgef�hrt; true: sie werden gespeichert und ausgef�hrt
	int refProject_id, refScen;
	int project_id;
	int IDScenBeg, IDScenEnd;
	int parameter_id;

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_SCEN_H_ */
