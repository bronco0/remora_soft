// **********************************************************************************
// Teleinfo management file for remora project
// **********************************************************************************
// Copyright (C) 2014 Thibault Ducret
// Licence MIT
//
// History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
//                      Intégration de version 1.2 de la carte electronique
//           13/04/2015 Theju
//                      Modification des variables cloud teleinfo
//                      (passage en 1 seul appel) et liberation de variables
//           15/09/2015 Charles-Henri Hallard Utilisation Librairie Teleinfo Universelle
// **********************************************************************************

#include "tinfo.h"

#ifdef MOD_TELEINFO

// Instanciation de l'objet Téléinfo
TInfo tinfo;


uint mypApp           = 0;
uint myiInst          = 0;
uint myindexHC        = 0;
uint myindexHP        = 0;
//uint myimax           = 0;
uint myisousc         = ISOUSCRITE; // pour calculer la limite de délestage
char myOptarif[5]     = "";
//char myPeriode[8]     = "";
//char mytinfo[250]     = "";
//char mycompteur[64]   = "";
float ratio_delestage = DELESTAGE_RATIO;
float ratio_relestage = RELESTAGE_RATIO;
float myDelestLimit   = 0.0;
float myRelestLimit   = 0.0;
int lastPtec          = PTEC_HP;
unsigned long tinfo_led_timer = 0; // Led blink timer
unsigned long tinfo_last_frame = 0; // dernière fois qu'on a recu une trame valide

ptec_e ptec; // Puissance tarifaire en cours

/* ======================================================================
Function: ADPSCallback
Purpose : called by library when we detected a ADPS on any phase
Input   : phase number
            0 for ADPS (monophase)
            1 for ADIR1 triphase
            2 for ADIR2 triphase
            3 for ADIR3 triphase
Output  : -
Comments: should have been initialised in the main sketch with a
          tinfo.attachADPSCallback(ADPSCallback())
====================================================================== */
#ifdef MOD_ADPS
void ADPSCallback(uint8_t phase)
{
  // Led Rouge
  LedRGBON(COLOR_RED);
  tinfo_led_timer = millis();

  // Monophasé

  if (phase == 0 ) {
    Log.verbose(F("ADPS\r\n"));
  } else {
    Log.verbose(F("ADPS Phase %d\r\n"), phase);
  }

  // nous avons une téléinfo fonctionelle
  status |= STATUS_TINFO;
  tinfo_last_frame = millis();
}
#endif

/* ======================================================================
Function: DataCallback
Purpose : callback when we detected new or modified data received
Input   : linked list pointer on the concerned data
          value current state being TINFO_VALUE_ADDED/TINFO_VALUE_UPDATED
Output  : -
Comments: -
====================================================================== */
void DataCallback(ValueList * me, uint8_t flags)
{
  // Do whatever you want there
  Log.verbose(me->name);
  Log.verbose("=");
  Log.verbose(me->value);

  //Debug(" Flags=0x");
  //DEBUG_SERIAL.print(flags, HEX);

  if ( flags & TINFO_FLAGS_NOTHING ) Log.verbose(F(" Nothing"));
  if ( flags & TINFO_FLAGS_ADDED )   Log.verbose(F(" Added"));
  if ( flags & TINFO_FLAGS_UPDATED ) Log.verbose(F(" Updated"));
  if ( flags & TINFO_FLAGS_EXIST )   Log.verbose(F(" Exist"));
  if ( flags & TINFO_FLAGS_ALERT )   Log.verbose(F(" Alert"));
  Log.verbose("\r\n");

  // Nous venons de recevoir la puissance tarifaire en cours
  // To DO : gérer les autres types de contrat
  if (!strcmp_P(me->name, PSTR("PTEC"))) {
    // Récupération de la période tarifaire en cours
    //strncpy(myPeriode, me->value, strlen(me->value));

    // Determination de la puissance tarifaire en cours
    // To DO : gérer les autres types de contrat
    if (!strncmp_P(me->value, PSTR("HP.."), 2)) ptec = PTEC_HP;  // Comparaison sur les 2 premiers caratère pour être compatible
    if (!strncmp_P(me->value, PSTR("HC.."), 2)) ptec = PTEC_HC;  // avec les options tarifaire HC et Tempo
    if (!strcmp_P(me->value, PSTR("TH..")))     ptec = PTEC_HP;

    //=============================================================
    //    Ajout de la gestion du relais aux heures creuses
    //=============================================================
    if (fnctRelais == FNCT_RELAIS_AUTO && lastPtec != ptec) {
      //Debug("PTEC: ");
      if (ptec == PTEC_HC) {
        //Debugln(" HC");
        relais("1");
      } else {
        //Debugln(" HP");
        relais("0");
      }
      lastPtec = (int)ptec;
    }
  }

  // Mise à jour des variables
  if (!strcmp_P(me->name, PSTR("OPTARIF"))) strcpy(myOptarif, me->value);

  if (!strcmp_P(me->name, PSTR("PAPP")))    mypApp     = atoi(me->value);
  if (!strcmp_P(me->name, PSTR("IINST")))   myiInst    = atoi(me->value);

  // Tarif base
  if (!strcmp_P(me->name, PSTR("BASE")))   { myindexHP = atol(me->value); myindexHC = 0; }
  // Tarif HP/HC
  if (!strcmp_P(me->name, PSTR("HCHC")))    myindexHC  = atol(me->value);
  if (!strcmp_P(me->name, PSTR("HCHP")))    myindexHP  = atol(me->value);
  // Tarif Tempo
  if (!strcmp_P(me->name, PSTR("BBRHCJB"))) myindexHC  = atol(me->value);
  if (!strcmp_P(me->name, PSTR("BBRHPJB"))) myindexHP  = atol(me->value);
  if (!strcmp_P(me->name, PSTR("BBRHCJW"))) myindexHC  = atol(me->value);
  if (!strcmp_P(me->name, PSTR("BBRHPJW"))) myindexHP  = atol(me->value);
  if (!strcmp_P(me->name, PSTR("BBRHCJR"))) myindexHC  = atol(me->value);
  if (!strcmp_P(me->name, PSTR("BBRHPJR"))) myindexHP  = atol(me->value);

  // Isousc permet de connaitre l'intensité max pour le delestage
  if (!strcmp_P(me->name, PSTR("ISOUSC"))) {
    myisousc = atoi(me->value);
    myDelestLimit = ratio_delestage * myisousc;
    // Calcul de quand on déclenchera le relestage
    myRelestLimit = ratio_relestage * myisousc;

    // Maintenant on connait notre contrat, on peut commencer
    // A traiter le delestage eventuel et si celui-ci
    // n'a jamais été initialisé on le fait maintenant
    #ifdef MOD_ADPS
    if ( timerDelestRelest == 0 ) {
      timerDelestRelest = millis();
    }
    #endif
  }

  // nous avons une téléinfo fonctionelle
  status |= STATUS_TINFO;
}

/* ======================================================================
Function: NewFrame
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : -
Comments: -
====================================================================== */
void NewFrame(ValueList * me)
{
  // Light the RGB LED
  LedRGBON(COLOR_GREEN);
  tinfo_led_timer = millis();

  //char buff[32];
  //sprintf( buff, "New Frame (%ld Bytes free)", ESP.getFreeHeap() );
  //Debugln(buff);

  // Ok nous avons une téléinfo fonctionelle
  status |= STATUS_TINFO;
  tinfo_last_frame = millis();
}

/* ======================================================================
Function: NewFrame
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : -
Comments: it's called only if one data in the frame is different than
          the previous frame
====================================================================== */
void UpdatedFrame(ValueList * me)
{
  // Light the RGB LED (orange) and set timer
  LedRGBON(COLOR_ORANGE);
  tinfo_led_timer = millis();

  //char buff[32];
  //sprintf( buff, "Updated Frame (%ld Bytes free)", ESP.getFreeHeap() );
  //Debugln(buff);

  //On publie toutes les infos teleinfos dans un seul appel :
  //sprintf(mytinfo,"{\"papp\":%u,\"iinst\":%u,\"isousc\":%u,\"ptec\":%u,\"indexHP\":%u,\"indexHC\":%u,\"imax\":%u,\"ADCO\":%s}",
  //                  mypApp,myiInst,myisousc,ptec,myindexHP,myindexHC,myimax,mycompteur);

  #ifdef MOD_MQTT
    mqttTinfoPublish();
  #endif

  // nous avons une téléinfo fonctionelle
  status |= STATUS_TINFO;
  tinfo_last_frame = millis();
}

/* ======================================================================
Function: getTinfoListJson
Purpose : dump all teleinfo values in JSON
Input   : -
Output  : -
Comments: -
====================================================================== */
void  getTinfoListJson(String &response)
{
  ValueList * me = tinfo.getList();
  const size_t capacity = JSON_OBJECT_SIZE(16) + 230;
  StaticJsonDocument<capacity> doc;

   // Got at least one ?
  if (me) {
    char * p;

    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;

      if (tinfo.calcChecksum(me->name,me->value) == me->checksum) {
        // Check if value is a number
        strtol(me->value, &p, 10);

        // conversion failed, add "value"
        if (*p) {
          doc[me->name] = me->value;
        // number, add "value"
        } else {
          doc[me->name] = me->value;
        }
      } else {
        doc[me->name] = F("Error");
      }
      ESP.wdtFeed();
    }

    if (!doc.isNull())  {
      serializeJson(doc, response);
    }
  }
  else {
    response = "-1";
  }
}

/* ======================================================================
Function: tinfo_setup
Purpose : prepare and init stuff, configuration, ..
Input   : indique si on doit bloquer jusqu'à reception ligne téléinfo
Output  : false si on devait attendre et time out expiré, true sinon
Comments: -
====================================================================== */
bool tinfo_setup(bool wait_data) {
  bool ret = false;

  Log.notice(F("Initializing Teleinfo..."));

  // reset du timeout de detection de la teleinfo
  tinfo_last_frame = millis();

  // Init teleinfo
  tinfo.init();

  // Attacher les callback donc nous avons besoin
  #ifdef MOD_ADPS
    tinfo.attachADPS(ADPSCallback);
  #endif
  tinfo.attachData(DataCallback);
  tinfo.attachNewFrame(NewFrame);
  tinfo.attachUpdatedFrame(UpdatedFrame);

  // Doit-on attendre une ligne valide
  if (wait_data) {
    // ici on attend une trame complete ou le time out
    while ( !(status & STATUS_TINFO) && (millis()-tinfo_last_frame<TINFO_FRAME_TIMEOUT*1000)) {
      char c;
      // Envoyer le contenu de la serial au process teleinfo
      // les callback mettront le status à jour

      if (Serial.available()) {
        c = Serial.read();
        tinfo.process(c);
      }

      _yield();
    }
  }

  ret = (status & STATUS_TINFO)?true:false;

  if (ret) {
    Log.notice(F("OK!\r\n"));
  }
  else {
    Log.error(F("Erreur!\r\n"));
  }

  return ret;
}

/* ======================================================================
Function: tinfo_loop
Purpose : gestion des trames reçues par la librairie teleinfo
Input   : -
Output  : -
Comments: -
====================================================================== */
void tinfo_loop(void)
{
  char c;
  uint8_t nb_char=0;

  // Evitons les conversions hasardeuses, parlons float
  #ifdef MOD_ADPS
    float fiInst = myiInst;
  #endif

  // on a la téléinfo présente ?
  if ( status & STATUS_TINFO) {
    // est ce que cela fait un moment qu'on a pas recu de trame
    if ( millis()-tinfo_last_frame>TINFO_FRAME_TIMEOUT*1000) {
      // Indiquer qu'elle n'est pas présente
      status &= ~STATUS_TINFO;
      Log.error(F("Teleinfo absente/perdue!\r\n"));
    }

  // Nous n'avions plus de téléinfo
  } else  {
    // est ce que cela fait un moment qu'on a pas recu de data
    if ( millis()-tinfo_last_frame > TINFO_DATA_TIMEOUT*1000) {
      // Light the RGB LED (RED) and set timer with
      // 500ms more than classic blink
      LedRGBON(COLOR_RED);
      tinfo_last_frame = millis();
      tinfo_led_timer = millis();
      Log.verbose(F("Teleinfo toujours absente!\r\n"));
    }
  }

  // Caractère présent sur la sérial téléinfo ?
  // On prendra maximum 8 caractères par passage
  // les autres au prochain tour, çà evite les
  // long while bloquant pour les autres traitements
  while (Serial.available() && nb_char<8) {
    c = (Serial.read());
    tinfo.process(c);
    nb_char++;
  }

  // Faut-il enclencher le delestage ?
  #ifdef MOD_ADPS
    //On dépasse le courant max?
    if (fiInst > myDelestLimit) {
      if ((millis() - timerDelestRelest) > 5000L)  {
        //On ne passe pas dans la boucle si l'on a délesté ou relesté une zone il y a moins de 5s
        //On évite ainsi de délester d'autres zones avant que le délestage précédent ne fasse effet
        delester1zone();
        #ifdef MOD_MQTT
          mqttDelestagePublish();
          mqttFpPublish();
        #endif
        timerDelestRelest = millis();
      }
    } else {
      // Un délestage est en cours (nivDelest > 0)
      // Le délestage/relestage de la dernière zone date de plus de 3 minutes
      // On attend au moins ce délai pour relester ou décaler
      // pour éviter les délestage/relestage trop rapprochés
      if (nivDelest > 0 && (millis() - timerDelestRelest) > 180000L) {
        //Le courant est suffisamment bas pour relester
        if (fiInst < myRelestLimit) {
          relester1zone();
          #ifdef MOD_MQTT
            mqttDelestagePublish();
            mqttFpPublish();
          #endif
          timerDelestRelest = millis();
        } else {
          // On fait tourner le délestage
          // ex : AVANT = "DDCEEEE" => APRES = "CDDEEEE"
          decalerDelestage();
          #ifdef MOD_MQTT
            mqttFpPublish();
          #endif
          timerDelestRelest = millis();
        }
      }
    }
  #endif //ADPS active

  // Do we have RGB led timer expiration ?
  if (tinfo_led_timer && (millis()-tinfo_led_timer >= TINFO_LED_BLINK_MS)) {
      LedRGBOFF(); // Light Off the LED
      tinfo_led_timer=0; // Stop virtual timer
  }
}

#endif // MOD_TELEINFO