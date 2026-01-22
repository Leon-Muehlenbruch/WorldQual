/*
 * GB_CLASS.cpp
 *
 *  Created on: 03.07.2012
 *      Author: reder
 */

#include "gb.h"


extern mysqlpp::Connection con;
//extern const int      NODATA;

/*
 * Constructor to get geogenic background rates (e.g. deposition) for each land-use type
 */
GB_CLASS::GB_CLASS(const int parameter_id, const int toCalc) {

	this->parameter_id=parameter_id;

	nrLanduse=getNrLanduse(); // number of land use ID and fill array lu[]
	//cout<<"nrLanduse: "<<nrLanduse<<endl;

	geo_back = new double[nrLanduse];

	for(int i=0;i<nrLanduse;i++){
		geo_back[i]= -9999.99;
	}

	if(toCalc==0 ||toCalc==3 || toCalc==4)
		getData();


	//for(int i=0;i<nrLanduse;i++){
	//	cout<<"geo_back[]: "<<geo_back[i]<<" i: "<<i<<endl;
	//}

	cout << "GB_CLASS done."<<endl;

}


int GB_CLASS::getNrLanduse(){
	mysqlpp::Query      query = con.query();
	query.reset();
	query << "SELECT `LU` FROM wq_general._landuse l ORDER BY l.`LU`;";
	res = query.store();
	if (res.empty()) {cerr << "GB_CLASS: error in table _landuse.\n";  return 0;}

	int number = res.size();
	lu = new int[number];

	for (unsigned int rowNum=0; rowNum<res.size(); rowNum++) {
		lu[rowNum] = res[rowNum]["LU"];
	}

	return number;
}


void GB_CLASS::getData(){
	int lu_dummy;

	mysqlpp::Query      query = con.query();
	query.reset();
	query << "SELECT g.`geo_back`, g.`LU` FROM "<<MyDatabase<<"wq_load.geogenic_background_input g WHERE g.`parameter_id`="<<parameter_id<<" ORDER BY g.`LU`;";

	res = query.store();
	if (res.empty()) {cerr << "GB_CLASS: error in table geogenic_background_input.\n"; }

	{
		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){

			//cout << res[rowNum]["LU"]<<" "<<res[rowNum]["geo_back"]<<" "<<endl;

			lu_dummy   = res[rowNum]["LU"];
			int lu_pos = getLUpos(lu_dummy);

			if (lu_pos>=0) geo_back[lu_pos]= res[rowNum]["geo_back"];
		}
	}
}

bool GB_CLASS::getGeoBackData(short LU_cell, double &geoback){
	int lu_pos = getLUpos(LU_cell);

	if(lu_pos < 0 || geo_back[lu_pos]==-9999.99) {
		cerr<<"Error in GB: geogenic background rate for land use: "<<LU_cell<<" not found!"<<endl;
		return 1;
	} else
		geoback = geo_back[lu_pos];

	return 0;
}

int GB_CLASS::getLUpos(short LU_cell) {

	for (int i=0; i<nrLanduse; i++) {
		if (lu[i]==LU_cell) return i;
	}
	return -1; // not found
}
