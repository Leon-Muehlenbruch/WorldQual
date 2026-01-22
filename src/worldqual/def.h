/**
 * @file def.h
 * @brief Globale Definitionen für Grid-Dimensionen verschiedener Kontinente
 * 
 * Definiert Grid-Parameter (Anzahl der Zellen, Zeilen, Spalten) und
 * Pfade zu GCRC-Dateien für verschiedene WaterGAP-Kontinente.
 * 
 * Nur eine Kontinent-Definition sollte zur Kompilierzeit aktiv sein.
 * Derzeit aktiv: Europa (eu)
 */

/*
// ============================================================================
// WORLD - WaterGAP 2 (0.5° Auflösung)
// ============================================================================
#define ng 66896              ///< Gesamtzahl der Zellen (World WG2)
#define ng_land 66663         ///< Anzahl der Landzellen (World WG2)
#define nrows 360             ///< Anzahl der Zeilen (World WG2)
#define ncols 720             ///< Anzahl der Spalten (World WG2)
#define FILEROWCOL "/grid/watergap/watergap_data/hydro_input/2.1f/INPUT/GCRC.UNF4"

// ============================================================================
// AFRIKA (af)
// ============================================================================
#define ng 371410             ///< Gesamtzahl der Zellen (Afrika)
#define ng_land 370005        ///< Anzahl der Landzellen (Afrika)
#define nrows 1090            ///< Anzahl der Zeilen (Afrika)
#define ncols 1237            ///< Anzahl der Spalten (Afrika)
#define FILEROWCOL "/grid/watergap/watergap_data/hydro_input/3.1/Africa/INPUT/GCRC.UNF4"

// ============================================================================
// ASIEN (as)
// ============================================================================
#define ng 841703             ///< Gesamtzahl der Zellen (Asien)
#define ng_land 839180        ///< Anzahl der Landzellen (Asien)
#define nrows 1258            ///< Anzahl der Zeilen (Asien)
#define ncols 4320            ///< Anzahl der Spalten (Asien)
#define FILEROWCOL "/grid/watergap/watergap_data/hydro_input/3.1/Asia/INPUT/GCRC.UNF4"

// ============================================================================
// AUSTRALIEN (au)
// ============================================================================
#define ng 109084             ///< Gesamtzahl der Zellen (Australien)
#define ng_land 109073        ///< Anzahl der Landzellen (Australien)
#define nrows 740             ///< Anzahl der Zeilen (Australien)
#define ncols 4309            ///< Anzahl der Spalten (Australien)
#define FILEROWCOL "/grid/watergap/watergap_data/hydro_input/3.1/Australia/INPUT/GCRC.UNF4"
*/

// ============================================================================
// EUROPA (eu) - AKTIV
// ============================================================================
/// Gesamtzahl der Zellen (Europa)
#define ng 180721

/// Anzahl der Landzellen (Europa)
#define ng_land 179901

/// Anzahl der Zeilen im Grid (Europa)
#define nrows 641

/// Anzahl der Spalten im Grid (Europa)
#define ncols 1000

/// Pfad zur GCRC.UNF4 Datei (Grid Cell Row Column) für Europa
#define FILEROWCOL "/grid/watergap/watergap_data/hydro_input/3.1/Europe/INPUT/GCRC.UNF4"

/*
// ============================================================================
// NORDAMERIKA (na)
// ============================================================================
#define ng 461694             ///< Gesamtzahl der Zellen (Nordamerika)
#define ng_land 456290        ///< Anzahl der Landzellen (Nordamerika)
#define nrows 915             ///< Anzahl der Zeilen (Nordamerika)
#define ncols 1519            ///< Anzahl der Spalten (Nordamerika)
#define FILEROWCOL "/grid/watergap/watergap_data/hydro_input/3.1/NA/INPUT/GCRC.UNF4"

// ============================================================================
// SÜDAMERIKA (sa)
// ============================================================================
#define ng 226852             ///< Gesamtzahl der Zellen (Südamerika)
#define ng_land 226668        ///< Anzahl der Landzellen (Südamerika)
#define nrows 824             ///< Anzahl der Zeilen (Südamerika)
#define ncols 1356            ///< Anzahl der Spalten (Südamerika)
#define FILEROWCOL "/grid/watergap/watergap_data/hydro_input/3.1/SA/INPUT/GCRC.UNF4"
*/

/**
 * @note Um einen anderen Kontinent zu verwenden:
 * 1. Auskommentieren der aktuellen Definition (Europa)
 * 2. Einkommentieren der gewünschten Kontinent-Definition
 * 3. Neu kompilieren
 * 
 * Die GCRC.UNF4 Dateien enthalten die Grid-Cell-Row-Column Zuordnungen
 * für die jeweiligen Kontinente im WaterGAP 3.1 Format.
 */
