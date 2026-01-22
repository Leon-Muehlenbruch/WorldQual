/*
 * options.cpp
 *
 *  Created on: 11.05.2015
 *      Author: kynast
 *      based on optionClass from WaterGAP hydro model
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>      // std::setw

//#include "../general_function/general_functions.h"
#include "options.h"
using namespace std;

optionsClass::optionsClass(int optionc, char* option_v[], const char* CL) {
	IDVersion = 3;
	IDReg = 1;
	InputType = 0;
	maxcharlength          = 2550;
	MyHost = new char[maxcharlength];
	MyUser = new char[maxcharlength];
	MyPassword   = new char[maxcharlength];
	input_dir    = new char[maxcharlength];
	filename_opt = new char[maxcharlength];

	programme = new char[maxcharlength];
	commandLine = new char[maxcharlength];

	strcpy(programme, option_v[0]);
	strcpy(commandLine, CL);
}

optionsClass::~optionsClass() {
	delete[] MyHost;
	delete[] MyUser;
	delete[] MyPassword;
	delete[] input_dir;
	delete[] filename_opt;

	delete[] programme;
	delete[] commandLine;
}

// separate option and option value
void optionsClass::separateCommandLineOption(char* CLoption, char option[5], char* optionValue) {
	strncpy(option, CLoption, 4);
	option[2]='\0';                                // cat string at 3rd position
	strncpy(optionValue, CLoption+2, maxcharlength);
	cout << "separate: " << option << " : " << optionValue << endl;
}

short optionsClass::init(int option_c,  char* option_v[])
{
	// default values
	strcpy(filename_opt,"OPTIONS.DAT");

	char option[5];      // to process options, e.g. "-d", "-t", "-o", "-r", "-s"
	char optionValue[maxcharlength]; // option value (filenames)

	if (option_c) {
		for (int i=0; i<option_c; i++) {
			separateCommandLineOption(option_v[i], option, optionValue);

 		  	// check OPTIONS.DAT file
 			if (!strcmp(option, "-h")){
				Help();
				exit(0);
 			}

 		  	// check OPTIONS.DAT file
 			if (!strcmp(option, "-o")){
				strncpy(filename_opt, optionValue, maxcharlength);
 			}
		} // for(i)
	}

	//#############
	// set default options
	// which will be used, when OPTIONS.DAT is not found
	IDVersion = 3;
	IDReg = 1;
	sprintf(continent_abb, "eu");
	sprintf(MyHost,"%s","MySQL");
	sprintf(MyUser,"%s","worldqual");
	sprintf(MyPassword,"%s","1qay");
	InputType = 0;
	strcpy(input_dir,".");

	return 0;
 } // init()

void optionsClass::printOptions() {
	switch (IDVersion) {
		case 2: {
			switch (IDReg) {
			case 1: sprintf(continent_abb, "wg2"); cout<<"continent: WaterGAP2  World"<<endl; break;
			case 2: sprintf(continent_abb, "wg2"); cout<<"continent: WaterGAP2  World Watch"<<endl; break;
			case 3: sprintf(continent_abb, "wg2"); cout<<"continent: WaterGAP2 World with 5min"<<endl; break;
			}
		}
		break;
		case 3: {
			switch (IDReg) {
			case 1: sprintf(continent_abb, "eu"); cout<<"continent: WaterGAP3 Europe"<<endl; break;
			case 2: sprintf(continent_abb, "af"); cout<<"continent: WaterGAP3 Africa"<<endl; break;
			case 3: sprintf(continent_abb, "as"); cout<<"continent: WaterGAP3 Asia"<<endl; break;
			case 4: sprintf(continent_abb, "au"); cout<<"continent: WaterGAP3 Australia"<<endl; break;
			case 5: sprintf(continent_abb, "na"); cout<<"continent: WaterGAP3 North America"<<endl; break;
			case 6: sprintf(continent_abb, "sa"); cout<<"continent: WaterGAP3 South America"<<endl; break;
			}
		}
		break;
	}

	cout << "continent_abb: ." << continent_abb <<"."<< endl<<endl;
	cout << "Host:          ." << MyHost <<"."<< endl;
	cout << "User:          ." << MyUser <<"."<< endl;
	cout << "Password:      . *****" << endl<<endl; //MyPassword <<"."<< endl;
	if (!InputType) cout << "Daten aus der Datenbank nehmen."<<endl;
	else cout << "Daten aus UNF-Dateien nehmen"<<endl;
	cout <<"input_dir: "<< input_dir<<endl<<endl;

} // printOptions()

void optionsClass::StringCheck(char* str) {
	// Leerzeichen, tab und Zeilenende finden
	char keys[] = " \t\n";
	unsigned int i = strcspn(str, keys);

	if (i<strlen(str)) str[i]='\0'; // alles was ab gefundenem Zeichen gab l�schen mit diesem Zeichen

} // StringCheck()


short optionsClass::read_parentOptions(int parent_number, FILE *file_ptr) { // lesen aus OPTIONS.DAT letzte Teil mit Optionen aus optionsClass
	char string[maxcharlength];
	int i = 0, n;

	while (fgets(string, maxcharlength, file_ptr)) {
				if (0 == strncmp(string, "Value:", 6)) {
					i++;
					n = atoi(&string[7]);
					if (1 == i) {
						if (n == 2 || n == 3)
							IDVersion = n;
						else {
							cerr << "OPTIONS ERROR: 3. Option 'IDVersion' darf 2 (WaterGAP2) oder 3 (WaterGAP3) sein!\n";
							return 1;
						}
						}
					if (2 == i) {
						if ( (IDVersion == 3 && n>=1 && n<=6) || (IDVersion == 2 && n>=1 && n<=3) )
							IDReg = n;
						else {
							cerr << "OPTIONS ERROR: IDReg darf zwischen 1 und 6 f�r WaterGAP3 und 1 und 3 f�r WaterGAP2 sein.\n";
							cerr << " 1 WaterGAP3 eu" << endl;
							cerr << " 2 WaterGAP3 af" << endl;
							cerr << " 3 WaterGAP3 as" << endl;
							cerr << " 4 WaterGAP3 au" << endl;
							cerr << " 5 WaterGAP3 na" << endl;
							cerr << " 6 WaterGAP3 sa" << endl;
							cerr << " 1 WaterGAP2 world standard" << endl;
							cerr << " 2 WaterGAP2 world watch" << endl;
							cerr << " 3 WaterGAP2 world with 5min" << endl;
							return 1;
						}
					}
					if (3 == i) {
						StringCheck(&string[7]);
						strcpy(MyHost, &string[7]);
					}
					if (4 == i) {
						StringCheck(&string[7]);
						strcpy(MyUser, &string[7]);
					}
					if (5 == i) {
						StringCheck(&string[7]);
						strcpy(MyPassword, &string[7]);
					}
					if (6 == i) {
						if (n == 0 || n == 1)
							InputType = n;
						else {
							cerr << "OPTIONS ERROR: 11. Option 'InputType' darf 0 oder 1 sein!\n";
							cerr << "  0: Daten aus der Datenbanlk einlesen\n";
							cerr << "  1: Daten aus UNF-Dateien. Es wird Option 12: input_dir gebraucht.\n";
							return 1;
						}
					}
					if (7 == i) {
						StringCheck(&string[7]);
						strcpy(input_dir, &string[7]);
					}
				} // if(Value)
			}
	return 0;
} // read_parentOptions()

void optionsClass::Help(int num) {
	cout << std::setw(3) << num++ << ". IDVersion\n";
	cout << std::setw(3) << num++ << ". IDReg\n";
	cout << std::setw(3) << num++ << ". MyHost\n";
	cout << std::setw(3) << num++ << ". MyUser\n";
	cout << std::setw(3) << num++ << ". MyPassword\n";
	cout << std::setw(3) << num++ << ". InputType\n";
	cout << std::setw(3) << num++ << ". input_dir\n";
	cout << endl;
}
