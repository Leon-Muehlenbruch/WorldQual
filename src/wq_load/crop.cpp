/*
 * crop.cpp
 *
 *  Created on: 18.04.2012
 *      Author: reder
 */

#include "crop.h"


extern mysqlpp::Connection con;
extern const int      NODATA;

/*
 * Constructor to fill application rate for N and P for each country
 */
CROP_CLASS::CROP_CLASS(const int parameter_id, COUNTRY_CLASS* countries, const int toCalc, const int year) {
	this->parameter_id=parameter_id;
	this->toCalc=toCalc;

	if(toCalc!=3) {  //  && toCalc!=4 für TP wird nicht mehr benutzt; stattdessen rate_p_cell
		numberCT = 0;
		YearFrom = 0;
		crop_type = NULL;
		glcc = NULL;
		rate_N = NULL;
		rate_P = NULL;
		cout << "CROP_CLASS: not needed.\n";
	} else {


		numberCT = getDataNumberCT();
		YearFrom = getYearFrom(year);

		if (numberCT<=0) {cerr << "ERROR: no data for crop types\n"; exit(1);}

		crop_type 	= new short[numberCT];
		glcc		= new short[numberCT];
		//crop_number = new int[numberCT];
		//ccn 		= new int[1];


		rate_N	= new double *[countries->number];
		rate_P	= new double *[countries->number];
		for(int i=0;i<countries->number;i++){
			rate_N[i]	= new double[numberCT];
			rate_P[i]	= new double[numberCT];
		}

		for(int j=0;j<numberCT;j++){
			crop_type[j] = -99;
			glcc[j]      = -99;
			for(int i=0;i<countries->number;i++){
				rate_N[i][j]=NODATA;
				rate_P[i][j]=NODATA;
			}
		}

		getGLCC();

		getData(countries);


//		for(int i=0;i<countries->number;i++){
//			for(int j=0;j<numberCT+1;j++){
//				cout <<i<<"	"<<j<<"	"<<rate_N[i][j]<<"	"<<rate_P[i][j]<<endl;
//
//			}
//		}
//
//		for(int i=0;i<21;i++){
//			cout<<"crop type[i] "<<crop_type[i]<<" i= "<<i<<endl;
//			cout<<"glcc[i] "<<glcc[i]<<" i= "<<i<<endl;
//		}

		cout << "CROP_CLASS: done.\n";
	}

}


/*
 * Method to get Number of crop types, the highest available number is returned.
 * This ensures the number of crop types, because the crop types ids are numbered serially
 */
int CROP_CLASS::getDataNumberCT() {

	mysqlpp::Query      query = con.query();
	query.reset();
	query << "SELECT count(distinct(i.`crop_type_id`)) as `numberCT` FROM "<<MyDatabase<<"wq_load.ind_fert_use_input i;";


	res = query.store();
	if (res.empty()) {cerr << "CROP_CLASS: error in table ind_fert_use_input.\n"; return 0;}

	int number = res.at(0)["numberCT"];

	return number;
} //getDataNumberCT()

int CROP_CLASS::getYearFrom(int year) {

	mysqlpp::Query      query = con.query();
	query.reset();
	query << "SELECT max(i.`YearFrom`) as `YearFrom` FROM "<<MyDatabase<<"wq_load.ind_fert_use_input i "
		<< "WHERE i.`YearFrom`<= " << year<<";";

	res = query.store();
	if (res.empty()) {cerr << "CROP_CLASS: error in table ind_fert_use_input.\n"; return 0;}

	int YearFrom = res.at(0)["YearFrom"];

	return YearFrom;
} //getDataNumberCT()

/*
 * Method which reads in the glcc number and crop type id used for WorldQual
 * both arrays have the same length and same crops on the same position
 * it is only the id that differs
 */
void CROP_CLASS::getGLCC() {

	mysqlpp::Query      query = con.query();
	query.reset();
	query << "SELECT distinct i.crop_type_id, IFNULL(c.GLCC,"<<NODATA<<") as `GLCC` FROM "
			<<MyDatabase<<"wq_load.ind_fert_use_input i, wq_general._crop_type c "
			<< "WHERE c.crop_type_id=i.crop_type_id;";

	res = query.store();
	if (res.empty()) {cerr << "CROP_CLASS: error with reading GLCC from _crop_type.\n"; }

	{
		int i=0;
		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){
			crop_type[i] 	= res[rowNum]["crop_type_id"];
			glcc[i]			= res[rowNum]["GLCC"];
			i++;
		}
	}

} //getGLCC()


/*
 * Method to fill *crop_numb with crop types if the number differs for every country
 * This is not the case anymore, therefore this method is not used anymore
 */
bool CROP_CLASS::getDataNumberActCrops(int countryIsoNr, int *crop_numb, int *ccn) {

	mysqlpp::Query      query = con.query();
	query.reset();
	query << "SELECT i.`country_id`, i.`crop_type_id` as `crop_type` FROM "<<MyDatabase<<"wq_load.ind_fert_use_input i WHERE i.`country_id`="<<countryIsoNr<<";";


	res = query.store();
	if (res.empty()) {cerr << "CROP_CLASS: country: "<<countryIsoNr<<" not in table ind_fert_use_input for act crops.;\n"; return false;}


	{
			int i=0;
			for (unsigned int rowNum=0; rowNum<res.size(); rowNum++){

				//cout << "i="<<i<<" "<<res[rowNum]["crop_type"]<<endl;

				crop_numb[i] 	= res[rowNum]["crop_type"];
				i++;
			}

	ccn[0]=i;

	}

	return true;
} //getDataNumberActCrops()


/*
 * Method to load data into rate_N and rate_P.
 */
void CROP_CLASS::getData(COUNTRY_CLASS* countries) {
	int i=0;

	mysqlpp::Query      query = con.query();

	for(int co_num=0; co_num<countries->number; co_num++){
		for(int crop_num=0; crop_num<numberCT; crop_num++){

			//cout << "CROP_CLASS: getData for entered. co_num = "<<co_num<<" ,crop_num = "<<crop_num<<" ,crop_number[co_num]: "<<crop_number[crop_num]<<endl;

			query.reset();
			query << "SELECT i.`crop_type_id`, IFNULL(i.`rate_N`,"<<NODATA<<") as `rate_N`"
				<<", IFNULL(i.`rate_P`,"<<NODATA<<") as `rate_P`"
				<<" FROM "<<MyDatabase<<"wq_load.ind_fert_use_input i WHERE i.`country_id`="<<countries->get_ISONUM(co_num)
				<<" AND i.`crop_type_id`="<<crop_type[crop_num]
				<<" AND i.`YearFrom`=" << YearFrom << ";";

			//cout << "Query: " << query << endl << endl; //cin >> j;

			try{
				res = query.store();

				//cout << "*********************** res.size()="<<res.size()<<endl;

				if (res.size()>0) // Falls in der Tabelle keine Werte gibt, bleiben NODATA-Werte aus der Initialisierung
				{
					int rowNum = 0; // Zeile 0 nehmen, weil Select mit Primary Key in Where, gibt nur eine Zeile

					//cout << "i="<<i<<" "<<res[rowNum]["crop_type_id"]<<" "<<res[rowNum]["rate_N"]
					//     <<" "<<res[rowNum]["rate_P"]<< res[rowNum]["YearFrom"]<<endl;

					rate_N[co_num][crop_num] = res[rowNum]["rate_N"];
					rate_P[co_num][crop_num] = res[rowNum]["rate_P"];
					i++;
				}

			}catch(BadQuery& er){
		        cerr << "Query error: " << er.what() << "." << endl;
			}
		} //for (crop_num)
		//}//if
	}//for (co_num)
} // getData()


/*
 * Method to get application rate of N or P for a certain country and crop type
 */
double CROP_CLASS::getCrop_Appl_Rate(int country, int crop, int cell) {
	double ind_fert_use = NODATA;

	if(toCalc!=3 && toCalc!=4) return ind_fert_use; // only for TN or TP

	int ct_pos=-1;
	for (int i=0; i<numberCT+1; i++) {
		if (glcc[i]==crop) {
			ct_pos=i; break;
		}
	}

	if (ct_pos<0) {
		ind_fert_use=NODATA;

		/*cout << " crop type: " << crop<<" not found, this means GLCC value refers to other land uses such as forest etc. \n"; */
	}
	else{

		if(toCalc==3){
			ind_fert_use  = rate_N[country][ct_pos];
		}
		else if(toCalc==4)
			ind_fert_use  = rate_P[country][ct_pos];

	}
	if(cell==144899)
		cout<<"empty: "<<ind_fert_use<<endl;
	return ind_fert_use;
}
