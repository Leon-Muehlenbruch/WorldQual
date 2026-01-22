/*
 * GB_CLASS.h
 *
 *  Created on: 03.07.2012
 *      Author: reder
 */

#ifndef GB_H_
#define GB_H_

#include <stdio.h>
#include <mysql++.h>
#include "../general_function/general_functions.h"

using namespace std;
using namespace mysqlpp;

extern char  MyDatabase[maxcharlength];


class GB_CLASS {
public:

	GB_CLASS(const int parameter_id, const int toCalc);


	bool getGeoBackData(short LU_cell, double &geoback);

private:
	int parameter_id;
	int* lu;
	double *geo_back;
	int nrLanduse;

	mysqlpp::StoreQueryResult res; //mysqlpp::Result     res;
	void getData();
	int getNrLanduse();
	int getLUpos(short LU_cell);

};

#endif /* GB_CLASS_H_ */
