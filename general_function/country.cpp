#include "country.h"

extern mysqlpp::Connection con;

COUNTRY_CLASS::COUNTRY_CLASS(const int IDScen, const int year) {
	this->IDScen=IDScen;
	this->year=year;
	
	number = getCouintryNumber();
	
	Country = new int[number];
	
	getCouintries();


/*
	for(int i=0; i<number; i++){
		cout<<"country ausgabe for"<<endl;
		cout<<"Country[i]: "<<Country[i]<<" i: "<<i<<endl;
	}
*/
	cout<<"country class done"<<endl;

}

int COUNTRY_CLASS::getCouintryNumber() {
	mysqlpp::Query      query = con.query();
	
	query.reset();
	query << "SELECT c.`country_id` FROM "<<MyDatabase<<"wq_load.country_input c WHERE c.`IDScen`= "<<IDScen
		<<" AND c.`time`= " << year << " GROUP BY c.`country_id`;";
	//cout<<"country number query: "<<query<<endl;
	res = query.store();
	if (res.empty()) {cout << "getCouintryNumber: error in table country_input.\n"; return 0;}
	
	int number = res.size();
	

	return number;
}
int COUNTRY_CLASS::getCouintries() {
	mysqlpp::Query      query = con.query();
	
	query.reset();
	query << "SELECT c.`country_id` FROM "<<MyDatabase<<"wq_load.country_input c WHERE c.`IDScen`="<<IDScen
		<<" AND c.`time`=" << year << " GROUP BY c.`country_id`;";
	//cout<<"get countries query: "<<query<<endl;
	res = query.store();
	if (res.empty()) {cout << "getCouintries: error in table country_input.\n"; return 1;}
	
//	try 
	{
		int i=0;
		//while (row = res.fetch_row()) 
		for (unsigned int rowNum=0; rowNum<res.size(); rowNum++)
		{
			if (i>=number) {cout << "getCouintries: error in table country_input.\n"; return 0;}
			Country[i++]=res[rowNum]["country_id"];
		}
		
	}//try for res
//	catch (const mysqlpp::EndOfResults&) {
//		// Last query result received.  Exit normally.
//		// nichts tun
//		// cout << "OK.";
//	} // catch
	
	return 0;
}

int COUNTRY_CLASS::get_countryPos(int country) {
	int i;
	for (i=0; i<number; i++)
		if (Country[i]==country) return i;
	return -1;  // Land nicht gefunden
}
