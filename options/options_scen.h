/*
 * options.h
 *
 *  Created on: 22.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_SCEN_H_
#define OPTIONS_SCEN_H_

class options_scenClass: public optionsClass {
public:
	options_scenClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		project_id = 4;
		IDScen = 91;
	}
	virtual ~options_scenClass();

	// Optionen f�r wq_load
	int project_id;
	int IDScen;

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_SCEN_H_ */
