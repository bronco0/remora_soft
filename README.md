# Logiciel Remora V2.0 [![Build Status](https://travis-ci.org/AuFilElec/remora_soft.svg?branch=dev-2.0)](https://travis-ci.org/AuFilElec/remora_soft)

Le logiciel Remora_soft fonctionne sur les [cartes Remora](https://github.com/thibdct/programmateur-fil-pilote-wifi/).

Cette version logicielle est compatible avec la version matérielle [1.2](https://github.com/thibdct/programmateur-fil-pilote-wifi/tree/master/Mat%C3%A9riel/1.2), [1.3](https://github.com/thibdct/programmateur-fil-pilote-wifi/tree/master/Mat%C3%A9riel/1.3) et [1.4][1].

Elle est compatible avec les cartes à base d'ESP8266 via un [adaptateur][5]

## Sommaire

* [Installation (Arduino ESP8266 NodeMCU / Wemos)](#installation-arduino-esp8266-nodemcu--wemos)
   * [Environnement de compilation et d'upload](#environnement-de-compilation-et-dupload)
* [Compilation et upload](#compilation-et-upload)
* [Update via OTA](#update-via-ota)
* [Debug](#debug)
   * [Wemos d1 mini](#wemos-d1-mini)
* [Configuration](#configuration)
   * [Les versions de carte Remora](#les-versions-de-carte-remora)
   * [Les modules de la Remora](#les-modules-de-la-remora)
* [Utilisation](#utilisation)
   * [Les codes couleurs LED RGB](#les-codes-couleurs-led-rgb)
   * [API Exposées (ESP8266 NodeMCU / Wemos)](#api-exposées-esp8266-nodemcu--wemos)
      * [Les différents états](#les-différents-états)
         * [Les Etats de fil pilote](#les-etats-de-fil-pilote)
         * [Les Etats du mode de fonctionnement du relais](#les-etats-du-mode-de-fonctionnement-du-relais)
      * [Les API d'intérrogation](#les-api-dintérrogation)
         * [Système](#système)
         * [Relais](#relais)
         * [Délestage](#délestage)
         * [Fil pilote](#fil-pilote)
         * [Téléinfo](#téléinfo)
      * [Les API d'action](#les-api-daction)
         * [Système](#système-1)
         * [Relais](#relais-1)
         * [Fil pilote](#fil-pilote-1)
   * [MQTT](#MQTT)
      * [Topic d'information](#topic-d-information)
      * [Topic de commande](#-opic-de-commande)
* [A faire](#a-faire)
* [Historiques des Modifications](#historiques-des-modifications)
* [Exemple](#exemple)

## Installation (Arduino ESP8266 NodeMCU / Wemos)

### Environnement de compilation et d'upload

- Installer l'environnement [Platformio](http://platformio.org/), au choix:
  - La version [IDE](http://platformio.org/platformio-ide)
  - Ou la version [Core](http://docs.platformio.org/en/latest/installation.html)
- Installer les plateformes suivantes:
  - **espressif8266**: `platformio platform install espressif8266 --with-package=tool-mkspiffs`
  - **espressif8266_stage**: `platformio platform install https://github.com/platformio/platform-espressif8266.git#feature/stage`

## Compilation et upload

- Compiler les sources pour votre carte:
  - **Wemos d1 mini**: `platformio run --environment d1_mini`
  - **NodeMCU 1.0**: `platformio run --environment nodemcuv2`

- Lancer l'upload des sources:
  - **Wemos d1 mini**: `platformio run --environment d1_mini --target upload --upload-port COM??`
  - **NodeMCU 1.0**: `platformio run --environment nodemcuv2 --target upload --upload-port COM??`

- Compiler le SPIFFS:
  - **Wemos d1 mini**: `platformio run --environment d1_mini_spiffs --target buildfs`
  - **NodeMCU 1.0**: `platformio run --environment nodemcuv2_spiffs --target buildfs`

- Lancer l'upload SPIFFS (les fichiers WEB)
  - **Wemos d1 mini**: `platformio run --environment d1_mini_spiffs --target uploadfs`
  - **NodeMCU 1.0**: `platformio run --environment nodemcuv2_spiffs --target uploadfs`

## Update via OTA
- Upload sources
  - **Wemos d1 mini**: `platformio run --environment d1_mini --target upload --upload-port IP_ADDRESS_HERE or mDNS_NAME.local`
  - **NodeMCU 1.0**: `platformio run --environment nodemcuv2 --target upload --upload-port IP_ADDRESS_HERE or mDNS_NAME.local`
- Upload SPIFFS
  - **Wemos d1 mini**: `platformio run --environment d1_mini_spiffs --target uploadfs --upload-port IP_ADDRESS_HERE or mDNS_NAME.local`
  - **NodeMCU 1.0**: `platformio run --environment nodemcuv2_spiffs --target uploadfs --upload-port IP_ADDRESS_HERE or mDNS_NAME.local`

**Attention**, pour pouvoir uploader via le cable USB, la téléinfo ne doit pas être connectée (en OTA pas de soucis) car le port série est partagé. Donc, soit il faut la débrancher, soit il faut sortir le module NodeMCU/Wemos du support, ou bien déconnecter le jumper *JP2* sur l'adaptateur.

## Debug

Pour les mêmes raisons, et afin d'afficher les informations de debug dans une console, celle-ci doit être configurée à la même vitesse que la téléinfo (car c'est le même port série) donc 1200 bps, parité paire et 7 bits de data, or **cette configuration n'est pas possible dans le moniteur série de l'Arduino**. Il faut alors prendre un autre terminal comme putty, coolterm, ...

N'oubliez pas de décommenter la variable `DEBUG` dans le fichier [remora.h](https://github.com/AuFilElec/remora_soft/blob/platformio/src/remora.h), pour activer le debug.

### Wemos d1 mini

Sur le **Wemos d1 mini**, il vous est possible d'utiliser le port série TX1 (pin: D4) à 115200 bps. Pour cela, il vous faut définir la variable `DEBUG_SERIAL` à **Serial1** et décommenter la ligne `DEBUG_INIT` dans le fichier [remora.h](https://github.com/AuFilElec/remora_soft/blob/platformio/src/remora.h).
Branchez un adaptateur USB TTL sur votre Wemos (*Adaptateur*: GND et RX => *Wemos*: GND et D4).

## Configuration

### Les versions de carte Remora

En fonction de la carte que vous possédez, il vous faut définir la version dans le fichier [remora.h](https://github.com/AuFilElec/remora_soft/blob/platformio/src/remora.h).

Variables: `REMORA_BOARD_V1X` allant de 1.2 à 1.4 actuellement.

### Les modules de la Remora

Les modules vous permettent d'activer certaines fonctionnalités au sein de la Remora.

Pour activer un module, il vous suffit de décommenter la ligne du module dans le fichier [remora.h](https://github.com/AuFilElec/remora_soft/blob/platformio/src/remora.h).

| Modules 		| Description |
| ------------- | ----------- |
| MOD_TELEINFO  | Module de récupération de la téléinformation du compteur Enedis (EDF) |
| MOD_ADPS      | Module de gestion du délestage |
| MOD_OLED      | Module de l'afficheur OLED |
| MOD_RF69      | Module radio avec les éléments RF69 (en cours de développement) |
| MOD_RF_OREGON | Module radio des sondes Oregon (pas encore géré) |
| MOD_EMONCMS   | Module de communication avec emoncms.org |
| MOD_JEEDOM    | Module de communication avec Jeedom |
| MOD_MQTT      | Module de communication via Mqtt |

## Utilisation

### Les codes couleurs LED RGB

- ![Orange](https://placehold.it/25/ffa500/000000?text=+)
	- Durant la connexion WiFi
	- Durant la mise à jour d'une trame teleinfo
- ![Magenta](https://placehold.it/25/ff00ff/000000?text=+)
 	- Durant l'initialisation de l'OTA
 	- Durant un upload OTA
 	- Durant un upload via l'interface Web
- ![Rouge](https://placehold.it/25/ff0000/000000?text=+)
	- En cas d'erreur
	- Lorsque la téléinfo n'est pas trouvée
- ![Vert](https://placehold.it/25/00ff00/000000?text=+)
	- Lorsque les modules ont été initialisés
	- Lorsqu'une nouvelle trame de téléinfo est récupérée
- ![Jaune](https://placehold.it/25/ffff00/000000?text=+)
	- Lorsque l'initialisation est terminée

### API Exposées (ESP8266 NodeMCU / Wemos)

Toutes les API se font via des requêtes HTTP sur le Remora. Il existe deux formats possibles si l'on veut récupérer des données ou exécuter des action avec le Remora. Chaque requête se verra retourner des données (ou un code de bonne éxécution) au format JSON.

Toute requête sera donc adressée sous la forme
`http://adresse_ip_du_remora/requete_plus_ou_moins_longue` dans les exemples ci dessous l'adresse IP de mon Remora est la 192.168.1.201, veillez à bien la changer pour mettre la vôtre. Les exemples ont été exécutés depuis la ligne de commande avec curl mais ils pourraient l'être depuis la barre d'addresse de votre navigateur.

#### Les différents états

##### Les Etats de fil pilote

Les différents états possibles de fil pilote dans l'API correspondent à la notation suivante, une lettre représente l'état lu ou le mode à positionner tel que :
```
C = Confort
1 = Confort -1
2 = Confort -2
A = Arrêt
E = Eco
H = Hors gel
```

##### Les Etats du mode de fonctionnement du relais

Voici les différents modes de fonctionnement du relais que vous pourrez trouver dans l'API:
```
0: arrêt
1: marche forcée
2: automatique
```

#### Les API d'intérrogation

Les API d'intérrogation se présentent sous la forme
`http://adresse_ip_du_remora/ma_donnee` et la/les donnée(s) sont retournées au format JSON (j'ai volontairement supprimé certains sauts de lignes de sortie pour une meilleure lecture)

##### Système
- Durée de fonctionnement en secondes depuis le dernier reboot/reset/allumage `http://ip_du_remora/uptime`
````shell
    ~ # curl http://192.168.1.201/uptime
    { "uptime": 120 }
````
##### Relais
- Etat du relais et du mode de fonctionnement `http://ip_du_remora/relais`
````shell
    ~ # curl http://192.168.1.201/relais
    { "relais": 0, "fnct_relais": 2 }
````
##### Délestage
- Etat du délestage `http://ip_du_remora/delestage`
````shell
		~ # curl http://192.168.1.201/delestage
		{ "niveau": 0, "zone": 1 }
````
Si le délestage est désactivé `http://ip_du_remora/delestage`
````shell
		~ # curl http://192.168.1.201/delestage
		{ "etat": "désactivé" }
````
##### Fil pilote
- Etat d'un fil pilote `http://ip_du_remora/fpn` avec n = numéro du fil pilote (1 à 7)
````shell
		~ # curl http://192.168.1.201/fp3
		{ "fp3": "E" }
````
- Etat de tous les fils pilotes **http://ip_du_remora/fp**
````shell
		~ # curl http://192.168.1.201/fp
		{
		"fp1": "A",
		"fp2": "E",
		"fp3": "E",
		"fp4": "E",
		"fp5": "C",
		"fp6": "H",
		"fp7": "C"
		}
````
##### Téléinfo
- Récupérer une étiquette Téléinfo par non nom `http://ip_du_remora/Nom_Etiquette`
````shell
		~ # curl http://192.168.1.201/PAPP
		{ "PAPP": 170 }
		~ # curl http://192.168.1.201/IINST
		{ "IINST": 1 }
		~ # curl http://192.168.1.201/PTEC
		{ "PTEC":"HC.." }
````
- Récupérer toutes les étiquettes Téléinfo en une fois `http://ip_du_remora/tinfo`
````shell
		~ # curl http://192.168.1.201/tinfo
		{ "_UPTIME":1614,
			"ADCO":31428067147,
			"OPTARIF":"HC..",
			"ISOUSC":15,
			"HCHC":410994,
			"HCHP":0,
			"PTEC":"HC..",
			"IINST":1,
			"IMAX":1,
			"PAPP":170,
			"HHPHC":3,
			"MOTDETAT":0
		}
````
A noter la présence de certaines étiquettes virtuelles commencant par un `_`


#### Les API d'action

Les API d'action se presentent sous la forme
`http://adresse_ip_du_remora/?action=ma_donnee`, notez la différence avec les intérrogations, le `?`. Le résultat est retourné au format JSON avec un code réponse, il est :
- négatif en cas d'erreur
- à 0 si tout est OK
- positif pour indiquer un code retour OK différent si besoin.

Note, il est possible d'enchainer les actions en une requête mais un seul code d'erreur sera retourné pour l'ensemble, si une des commandes échoue, il faudra intérroger afin de savoir laquelle n'a pas fonctionnée.

##### Système
- Faire un reset (reboot) `http://ip_du_remora/reset`
````shell
		# curl http://192.168.1.201/reset
		OK, Redémarrage en cours
````

- Faire une remise à zero usine `http://ip_du_remora/factory_reset`, **attention tout va être vidé**, y compris le système de fichier SPIFFS, c'est comme si votre NodeMCU arrivait d'usine
````shell
		# curl http://192.168.1.201/factory_reset
		OK, Redémarrage en cours
````
##### Relais
- Activer le relais `http://ip_du_remora/?relais=1`
````shell
		# curl http://192.168.1.201/?relais=1
		{ "response": 0 }
````
- désactiver le relais `http://ip_du_remora/?relais=0`
````shell
		# curl http://192.168.1.201/?relais=0
		{ "response": 0 }
````
- Exemple d'erreur avec le relais `http://ip_du_remora/?relais=3`
````shell
		# curl http://192.168.1.201/?relais=3
		{ "response": -1 }
````
- Utiliser le mode automatique du relais basé sur les changements de périodes tarifaires
````shell
		# curl http://192.168.1.201/?frelais=2
		{ "response": 0 }
````
- Arrêter la gestion du relais et ouvrir le contact
````shell
		# curl http://192.168.1.201/?frelais=0
		{ "response": 0 }
````
- Mettre le relais en marche forcée
````shell
		# curl http://192.168.1.201/?frelais=1
		{ "response": 0 }
````
Il est aussi possible de forcer le relais jusqu'au prochain changement de période tarifaire, si le mode est en automatique. Pour cela, il vous suffit d'activer le relais directement.
````shell
		# curl http://192.168.1.201/?relais=1
		{ "response": 0 }
````
##### Fil pilote
- selectionne le mode d'un des fils pilotes `http://ip_du_remora/?setfp=na` avec n=numéro du fil pilote et a=le mode à positionner (non sensible à la casse)
  Fil pilote 1 en arret
````shell
		# curl http://192.168.1.201?setfp=1a
		{ "response": 0 }
````
  Fil pilote 7 en Eco
````shell
		# curl http://192.168.1.201?setfp=7E
		{ "response": 0 }
````
  Mauvaise commande
````shell
		curl http://192.168.1.201?setfp=5X
		{ "response": -1 }
````
- Selectionne le mode d'un ou plusieurs les fils pilotes d'un coup `http://ip_du_remora/?fp=CMD` avec 7 commandes de fil pilote.
  Tous les fils pilote en confort
````shell
		curl http://192.168.1.201/?fp=CCCCCCC
		{ "response": 0 }
````
  Tous les fils pilote en arret
````shell
		curl http://192.168.1.201/?fp=AAAAAAA
		{ "response": 0 }
````
  Tous les fils pilote en eco
````shell
		curl http://192.168.1.201/?fp=EEEEEEE
		{ "response": 0 }
````
  Tous OFF sauf le fil pilote 1 en confort
````shell
		curl http://192.168.1.201/?fp=CAAAAAA
		{ "response": 0 }
````
  Tous OFF sauf le fil pilote 2 A-AAAAA
````shell
		curl http://192.168.1.201/?fp=CCCCCCC
		{ "response": 0 }
````
  FP1 Eco, FP2 inchangé, FP3 confort, FP4 hors gel, FP5 arrêt, FP6 Confort -1, FP7 Confort -2
````shell
		curl http://192.168.1.201/?fp=E-CHA12
		{ "response": -1 }
````

### MQTT
#### Topic d'information
##### LWT
Message LWS pour gérer les deconnections/reconnections au broker.
````
topic : remora/online 
payload : 1
retain = true
````
##### Système
Récupérer les information du système.
````
topic : remora/system
paylod {"version": "2.0.0-beta", "board": "Remora v1.5", "ip": "192.168.1.31", "mac": "cd:cf:34:45:fc:4e", "rssi": "64", "freeheap": "2342", }
retain = true
````
##### Téléinfo
Récupérer la téléinfo, message envoyer lors de la réception d'une nouvelle trame.
````
topic : remora/teleinfo {"etat": "1", "adco": "03428067", "optarif": "HC", "isousc": "15", "hchc": "000261373", "hchp": "000132345", "ptec": "HC", etc...} | retain = false
````
##### Déléstage
Récupérer les information de délestage.
````
topic : remora/delestage 
payload : {"status": "1", "nb_zone": "0", "old_zone": "0"} | retain = false
````
##### Fils pilote
````
topic : remora/fp {"fp1": "C", "fp2": "H", "fp3": "H", "fp4": "H", "fp5": "C", "fp6": "H", "fp7": "E"} | retain = false
topic : remora/fp {"fp1": "C"} | retain = false
````
##### Relais
````
topic : remora/relais {"mode": "0", "status": "2"} | retain = false
````

#### Topic de commande
````
remora/fp/get
remora/fp/1/get 
remora/fp/set {"fp1": "C", "fp2": "H", "fp3": "H", "fp4": "H", "fp5": "C", "fp6": "H", "fp7": "E"}
remora/fp/set {"fp1": "C"}
remora/relais/set {"mode": "0", "status": "2"}`
````

## A faire

- Mettre des icones plus sympas sur l'afficheur
- Gérer les retour de sondes RF, pour le moment seules les trames sont affichées sur la serial, rien n'est encore fait
- Gérer le RF des sondes Oregon Scientific
- pour le moment seul les [OLED][4] I2C sont gérés
- tout autre idée est bienvenue

## Historiques des Modifications

16/02/2015 : Ajout délestage cascadocyclique / Possibilité de ne récupérer l'état que d'un seul fil pilote

14/04/2015 : Ajout d'une variable spark pour la teleinfo. Passage en un seul appel pour script jeedom. Les variables d'origine restent utilisables.

30/09/2015 : voir le post [dédié][6].

02/12/2015 : Ajout [Version 1.3][7] Remora NodeMCU

04/01/2016 : Integration Interface WEB

01/10/2016 : Ajout de la (dés)activation du délestage. Ajout de la gestion automatisée du mode du relais.

07/01/2017 : Modification de la taille de la clé API pour Jeedom (32 => 48)

08/02/2017 : Ajout de la variable RELAIS_REVERSE à décommenter dans remora.h pour inverser l'état du relais en cas de problème

04/03/2017 : Ajout des connexions TCP Asynchrones

27/08/2017 : Modification de la structure du code source pour s'adapter à l'environnement Platformio

## Exemple

Pour les photos la téléinfo n'était pas branchée sur la carte, c'est pour celà que les compteurs et le bargraphe sont à 0

<img src="https://github.com/thibdct/programmateur-fil-pilote-wifi/blob/master/Logiciel/images/remora01.jpg" alt="Top"    width="45%" height="45%">&nbsp;
<img src="https://github.com/thibdct/programmateur-fil-pilote-wifi/blob/master/Logiciel/images/remora02.jpg" alt="Bottom" width="45%" height="45%">
<br>

Avec la téléinfo branchée sur la carte, le bargraph indique la puissance instantanée par rapport au contrat souscrit, ici soit 11% car on consomme 5A sur un contrat de 45A. Nous sommes en heures pleines (affichage sur fond blanc)
<img align="center" src="https://github.com/thibdct/programmateur-fil-pilote-wifi/blob/master/Logiciel/images/remora03.jpg" alt="Bottom" width="45%" height="45%">


[1]: https://github.com/AuFilElec/remora_hard/
[4]: http://www.ebay.com/itm/291216700457
[5]: https://github.com/hallard/Particle2NodeMCU/blob/master/README.md
[6]: https://community.ch2i.eu/topic/92/nouvelle-version-remora-compatible-esp8266
[7]: https://github.com/thibdct/programmateur-fil-pilote-wifi/tree/master/Mat%C3%A9riel/1.3
