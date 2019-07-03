// **********************************************************************************
// Programmateur Fil Pilote et Suivi Conso
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//           15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266
//
// **********************************************************************************

#ifndef __PILOTES_h__
#define __PILOTES_h__

#include "remora.h"

// Fils pilotes
#define NB_FILS_PILOTES 7

// Les fils pilotes sont connectés de la façon suivante sur l'I/O expander
// # Fil Pilote    MCP IO/Port   Digital Port
// FP1 (a et b) -> GPB6, GPB7  ->  14/15
// FP2 (a et b) -> GPB4, GPB5  ->  12/13
// FP3 (a et b) -> GPB2, GPB3  ->  10/11
// FP4 (a et b) -> GPA1, GPA0  ->  1/0
// FP5 (a et b) -> GPA3, GPA2  ->  3/2
// FP6 (a et b) -> GPA5, GPA4  ->  5/4
// FP7 (a et b) -> GPA7, GPA6  ->  7/6
// Relais       -> GPB1        ->  9
// LED          -> GPB0        ->  8
#define FP1 14,15
#define FP2 12,13
#define FP3 10,11
#define FP4  1,0
#define FP5  3,2
#define FP6  5,4
#define FP7  7,6

#define LED_PIN    8
#define RELAIS_PIN 9

// Carte 1.2
#if defined (REMORA_BOARD_V12)
  // Creation macro unique et indépendante du type de
  // carte pour le controle des I/O
  #define _digitalWrite(p,v)  mcp.digitalWrite(p,v)

// Carte 1.3+
#elif defined (REMORA_BOARD_V13) || defined(REMORA_BOARD_V14) || defined(REMORA_BOARD_V15)
  //#define RELAIS_REVERSE // Decommenter pour inverser le relais (si problème de relais on au lieu de off)

  // Creation macro unique et indépendante du type de
  // carte pour le controle des I/O
  // sur la V1.3 on pilote à l'inverse (met à 0 pour passer)
  #define _digitalWrite(p,v)  mcp.digitalWrite(p,v==0?1:0)
#endif

#define _pinMode(p,v) mcp.pinMode(p,v)

#define FNCT_RELAIS_ARRET 0 // Mode arrêt du relais
#define FNCT_RELAIS_FORCE 1 // Mode marche forcée du relais
#define FNCT_RELAIS_AUTO 2  // Mode auto du relais

// Variables exported to other source file
// ========================================
//extern Adafruit_MCP23017 mcp;
//extern int SortiesFP[];
extern char etatFP[];
//extern char memFP[];
extern uint8_t nivDelest;
extern uint8_t plusAncienneZoneDelestee;
extern unsigned long timerDelestRelest;
extern int etatrelais;
extern int fnctRelais;   // Mode de fonctionnement du relais (0: arrêt, 1: marche forcée, 2: auto)

// Function exported for other source file
// =======================================
#ifdef MOD_ADPS
  void delester1zone(void);
  void relester1zone(void);
  void decalerDelestage(void);
#endif
bool pilotes_setup(void);
bool pilotes_loop(void);
void initFP(void);
void conf12(char);
int setfp(String);
int setfp_interne(uint8_t fp, char cOrdre);
int relais(String);
int fnct_relais(String command);

#endif // PILOTES_H