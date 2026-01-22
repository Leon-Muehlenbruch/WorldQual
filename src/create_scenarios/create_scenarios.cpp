/*
 * create_scenarios.cpp
 *
 *  Created on: 27.02.2014
 *      Author: reder
 */


#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h>  //<custom.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <math.h>
#include <math.h>
#include <list>
#include <iostream>
#include "../general_function/country.h"
#include "../general_function/timestring.h"
#include "../general_function/general_functions.h"
//#include "def.h"

#define MY_DATABASE ""
#define MY_HOST "MySQL"
#define MY_USER "worldqual"
#define MY_PASSWORD "1qay"
#define BYTESWAP_NECESSARY
//ACHTUNG: Hier muss die richtige Anzahl an Werten in der Tabelle hin(Zeilen und Spalten)!
#define maxrow 510
#define maxcol 42



#define DEBUG
//#define DEBUG_queries
//#undef DEBUG_queries


int continent[5];  // 0 = ng (number of cells); 1 - ng_land (number of land cells); 2 - nrows (number of rows); 3 - ncols (number of columns); 4 - cellFraction

char  hydro_input[maxcharlength];              // in watergap_unf.watergap_region(hydro_input) path to waterGAP-Inputpath
char  MyDatabase[maxcharlength];
char continent_abb[10];			// Abbreviation of the continent name; used for detection of corresponding database


mysqlpp::Connection con(use_exceptions);
mysqlpp::Query      query = con.query();
mysqlpp::StoreQueryResult     res;
mysqlpp::UseQueryResult     Use_res;
mysqlpp::Row        row;


sql_create_1(Liste_X, 1, 0,
			 sql_int_unsigned, cell);
sql_create_2(Routing_upd, 1, 0,
			 sql_int_unsigned, cell,
			 sql_int_unsigned, counter);




//state all methods available
void readLHS(double (*LHS)[maxcol]);

void readScenario(std::string ScenName,std::string ScenDescr, int IDScen, int project_id);
int readCellInput(int IDScen, int year, int *country_cell);
void readCountryInput(int IDScen, int year, COUNTRY_CLASS countries,
		double (*connectivity)[4], double *stp_failure,
		double *connec_treat_unknown, double *hanging_lat, double *open_defec);
void readCountryParameterInput(int IDScen, int year, int parameter_id,
	COUNTRY_CLASS countries, double *ef, double (*conc_man)[7], double *conc_urb);
void readLsExrRate(double (*ex_ls)[12], int parameter_id, int refScen);
void readParameterInput(int IDScen, int year, int parameter_id, double rem[9],
	double treat_failure[1], double sed_veloc[1], double *rad_data);
void readRunlist(int IDrun, string *Name, int parameter_id_input[1], int parameter_id,
		int IDScen, int IDScen_wq_load, int IDTemp[1], double TETA[1], int svn_version_id[1], string *comment);
void readFactors(int IDrun, double *factor);

int main(int argc, char *argv[])
{
	// print current date
	cout <<"\nProgram create scenarios started at: " << getTimeString() << endl << endl;

	if (argc<6) {
    cout << "\n./create_scenarios startScenario endScenario year parameterID referenceScenario [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region]"
         << "\n\nparameters apart from startScenario, endScenario, year, parameterID, referenceScenario must not be in order. "
         << "\nif one (or more) parameters is missing the following default values are used:  "
         << "\n\nDATABASE      \"empty string\""
         << "\nHOST          \"MySQL\"    "
         << "\nUSER          \"worldqual\""
         << "\nPASSWORD      \"*******\"     "
         << "\nIDVersion     3 (WaterGAP3)"
         << "\nIDReg         1 (Europa) \n\n";
         return 0;
	}


	int IDVersion = IDVersion_const;
	int IDReg     = IDReg_const;
									 sprintf(MyDatabase,"%s",MY_DATABASE);
	char  MyHost[maxcharlength];     sprintf(MyHost,"%s",MY_HOST);
	char  MyUser[maxcharlength];     sprintf(MyUser,"%s",MY_USER);
	char  MyPassword[maxcharlength]; sprintf(MyPassword,"%s",MY_PASSWORD);

	if(!IsNumber(argv[1]) || !IsNumber(argv[2]) || !IsNumber(argv[3])|| !IsNumber(argv[4])|| !IsNumber(argv[5])) {
		cerr << "ERROR: Wrong parameter" << endl;
		cerr << "usage: ./create_scenarios startScenario endScenario year parameterID referenceScenario [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region]"<<endl;
		exit(1);
	}



	int startScen     = atoi(argv[1]);
	int endScen       = atoi(argv[2]);
	int year          = atoi(argv[3]);
	int parameter_id  = atoi(argv[4]);
	int refScen   	  = atoi(argv[5]);


	for (int num=6; num<argc; num++) {
		if (!strcmp(argv[num],"-h"))       sprintf(MyHost,     "%s", argv[++num]); // hostname
		else if (!strcmp(argv[num],"-d"))  sprintf(MyDatabase, "%s", argv[++num]); // database
		else if (!strcmp(argv[num],"-u"))  sprintf(MyUser,     "%s", argv[++num]); // user
		else if (!strcmp(argv[num],"-p"))  sprintf(MyPassword, "%s", argv[++num]); // password
		else if (!strcmp(argv[num],"-v"))  IDVersion = atoi(argv[++num]);          // in watergap_unf.watergap_region(IDVersion, IDRegion)
		else if (!strcmp(argv[num],"-r"))  IDReg     = atoi(argv[++num]);
	}


	if(strcmp(MyDatabase,"")==0){

		cout << "Database: The CESR version of the database is used. Not a project dependent database" << endl;
	}else{
		sprintf(MyDatabase,"%s_", MyDatabase);
		cout << "Database: The database of the project: " << MyDatabase <<" is used."<< endl;
	}
	cout << "MyHost:     " << MyHost     << endl;
	cout << "MyUser:     " << MyUser     << endl;
	cout << "MyPassword: " << endl; //<< MyPassword << endl;
	cout << "IDVersion:  " << IDVersion  << endl;
	cout << "IDReg:      " << IDReg      << endl;
	cout << "Scenarios start to end:      " << startScen << " - " << endScen << endl;


	if (startScen>endScen) {
		cerr << "ERROR: Wrong parameter. startScen> endScen!!!" << endl;
		cerr << "usage: ./create_scenarios startScenario endScenario year parameterID referenceScenario [-h hostname] [-d database] [-u user] [-p password] [-v version] [-r region]"<<endl;
		exit(1);
	}

	switch(IDReg){
		case 1: sprintf(continent_abb, "eu"); cout<<"continent: Europe"<<endl; break;
		case 2: sprintf(continent_abb, "af"); cout<<"continent: Africa"<<endl; break;
		case 3: sprintf(continent_abb, "as"); cout<<"continent: Asia"<<endl; break;
		case 4: sprintf(continent_abb, "au"); cout<<"continent: Australia"<<endl; break;
		case 5: sprintf(continent_abb, "na"); cout<<"continent: North America"<<endl; break;
		case 6: sprintf(continent_abb, "sa"); cout<<"continent: South America"<<endl; break;
		default: cerr<<"Wrong use of IDReg integer of interval [1,6]"<<endl; break;
	}


	try {
		cout << "start:" << endl;
        char start_db[maxcharlength]; sprintf(start_db, "%sworldqual_%s", MyDatabase, continent_abb);

		if (!con.connect(&start_db[0], &MyHost[0], &MyUser[0], &MyPassword[0],3306)) {
			cout << "database " << start_db << " connection failed..." << endl;
			return 1;
		}
		////////////////////////////////
		cout << "database " << start_db << " connection ok..." << endl; //return 0;
		////////////////////////////////

		// class containing information about the countries
		COUNTRY_CLASS countries(refScen, year);

		// Parameter Landmaske einlesen (set values of continent)
		if(getCELLS(IDVersion, IDReg)) exit(1); cout << "Parameter Landmaske wurden eingelesen\n"<<endl;


		/********************parameter initinalization***************/

		double (*LHS)[maxcol]=new double[maxrow][maxcol];	//matrix which contains all values from the Latin Hypercube Sampling (LHS)
		std::string ScenName;								//Scenario name
		std::string ScenDescr;								//Scenario description


		int project_id=3;									//ProjectID is the CESR Database =1; WWQA=2; sensitvity analysis =3

		int *country_cell      = new int[continent[0]];        // isonum - country for cell

		double (*connectivity)[4] = new double[countries.number][4];  // primary, secondary, tertiary  connectivity, no treatment [%]
		double (*con_help)[4]     = new double[countries.number][4];  // help variable primary, secondary, tertiary  connectivity, no treatment [%]



		double *stp_failure   = new double[countries.number];     			// percentage of sewage treatment plants (STP) not working or in a bad condition in a country
		double *connec_treat_unknown    = new double[countries.number];     // percent of total pop connected to septic tanks, pit latrines, unknown place, bucket latrine and composting toilet -> some treatment but much is unknown
		double *hanging_lat   = new double[countries.number];     			// percent of total population connected to hanging latrines
		double *open_defec    = new double[countries.number];     			// percent of total population practicing open defecation

		double *connec_treat_unknown_help    = new double[countries.number];     // help variable percent of total pop connected to septic tanks, pit latrines, unknown place, bucket latrine and composting toilet -> some treatment but much is unknown
		double *hanging_lat_help   = new double[countries.number];     			// help variable percent of total population connected to hanging latrines
		double *open_defec_help    = new double[countries.number];     			// help variable percent of total population practicing open defecation



		double *ef            = new double[countries.number];     // emission factor from population [kg/cap/year]; FC: 10^10 cfu/(a*head)

		//ACHTUNG: number of FAORegions und LS hier hart codiert, da ich es nur einmal zu nutzen gedenke!
		double (*ex_ls)[12]		= new double[11][12];	//excretion rate FC: [10^10 cfu/(a*head)]


		double (*conc_man)[7] = new double[countries.number][7];  // manufacturing concentration of determinant in return flow [mg/l]; FC: 10^10 cfu/L
			                                                       // 0 - food, drink, tobacco
			                                                       // 1 - textiles
			                                                       // 2 - paper, palp, printing
			                                                       // 3 - chemical industry
			                                                       // 4 - glas, ceramics, cement
			                                                       // 5 - metals
			                                                       // 6 - other
		double *conc_urb       = new double[countries.number];    // concentration of urban runoff [mg/l] FC: cfu/100ml

		double treat_failure[1];			     								// percentage of failure of septic tanks, pit latrines, composting toilets and to take the unknown of bucket and unknown place into account
		double rem[9]; 											// 0 - primary
																		// 1 - secondary
																		// 2 - tertiary
																		// 3 - no treatment removal rate
																		// 4 - removal rate in soil -> here not used!
																		// 5 - reduction factor organic from manure
																		// 6 - reduction factor inorganic -> here not used!
																		// 7 - removal rate of manure in storage
																		// 8 - removal rate of manure on soil


		double sed_veloc[1];									//sedimentation velocity [m/d]

		double *rad_data = new double[3];							//0 - ks sunlight inactivation [cm²/cal]; only used for FC
																	//1 - alpha from linear regression of ke and TSS
																	//2 - beta from linear regression of ke and TSS

		string *Name =new string();										//name of the substance
		string *comment=new string();										//comments
		int IDrun;													//runid
		int parameter_id_input[1];										//parameter_id
		int refScen_wq_load;										//ScenarioId of the reference scenario which is used for read in functions
		int IDTemp[1]; 												//Water temperature id
		double TETA[1];  												//teta = temperature correction
		int svn_version_id[1];											//version of the svn repository

		double *decomposition    = new double[continent[0]];    // Abbaufaktor(1/sec, 1/Monat)

		int IDScen;												//Scenario id of each newly produced scenario

		double perc_connc_treat;								//percent of the connected population which received treatment
		double sum_con; 										//sum of population connected and with treatment to a sewer system in the default scenario
		double perc_unconnc_red;								//percent of the unconnected population which received some type of reduction
		double sum_uncon_no_red; 								//sum of population not connected and without reduction from the default scenario


		//ACHTUNG!!Hier muss refScen noch angepasst werden! und ggf projectid!!!
		if(project_id==3 && parameter_id==310 && refScen==0)
			IDrun=303100000;
		else{
			cerr<<"ERROR: This runid is unknown. Projectid="<<project_id<<" refScen="<<refScen<<endl; exit(1);
		}

		/********************Start of the read in methods********************/

		//read data of the Latin Hypercube sampling
		readLHS(LHS);

		/*for(int i=0;i<maxrow;i++){
			for(int j=0; j<maxcol;j++){
				cout<<LHS[i][j]<<"\t";
				//cout<<"LHS["<<i<<"]["<<j<<"]"<<LHS[i][j]<<"\n";
			}
			cout<<"\n";
		}*/

		//read reference scenario -> from which alteration will be done to
		readScenario(ScenName, ScenDescr, refScen, project_id);

		readCellInput(refScen, year, country_cell);

		readCountryInput(refScen,  year, countries, connectivity,
				 stp_failure, connec_treat_unknown, hanging_lat, open_defec);

		/*for(int country=0; country<countries.number;country++){
			cout<<"country="<<country<<" connectivity[country][0]="<<connectivity[country][0]
			    <<" connectivity[country][1]="<<connectivity[country][1]<<" connectivity[country][2]="<<connectivity[country][2]
			    <<" connectivity[country][3]="<<connectivity[country][3]<<" connectivity[country][4]="<<connectivity[country][4]
			    <<" connec_treat_unknown="<<connec_treat_unknown[country]
			    <<" hanging_lat="<<hanging_lat[country]<<" open_defec="<<open_defec[country]
			    <<" get_countryPos(country)="<<countries.get_countryPos(country)<<endl;
		}*/

		readCountryParameterInput(refScen, year, parameter_id, countries, ef, conc_man, conc_urb);

		readLsExrRate(ex_ls, parameter_id, refScen);

		readParameterInput(refScen, year, parameter_id, rem, treat_failure, sed_veloc, rad_data);

		readRunlist(IDrun, Name, parameter_id_input, parameter_id,
				refScen, refScen_wq_load, IDTemp, TETA, svn_version_id, comment);

		cout<<"parameter_id_input: "<<parameter_id_input[0]<<endl;
		cout << "IDrun = " <<IDrun<<" (" << Name[0] <<") "<<endl;
		cout << "IDTemp: "<<IDTemp[0]<<"\tTETA: "<<TETA[0]<<endl;
		cout << "svn_version_id: "<<svn_version_id[0]<<" comment: "<<comment[0]<<endl;

		readFactors(IDrun, decomposition);



		/************Start the update of the values for each scenario**************/

		int row=0;
		//For every value of LHS the update is done
		//for(int row=0; row<maxrow;row++){
		while(row<endScen){

				//update general variables
				IDScen=startScen;

				//update des Scenarios
				startScen++;

				//update parameter_input
				rem[0]=LHS[row][16];	//primary
				rem[1]=LHS[row][17];	//secondary
				rem[2]=LHS[row][18];	//tertiary
				rem[5]=LHS[row][19];	//red_fac_org from manure
				rem[8]=LHS[row][21];	//removal rate of manure on soil
				treat_failure[0]=LHS[row][20];
				sed_veloc[0]=LHS[row][24];
				rad_data[0]=LHS[row][23];	//ks
				rad_data[1]=LHS[row][25];	//alpha from ke TSS regession
				rad_data[2]=LHS[row][26];	//beta from ke TSS regession

				/*cout<<"IDScen="<<IDScen
					<<" pim rem[0]="<<rem[0]<<" LHS[row][16]="<<LHS[row][16]
				    <<" sec rem[1]="<<rem[1]
				    <<" tert rem[2]="<<rem[2]
				    <<" red_fac_org rem[5]="<<rem[5]
				    <<" soil rem[8]="<<rem[8]
				    <<" treat_failure="<<treat_failure[0]
				    <<" sed_veloc="<<sed_veloc[0]
				    <<" ks rad_data[0]="<<rad_data[0]
				    <<" alpha rad_data[1]="<<rad_data[1]
				    <<" beta rad_data[2]="<<rad_data[2]<<endl;
				*/

				//update runlist
				IDrun=IDrun+1;
				TETA[0]=LHS[row][12];

				//cout<<"TETA="<<TETA<<endl;




				for(int FAOReg=0;FAOReg<11;FAOReg++)
					for(int lsu=0;lsu<12;lsu++){
						ex_ls[FAOReg][lsu]=LHS[row][lsu];
						//cout<<"ex_ls["<<FAOReg<<"]["<<lsu<<"]="<<ex_ls[FAOReg][lsu]<<" LHS[row][lsu]="<<LHS[row][lsu]<<endl;
				}


				for(int country=0; country<countries.number;country++){

					//update country_input

					//must not be divided by 100 because LHS[row][40] is already in 0.XXX (divided by 100)
					perc_connc_treat=(LHS[row][39]*LHS[row][40]);	//percent of the connected population which received treatment
					sum_con=connectivity[country][0]+connectivity[country][1]+connectivity[country][2]; //sum of population connected and with treatment to a sewer system in the default scenario

					if(country==0||country==9||country==12)
					cout<<" before connectivity[country][0]="<<connectivity[country][0]<<" country="<<country
						<<" connectivity[country][1]="<<connectivity[country][1]
						<<" connectivity[country][2]="<<connectivity[country][2]<<" connectivity[country][3]="<<connectivity[country][3]
						<<"\nLHS[row][39]="<<LHS[row][39]<<" LHS[row][40]="<<LHS[row][40]<<" LHS[row][41]="<<LHS[row][41]<<endl;


					//the share of the population connected to a sewer system from the default scenario for share of e.g. primary
					//is multiplied with the percentage of the population with treatment
					if(sum_con==0 && perc_connc_treat==0){
						con_help[country][0]=0;
						con_help[country][1]=0;
						con_help[country][2]=0;

					}else if(sum_con==0 && perc_connc_treat!=0){
						//if the distribution of prim, sec, tert is equal because all of them are zero than it will
						//all be attributed to primary and secondary in equal parts -> tertiary is in AF very unlikely
						con_help[country][0]=100*perc_connc_treat/2;
						con_help[country][1]=100*perc_connc_treat/2;
						con_help[country][2]=0;
					}else{
						con_help[country][0]=100*perc_connc_treat*(connectivity[country][0]/sum_con);	//connected and primary
						con_help[country][1]=100*perc_connc_treat*(connectivity[country][1]/sum_con);	//connected and secondary
						con_help[country][2]=100*perc_connc_treat*(connectivity[country][2]/sum_con);	//connected and tertiary
					}

					con_help[country][3]=100*(LHS[row][39]-perc_connc_treat); //connected and untreated

					if(country==0||country==9||country==12)
					cout<<"con_help[country][0]="<<con_help[country][0]<<" country="<<country
						<<" perc_connc_treat="<<perc_connc_treat
						<<" sum_con="<<sum_con<<" con_help[country][1]="<<con_help[country][1]
						<<" con_help[country][2]="<<con_help[country][2]<<" con_help[country][3]="<<con_help[country][3]<<endl;


					//must not be divided by 100 because LHS[row][41] is already in 0.XXX (divided by 100)
					perc_unconnc_red=((1-LHS[row][39])*LHS[row][41]);	//percent of the unconnected population which received some type of reduction
					sum_uncon_no_red=NODATA;



					if(country==0||country==9||country==13)
						cout<<"\nbefore country="<<country<<" sum_uncon_no_red="<<sum_uncon_no_red
							<<" connec_treat_unknown[country]="<<connec_treat_unknown[country]
							<<" hanging_lat[country]="<<hanging_lat[country]<<" open_defec[country]="<<open_defec[country]<<"\n"<<endl;


					connec_treat_unknown_help[country]=100*perc_unconnc_red;

					if((hanging_lat[country]==NODATA && open_defec[country]==NODATA) || (hanging_lat[country]==NODATA && open_defec[country]!=NODATA)){
						sum_uncon_no_red=NODATA;
						hanging_lat_help[country]=0;
						open_defec_help[country]=100*((1-LHS[row][39])-perc_unconnc_red);
					}else if(hanging_lat[country]!=NODATA && open_defec[country]==NODATA){
						hanging_lat_help[country]=100*((1-LHS[row][39])-perc_unconnc_red)/2;
						open_defec_help[country]=100*((1-LHS[row][39])-perc_unconnc_red)/2;
					}else{
						sum_uncon_no_red=hanging_lat[country]+open_defec[country];
						hanging_lat_help[country]=100*((1-LHS[row][39])-perc_unconnc_red)*(hanging_lat[country]/sum_uncon_no_red);
						open_defec_help[country]=100*((1-LHS[row][39])-perc_unconnc_red)*(open_defec[country]/sum_uncon_no_red);

					}


					if(country==0||country==9||country==13||country==268)
					cout<<"\ncountry="<<country
						<<" perc_unconnc_red="<<perc_unconnc_red
						<<" sum_uncon_no_red="<<sum_uncon_no_red<<" connec_treat_unknown_help[country]="<<connec_treat_unknown_help[country]
						<<" hanging_lat[country]_help="<<hanging_lat_help[country]<<" open_defec[country]_help="<<open_defec_help[country]<<"\n"<<endl;



					stp_failure[country]=LHS[row][27];

					//if(country==0||country==9||country==12)
					//cout<<"stp_failure[country]="<<stp_failure[country]<<" country="<<country<<" LHS[row][27]="<<LHS[row][27]<<endl;

					//update country_parameter_input
					ef[country]=LHS[row][13];

					//only nd (other) is used so all others are set to NODATA
					for(int ind=0;ind<6;ind++)
						conc_man[country][ind]=NODATA;
					conc_man[country][6]=LHS[row][14];
					conc_urb[country]=LHS[row][15];

					/*if(country==0||country==9||country==12)
					cout<<"ef[country]="<<ef[country]<<" country="<<country
							<<" conc_man[6]="<<conc_man[country][6]<<" conc_urb="<<conc_urb[country]<<endl;
					*/
				}

				for(int cell=0;cell<continent[0];cell++){
					//update factors
					decomposition[cell]=LHS[row][22];

					if(cell==1 || cell==187000)
					cout<<"cell="<<cell<<" decomposition[cell]="<<decomposition[cell]<<endl;

				}

			/****Start inserting updated values as a new scenario****/

			//insert into _scenario
			query.reset();
			query << "DELETE FROM "<<"wq_general._szenario"<<" WHERE `IDScen`= " << IDScen<<" AND `project_id`= " <<project_id<<";";
			//cout<<query<<endl;
			query.execute();

			query.reset();
			query << setprecision(30);

			query <<"insert into "<<"wq_general._szenario (`IDScen`, `project_id`, `ScenName`, `description`) values("
				  <<IDScen<<", "<<project_id<<", 'sensitivity analysis run', 'like run 0 project_id 3 with LHS changes' "<<");"<<endl;
			//cout<<query<<endl;
			query.execute();


			//insert into parameter_input
			query.reset();
			query << "DELETE FROM "<<MyDatabase<<"wq_load.parameter_input"<<" WHERE `IDScen`= " << IDScen<<" AND `parameter_id`= " <<parameter_id<<";";
			//cout<<query<<endl;
			query.execute();

			query.reset();
			query << setprecision(30);

			query <<"insert into "<<MyDatabase<<"wq_load.parameter_input (`IDScen`, `parameter_id`, `time`, `rem_prim`, `rem_sec`, "
				  <<" `rem_tert`, `rem_untr`, `treat_failure`, `red_fac_org`, `k_soil`, `ks`, `sed_veloc`, `ke_tss_reg_alpha`, "
				  <<"`ke_tss_reg_beta`) values("
				  <<IDScen<<", "<<parameter_id<<", "<<year<<", "<<rem[0]<<", "<<rem[1]<<", "<<rem[2]<<", "<<rem[3]<<", "
				  <<treat_failure[0]<<", "<<rem[5]<<", "<<rem[8]<<", "<<rad_data[0]<<", "<<sed_veloc[0]<<", "
				  <<rad_data[1]<<", "<<rad_data[2]
				  <<");"<<endl;
			//cout<<query<<endl;
			query.execute();


			//insert into runlist
			query.reset();
			query << "DELETE FROM "<<"wq_general._runlist"<<" WHERE `IDrun`= " << IDrun<<";";
			//cout<<query<<endl;
			query.execute();

			query.reset();
			query << setprecision(30);

			//ACHTUNG z.T. hart für FC codiert! e.g. conservative =0
			query <<"insert into wq_general._runlist (`IDrun`, `Name`, `conservative`, `lake`, `IDScen`, `IDScen_wq_load`, `project_id`, "
				  <<" `parameter_id_input`, `parameter_id`, `IDTemp`, `teta`, `teta_lake`, `parameter_id_load`, "
				  <<" `svn_version_id`, `comment`) values("
				  <<IDrun<<", '"<<Name[0]<<"', "<<0<<", null, "<<IDScen<<", "<<IDScen<<", "<<project_id<<", "
				  <<parameter_id_input[0]<<", "<<parameter_id<<", "<<IDTemp[0]<<", "<<TETA[0]<<", null, "<<parameter_id+4<<", "
				  <<svn_version_id[0]<<", 'sensitivity analysis run same as 303100000 but with LHS changes'"
				  <<");"<<endl;
			//cout<<query<<endl;
			query.execute();

			//insert into ls_excr_rate
			query.reset();
			query << "DELETE FROM "<<MyDatabase<<"wq_load.ls_exr_input"<<" WHERE `parameter_id`= " << parameter_id<<" AND `IDScen`="<<IDScen<<";";
			//cout<<query<<endl;
			query.execute();

			for(int lsu=0;lsu<12;lsu++)
				for(int FAOReg=0; FAOReg<11;FAOReg++){
					query.reset();
					query << setprecision(30);
					query <<"insert into "<<MyDatabase<<"wq_load.ls_exr_input "<<"(`parameter_id`, `IDScen`, `LS`, `IDFAOReg`, `ls_exr_rate`) values("
							<<parameter_id<<", "<<IDScen<<", "<<lsu<<", "<<FAOReg+1<<", "<<ex_ls[FAOReg][lsu]
							                                                                           <<");"<<endl;
					//cout<<query<<endl;
					query.execute();
			}


			query.reset();
			query << "DELETE FROM "<<MyDatabase<<"wq_load.country_input"<<" WHERE `IDScen`= " << IDScen<<" AND `time`= " <<year<<";";
			//cout<<query<<endl;
			query.execute();

			query.reset();
			query << "DELETE FROM "<<MyDatabase<<"wq_load.country_parameter_input"<<" WHERE `IDScen`= " << IDScen<<" AND `time`= " <<year<<" AND `parameter_id`="<<parameter_id<<";";
			//cout<<query<<endl;
			query.execute();

			for(int country=0; country<countries.number;country++){

				//insert into country_input
				query.reset();
				query << setprecision(30);

				query <<"insert into "<<MyDatabase<<"wq_load.country_input (`IDScen`, `country_id`, `time`, `fra_man_f`, `fra_man_t`, `fra_man_p`, `fra_man_c`, `fra_man_g`, `fra_man_m`, `fra_man_nd`, "
					  <<"`con_prim`, `con_sec`, `con_tert`, `con_untr`, `stp_failure`, `to_treat_and_unknown`, `to_hanging_t`, `to_open_def` ) values("
					  <<IDScen<<", "<<countries.get_ISONUM(country)<<", "<<year<<",0,0,0,0,0,0,1, ";


				if(con_help[country][0]==NODATA) query <<"NULL, ";
					else query<<con_help[country][0]<<", ";

				if(con_help[country][1]==NODATA) query <<"NULL, ";
					else query<<con_help[country][1]<<", ";

				if(con_help[country][2]==NODATA) query <<"NULL, ";
					else query<<con_help[country][2]<<", ";

				if(con_help[country][3]==NODATA) query <<"NULL, ";
					else query<<con_help[country][3]<<", ";

				if(stp_failure[country]==NODATA) query <<"NULL, ";
					else query<<stp_failure[country]<<", ";

				if(connec_treat_unknown_help[country]==NODATA) query <<"NULL, ";
					else query<<connec_treat_unknown_help[country]<<", ";

				if(hanging_lat_help[country]==NODATA) query <<"NULL, ";
					else query<<hanging_lat_help[country]<<", ";

				if(open_defec_help[country]==NODATA) query <<"NULL); ";
					else query<<open_defec_help[country]<<");"<<endl;

				//cout<<query<<endl;
				query.execute();



				//insert into country_parameter_input
				query.reset();
				query << setprecision(30);

				query <<"insert into "<<MyDatabase<<"wq_load.country_parameter_input "
					<<"(`IDScen`, `parameter_id`, `country_id`, `time`, `ef`, `conc_man_f`, `conc_man_t`, `conc_man_p`, "
					<<"`conc_man_c`, `conc_man_g`, `conc_man_m`, `conc_man_nd`, `conc_tds1`, `conc_tds2`, `conc_tds3`, "
					<<"`conc_tds4`, `c_geogen`, `conc_urb`) values("
					<<IDScen<<", "<<parameter_id<<", "<<countries.get_ISONUM(country)<<", "<<year<<", ";

				if(ef[country]==NODATA) query <<"NULL, ";
					else query<<ef[country]<<", ";

				if(conc_man[country][0]==NODATA) query <<"0, ";
					else query<<conc_man[country][0]<<", ";

				if(conc_man[country][1]==NODATA) query <<"0, ";
					else query<<conc_man[country][1]<<", ";

				if(conc_man[country][2]==NODATA) query <<"0, ";
					else query<<conc_man[country][2]<<", ";

				if(conc_man[country][3]==NODATA) query <<"0, ";
					else query<<conc_man[country][3]<<", ";

				if(conc_man[country][4]==NODATA) query <<"0, ";
					else query<<conc_man[country][4]<<", ";

				if(conc_man[country][5]==NODATA) query <<"0, ";
					else query<<conc_man[country][5]<<", ";

				if(conc_man[country][6]==NODATA) query <<"0, ";
					else query<<conc_man[country][6]<<", ";

				query<<" null, null, null, null, null, ";	//TDS and c_geogen is always null for FC run

				if(conc_urb[country]==NODATA) query <<"NULL); ";
					else query<<conc_urb[country]<<"); "<<endl;

				//cout<<query<<endl;
				query.execute();


			}//for (country)



			query.reset();
			query << "DELETE FROM "<<MyDatabase<<"wq_load_"<<continent_abb<<".cell_input"<<" WHERE `IDScen`= " << IDScen<<" AND `time`= " <<year<<";";
			cout<<query<<endl;
			query.execute();

			query.reset();
			query << "DELETE FROM "<<MyDatabase<<"worldqual_"<<continent_abb<<".factors"<<" WHERE `IDrun`= " << IDrun<<";";
			cout<<query<<endl;
			query.execute();

			//Data which is cell based
			for(int cell=0;cell<continent[0];cell++){

				//insert into cell_input
				query.reset();
				query << setprecision(30);

				query <<"insert into "<<MyDatabase<<"wq_load_"<<continent_abb<<".cell_input"
					  <<"(`IDScen`, `cell`, `time`, `country_id`) values("
					<<IDScen<<", "<<cell+1<<", "<<year<<", ";

				if(country_cell[cell]==NODATA) query <<"NULL, ";
					else query<<country_cell[cell]<<");";

				if(cell==0||cell==2)
					cout<<query<<endl;
				query.execute();


				//insert into factors

				query.reset();
				query << setprecision(30);

				query <<"insert into "<<MyDatabase<<"worldqual_"<<continent_abb<<".factors"
					  <<"(`IDrun`, `cell`, `decomposition`, `c_geogen`, `decay_rate_lake`) values("
					<<IDrun<<", "<<cell+1<<", ";

				if(decomposition[cell]==NODATA) query <<"NULL, ";
					else query<<decomposition[cell]<<", ";


				query<<"null, null);"<<endl;

				if(cell==0||cell==2)
					cout<<query<<endl;
				query.execute();




			}

		row++;
		}//row des LHS, d.h. scenario 1-n

		if((startScen-1)!=endScen){
			cerr<<"Error - number of Scenarios is not correct!"
				<<" startScen="<<startScen<<" startScen-1="<<startScen-1
				<<" endScen="<<endScen<<" row="<<row<<" IDScen="<<IDScen<<endl;
		}


	}  // end try
	catch (const mysqlpp::BadQuery& er) {
        // Handle any query errors
        cerr << "Query error: " << er.what() << "." << endl;
        //return -1;
    }
	catch (const mysqlpp::BadConversion& er) {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
                "\tretrieved data size: " << (int)er.retrieved <<
                ", actual size: " << (int)er.actual_size << endl;
        //return -1;
    }
	//catch (const mysqlpp::SQLQueryNEParms& er) {}  // version mysql++.2.3.3  SQLQueryNEParms renamed to BadParamCount
	catch (const mysqlpp::BadParamCount& er) {
		cerr << "parms error: " << er.what() << endl;
	}
	catch (const std::exception& er) {
		// Catch-all for any other MySQL++ exceptions
		cerr << "Error: " << er.what() << endl;
		//return 1;
	}
    catch (char * str) {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << str << endl;
        //return -1;
    }
	//cout << "end...";  cin >> j;

	// print current date
	cout <<"\nProgram worldqual endet at: " << getTimeString() << endl << endl;
	return 0;
} // end of main()


/*************************Methods implemented***************************/

/*
 * Method to read in the LHS txt file
 */
void readLHS(double (*LHS)[maxcol]){

		char filename[maxcharlength];
		sprintf(filename, "LHS_erzeugt.txt");

		//reads file into ifstream
		ifstream inFile(filename);

		if (!inFile) {
			cerr << "ERROR: Can't open input file" << filename << endl;
			cerr << "exiting!" << endl;
			exit(1);
		}


		for (int row=0; row<maxrow; row++) {
			for (int col=0; col<maxcol; col++) {
				//writes data into array
				//Attention -> only double values my be in the txt file!
				//Delete all captions before the read
				inFile>>LHS[row][col];
				//cout<<LHS[row][col]<<"\t";
			}
			//cout<<"\n";
		}

		inFile.close();

}

/*
 * Method to read in the information on a scenario
 */
void readScenario(std::string ScenName,std::string ScenDescr, int IDScen, int project_id){

	int j;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
	//mysqlpp::Row    rowQuery;

	query.reset();

	j=sprintf(sql,    "SELECT s.`ScenName`, s.`description` FROM wq_general._szenario s ");
	j+=sprintf(sql+j, " WHERE s.`IDScen`=%d AND s.`project_id`=%d; ", IDScen, project_id);


	query << sql;
	//cout << "Query: " << query << endl << endl;
	resQuery = query.store();
	if (resQuery.empty()) {cerr << "readScenario: error in table wq_general._szenario.\n"; exit(1);}

//	try
	{

		mysqlpp::String ScenaName = resQuery[0]["ScenName"];
		mysqlpp::String ScenaDescr  = resQuery[0]["description"];
		ScenName=(string)ScenaName;
		ScenDescr=(string)ScenaDescr;

	}
	//cout<<"ScenName="<<ScenName<<" ScenDescr="<<ScenDescr<<endl;

}

/*
 * Method to read the cell input
 * ATTENTION: This is done for only FC! This means it reads only FC cell input and
 * ignores columns which are null for FC!!!
 */
int readCellInput(int IDScen, int year, int *country_cell){

		int j;
		char sql[2000];                        // variable for SQL-query
		mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
		//mysqlpp::Row    rowQuery;

		int cell;

		for (cell=0; cell<continent[0]; cell++) {
			country_cell[cell] = NODATA;

		}

		query.reset();

		j=sprintf(sql,    "SELECT `cell`, IFNULL(`country_id`,%d) as `country_id` ", NODATA);
		j+=sprintf(sql+j, "FROM %swq_load_%s.cell_input ", MyDatabase, continent_abb);
		j+=sprintf(sql+j, "WHERE `IDScen`=%d AND `time`=%d ORDER BY `cell`; ", IDScen, year);

		query << sql;
		#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		#endif
		resQuery = query.store();
		if (resQuery.empty()) {cerr << "readCellInput: error in table cell_input.\n"; exit(1);}

	//	try
		{

			for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
			{
				cell    = resQuery[rowNum]["cell"]-1;

				country_cell[cell] = resQuery[rowNum]["country_id"];


				/*cout<<"cell="<<cell<<" country_cell[cell]="<<country_cell[cell]
					<<endl;
				 */
			}
		}
		cout<<"readCellInput done"<<endl;
		return 0;
}



/*
 * Method to read the country input
 */
void readCountryInput(int IDScen, int year, COUNTRY_CLASS countries,
		double (*connectivity)[4], double *stp_failure,
		double *connec_treat_unknown, double *hanging_lat, double *open_defec){

		int j;
		char sql[2000];                        // variable for SQL-query
		mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
		//mysqlpp::Row    rowQuery;

		int country;

		//Initalisierung der Variablen
		for (country=0; country<countries.number; country++) {
			for (int i=0; i<4; i++) connectivity[country][i] = NODATA;
		}

		query.reset();
		//Scheibt den Text in sql und zählt die anzahl der character in j
		j=sprintf(sql,    "SELECT c.`country_id`, ");
		j+=sprintf(sql+j, "IFNULL(c.`con_prim`,%d) as `con_prim`, IFNULL(c.`con_sec`,%d) as `con_sec`, ", 0, 0);
		j+=sprintf(sql+j, "IFNULL(c.`con_tert`,%d) as `con_tert`, IFNULL(c.`con_untr`,%d) as `con_untr`, ", 0, 0);
		j+=sprintf(sql+j, "IFNULL(c.`stp_failure`,%d) as `stp_failure`, IFNULL(c.`to_treat_and_unknown`,%d) as `connec_treat_unknown`, ", NODATA, NODATA);
		j+=sprintf(sql+j, "IFNULL(c.`to_hanging_t`,%d) as `hanging_lat`, IFNULL(c.`to_open_def`,%d) as `open_defec` ", NODATA, NODATA);
		j+=sprintf(sql+j, "FROM %swq_load.country_input c ", MyDatabase);
		j+=sprintf(sql+j, "WHERE c.`IDScen`=%d AND c.`time`=%d ORDER BY c.`country_id`; ", IDScen, year);

		query << sql;
		//cout<<"country query: "<<query<<endl;
		#ifdef DEBUG_queries
		  cout << "Query: " << query << endl << endl; //cin >> j;
		#endif
		resQuery = query.store();
		if (resQuery.empty()) {cout << "get_country_input: error in table country_input.\n"; exit(1);}

	//	try
		{
			//while (rowQuery = resQuery.fetch_row())
			for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
			{
				country=resQuery[rowNum]["country_id"];

				int countryPos = countries.get_countryPos(country);
				if (countryPos<0)  {cerr << "country "<< country << " not exist!!! error in country_input.\n"; exit(1);}

				//Die Variablen werden mit den Werten aus der Datenbank gefuellt
				connectivity[countryPos][0]         = resQuery[rowNum]["con_prim"];
				connectivity[countryPos][1]         = resQuery[rowNum]["con_sec"];
				connectivity[countryPos][2]         = resQuery[rowNum]["con_tert"];
				connectivity[countryPos][3]         = resQuery[rowNum]["con_untr"];
				stp_failure[countryPos]    = resQuery[rowNum]["stp_failure"];
				connec_treat_unknown[countryPos]    = resQuery[rowNum]["connec_treat_unknown"];
				hanging_lat[countryPos]    = resQuery[rowNum]["hanging_lat"];
				open_defec[countryPos]     = resQuery[rowNum]["open_defec"];


			}
		}
		cout<<"readCountryInput done"<<endl;
}

/*
 * Method to read the country parameter input
 * ATTENTION: This is done for only FC! This means it reads only FC cell input and
 * ignores columns which are null for FC!!!
 */
void readCountryParameterInput(int IDScen, int year, int parameter_id,
	COUNTRY_CLASS countries, double *ef, double (*conc_man)[7], double *conc_urb){

			int j;
			char sql[2000];                        // variable for SQL-query
			mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
			//mysqlpp::Row    rowQuery;
			//cout << "year, parametar_id: " << year <<endl << parameter_id<<endl;

			int country;
			for (country=0; country<countries.number; country++) {
				ef[country]       = NODATA;
				for (int man_fra=0; man_fra<7; man_fra++)
					conc_man[country][man_fra] = NODATA;
			}

			query.reset();
			j=sprintf(sql,    "SELECT c.`country_id`, ");
			j+=sprintf(sql+j, "IFNULL(c.`ef`,%d) as `ef`, ", NODATA);
			j+=sprintf(sql+j, "IFNULL(c.`conc_man_f`,%d) as `conc_man_f`, IFNULL(c.`conc_man_t`,%d) as `conc_man_t`, ", NODATA, NODATA);
			j+=sprintf(sql+j, "IFNULL(c.`conc_man_p`,%d) as `conc_man_p`, IFNULL(c.`conc_man_c`,%d) as `conc_man_c`, ", NODATA, NODATA);
			j+=sprintf(sql+j, "IFNULL(c.`conc_man_g`,%d) as `conc_man_g`, IFNULL(c.`conc_man_m`,%d) as `conc_man_m`, ", NODATA, NODATA);
			j+=sprintf(sql+j, "IFNULL(c.`conc_man_nd`,%d) as `conc_man_nd`, IFNULL(c.`conc_urb`,%d) as `conc_urb` ", NODATA, NODATA);
			j+=sprintf(sql+j, "FROM %swq_load.`country_parameter_input` c WHERE c.`IDScen`=%d AND ", MyDatabase, IDScen);
			j+=sprintf(sql+j, "c.`parameter_id`=%d AND c.`time`=%d ORDER BY c.`country_id`; ", parameter_id, year);

			query << sql;

			#ifdef DEBUG_queries
			  cout << "Query: " << query << endl << endl; //cin >> j;
			#endif
			resQuery = query.store();
			if (resQuery.empty()) {cout << "readCountryParameterInput: error in table country_parameter_input.\n"; exit(1);}

		//	try
			{

				for (unsigned int rowNum=0; rowNum<resQuery.size(); rowNum++)
				{
					country=resQuery[rowNum]["country_id"];

					int countryPos = countries.get_countryPos(country);
					if (countryPos<0)  {cerr << "country "<< country << " not exist!!! error in country_parameter_input.\n"; exit(1);}

					ef[countryPos]          = resQuery[rowNum]["ef"];
					conc_man[countryPos][0] = resQuery[rowNum]["conc_man_f"];
					conc_man[countryPos][1] = resQuery[rowNum]["conc_man_t"];
					conc_man[countryPos][2] = resQuery[rowNum]["conc_man_p"];
					conc_man[countryPos][3] = resQuery[rowNum]["conc_man_c"];
					conc_man[countryPos][4] = resQuery[rowNum]["conc_man_g"];
					conc_man[countryPos][5] = resQuery[rowNum]["conc_man_m"];
					conc_man[countryPos][6] = resQuery[rowNum]["conc_man_nd"];
					conc_urb[countryPos]    = resQuery[rowNum]["conc_urb"];


				}
			}

	cout<<"readCountryParameterInput done"<<endl;
} // end of readCountryParameterInput()


/*
  * Method to read the ls_exr_rate input
 */
void readLsExrRate(double (*ex_ls)[12], int parameter_id, int refScen) {
	mysqlpp::Query      query = con.query();

	for(int FAO_num=0; FAO_num<11; FAO_num++){

			query.reset();
			query << "SELECT l.`LS`, l.`IDFAOReg` as `FAOReg`, IFNULL(l.`ls_exr_rate`,"<<NODATA<<") as `ls_ex`"
				<<" FROM "<<MyDatabase<<"wq_load.ls_exr_input l WHERE l.`parameter_id`= "<<parameter_id
				<<" And l.`IDFAOReg`= "<<FAO_num+1<<" AND l.`IDScen`="<<refScen<<" ;";

			//cout << "Query: " << query << endl << endl; //cin >> j;

			res = query.store();
			//cout << "*********************** res.size()="<<res.size()<<endl;
			if (res.empty()) {cerr << "Error in table ls_exr_input.\n";}


			//	try
			{
				int i=0;

				for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
				{
					//cout << "i="<<i<<" "<<res[rowNum]["LS"]<<" "<<res[rowNum]["FAOReg"]<<" "
						//	<<res[rowNum]["ls_ex"]<<endl;

					ex_ls[FAO_num][i]		= res[rowNum]["ls_ex"];
					i++;
				}

			}
	}
	cout<<"readLsExrRate done"<<endl;
} // readLsExrRate()


/*
 * Method to read the country parameter input
 * ATTENTION: This is done for only FC! This means it reads only FC cell input and
 * ignores columns which are null for FC!!!
 */
void readParameterInput(int IDScen, int year, int parameter_id, double rem[9],
	double treat_failure[1], double sed_veloc[1], double *rad_data) {

	int j;
	char sql[2000];                        // variable for SQL-query
	mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
	//mysqlpp::Row    rowQuery;

	for (int i=0; i<9; i++)
		rem[i]=NODATA;


	query.reset();
	j=sprintf(sql,    "SELECT IFNULL(p.`rem_prim`,%d) as `rem_prim`, IFNULL(p.`rem_sec`,%d) as `rem_sec`,  ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`rem_tert`,%d) as `rem_tert`, IFNULL(p.`rem_untr`,%d) as `rem_untr`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`red_fac_org`,%d) as `red_fac_org`, ", NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`k_storage`,%d) as `k_storage`,  IFNULL(p.`k_soil`,%d) as `k_soil`, ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`sed_veloc`,%d) as 'sed_veloc' ,  IFNULL(p.`ks`,%d) as 'ks', ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`ke_tss_reg_alpha`,%d) as 'ke_tss_reg_alpha' ,  IFNULL(p.`ke_tss_reg_beta`,%d) as 'ke_tss_reg_beta', ", NODATA, NODATA);
	j+=sprintf(sql+j, "IFNULL(p.`treat_failure`,%d) as `treat_failure` ", NODATA);
	j+=sprintf(sql+j, "FROM %swq_load.`parameter_input` p ", MyDatabase );
	j+=sprintf(sql+j, "WHERE p.`idScen`=%d AND p.`parameter_id`=%d AND p.`time`=%d ; ", IDScen, parameter_id, year);

	query << sql;

	#ifdef DEBUG_queries
	  cout << "Query: " << query << endl << endl; //cin >> j;
	#endif



	resQuery = query.store();
	if (resQuery.empty()) {cout << "get_parameter_input: error in table parameter_input.\n"; exit(1);}

	rem[0]   = resQuery.at(0)["rem_prim"];
	rem[1]   = resQuery.at(0)["rem_sec"];
	rem[2]   = resQuery.at(0)["rem_tert"];
	rem[3]   = resQuery.at(0)["rem_untr"];
	rem[5]   = resQuery.at(0)["red_fac_org"];
	rem[7]   = resQuery.at(0)["k_storage"];
	rem[8]   = resQuery.at(0)["k_soil"];
	treat_failure[0]  = resQuery.at(0)["treat_failure"];
	sed_veloc[0]	= resQuery.at(0)["sed_veloc"];
	rad_data[0]= resQuery.at(0)["ks"];
	rad_data[1]= resQuery.at(0)["ke_tss_reg_alpha"];
	rad_data[2]= resQuery.at(0)["ke_tss_reg_beta"];


	cout<<"readParameterInput done"<<endl;


} // end of readParameterInput

/*
 * Method to read the runlist information
 * ATTENTION: This is done for only FC! This means it reads only FC cell input and
 * ignores columns which are null for FC!!!
 */
void readRunlist(int IDrun, string *Name, int parameter_id_input[1], int parameter_id,
		int IDScen, int IDScen_wq_load, int IDTemp[1], double TETA[1], int svn_version_id[1], string *comment){

			int j;
			char sql[2000];                        // variable for SQL-query
			mysqlpp::StoreQueryResult resQuery; //mysqlpp::Result     resQuery;
			//mysqlpp::Row    rowQuery;

			query.reset();
			j=sprintf(sql,    "SELECT s.`Name`, IFNULL(s.`IDScen`,%d) as `IDScen`, IFNULL(s.`IDScen_wq_load`,%d) as `IDScen_wq_load`, ", NODATA, NODATA);
			j+=sprintf(sql+j, "IFNULL(s.`conservative`,%d) as `conservative`, ", NODATA);
			j+=sprintf(sql+j, "IFNULL(s.`lake`,%d) as `lake`, ", NODATA);
			j+=sprintf(sql+j, "s.`parameter_id_input`, s.`parameter_id`,  ");
			j+=sprintf(sql+j, "IFNULL(s.`IDTemp`,%d) as `IDTemp`, IFNULL(s.`teta`,%d) as `teta`, ", NODATA, NODATA);
			j+=sprintf(sql+j, "IFNULL(s.`svn_version_id`,%d) as `svn_version_id`, IFNULL(s.`comment`,%d) as `comment` ", NODATA, NODATA);
			j+=sprintf(sql+j, "FROM wq_general._runlist s ");
			j+=sprintf(sql+j, "WHERE s.IDrun=%d ;", IDrun);
			query << sql;
			//cout<<"Query: "<<query<<endl;
			res = query.store();
			if (res.empty()) {cerr << "ERROR: Wrong parameter: IDrun" << endl; exit(1);}  // falsche IDrun
			mysqlpp::String ScenaName=res.at(0)["Name"];
			Name[0]=(string)ScenaName;
			parameter_id_input[0]=res.at(0)["parameter_id_input"];
			parameter_id=res.at(0)["parameter_id"];
			//cout<<"parameter_id: "<<parameter_id<<endl;
			IDScen=res.at(0)["IDScen"];
			IDScen_wq_load=res.at(0)["IDScen_wq_load"];
			//cout << "IDrun = " <<IDrun<<" (" << Name[0] <<")"<<endl;
			IDTemp[0] = res.at(0)["IDTemp"];
			TETA[0]   = res.at(0)["TETA"];
			//cout << "IDTemp: "<<IDTemp[0]<<"\tTETA: "<<TETA<<endl;
			svn_version_id[0]=res.at(0)["svn_version_id"];
			mysqlpp::String StrComment=res.at(0)["comment"];
			comment[0]=(string)StrComment;
			//cout << "svn_version_id: "<<svn_version_id[0]<<" comment: "<<comment[0]<<endl;



			cout<<"readRunlist done"<<endl;

}

/*
 * Method to read the decomposition rate
 * ATTENTION: This is done for only FC! This means it reads only FC cell input and
 * ignores columns which are null for FC!!!
 */
void readFactors(int IDrun, double *factor) {
	int j, cell;

	char sql[2000];                        // variable for SQL-query
	mysqlpp::StoreQueryResult res; 		//mysqlpp::Result     resQuery;



	query.reset();
	j=sprintf(sql, "SELECT f.cell, IFNULL(f.decomposition,0) as `decomposition` ");
	j+=sprintf(sql+j, "FROM %sworldqual_%s.factors f WHERE f.IDrun=%d;", MyDatabase, continent_abb, IDrun);

	query << sql;
	#ifdef DEBUG_queries
	  cout << "Query: " << query.preview() << endl << endl; //cin >> j;
	#endif
	  res = query.store();

	if (res.empty()) {
		cerr << "readFactors: error in table factors. table is not exists or empty." << endl;
		exit(1);  // Zelle nicht in berechnetem Region
	}

//	try
	{

		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
		{
			cell=res[rowNum]["cell"];
			if (cell>continent[0]) {cerr << "cell "<< cell << " not exist! error in g_outflc.\n"; exit(1);}
			factor[cell-1]      = res[rowNum]["decomposition"];

		}
	}//try for res


	cout<<"readFactors done"<<endl;

} // end of readFactors()

