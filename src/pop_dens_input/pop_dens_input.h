/**
 * @file pop_dens_input.h
 * @brief Klasse für Bevölkerungsdichte-Input-Verarbeitung
 * @author reder
 * @date 23.01.2013
 * 
 * Diese Klasse verarbeitet Bevölkerungsdichte-Daten für die räumliche
 * Verteilung von Bevölkerung auf das Modell-Grid.
 */

#ifndef POP_DENS_INPUT_H_
#define POP_DENS_INPUT_H_

/**
 * @class pop_dens_input
 * @brief Verarbeitet Bevölkerungsdichte-Eingabedaten
 * 
 * Diese Klasse ist verantwortlich für das Einlesen und Verarbeiten
 * von Bevölkerungsdichte-Daten, die für punktuelle Belastungsberechnungen
 * (häusliche und urbane Abwässer) benötigt werden.
 */
class pop_dens_input {
public:
	/**
	 * @brief Konstruktor
	 */
	pop_dens_input();
	
	/**
	 * @brief Destruktor
	 */
	virtual ~pop_dens_input();
};

#endif /* POP_DENS_INPUT_H_ */
