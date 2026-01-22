/*
 * options.h
 *
 *  Created on: 22.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_SCEN_H_
#define OPTIONS_SCEN_H_

class options_rtfinput: public optionsClass {
public:
	options_rtfinput(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		project_id = 4;
		IDScen = 91;
	}
	virtual ~options_rtfinput();

	// Optionen f�r wq_load
	int project_id;
	int IDScen;
	char*  input_dir_griddi;     // input directory for G_MAN_WW/WC/WCOOL_m3_[year].UNF0
	char*  input_dir_gwsws;      // input directory for G_IRRIG_RTF_SW_m3_[year].12.UNF0
	char*  input_dir_corrfactor; // input directory for G_CORR_FACT_RTF_[year].12.UNF0

	// IDScen und parameterID in table name (cell_input, cell_) default: 0
	short IDInTableName;

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_SCEN_H_ */
