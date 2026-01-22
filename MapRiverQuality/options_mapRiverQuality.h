/*
 * options.h
 *
 *  Created on: 11.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_WQ_H_
#define OPTIONS_WQ_H_

class options_mapRiverQualityClass: public optionsClass {
public:
	options_mapRiverQualityClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		flagTH = 0;
	}
	virtual ~options_mapRiverQualityClass();

	// Optionen f�r mapRiverQuality

	//Thresholds used -> 0 standard thresholds -> 1 e.g. thresholds of European bathing directive etc. depending on substance used
	short flagTH;

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_H_ */
