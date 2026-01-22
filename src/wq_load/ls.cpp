/*
 * ls.cpp
 *
 *  Created on: 18.04.2012
 *      Author: reder
 */

#include "ls.h"


extern mysqlpp::Connection con;
extern const int      NODATA;

/*
 * Constructor to get excretion rates for each animal and FAO Region
 */
LS_CLASS::LS_CLASS(const int parameter_id, const int IDScen, COUNTRY_CLASS* countries, const int toCalc) {
	this->parameter_id=parameter_id;
	this->IDScen=IDScen;


	if(toCalc!=5) {  // pesticide brauchen es nicht

		number		 = getDataNumber(); // number of livestock ID and fill array ls[]
		numberFAOreg = getDataNumberFAOreg();

		if (number<=0) {cerr << "ERROR: no data for livestock\n"; exit(1);}

		//FAOReg    	= new short[numberFAOreg];
		FAORegCoun	= new int[countries->number];
		ex_ls		= new double*[numberFAOreg];

		for(int i=0;i<numberFAOreg;i++){
			ex_ls[i]	= new double[number];
			for (int ls_idx=0; ls_idx<number; ls_idx++)
				ex_ls[i][ls_idx] = NODATA;
		}


		getFAOREgion(countries);

		//if the run is made for the sensitivity analysis
		if(strcmp(MyDatabase,"sen_anlys_")==0)
			getData(1);
		else
			getData(0);


//		for(int i=0;i<numberFAOreg;i++){
//				for(int j=0;j<number;j++){
//					cout <<i<<"	"<<j<<"	"<<ex_ls[i][j]<<endl;
//
//			}
//		}
//
//		for(int i=0;i<countries.number;i++){
//					cout<<"FAORegCoun[i]: " <<countries.get_ISONUM(i)<<"	"<<FAORegCoun[i]<<endl;
//		}

	}
	cout << "LS_CLASS: done"<<endl;
}


/*
 * Method to get number of livestock
 */
int LS_CLASS::getDataNumber() {
	mysqlpp::Query      query = con.query();
	query.reset();
	query << "SELECT distinct(`LS`) as `LS` FROM "<<MyDatabase<<"wq_load.ls_exr_input l;";

	res = query.store();
	if (res.empty()) {cerr << "LS_CLASS: error in table ls_exr_input.\n"; return 0;}

	int number = res.size();
	ls 			= new short[number];

	for (unsigned int rowNum=0; rowNum<res.size(); rowNum++) {
		ls[rowNum] = res[rowNum]["LS"];
	}

	return number;
} //getDataNumber()



/*
 * Method to get number of FAO Regions
 */
int LS_CLASS::getDataNumberFAOreg() {
	mysqlpp::Query      query = con.query();
	query.reset();
	query << "SELECT count(distinct(`IDFAOReg`)) as `number` FROM "<<MyDatabase<<"wq_load.ls_exr_input l;";

	res = query.store();
	if (res.empty()) {cerr << "LS_CLASS: error in table ls_exr_input for FAOreg.\n"; return 0;}

	int number = res.at(0)["number"];

	return number;
} //getDataNumberFAOreg()



/*
 * Method to get a corresponding FAO Region for one country
 */
int LS_CLASS::getFAOREgion(COUNTRY_CLASS* countries){

	mysqlpp::Query      query = con.query();

	for(int countries_num=0; countries_num<countries->number; countries_num++){
		query.reset();
		query << "SELECT c.`IDFAOReg` FROM wq_general._country c WHERE c.`country_id`="<<countries->get_ISONUM(countries_num)<<";";

		res = query.store();
		if (res.empty()) {cout << "LS_CLASS: county in table _country.\n"; return 1; }
		else
			FAORegCoun[countries_num] = res.at(0)["IDFAOReg"];
	}
	return 0;

}//end getFAOREgion()


/*
 * Method to fill ex_ls with data
 * if isSenAnlys == TRUE Method to fill ex_ls with data witch is scenario dependent -> used for sensitivity analysis
 */
void LS_CLASS::getData(bool isSenAnlys) {

	mysqlpp::Query      query = con.query();

	for(int FAO_num=0; FAO_num<numberFAOreg; FAO_num++){

			query.reset();
			query << "SELECT l.`LS`, l.`IDFAOReg` as `FAOReg`, IFNULL(l.`ls_exr_rate`,"<<NODATA<<") as `ls_ex`"
				<<" FROM "<<MyDatabase<<"wq_load.ls_exr_input l WHERE l.`parameter_id`= "<<parameter_id
				<<" And l.`IDFAOReg`= "<<FAO_num+1;
			if (isSenAnlys) query <<" AND l.`IDScen`="<<IDScen;
			query <<" ;";

			//cout << "Query: " << query << endl << endl; //cin >> j;

			res = query.store();
			//cout << "*********************** res.size()="<<res.size()<<endl;
			if (res.empty()) {cerr << "LS_CLASS: error in table ls_exr_input.\n";}


			for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
			{
				//cout << "i="<<i<<" "<<res[rowNum]["LS"]<<" "<<res[rowNum]["FAOReg"]<<" "
					//	<<res[rowNum]["ls_ex"]<<endl;

				short ls_value          = res[rowNum]["LS"];
				int ls_idx=getAnimalPos(ls_value);
				if (ls_idx>=0)  {// ls found
					//FAOReg[FAO_num]  		= res[rowNum]["FAOReg"];
					ex_ls[FAO_num][ls_idx]		= res[rowNum]["ls_ex"];
				}
			}
	}
} // getData()

/*
 * Method to get excretion rate for a certain animal in a certain country (e.g. FAO Region)
 */
int LS_CLASS::getLS_ex_rate(int country, int animal, int toCalc,  double &exr_ls, COUNTRY_CLASS* countries) {

	int animal_pos=getAnimalPos(animal);

	if (animal_pos<0) {cerr << "no animal types found\t" << animal << endl; return 1; } // animal type not found

	exr_ls  = ex_ls[FAORegCoun[country]-1][animal_pos];

	return 0;
}

int LS_CLASS::getAnimalPos(int animal) {

	for (int i=0; i<number; i++) {
		if (ls[i]==animal) return i;
	}
	return -1;
}

