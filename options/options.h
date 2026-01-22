/*
 * options.h
 *
 *  Created on: 11.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

class optionsClass {
public:
	optionsClass(int optionc, char* optionv[], const char* CL);
	virtual ~optionsClass();

	int IDVersion;
	int IDReg;
	char continent_abb[10];			// Abbreviation of the continent name; used for detection of corresponding database

	char*  MyHost;
	char*  MyUser;
	char*  MyPassword;

	short InputType;                // 0 - q, runoff_total aus Datenbank einlesen (auch Fliessgeschwindigkeit, wenn flowVelocityVariant==0)
	                                // 1 - Daten aus UNF-Dateien einlesen
	char*  input_dir;               // input directory for data

	short maxcharlength;

protected:
	char *filename_opt;
	void StringCheck(char* str); // bei den nicht-digitalen Optionen Zeilenende und evtl. Leerzeichen oder Tabulator am Ende l�schen
	void printOptions();
	short init(int optionc, char* optionv[]);
	short read_parentOptions(int num, FILE *file_ptr); // lesen aus OPTIONS.DAT letzte Teil mit Optionen aus optionsClass

	// separate option and option value
	void separateCommandLineOption(char* CLoption, char option[5], char* optionValue);

	virtual void Help(int num =1)=0;

	char* programme;  // ProgrammName z.B. "rtf_input"
	char* commandLine; // z.B.  "./rtf_input startYear endYear"

private:

};

#endif /* OPTIONS_H_ */
