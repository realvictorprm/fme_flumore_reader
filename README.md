This software is used in the environment department Baden-Württemberg in Germany. Therefore the description is both in English and German. 

## English

This repository contains the complete project for building a FME-Reader for the flood simulation data format `FLUMORE`. 

Requirements:
- FME-64 bit installation _with_ the SDK (Software Development Kit)
- Mono 64-bit installation, at least Version 5.x (won't work with older versions!)
- Visual Studio 2017 with C++ and F# Development Tools. Additional a compatible windows kit must be installed (see below which minimum).
- Minimum Windows 7 is supported

Build steps:
1. Define the environment variable `MONO64`. It must point to the root folder of your local mono installation.
2. Define the environment variable `FME_HOME`. It must point to the root folder of your local 64-bit FME installation.
3. Build the solution in `Release` mode.
4. Ready to use

Distribution:
All files which are copied to your local FME location are also copied into the folder `FLUMORE_Format_Dependencies`.
To distribute the plugin just tell your users to copy the content of this folder into their local FME installation.

Usage:
1. Open the reader dialog
2. Select under additional formats the "FLUMORE" formats
4. Select a FLUMORE compatible file
5. Ready to go

If there are any bugs and or questions, please open issues here. All workflow is supposed to be here.

## Deutsch

Dieses Verzeichnis beinhaltet die Quelldateien samt Erstellsystem des FME-Reader für das Flutsimulationsdaten-Format `FLUMORE`. 

Mindestanforderungen:
- FME 64-Bit Installation mit dem "Software Development Kit" (SDK).
- Mono 64-Bit Installation, mindestens jedoch Version 5. Vorherige Versionen funktionieren *nicht*.
- Visual Studio 2017 mit C++ und F# Entwicklungstools. Zusätzlich muss eine kompatible Windows SDK installiert sein (siehe unten).
- Mindestens unterstützt ist Windows 7. Vorherige Versionen von Windows funktionieren nicht da diese bestimmte Systemaufrufe nicht unterstützen. 

Bauanweisungen:
1. Definiere die Umgebungsvariable `MONO64`. Diese muss zwingend zum Installationsordner von der Mono 64-bit Version zeigen.
2. Definiere die Umgebungsvariable `FME_HOME`. Diese muss zwingend zum Installationsordner von der FME 64-bit Version zeigen.
3. Erstellen Sie die Visual Studio Solution im `Release` Modus.
4. Bereit zur Benutzung, siehe Punkt `Benutzung`

Weitergabe des erstellten "FME-Readers":
Alle Dateien, welche in den FME Ordner kopiert wurden, sind auch vorhanden im Ordner `FLUMORE_Format_Dependencies`.
In diesem werden die benötigten Dateien vor dem Kopiervorgang zwischengespeichert. Deshalb ist nur notwendig Benutzern den Ordner zur Verfügung zu stellen, mit der Anweisung dessen Inhalt in das FME Hauptverzeichnis zu kopieren.

Benutzung:
1. Reader Dialog öffnen
2. Unter weitere Formate das Format "FLUMORE" auswählen.
3. Eine Datei auswählen die mit FLUMORE kompatibel ist.
5. Nun kann der Reader verwendet werden.


Bei Fehlern, Bugs oder Fragen bitte hier auf GitHub `Issues` öffnen.

Licensed under Apache License 2.0 / Lizensiert unter der Apache Lizenz Version 2.0
For more information please look into LICENSE.md 
Copyright (c) 2017 Victor Peter Rouven Müller
