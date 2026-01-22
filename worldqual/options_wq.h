/*
 * options.h
 *
 *  Created on: 11.05.2015
 *      Author: kynast
 */

#ifndef OPTIONS_WQ_H_
#define OPTIONS_WQ_H_

class options_wqClass: public optionsClass {
public:
	options_wqClass(int option_c, char* option_v[], const char* CL) : optionsClass(option_c, option_v, CL) {
		riverLength = 0;
		routingOrder = 0;
		flowVelocityVariant = 0;
		ConstVelocity = 0.0;
		climate = 0;
	}
	virtual ~options_wqClass();

	// Optionen für worldqual
	bool riverLength;
	bool routingOrder;
	short flowVelocityVariant;      // 0 - mit variablen Fliessgeschwindigkeit  aus der Tabelle oder UNF-Datei  als Standart
	                                // 1 - mit variablen Fliessgeschwindigkeit (berechnet)
	                                // 2 - mit konstanten Fliessgeschwindigkeit (ein Wert f�r alle Zellen)
	double ConstVelocity;           // Wenn mit konstanten Fliessgeschwindigkeit, Wert wird aus Befehlszeile entnommen;

	short climate;				    //climate input in 0.5° = 0 ; 5min = 1; 0.5 CRU/WATCH = 2; default 0

	// 0: Wassertemperature wird aus DB eingelesen; 
	// 1: UNF-Dateien fuer Lufttemperatur; bei climate==0 wird G_ELEV_MEAN_CLM.UNF0 nötig sein; bei climate==2 G_ELEV_MEAN_WATCH.UNF0
	bool isWaterTempFromAir;

	// OUTPUT von WaterGAP (hydrology): G_RIVER_VELOCITY_%d.12.UNF0 G_GLO_LAKE_STOR_MEAN_%d.12.UNF0 G_RES_STOR_MEAN_%d.12.UNF0
	char* path_watergap_output;
	
	// GSHORTWAVE_%d.12.UNF2; vor water temperature from air temperature: GTEMP_[year].12.UNF2
	char* path_climate;

	// G_WG3_WG2WITH5MIN.UNF4 G_MEANDERING_RATIO.UNF0; 
	// if water temperature calculated fro air temp and climate in 0.5 or 0.5 CRU/WATCH : G_ELEV_MEAN_CLM.UNF0, G_ELEV_MEAN_WATCH.UNF0
	char* path_hydro_input;
	
	// GCELLDIST.9.UNF0
	char* path_hydro_routing;
	
	short loadingType; // 0: Table load; 1: Table load_[IDScen]_[parameter_id]; 2: aufsummieren aus der Ergebnissen von wq_load

	short init(int optionc, char* optionv[]);
	void Help(int num=1);

private:

};

#endif /* OPTIONS_H_ */
