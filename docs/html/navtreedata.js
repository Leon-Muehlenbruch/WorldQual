/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "WorldQual", "index.html", [
    [ "WorldQual Übersicht", "index.html", null ],
    [ "Installation", "tutorial_installation.html", [
      [ "WorldQual Installation", "tutorial_installation.html#autotoc_md11", [
        [ "Schritt 1: Repository klonen", "tutorial_installation.html#autotoc_md13", null ],
        [ "Schritt 2: Dependencies installieren", "tutorial_installation.html#autotoc_md15", null ],
        [ "Schritt 3: Kompilieren", "tutorial_installation.html#autotoc_md17", null ],
        [ "Schritt 4: Ausführen", "tutorial_installation.html#autotoc_md19", null ],
        [ "Fertig! 🎉", "tutorial_installation.html#autotoc_md21", null ],
        [ "Datenbank einrichten (Optional)", "tutorial_installation.html#autotoc_md23", [
          [ "Datenbank erstellen:", "tutorial_installation.html#autotoc_md24", null ],
          [ "Verbindung konfigurieren:", "tutorial_installation.html#autotoc_md25", null ]
        ] ],
        [ "Weitere Module kompilieren", "tutorial_installation.html#autotoc_md27", [
          [ "Nährstoffeinträge (wq_load):", "tutorial_installation.html#autotoc_md28", null ],
          [ "Statistik (wq_stat):", "tutorial_installation.html#autotoc_md29", null ],
          [ "Visualisierung (MapRiverQuality):", "tutorial_installation.html#autotoc_md30", null ],
          [ "Wassertemperatur:", "tutorial_installation.html#autotoc_md31", null ]
        ] ],
        [ "Troubleshooting", "tutorial_installation.html#autotoc_md33", [
          [ "Problem: \"brew: command not found\"", "tutorial_installation.html#autotoc_md34", null ],
          [ "Problem: \"make: command not found\"", "tutorial_installation.html#autotoc_md35", null ],
          [ "Problem: \"mysql++.h not found\"", "tutorial_installation.html#autotoc_md36", null ],
          [ "Problem: Datenbankverbindung fehlgeschlagen", "tutorial_installation.html#autotoc_md37", null ]
        ] ],
        [ "System-Anforderungen", "tutorial_installation.html#autotoc_md39", [
          [ "Unterstützte Systeme:", "tutorial_installation.html#autotoc_md40", null ],
          [ "Erforderlich:", "tutorial_installation.html#autotoc_md41", null ],
          [ "Windows-Benutzer:", "tutorial_installation.html#autotoc_md42", null ],
          [ "Empfohlen:", "tutorial_installation.html#autotoc_md43", null ]
        ] ],
        [ "Schnellstart-Zusammenfassung", "tutorial_installation.html#autotoc_md45", null ],
        [ "Weitere Hilfe", "tutorial_installation.html#autotoc_md47", null ]
      ] ]
    ] ],
    [ "Konfiguration &amp; Daten-Setup", "tutorial_konfiguration.html", [
      [ "WorldQual Konfiguration &amp; Daten-Setup", "tutorial_konfiguration.html#autotoc_md48", [
        [ "Überblick", "tutorial_konfiguration.html#autotoc_md50", null ],
        [ "Teil 1: OPTIONS.DAT Konfiguration", "tutorial_konfiguration.html#autotoc_md52", [
          [ "Speicherort", "tutorial_konfiguration.html#autotoc_md53", null ],
          [ "Wichtigste Parameter", "tutorial_konfiguration.html#autotoc_md54", null ],
          [ "Beispiel-Konfiguration", "tutorial_konfiguration.html#autotoc_md55", null ]
        ] ],
        [ "Teil 2: MySQL-Datenbank Setup", "tutorial_konfiguration.html#autotoc_md57", [
          [ "Schritt 1: Schema installieren", "tutorial_konfiguration.html#autotoc_md58", null ],
          [ "Schritt 2: User einrichten", "tutorial_konfiguration.html#autotoc_md59", null ],
          [ "Schritt 3: Test-Daten einfügen", "tutorial_konfiguration.html#autotoc_md60", null ],
          [ "Datenbank-Übersicht", "tutorial_konfiguration.html#autotoc_md61", null ]
        ] ],
        [ "Teil 3: Input-Daten - UNF-Dateien", "tutorial_konfiguration.html#autotoc_md63", [
          [ "Was sind UNF-Dateien?", "tutorial_konfiguration.html#autotoc_md64", null ],
          [ "Benötigte UNF-Dateien (nach Pfad)", "tutorial_konfiguration.html#autotoc_md65", [
            [ "A. path_watergap_output (Zeitreihen - PRO JAHR!)", "tutorial_konfiguration.html#autotoc_md66", null ],
            [ "B. path_climate (Zeitreihen - PRO JAHR!)", "tutorial_konfiguration.html#autotoc_md67", null ],
            [ "C. path_hydro_input (Statisch - EINMALIG!)", "tutorial_konfiguration.html#autotoc_md68", null ],
            [ "D. path_hydro_routing (Statisch - EINMALIG!)", "tutorial_konfiguration.html#autotoc_md69", null ]
          ] ],
          [ "Gesamtgröße für 10-Jahres-Simulation", "tutorial_konfiguration.html#autotoc_md70", null ]
        ] ],
        [ "Teil 4: Daten beschaffen", "tutorial_konfiguration.html#autotoc_md72", [
          [ "Option A: ISIMIP Portal (Empfohlen, KOSTENLOS)", "tutorial_konfiguration.html#autotoc_md73", null ],
          [ "Option B: Direkter Kontakt WaterGAP-Gruppe", "tutorial_konfiguration.html#autotoc_md74", null ],
          [ "Option C: Test-Daten generieren (Schnellstart)", "tutorial_konfiguration.html#autotoc_md75", null ]
        ] ],
        [ "Teil 5: Kompletter Workflow", "tutorial_konfiguration.html#autotoc_md77", [
          [ "Schritt-für-Schritt von 0 zu ersten Ergebnissen", "tutorial_konfiguration.html#autotoc_md78", null ]
        ] ],
        [ "Teil 6: Troubleshooting", "tutorial_konfiguration.html#autotoc_md80", [
          [ "Problem: \"Database connection failed\"", "tutorial_konfiguration.html#autotoc_md81", null ],
          [ "Problem: \"UNF file not found\"", "tutorial_konfiguration.html#autotoc_md82", null ],
          [ "Problem: \"Segmentation fault\"", "tutorial_konfiguration.html#autotoc_md83", null ]
        ] ],
        [ "Teil 7: Daten-Checkliste", "tutorial_konfiguration.html#autotoc_md85", [
          [ "Vor dem ersten Lauf prüfen:", "tutorial_konfiguration.html#autotoc_md86", null ]
        ] ],
        [ "Zusammenfassung", "tutorial_konfiguration.html#autotoc_md88", null ]
      ] ]
    ] ],
    [ "Klassen", "annotated.html", [
      [ "Auflistung der Klassen", "annotated.html", "annotated_dup" ],
      [ "Klassen-Verzeichnis", "classes.html", null ],
      [ "Klassenhierarchie", "hierarchy.html", "hierarchy" ],
      [ "Klassen-Elemente", "functions.html", [
        [ "Alle", "functions.html", "functions_dup" ],
        [ "Funktionen", "functions_func.html", null ],
        [ "Variablen", "functions_vars.html", "functions_vars" ]
      ] ]
    ] ],
    [ "Dateien", "files.html", [
      [ "Auflistung der Dateien", "files.html", "files_dup" ],
      [ "Datei-Elemente", "globals.html", [
        [ "Alle", "globals.html", "globals_dup" ],
        [ "Funktionen", "globals_func.html", null ],
        [ "Variablen", "globals_vars.html", null ],
        [ "Makrodefinitionen", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_class_w_qload_8cpp.html",
"class_l_s___c_l_a_s_s.html#a48aa9326ec724fcec43f8b08fadb7d43",
"copy__scen_8cpp.html#adc58c9862730d1b6c6bc472aaa13f6a7",
"map_river_quality_8cpp.html#ae2b9d5fbeb940658c8ab8ec58f40bd62",
"wq__stat_8cpp.html#a342be6ae3e892ad1029493eb8ef0c7fd"
];

var SYNCONMSG = 'Klicken um Panelsynchronisation auszuschalten';
var SYNCOFFMSG = 'Klicken um Panelsynchronisation einzuschalten';
var LISTOFALLMEMBERS = 'Aufstellung aller Elemente';