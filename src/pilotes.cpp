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

#include "pilotes.h"

int SortiesFP[NB_FILS_PILOTES*2] = {FP1};

char etatFP[NB_FILS_PILOTES+1] = "";
char memFP[NB_FILS_PILOTES+1] = "";

Ticker Tick_conf1_up;
Ticker Tick_conf2_up;
Ticker Tick_conf12_5m;

/* ======================================================================
Function: conf12
Purpose : Callback pour les Ticker
           - Tick_conf1_up
           - Tick_conf2_up
           - Tick_conf12_5m
Input   : 0, 1 ou 2
          0 => réinitialisation, fin des 5m
          1 => Confort -1, 3s
          2 => Confort -2, 7s
Output  :
Comments:
====================================================================== */
void conf12(char state)
{
  char cOrdre;

  if (state == '0') {
    // Réinitialisation des Ticker conf1 et conf2
    Tick_conf1_up.detach();
    Tick_conf2_up.detach();
    Tick_conf1_up.attach(297, conf12, '1');
    Tick_conf2_up.attach(293, conf12, '2');

    cOrdre = 'Z';
  }
  else {
    cOrdre = 'O';
  }

  if ( (memFP[0] == state && state != '0') || ((memFP[0] == '1' || memFP[0] == '2') && state == '0') ) {
    Log.verbose(F("conf12 return : %d\r\n"), setfp_interne(1, cOrdre));
  }
}

/* ======================================================================
Function: setfp
Purpose : selectionne le mode d'un des fils pilotes
Input   : commande numéro du fil pilote + commande optionelle
          C=Confort, A=Arrêt, E=Eco, H=Hors gel, 1=Confort-1, 2=Confort-2
          ex: 1A => FP1 Arrêt
              41 => FP4 confort -1 (To DO)
              6C => FP6 confort
              72 => FP7 confort -2 (To DO)
          Si la commande est absente la fonction retourne l'état du FP
          ex: 1 => si état FP1 est "arret" retourne code ASCII du "A" (65)
Output  : 0 ou etat commande, si ok -1 sinon
Comments: exposée par l'API spark donc attaquable par requête HTTP(S)
====================================================================== */
int setfp(String command)
{
  command.trim();
  command.toUpperCase();

  Log.verbose(F("\r\nsetfp="));
  Log.verbose(command.c_str());
  Log.verbose(F("\r\n"));

  int returnValue = -1;

  // curl http://192.168.1.201?setfp=1
  // Vérifier que l'on demande l'état d'un seul fil pilote
  if (command.length() == 1) {
    returnValue = (etatFP[0])  ;
  }
  // curl http://192.168.1.201?setfp=1a
  else if (command.length() == 2) {
    // numéro du fil pilote concerné, avec conversion ASCII > entier
    // la commande est vérifiée dans fpC, pas besoin de traiter ici
    uint8_t fp = command[0]-'0';
    char cOrdre= command[1];

    if ( (fp == 1) &&
        (cOrdre == 'C' || cOrdre == 'E' || cOrdre == 'H' || cOrdre == 'A' || cOrdre == '1' || cOrdre == '2')) {
      memFP[fp-1] = cOrdre; // On mémorise toujours la commande demandée
      char cOrdreEnCours = etatFP[fp-1]; // Quel est l'état actuel du fil pilote?
      if (cOrdreEnCours != 'D')
      {
        // Si un délestage est en cours sur ce fil pilote, on n'exécute pas la commande
        // La commande est néanmoins mémorisé
        // Elle sera appliquée lors du relestage
        returnValue = setfp_interne(fp, cOrdre);
      }
    }
    else {
      // erreur
      Log.error(F("Argument incorrect : %d => %c\r\n"), fp, cOrdre);
    }
  }
  // curl http://192.168.1.201/?fp=CCCCCCC
  else if (command.length() == NB_FILS_PILOTES)
  {
    uint8_t fp;
    char cOrdre;

    returnValue = 0;

    // envoyer les commandes pour tous les fils pilotes
    fp     = 1 ;
    cOrdre = command[0]; // l'index de la chaine commence à 0 donc i-1

    // Si on ne doit pas laisser le fil pilote inchangé
    if (cOrdre != '-' ) {
      if ( (fp == 1)  &&
        (cOrdre == 'C' || cOrdre == 'E' || cOrdre == 'H' || cOrdre == 'A' || cOrdre == '1' || cOrdre == '2')) {
        memFP[fp-1] = cOrdre; // On mémorise toujours la commande demandée
        char cOrdreEnCours = etatFP[fp-1]; // Quel est l'état actuel du fil pilote?
        if (cOrdreEnCours != 'D') {
          // ok ici au cas ou la commande setFP n'est pas bonne
          // on positionne le code de retour à -1 mais on
          // continue le traitement, les suivantes sont
          // peut-être correctes
          if (setfp_interne(fp, cOrdre) == -1) {
            returnValue = -1;
          }
        }
      }
      else {
        // erreur
        Log.error(F("Argument incorrect : %d = %c\r\n"), fp, cOrdre);
      }
    }

    // Feed the dog
    _wdt_feed();
  }
  else {
    returnValue = -1;
  }

  if (returnValue != -1) {
    mqttFpPublish();
  }

  return(returnValue);
}

/* ======================================================================
Function: setfp_interne
Purpose : selectionne le mode d'un des fils pilotes
Input   : numéro du fil pilote (1 à NB_FILS_PILOTE)
          ordre à appliquer
          C=Confort, A=Arrêt, E=Eco, H=Hors gel, 1=Conf-1, 2=Conf-2, D=Délestage ( O & Z pour la commande confort -1 et -2 )
          ex: 1,'A' => FP1 Arrêt
              4,'1' => FP4 conf -1
              6,'C' => FP6 confort
              7,'2' => FP7 conf -2
              5,'D' => FP5 délestage (=> hors-gel et blocage des nouvelles commandes)
Output  : 0 si ok -1 sinon
Comments: non exposée par l'API spark car on y gère le délestage
====================================================================== */
int setfp_interne(uint8_t fp, char cOrdre)
{
  // Vérifier que le numéro du fil pilote ne dépasse le MAX et
  // que la commande est correcte
  // 'D' correspond à délestage

  Log.verbose(F("setfp_interne : fp=%d ; cOrdre=%c\r\n"), fp, cOrdre);

  if ( (fp != 1) ||
      (cOrdre!='C' && cOrdre!='1' && cOrdre!='2' && cOrdre!='E' && cOrdre!='H' && cOrdre!='A' && cOrdre!='D' && cOrdre!='O' && cOrdre!='Z') ) {
      // erreur
      return (-1);
  }
  // Ok ici tout est correct
  else {
    // Commande à passer
    uint8_t fpcmd1 = 0, fpcmd2 = 0;

    // tableau d'index de 0 à 6 pas de 1 à 7
    // on en profite pour Sauver l'état sauf si les conmmandes sont O et Z
    if (cOrdre != 'O' && cOrdre != 'Z') {
      etatFP[fp-1]=cOrdre;
    }

    Log.verbose(F("etatFP="));
    Log.verbose(etatFP);
    Log.verbose("\r\n");

    switch (cOrdre)
    {
        // Confort => Commande 0/0
        case 'C': fpcmd1 = LOW;  fpcmd2 = LOW;  break;
        // Confort - 1 au changemant d'état du fp
        case '1': fpcmd1 = LOW;  fpcmd2 = LOW;  break;
        // Confort - 2 au changemant d'état du fp
        case '2': fpcmd1 = LOW;  fpcmd2 = LOW;  break;
        // Eco => Commande 1/1
        case 'E': fpcmd1 = HIGH; fpcmd2 = HIGH; break;
        // Hors gel => Commande 1/0
        case 'H': fpcmd1 = HIGH; fpcmd2 = LOW;  break;
        // Arrêt => Commande 0/1
        case 'A': fpcmd1 = LOW;  fpcmd2 = HIGH; break;
         // Délestage => Hors gel => Commande 1/0
        case 'D': fpcmd1 = HIGH; fpcmd2 = LOW;  break;
        // Pleine alternance pendant 3 ou 7 seconde
        case 'Z': fpcmd1 = LOW;  fpcmd2 = LOW;  break;
        // Pas de signale pendant 297 ou 293 seconde
        case 'O': fpcmd1 = HIGH; fpcmd2 = HIGH; break;
    }

    // On positionne les sorties physiquement
    _digitalWrite(SortiesFP[fp-1], fpcmd1);
    _digitalWrite(SortiesFP[fp], fpcmd2);
    return (0);
  }
}

/* ======================================================================
Function: initFP
Purpose : met tous les fils pilotes en mode hors-gel
Input   : -
Output  : -
Comments: -
====================================================================== */
void initFP(void)
{
  Tick_conf1_up.attach(297, conf12, '1');
  Tick_conf2_up.attach(293, conf12, '2');
  Tick_conf12_5m.attach(300, conf12, '0');

  _pinMode(SortiesFP[0], OUTPUT);
  _pinMode(SortiesFP[1], OUTPUT);

  // On positionne le FP en Hors-Gel
  setfp("1H");

  // Feed the dog
  _wdt_feed();
}