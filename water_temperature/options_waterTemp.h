/*
 * options.h
 *
 *  Created on: 22.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_SCEN_H_
#define OPTIONS_SCEN_H_

class options_waterTempClass: public optionsClass {
public:
	options_waterTempClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		project_id = 4;
		IDScen = 91;
		IDTemp = 0;
		climate = 0;
	}
	virtual ~options_waterTempClass();

	// Optionen f�r waterTemp
	int project_id;
	int IDScen;  // nur fuer sensitivity analyse eine Wert, sonst = NODATA!
	int IDTemp;
	short climate;				    //climate input in 0.5� = 0 ; 5min = 1; default 0

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_SCEN_H_ */
