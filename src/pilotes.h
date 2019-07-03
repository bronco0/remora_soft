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
#define NB_FILS_PILOTES 1

// Le fil pilote est connecté de la façon suivante
#define FP1 0,2

// Creation macro unique et indépendante du type de
// carte pour le controle des I/O
// sur la V1.3 on pilote à l'inverse (met à 0 pour passer)
#define _digitalWrite(p,v) digitalWrite(p,v==0?1:0) //??
#define _pinMode(p,v) pinMode(p,v) //??

// Variables exported to other source file
// ========================================
extern char etatFP[];

// Function exported for other source file
// =======================================
bool pilotes_setup(void);
bool pilotes_loop(void);
void initFP(void);
void conf12(char);
int setfp(String);
int setfp_interne(uint8_t fp, char cOrdre);

#endif // PILOTES_H