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

#if (NB_FILS_PILOTES == 7)
  int SortiesFP[NB_FILS_PILOTES*2] = {FP1, FP2, FP3, FP4, FP5, FP6, FP7};
#elif (NB_FILS_PILOTES == 6)
  int SortiesFP[NB_FILS_PILOTES*2] = {FP1, FP2, FP3, FP4, FP5, FP6};
#else
  #error "Définition du nombre de fils pilotes inccorect"
#endif

char etatFP[NB_FILS_PILOTES+1] = "";
char memFP[NB_FILS_PILOTES+1] = ""; //Commandes des fils pilotes mémorisées (utile pour le délestage/relestage)
int etatrelais = 0; // Etat du relais
int fnctRelais = 2; // Mode de fonctionnement du relais
#ifdef MOD_ADPS
  uint8_t nivDelest = 0; // Niveau de délestage actuel (par défaut = 0 pas de délestage)
  uint8_t plusAncienneZoneDelestee = 1; // Numéro de la zone qui est délestée depuis le plus de temps (entre 1 et nombre de zones)
  unsigned long timerDelestRelest = 0; // Timer de délestage/relestage
#endif

// Instanciation de l'I/O expander
Adafruit_MCP23017 mcp;

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

  for (uint8_t fp=1; fp<=NB_FILS_PILOTES; fp+=1) {
    if ( (memFP[fp-1] == state && state != '0') || ((memFP[fp-1] == '1' || memFP[fp-1] == '2') && state == '0') ) {
      Log.verbose(F("conf12 return : %d\r\n"), setfp_interne(fp, cOrdre));
    }
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

  Log.verbose(F("setfp="));
  Log.verbose(command.c_str());
  Log.verbose("\r\n");

  int returnValue = -1;

  // curl http://192.168.1.201?setfp=1
  // Vérifier que l'on demande l'état d'un seul fil pilote
  if (command.length() == 1) {
    uint8_t fp = command[0]-'0';

    // demande valide
    // retourner l'état du fil pilote (char)
    if (fp >= 1 && fp <= NB_FILS_PILOTES) {
      returnValue = (etatFP[fp-1])  ;
    }
  }
  // curl http://192.168.1.201?setfp=1a
  else if (command.length() == 2) {
    // numéro du fil pilote concerné, avec conversion ASCII > entier
    // la commande est vérifiée dans fpC, pas besoin de traiter ici
    uint8_t fp = command[0]-'0';
    char cOrdre= command[1];

    if ((fp > 0 || fp <= NB_FILS_PILOTES) ||
        (cOrdre == 'C' && cOrdre == 'E' && cOrdre == 'H' && cOrdre == 'A' && cOrdre == '1' && cOrdre == '2')) {
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
      Log.error(F("Argument incorrect : %c\r\n"), cOrdre);
    }
  }
  // curl http://192.168.1.201/?fp=CCCCCCC
  else if (command.length() == NB_FILS_PILOTES)
  {
    uint8_t fp;
    char cOrdre;

    returnValue = 0;

    // envoyer les commandes pour tous les fils pilotes
    for (uint8_t i=1; i<=NB_FILS_PILOTES; i++) {
      fp     = i ;
      cOrdre = command[i-1]; // l'index de la chaine commence à 0 donc i-1

      // Si on ne doit pas laisser le fil pilote inchangé
      if (cOrdre != '-' ) {
        if ( (fp > 0 || fp <= NB_FILS_PILOTES) ||
          (cOrdre == 'C' && cOrdre == 'E' && cOrdre == 'H' && cOrdre == 'A' && cOrdre == '1' && cOrdre == '2')) {
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
          Log.error(F("Argument incorrect : %c\r\n"), cOrdre);
        }
      }

      // Feed the dog
      _wdt_feed();
    }
  }
  else {
    returnValue = -1;
  }

  #ifdef MOD_MQTT
    mqttFpPublish();
  #endif

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

  if ( (fp < 1 || fp > NB_FILS_PILOTES) ||
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
    _digitalWrite(SortiesFP[2*(fp-1)], fpcmd1);
    _digitalWrite(SortiesFP[2*(fp-1)+1], fpcmd2);
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

  // buffer contenant la commande à passer à setFP
  char cmd[] = "xH" ;

  // On positionne tous les FP en Hors-Gel
  for (uint8_t i=1; i<=NB_FILS_PILOTES; i+=1)
  {
    cmd[0]='0' + i;
    setfp(cmd);

    // Feed the dog
    _wdt_feed();
  }
}

/* ======================================================================
Function: delester1zone
Purpose : déleste une zone de plus
Input   : variables globales nivDelest et plusAncienneZoneDelestee
Output  : màj variable globale nivDelest
Comments: -
====================================================================== */
#ifdef MOD_ADPS
void delester1zone(void)
{
  uint8_t numFp; // numéro du fil pilote à délester

  Log.verbose(F("delester1zone() : avant : nivDelest=%d ; plusAncienneZoneDelestee=%d\r\n")
               , nivDelest
               , plusAncienneZoneDelestee
              );

  if (nivDelest < NB_FILS_PILOTES) // On s'assure que l'on n'est pas au niveau max
  {
    nivDelest++;
    numFp = ((plusAncienneZoneDelestee-1 + nivDelest-1) % NB_FILS_PILOTES)+1;
    setfp_interne(numFp, 'D');
  }

  Log.verbose(F("delester1zone() : apres : nivDelest=%d ; plusAncienneZoneDelestee=%d\r\n")
               , nivDelest
               , plusAncienneZoneDelestee
              );
}
#endif

/* ======================================================================
Function: relester1zone
Purpose : retire le délestage d'une zone
Input   : variables globales nivDelest et plusAncienneZoneDelestee
Output  : màj variable globale nivDelest et plusAncienneZoneDelestee
Comments: -
====================================================================== */
#ifdef MOD_ADPS
void relester1zone(void)
{
  uint8_t numFp; // numéro du fil pilote à passer HORS-GEL

  Log.verbose(F("delester1zone() : avant : nivDelest=%d ; plusAncienneZoneDelestee=%d\r\n")
               , nivDelest
               , plusAncienneZoneDelestee
              );

  if (nivDelest > 0) // On s'assure qu'un délestage est en cours
  {
    nivDelest--;
    numFp = plusAncienneZoneDelestee;
    char cOrdreMemorise = memFP[numFp-1]; //On récupére la dernière valeur de commande pour cette zone
    setfp_interne(numFp,cOrdreMemorise);
    plusAncienneZoneDelestee = (plusAncienneZoneDelestee % NB_FILS_PILOTES) + 1;
  }

  Log.verbose(F("delester1zone() : apres : nivDelest=%d ; plusAncienneZoneDelestee=%d\r\n")
               , nivDelest
               , plusAncienneZoneDelestee
              );
}
#endif

/* ======================================================================
Function: decalerDelestage
Purpose : fait tourner la ou les zones délestées
Input   : variables globales nivDelest et plusAncienneZoneDelestee
Output  : màj variable globale plusAncienneZoneDelestee
Comments: -
====================================================================== */
#ifdef MOD_ADPS
void decalerDelestage(void)
{
  Log.verbose(F("delester1zone() : avant : nivDelest=%d ; plusAncienneZoneDelestee=%d\r\n")
               , nivDelest
               , plusAncienneZoneDelestee
              );

  if (nivDelest > 0 && nivDelest < NB_FILS_PILOTES)
  // On ne peut pas faire tourner les zones délestées s'il n'y en a aucune en cours
  // de délestage, ou si elles le sont toutes
  {
    relester1zone();
    delester1zone();
  }

  Log.verbose(F("delester1zone() : apres : nivDelest=%d ; plusAncienneZoneDelestee=%d\r\n")
               , nivDelest
               , plusAncienneZoneDelestee
              );
}
#endif

/* ======================================================================
Function: relais
Purpose : selectionne l'état du relais
Input   : état du relais (0 ouvert, 1 fermé)
Output  : etat du relais (0 ou 1)
Comments: exposée par l'API spark donc attaquable par requête HTTP(S)
====================================================================== */
int relais(String command)
{
  command.trim();
  uint8_t cmd = command.toInt();

  Log.verbose(F("relais="));
  Log.verbose(command.c_str());
  Log.verbose("\r\n");

  // Vérifier que l'on a la commande d'un seul caractère
  if (command.length()!=1 || cmd < 0 || cmd > 1)
    return (-1);

  // Conversion en 0,1 numerique
  etatrelais = cmd;
  int etatRelaisPin = cmd;

  // Inverse etat pin relais si definit dans remora.h
  #ifdef RELAIS_REVERSE
    etatRelaisPin = !etatRelaisPin;
  #endif

    // Allumer/Etteindre le relais et la LED
  #ifdef RELAIS_PIN
    _digitalWrite(RELAIS_PIN, etatRelaisPin);
  #endif
  #ifdef LED_PIN
    _digitalWrite(LED_PIN, etatrelais);
  #endif

  #ifdef MOD_MQTT
    mqttRelaisPublish();
  #endif

  return (etatrelais);
}

/* ======================================================================
Function: relais
Purpose : selectionne l'état du relais
Input   : état du relais (0 ouvert, 1 fermé)
Output  : etat du relais (0 ou 1)
Comments: exposée par l'API spark donc attaquable par requête HTTP(S)
====================================================================== */
int fnct_relais(String command)
{
  command.trim();
  uint8_t cmd = command.toInt();

  Log.verbose(F("fnct_relais="));
  Log.verbose(command.c_str());
  Log.verbose("\r\n");
  Log.verbose(F("command length=%d\ncmd: %d\r\n"), command.length(), cmd);

  // Vérifier que l'on a la commande d'un seul caractère
  if (command.length() != 1 || cmd < 0 || cmd > 2)
    return (-1);

  // On si il y a modification du fonctionnement du relais
  if (fnctRelais != cmd) {
    // Conversion en 0,1,2 numerique
    fnctRelais = cmd;
    // Si le mode est différent du mode auto, on applique la commande
    if (fnctRelais < FNCT_RELAIS_AUTO) {
      relais(command);
    }
    #ifdef MOD_TELEINFO
      else {
        ValueList * me = tinfo.getList();
        bool found = false;   // flag for PTEC found
        // Got at least one ?
        if (me) {
          // Loop thru the node
          while (me->next) {
            // go to next node
            me = me->next;
            // we're there
            _wdt_feed();
            //DebugF("me->name: "); Debug(me->name); DebugF(" - value: "); Debugln(me->value);
            // Check PTEC label
            if (me->name && !strcmp(me->name, "PTEC")) {
              //DebuglnF("PTEC found");
              // If "heures creuses", close relay
              if (me->value && !strncmp(me->value, "HC..", 2)) {
                //DebuglnF("PTEC == HC..");
                relais("1");
              } else {
                relais("0");
              }
              found = true;
              break;
            }
          }
          // If PTEC not found, close relay
          if (!found) {
            relais("0");
          }
        }
      }
    #endif
  }
  return (fnctRelais);
}

/* ======================================================================
Function: pilotes_Setup
Purpose : prepare and init stuff, configuration, ..
Input   : -
Output  : true if MCP23017 module found, false otherwise
Comments: -
====================================================================== */
bool pilotes_setup(void)
{
  Log.notice(F("Initializing MCP23017...Searching..."));

  // Détection du MCP23017
  if (!i2c_detect(MCP23017_ADDRESS))
  {
    Log.error(F("Not found!\r\n"));
    return (false);
  }
  else
  {
    Log.verbose(F("Setup..."));

    // et l'initialiser
    mcp.begin();
    // Mettre les 16 I/O PIN en sortie
    mcp.writeRegister(MCP23017_IODIRA,0x00);
    mcp.writeRegister(MCP23017_IODIRB,0x00);
    Log.verbose(F("OK!\r\n"));
  }

  // on l'a trouvé
  return (true);
}
