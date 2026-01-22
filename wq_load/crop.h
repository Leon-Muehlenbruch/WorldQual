/*
 * crop.h
 *
 *  Created on: 18.04.2012
 *      Author: reder
 */
#ifndef CROP_H_
#define CROP_H_

#include <stdio.h>
#include <mysql++.h>
#include "../general_function/country.h"
#include "../general_function/general_functions.h"

using namespace std;
using namespace mysqlpp;

extern char  MyDatabase[maxcharlength];
extern char continent_abb[10];			// Abbreviation of the continent name; used for detection of corresponding database


class CROP_CLASS{
public:
	CROP_CLASS(const int parameter_id, COUNTRY_CLASS* countries, const int toCalc, const int year);

	double getCrop_Appl_Rate(int country, int crop, int cell);

private:
	int numberCT;
	int parameter_id;
	int toCalc;
	int YearFrom; // Werte aus diesem Jahr nehmen, Falls keine gibt, nicht in der Zeit zurueck gehen. Wert gilt global fuer alle Laender und Croptypen

	//mysqlpp::Query      query;
	mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
	//mysqlpp::Row        row;

	short *crop_type;              // crop types
	short *glcc;				  //glcc equivalent to crop types
	//int *crop_number;		   // array with crops for one country
	//int ccn;					   // number of crop types in one selected country

	double **rate_N;		  // industrial fertilizer application rate TN in t/(km^2*a)
	double **rate_P;		  // industrial fertilizer application rate TP in t/(km^2*a)



	int getDataNumberCT();				//getter method to reach number of crop types
	void getGLCC();						//gets crop type number of GLCC
	bool getDataNumberActCrops(int countryIsoNr, int *crop_numb, int *ccn);		//getter method to get crop numbers for one country
	void getData(COUNTRY_CLASS* countries);		//getter method to fill variables
	int getYearFrom(int year);   // gibt maximalen Jahr fuer den Werte fuer rate_N und rate_P, was kleiner oder gleich als aktuelle Jahr ist

};

#endif /* CROP_H_ */

