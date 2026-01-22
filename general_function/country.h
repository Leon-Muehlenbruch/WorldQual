//
// country class
//
//
// Author: Ellen Teichert
//
#ifndef COUNTRY_H
#define COUNTRY_H

#include <stdio.h>
#include <mysql++.h>
#include "../general_function/general_functions.h"

using namespace std;
using namespace mysqlpp;

extern char  MyDatabase[maxcharlength];
extern char continent_abb[10];			// Abbreviation of the continent name; used for detection of corresponding database


class COUNTRY_CLASS{
public:
	int number;   // Anzahl Laender

	COUNTRY_CLASS(const int IDScen, const int year);
	int get_countryPos(int country);
	int get_ISONUM(int country_pos) {if (country_pos<0 || country_pos>=number) return 0; else return Country[country_pos];}
	

private:
	//mysqlpp::Connection con;
	int IDScen;
	int year;
	
	//mysqlpp::Query      query;
	mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
	mysqlpp::Row        row;
	
	int *Country;	

	int getCouintryNumber();
	int getCouintries();
	
};

#endif
