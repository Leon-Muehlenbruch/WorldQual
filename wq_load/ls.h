/*
 * ls.h
 *
 *  Created on: 18.04.2012
 *      Author: reder
 */
#ifndef LS_H_
#define LS_H_

#include <stdio.h>
#include <mysql++.h>
#include "../general_function/country.h"
#include "../general_function/general_functions.h"

using namespace std;
using namespace mysqlpp;

extern char  MyDatabase[maxcharlength];

class LS_CLASS{
public:
	int number;

	LS_CLASS(const int parameter_id, const int IDScen, COUNTRY_CLASS* countries, const int toCalc);

	int getLS_ex_rate(int country, int animal, int toCalc,  double &ex_ls, COUNTRY_CLASS* countries);

private:

	int numberFAOreg;
	int parameter_id;
	int IDScen;

	//mysqlpp::Query      query;
	mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
	//mysqlpp::Row        row;

	short *ls;					//different livestock types
	//short *FAOReg;				//FAO Regions
	double **ex_ls; 			//excreation rate of livestock type  [t/a*head], FC: [cfu/head*a]
	int *FAORegCoun;			//holds FAO Region for every country

	int getDataNumber();			//getter method to get number of livestock
	int getDataNumberFAOreg();		//getter method to get number of FAORegions
	void getData(bool isSenAnlys);					//getter method to fill variables
	//void getDataSenAnlys();//getter method to fill variables, same as getData() but with a scenario dependency
	int getFAOREgion(COUNTRY_CLASS* countries);	//getter method to get FAORegion of one country
	int getAnimalPos(int animal);

};

#endif /* LS_H_ */

