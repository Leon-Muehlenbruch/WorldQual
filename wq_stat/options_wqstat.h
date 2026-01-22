/*
 * options.h
 *
 *  Created on: 11.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_WQSTAT_H_
#define OPTIONS_WQSTAT_H_

class options_wqstatClass: public optionsClass {
public:
//	options_wqstatClass();
	options_wqstatClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		flowVelocityVariant = 0;
		ConstVelocity = 0.;
		climate = 0;
		watertype = 1;
	};
	virtual ~options_wqstatClass();

	// Optionen f�r wq_stat
	short flowVelocityVariant;      // 0 - mit variablen Fliessgeschwindigkeit  aus der Tabelle oder UNF-Datei  als Standart
	                                // 1 - mit variablen Fliessgeschwindigkeit (berechnet)
	                                // 2 - mit konstanten Fliessgeschwindigkeit (ein Wert f�r alle Zellen)
	double ConstVelocity;           // Wenn mit konstanten Fliessgeschwindigkeit, Wert wird aus Befehlszeile entnommen;

	short climate;				    // climate input in 0.5� = 0 ; 5min = 1; default 0
	int watertype;                  // default: 1, set on river

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:
};

#endif /* OPTIONS_WQSTAT_H_ */
