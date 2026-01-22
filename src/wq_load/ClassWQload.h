/**
 * @file ClassWQload.h
 * @brief Hauptklasse für Wasserqualitäts-Belastungsberechnungen
 * @author kynast
 * @date 30.01.2018
 * 
 * Diese Klasse berechnet verschiedene Schadstoff-Belastungen aus punktuellen und
 * diffusen Quellen (Haushalte, Industrie, Landwirtschaft, Bergbau) für
 * weltweite Flusseinzugsgebiete.
 */

#ifndef CLASSWQLOAD_H_
#define CLASSWQLOAD_H_

#include <stdio.h>
#include <mysql++.h>
#include <ssqls.h> //<custom.h>
#include <mysql.h>
#include <math.h>
#include "country.h"
#include "general_functions.h"
#include "options_wq_load.h"
#include "ls.h"
#include "crop.h"
#include "gb.h"

#ifndef DEBUG_queries
#define DEBUG_queries
//#undef DEBUG_queries
#endif

#define NO_MINING

/**
 * @class ClassWQ_load
 * @brief Hauptklasse zur Berechnung von Wasserqualitäts-Belastungen
 * 
 * Berechnet Schadstoffeinträge aus verschiedenen Quellen:
 * - Punktquellen: Haushalte, Industrie, Bergbau
 * - Diffuse Quellen: Landwirtschaft (Dünger, Vieh), urbaner Abfluss
 * - Geogener Hintergrund und atmosphärische Deposition
 * 
 * Unterstützte Parameter:
 * - BOD (Biologischer Sauerstoffbedarf)
 * - Salz (TDS - Total Dissolved Solids)
 * - FC (Fäkale Coliforme)
 * - TN (Gesamtstickstoff)
 * - TP (Gesamtphosphor)
 * - Pestizide
 */
class ClassWQ_load {
public:
	/**
	 * @brief Konstruktor
	 * @param options_ptr Zeiger auf WQ-Load Options-Objekt
	 * @param year Berechnungsjahr
	 */
	ClassWQ_load(options_wq_loadClass* options_ptr, int year);
	
	/**
	 * @brief Destruktor
	 */
	virtual ~ClassWQ_load();

private:
	// ========================================================================
	// Basis-Konfiguration
	// ========================================================================
	
	/**
	 * @brief Berechnungstyp
	 * - 0: BOD
	 * - 1: Salz
	 * - 2: FC (Fäkale Coliforme)
	 * - 3: TN
	 * - 4: TP
	 * - 5: Pestizide
	 */
	int toCalc;
	
	/// Berechnungsjahr
	int year;
	
	/// Kontinent-Informationen (Grid-Dimensionen, etc.)
	continentStruct continent;
	
	/// Zeiger auf Options-Objekt
	options_wq_loadClass* optionsWQ_load_ptr;

	// ========================================================================
	// Hilfsklassen für spezifische Daten
	// ========================================================================
	
	/// Länderinformationen
	COUNTRY_CLASS* countries;
	
	/// Kulturpflanzen-Informationen
	CROP_CLASS* crops;
	
	/// Viehhaltungs-Informationen
	LS_CLASS* LS;
	
	/// Geogener Hintergrund (z.B. Deposition)
	GB_CLASS* GB;

	// ========================================================================
	// Parameter
	// ========================================================================
	
	/// Einsatz anorganischer Dünger pro Land und Kulturtyp [t/km²]
	double ind_fert_use;
	
	/// Exkretionsrate Vieh [t/(a*Tier)], FC: [10^10 cfu/(a*Tier)]
	double ex_ls;
	
	/// Geogene Hintergrund-Rate (Deposition [t/(a*ha)] für TN/TP; Konzentration [t/L] für BOD)
	double geo_back_rate;

	// ========================================================================
	// Länder-Input-Daten
	// ========================================================================
	
	
	/// Bevölkerung [Gesamt, Urban, Rural] pro Land
	double (*pop)[3];
	
	/// Kanalanschluss [Urban, Rural] [Anzahl Personen]
	double (*sewerConn)[2];
	
	/// Prozent der national versorgten Bevölkerung mit sicherer Sanitärentsorgung (JMP)
	double *SPO_treated;
	
	/**
	 * @brief Anschlussraten an Klärstufen [%]
	 * [0]=Keine Behandlung, [1]=Primär, [2]=Sekundär, [3]=Tertiär, [4]=Quartär
	 */
	double (*connectivity)[5];
	
	/// Industrieabwasser-Rückfluss [m³/a]
	double *rtf_man;
	
	/// Häusliches Abwasser-Rückfluss [m³/a]
	double *rtf_dom;

	/**
	 * @brief Anteil des Industrieabwassers nach Sektor [%]
	 * [0]=Nahrung/Getränke/Tabak, [1]=Textilien, [2]=Papier/Druck,
	 * [3]=Chemieindustrie, [4]=Glas/Keramik/Zement, [5]=Metalle, [6]=Sonstige
	 */
	double (*rtf_man_fra)[7];

	/// Bewässerungs-Rückfluss [m³/a]
	double *rtf_irr;

	/**
	 * @brief Entfernungsraten für verschiedene Behandlungsstufen
	 * [0]=Keine Behandlung, [1]=Primär, [2]=Sekundär, [3]=Tertiär, [4]=Quartär,
	 * [5]=Entfernung im Boden, [6]=Reduktionsfaktor organisch aus Dung,
	 * [7]=Reduktionsfaktor anorganisch, [8]=Entfernung Dung in Lagerung,
	 * [9]=Entfernung Dung auf Boden
	 */
	double rem[10];

	/**
	 * @brief Parameter für Phosphor-Berechnung
	 * [0]=Lmax (maximale DP-Belastungsfraktion),
	 * [1]=a (Beziehung Abfluss-DP-Auslaugung),
	 * [2]=b (Beziehung Abfluss-DP-Auslaugung),
	 * [3]=c (PP mit erodiertem Sediment und P-Input)
	 */
	double TPpara[4];

	/// Emissionsfaktor Bevölkerung [kg/Einw./Jahr]; FC: 10^10 cfu/(a*Einw.)
	double *ef;

	/**
	 * @brief Industrieabwasser-Konzentrationen [mg/l]; FC: 10^10 cfu/L
	 * [0]=Nahrung, [1]=Textilien, [2]=Papier, [3]=Chemie,
	 * [4]=Glas/Keramik, [5]=Metalle, [6]=Sonstige
	 */
	double (*conc_man)[7];

	/// TDS-Konzentration im Rückfluss für SEPC-Klassen 1-4 [mg/l]
	double (*conc_tds)[4];
	
	/// Konzentration urbaner Abfluss [mg/l]; FC: cfu/100ml
	double *conc_urb;
	
	/// Geogene Hintergrundkonzentration, Länderwerte [mg/l]
	double *c_geogen;

	/**
	 * @brief TDS-Konzentration Bergbau-Rückfluss [mg/l]
	 * [0]=Kohle, [1]=Aluminium, [2]=Gold, [3]=Kupfer, [4]=Bauxit
	 */
	double (*conc_mining)[5];

	// ========================================================================
	// Zell-Input-Daten
	// ========================================================================
	
	/// Geogene Hintergrundkonzentration, Zellwerte [t/km³]
	double *c_geogen_cell;
	
	/// Bevölkerung pro Zelle [Gesamt, Urban, Rural]
	double (*pop_cell)[3];
	
	/// Industrieabwasser-Rückfluss pro Zelle [m³/a]
	double *rtf_man_cell;

	/**
	 * @brief Bergbau-Rückfluss pro Zelle und Ressource [m³/a]
	 * [0]=Kohle, [1]=Aluminium, [2]=Gold, [3]=Kupfer, [4]=Bauxit
	 */
	double (*rtf_mining_cell)[5];

	/// Bewässerungs-Rückfluss pro Zelle [m³/a]
	double *rtf_irr_cell;
	
	/// BIP pro Kopf [US$]
	double *gdp;
	
	/// Primäre Bodensalinität (1=nein, 2=ja)
	int *salinity;
	
	/// Feuchtigkeit (1=humid, 2=arid)
	int *humidity;
	
	/// Zellfläche [km²]
	double *cell_area;
	
	/// Versiegelte Fläche (built-up fraction) [-]
	double *built_up_frac;
	
	/// Landnutzungsklasse
	short *LU_cell;
	
	/// Mittlere Bodenerosion [kg/km²/a] (z.B. FAO LADA)
	double *soil_erosion;
	
	/// Atmosphärische TP-Depositionsrate [kg/km²/a] (z.B. Mahowald et al. 2008)
	double *ld_Patmdepos_rate;
	
	/// TP chemische Verwitterungsrate [kg/km²/a] (z.B. Hartmann et al. 2014)
	double *ld_Pcweathering_rate;
	
	/// Land-ID (isonum) für Zelle
	int *country_cell;

	/// GCRC-Array (gcrc[gcrc-1]=ArcID)
	int *gcrc;
	
	/// GROW-Array (grow[gcrc-1]=Zeile)
	int *grow;

	/// Zellfläche, Wert für jede Zeile [nrows]
	float *G_area;
	
	/// Anteil der Zelle der Landfläche ist [%]
	char *G_land_area;

	/// Urbaner Abfluss pro Zelle [mm/Monat]
	double (*urban_runoff_cell)[12];
	
	/// Temperatur pro Zelle [100 °C]
	short (*temperature_cell)[12];
	
	/// Mittlerer Jahrestemperaturwert [°C]
	double (*temperature_cell_year);
	
	/// Anzahl Regentage pro Monat und Zelle
	char (*perc_days)[12];
	
	/// Niederschlag pro Zelle [mm]
	short (*precipitation_cell)[12];

	/// Oberflächenabfluss [mm/Monat = L/(m²*Monat)]
	double (*sur_runoff_cell)[12];
	
	/// Mittlerer jährlicher Oberflächenabfluss [mm/a]
	double *sur_runoff_cell_mean;
	
	/// Grundwasser-Zufluss in Flüsse [mm/Monat]
	double (*gw_runoff_cell)[12];
	
	/// Oberflächenabfluss pro Jahr [mm/a]
	double *sur_runoff_cell_year;
	
	/// Anzahl Tiere pro Zelle [Tiere/(Zelle*a)]
	double (*ls_cell)[12];
	
	/// Dominanter Kulturtyp auf Zelle
	short *crop_type;
	
	/// Ackerfläche in der Zelle
	float *cropland_area;
	
	/// Rate P pro Zelle
	float *rate_p_cell;

	// ========================================================================
	// Pestizid-spezifische Parameter
	// ========================================================================
	
	/// Gehalt an organischem Kohlenstoff im Boden [%]
	double *organic_carbon;
	
	/// Masse des applizierten Pestizids [t/Monat]
	double *ld_input_pest;
	
	/// Halbwertszeit des Pestizids im Boden [Monate]
	double DT50soil;
	
	/// Bodenadsorptionskoeffizient [-]
	double koc;
	
	/// Breite von Pufferrandstreifen [m]
	double WBZ;
	
	/// Breite von Pufferrandstreifen, Zellwerte [m]
	double* WBZ_cell;

	/// Landgefälle [%]
	double *g_land_slope;

	// ========================================================================
	// Sanitär-Parameter
	// ========================================================================
	
	/// Prozentsatz nicht funktionierender Kläranlagen pro Land [%]
	double *stp_failure;

	/// Ausfallrate von Kleinkläranlagen (Septic Tanks, etc.) [1]
	double treat_failure;

	/// Prozent der Bevölkerung mit unbekannter Behandlung
	double *connec_treat_unknown;

	/// Prozent der Bevölkerung mit Hängelatrinen
	double *hanging_lat;

	/// Prozent der Bevölkerung mit offener Defäkation
	double *open_defec;

	/// Variable für fehlende Anschlussrate (!=0 wenn Summe !=100%)
	double miss_conn_rate;

	// ========================================================================
	// Länder-Output-Daten (Belastungen)
	// ========================================================================
	
	
	/// Berechnete Anschlussrate [Gesamt, Urban, Rural] [%]
	double (*calc_con)[3];
	
	/**
	 * @brief Häusliche Anschlussraten an Klärstufen [%]
	 * [0]=Keine, [1]=Primär, [2]=Sekundär, [3]=Tertiär, [4]=Quartär
	 */
	double (*frac_dom)[5];
	
	/**
	 * @brief Industrielle Anschlussraten an Klärstufen [%]
	 * [0]=Keine, [1]=Primär, [2]=Sekundär, [3]=Tertiär, [4]=Quartär
	 */
	double (*frac_man)[5];
	
	/**
	 * @brief Anschlussraten Streusiedlungen an Klärstufen [%]
	 * [0]=Keine, [1]=Primär, [2]=Sekundär, [3]=Tertiär, [4]=Quartär
	 */
	double (*frac_sc)[5];

	/// Unbehandelte häusliche Belastung pro Land [t/a]; FC: [10^10 no/a]
	double (*ld_untr_dom);
	
	/// Behandelte häusliche Belastung pro Land [t/a]; FC: [10^10 no/a]
	double (*ld_treat_dom);

	/// Unbehandelte urbane Belastung pro Land [t/Monat]; FC: [10^10 no/a]
	double (*ld_untr_urb);
	
	/// Behandelte urbane Belastung pro Land [t/Monat]; FC: [10^10 no/a]
	double (*ld_treat_urb);

	/// Behandelte direkte industrielle Belastung pro Land [t/a]; FC: [10^10 no/a]
	double (*ldd_treat_man);
	
	/// Unbehandelte indirekte industrielle Belastung pro Land [t/a]; FC: [10^10 no/a]
	double (*ldi_untr_man);
	
	/// Unbehandelte indirekte industrielle Belastung, Sektoren [t/a]; FC: [10^10 no/a]
	double (*ldi_untr_man_fra)[7];
	
	/// Behandelte indirekte industrielle Belastung pro Land [t/a]; FC: [10^10 no/a]
	double (*ldi_treat_man);
	
	/// Behandelte gesamte industrielle Belastung pro Land [t/a]; FC: [10^10 no/a]
	double (*ld_treat_man);

	/// Unbehandelte Belastung Streusiedlungen [t/a]; FC: [10^10 no/a]
	double (*ld_untr_sc);
	
	/// Behandelte Belastung Streusiedlungen (Grubenlatrinen, Septic Tanks, etc.) [t/a]; FC: [10^10 no/a]
	double (*ld_treat_sc);
	
	/// Diffuse Belastung Streusiedlungen (offene Defäkation) [t/a]; FC: [10^10 no/a]
	double (*ld_diff_untr_sc);
	
	/// Belastung durch Hängelatrinen pro Land [t/a]; FC: [10^10 cfu/a]
	double (*ld_hanging_l);

	/// Behandelte Bergbau-Belastung pro Land [t/a]
	double (*ld_treat_mining);
	
	/// Unbehandelte Bergbau-Belastung pro Land [t/a]
	double (*ld_untr_mining);

	/// Belastung durch Bewässerungs-Rückfluss [t/a]
	double (*ld_irr);
	
	/**
	 * @brief Landwirtschaftliche Belastung
	 * Salz: (ld_irr - s_geogen) [t/a]
	 * Pestizide: Belastung aus Landwirtschaft [t/a]
	 */
	double (*ld_agr);

	/// Belastung durch organischen Dünger [t/a]; FC: [10^10 cfu/a]
	double (*ld_manure);
	
	/// Belastung durch geogenen Hintergrund [t/a]
	double (*ld_geog_back);
	
	/// Belastung durch anorganischen Dünger [t/a]
	double (*ld_ind_fert);


	// ========================================================================
	// Zell-Output-Daten (Belastungen)
	// ========================================================================

	/// Unbehandelte häusliche Belastung pro Zelle [t/a]; FC: [10^10 no/a]
	double *ld_untr_dom_cell;
	
	/// Behandelte häusliche Belastung pro Zelle [t/a]; FC: [10^10 no/a]
	double *ld_treat_dom_cell;

	/// Unbehandelte urbane Belastung pro Zelle [t/Monat]; FC: [10^10 no/a]
	double (*ld_untr_urb_cell)[12];
	
	/// Behandelte urbane Belastung pro Zelle [t/Monat]; FC: [10^10 no/a]
	double (*ld_treat_urb_cell)[12];
	
	/// Unbehandelte urbane Belastung pro Zelle [t/a]; FC: [10^10 no/a]
	double *ld_untr_urb_cell_year;
	
	/// Behandelte urbane Belastung pro Zelle [t/a]; FC: [10^10 no/a]
	double *ld_treat_urb_cell_year;

	/// Behandelte direkte industrielle Belastung pro Zelle [t/a]; FC: [10^10 no/a]
	double *ldd_treat_man_cell;
	
	/// Unbehandelte indirekte industrielle Belastung pro Zelle [t/a]; FC: [10^10 no/a]
	double *ldi_untr_man_cell;
	
	/// Behandelte indirekte industrielle Belastung pro Zelle [t/a]; FC: [10^10 no/a]
	double *ldi_treat_man_cell;
	
	/// Behandelte gesamte industrielle Belastung pro Zelle [t/a]; FC: [10^10 no/a]
	double *ld_treat_man_cell;

	/// Unbehandelte Bergbau-Belastung pro Zelle [t/a]
	double *ld_untr_mining_cell;
	
	/// Behandelte Bergbau-Belastung pro Zelle [t/a]
	double *ld_treat_mining_cell;

	/// Unbehandelte Belastung Streusiedlungen pro Zelle [t/a]; FC: [10^10 no/a]
	double *ld_untr_sc_cell;
	
	/// Behandelte Belastung Streusiedlungen pro Zelle [t/a]; FC: [10^10 no/a]
	double *ld_treat_sc_cell;
	
	/// Diffuse Belastung Streusiedlungen pro Zelle [t/a]; FC: [10^10 no/a]
	double *ld_diff_untr_sc_cell;
	
	/// Belastung durch Hängelatrinen pro Zelle [t/a]; FC: [10^10 cfu/a]
	double *ld_hanging_l_cell;

	/// Natürliche Salzklasse 1-4 (niedrig bis hoch)
	int *Salt_class;
	
	/// BIP-Klasse 1-3 (niedrig bis hoch)
	double *GDPC;
	
	/// Salzkonzentration im Rückfluss [mg/l] (abgeleitet aus Salt_class und GDPC)
	int *SEPC;
	
	/// Behandelte Belastung Bewässerungs-Rückfluss pro Zelle [t/a]
	double *ld_irr_cell;

	/**
	 * @brief Landwirtschaftliche Belastung pro Zelle [t/Monat]
	 * Salz: (ld_irr - s_geogen)
	 * Pestizide: Belastung aus Landwirtschaft
	 */
	double (*ld_agr_cell)[12];

	/// Pestizide: Masse des Pestizids im Boden [t/Monat]
	double (*ld_agr_soil_cell)[12];

	/// Landwirtschaftliche Belastung pro Zelle [t/a]
	double *ld_agr_cell_year;

	/// Belastung durch organischen Dünger pro Zelle [t/a]; FC: [10^10 cfu/a]
	double *ld_manure_cell;
	
	/// Belastung durch anorganischen Dünger pro Zelle [t/a]
	double *ld_inorg_fert_cell;

	/// Belastung geogener Hintergrund pro Zelle [t/a]; FC: [10^10 cfu/a]
	double *ld_background_cell_year;
	
	/// Belastung atmosphärische P-Deposition pro Zelle [t/a]
	double *ld_Patmdepos_cell_year;
	
	/// Belastung P-Verwitterung pro Zelle [t/a]
	double *ld_Pcweathering_cell_year;
	
	/// Belastung geogener Hintergrund [t/Monat] (nur für Salz)
	double (*ld_background_cell)[12];

	// ========================================================================
	// Private Methoden - Input
	// ========================================================================
	
	/**
	 * @brief Liest Länder-Input-Daten
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int get_country_input();
	
	/**
	 * @brief Liest allgemeine Parameter-Input-Daten
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int get_parameter_input();
	
	/**
	 * @brief Liest länderspezifische Parameter-Input-Daten
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int get_country_parameter_input();
	
	/**
	 * @brief Liest Zell-Input-Daten
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int get_cell_input();

	/**
	 * @brief Liest zellspezifische Parameter-Input-Daten
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int get_cell_parameter_input();

	/**
	 * @brief Liest Pestizid-Zell-Input-Daten (organischer Kohlenstoff)
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int get_cell_pesticide_input();
	
	/**
	 * @brief Liest Pestizid-Zell-Input für spezifischen Monat
	 * @param month Monat (1-12)
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int get_cell_pesticide_input(int month);
	
	/**
	 * @brief Liest Pestizid-Daten vom Vorjahr
	 * @param month Monat
	 * @param ld_dez Dezember-Belastung vom Vorjahr (Output)
	 * @param tab Tabellenname
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int get_cell_pesticide_lastYear(int month, double* ld_dez, const string tab);
	
	/**
	 * @brief Berechnet landwirtschaftliche Pestizid-Belastung für Zelle
	 * @param cell Zellen-ID
	 * @param month Monat
	 * @param ld_agr_soil_lastMonth Boden-Belastung vom Vormonat
	 * @param ld_agr_lastMonth Gesamtbelastung vom Vormonat
	 */
	void calc_ld_agr_cell(int cell, int month, double ld_agr_soil_lastMonth, double ld_agr_lastMonth);

	// ========================================================================
	// Private Methoden - Berechnungen
	// ========================================================================
	
	/**
	 * @brief Berechnet Länder-Belastungen
	 */
	void CountryLoad();
	
	/**
	 * @brief Berechnet Zell-Belastungen
	 */
	void CellLoad();
	
	/**
	 * @brief Berechnet landwirtschaftliche Belastungen
	 * Berechnet ld_agr_cell, ld_agr_cell_year, ld_agr
	 */
	void LoadAgr();
	
	/**
	 * @brief Gibt Länderwerte auf stdout aus
	 */
	void CountryValuesOutput();

	// ========================================================================
	// Private Methoden - Datenbankoperationen
	// ========================================================================
	
	/**
	 * @brief Fügt Monatswerte in Tabelle ein (mit Korrekturfaktor)
	 * @param year Jahr
	 * @param tab Tabellenname
	 * @param ld_value_year Jahreswerte
	 * @param corr_factor Korrekturfaktoren [Zelle][12 Monate]
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int insert_into_tab(int year, const string tab, double *ld_value_year, double (*corr_factor)[12]);
	
	/**
	 * @brief Fügt Monatswerte in Tabelle ein
	 * @param year Jahr
	 * @param tab Tabellenname
	 * @param ld_value Monatswerte [Zelle][12 Monate]
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int insert_into_tab(int year, const string tab, double (*ld_value)[12]);
	
	/**
	 * @brief Fügt Monatswerte mit Oberflächenabfluss-Verteilung ein
	 * @param year Jahr
	 * @param tab Tabellenname
	 * @param ld_value_year Jahreswerte
	 * @param act_surface_runoff Monatlicher Oberflächenabfluss
	 * @param act_surface_runoff_year Jährlicher Oberflächenabfluss
	 * @param act_surface_runoff_summer Sommer-Oberflächenabfluss
	 * @param manure_timining Dung-Timing Flag
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int insert_into_tab(int year, const string tab, double *ld_value_year,
			double (*act_surface_runoff)[12], double *act_surface_runoff_year, double *act_surface_runoff_summer
			, short manure_timining);
	
	/**
	 * @brief Schreibt Monatswerte in Datenbank
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int monthValues();
	
	/**
	 * @brief Schreibt Jahreswerte in Datenbank
	 * @return 0 bei Erfolg, sonst Fehlercode
	 */
	int yearValues();
	
	/**
	 * @brief Berechnet Regentage für Abbau-Berechnung
	 * @param ld_init Initiale Belastung
	 * @param ks Abbaurate
	 * @param perc_days Regentage [Zelle][12 Monate]
	 * @param cell Zellen-ID
	 * @param year Jahr
	 * @param teta Temperaturkoeffizient
	 * @param temperature_cell Temperatur [Zelle][12 Monate]
	 * @param flag Flag
	 * @return Berechnete Belastung
	 */
	double getRainDays(double ld_init, double ks, char (*perc_days)[12], int cell, int year, const double teta, short (*temperature_cell)[12], short flag);


};

#endif /* CLASSWQLOAD_H_ */
