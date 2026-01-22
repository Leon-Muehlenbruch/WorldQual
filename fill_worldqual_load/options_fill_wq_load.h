/*
 * options.h
 *
 *  Created on: 22.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_FILL_WQ_LOAD_H_
#define OPTIONS_FILL_WQ_LOAD_H_

class options_fill_wq_loadClass: public optionsClass {
public:
	options_fill_wq_loadClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		parameter_id = 310;
		project_id = 4;
		IDScenBeg = 91;
		IDScenEnd = 91;
	}
	virtual ~options_fill_wq_loadClass();

	// Optionen f�r wq_load
	int parameter_id;
	int project_id;
	int IDScenBeg, IDScenEnd;

	// Achtung! in dieser Klasse werden OptionsClass-Mitglieder InputType und input_dir nicht vewendet!

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_FILL_WQ_LOAD_H_ */
