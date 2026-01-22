/**
 * @file river_start_end.h
 * @brief Klasse für Flussabschnitts-Definition (Start- und Endpunkte)
 * @author reder
 * @date 23.08.2012
 * 
 * Diese Klasse verwaltet Informationen über Flussabschnitte,
 * insbesondere Start- und Endpunkte für Modellierungen und
 * statistische Auswertungen.
 */

#include <mysql++.h>
#include <ssqls.h>
#include <mysql.h>
#include <fstream>

using namespace std;
using namespace mysqlpp;

#ifndef RIVER_START_END_H_
#define RIVER_START_END_H_

/// Flag für notwendiges Byte-Swapping bei Binärdateien
#define BYTESWAP_NECESSARY


/**
 * @class river_start_end
 * @brief Verwaltet Start- und Endpunkte von Flussabschnitten
 * 
 * Diese Klasse ist verantwortlich für die Verwaltung von Flussabschnitts-
 * Definitionen, die für wq_stat Berechnungen und Validierung verwendet werden.
 * Sie definiert welche Zellen zu einem Flussabschnitt gehören.
 */
class river_start_end {
public:
	/**
	 * @brief Destruktor
	 */
	virtual ~river_start_end();

};


#endif /* RIVER_START_END_H_ */
