////////////////////////////////////////////////////////////
//      Kangoo Typ2 Steckerverriegelung und Freigabe      //
//            Autor: B.Knöfel ***** Version 0.01          //
//                                                        //
//                   STATUS: Ungetestet!                  //
////////////////////////////////////////////////////////////


/*
  Changelog:
  30.10.2016 Anfang gemacht...


Notizen:
~~~~~~~~
 

  

///////////////////////////////////////////////////////////////////////////
// PP-Widerstandsmessung und Bestimmung der Belastbarkeit des Ladekabels //
/*                _______         _______
      PE+GND ----|__4k7__|---o---|__4k7__|---- +5V (Arduino)
                             |
                  _______    |
          PE ----|__PP_?_|---o---------------- AnalogInput
*/

//////// Ein- und Ausgänge:
int PP_mess    =  A0;   // Anschluss PP von Ladedose
int TasterSTOP =  6;    // Beenden der Ladung
int Relais1    =  7;    // Verriegelung Öffnen
int Relais2    =  8;    // Verriegelung Schließen 
int Relais3    =  9;    // Freigabe ans Fahrzeug "Ladung starten"
int Relais4    = 10;
int Led_ROT    = 11;    // LED Fehler
int Led_GRUEN  = 12;    // LED Status
int Led_BORD   = 13;    // Onbord LED Arduino

/////// Static:
int PP_calc = 0;        // Abgleichswert der PP Messung über Analogeingang
int PP = 0;             // ergibt die maximale Belastbarkeit des Ladekabels (zb. 32 für 32A)
bool Connected = 0;     // gibt Status zurück, ob Fahrzeug verbunden, oder nicht
bool rem_con = 0;
bool rem_discon = 0;
unsigned long remTime_con = 0;
unsigned long remTime_discon = 0;
unsigned long UpdateDisplay = 0;

// LCD Display 4x20 Zeichen
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
pinMode(PP, INPUT);
pinMode(TasterSTOP, INPUT);
pinMode(Relais1, OUTPUT);
pinMode(Relais2, OUTPUT);
pinMode(Relais3, OUTPUT);
pinMode(Relais4, OUTPUT);
pinMode(Led_ROT, OUTPUT);
pinMode(Led_GRUEN, OUTPUT);
pinMode(Led_BORD, OUTPUT);

// Serial.begin(9600);

// LCD Display 4x20 Zeichen
lcd.begin(20, 4);
lcd.setCursor(0,0); lcd.print("Display READY"); delay (500); lcd.clear(); delay (100);

}

void loop() {

///////////////////////////////////////////////////////////////////////////////
// Bestimmen des PP Widerstandes und Status Fahrzeug angeschlossen oder nicht
  
  PP_calc = analogRead(PP_mess) *1.96 ;  // mit mathematischen Rechnungen anpassen bis PP_calc bei offenem Messkreis bei ca. 1000 ist

  if (PP_calc >  500)                   PP =  1;       // offener Kreis   = Messung ergibt ca. 1000
  if (PP_calc <= 500 && PP_calc > 300)  PP = 13;       // 1500 Ohm =  13A = Messung ergibt ca.  390
  if (PP_calc <= 300 && PP_calc > 150)  PP = 20;       //  680 Ohm =  20A = Messung ergibt ca.  225
  if (PP_calc <= 150 && PP_calc >  60)  PP = 32;       //  220 Ohm =  32A = Messung ergibt ca.   85
  if (PP_calc <=  60 && PP_calc >  10)  PP = 63;       //  100 Ohm =  63A = Messung ergibt ca.   40
  if (PP_calc <=  10)                   PP = 99;       // Kurzschluss     = Messung ergibt ca.    0

///// Verbunden
  if ((PP==13 || PP==20 || PP==32 || PP==63) && rem_con==LOW){
    rem_con = HIGH;
    rem_discon = LOW;
    remTime_con = millis() + 500;                      // Verzögerungszeit zur Erkennung "Verbunden" ---> Entprellen
  }
  if (remTime_con != 0 && remTime_con < millis() && rem_con == HIGH) {
    Connected = 1;
    remTime_con = 0;
  }

///// Getrennt
  if ((PP==1 || PP==99) && rem_discon==LOW){    
    rem_discon = HIGH;
    rem_con = LOW;
    remTime_discon = millis() + 500;                   // Verzögerungszeit zur Erkennung "Getrennt" ---> Entprellen
  }
  if (remTime_discon != 0 && remTime_discon < millis() && rem_discon == HIGH) {
    Connected = 0;
    remTime_discon = 0;
  }


///////////////////////////////////////////////////////////////////////////////
// Bestimmen ob Verriegelung geöffnet oder geschlossen ist





///////////////////////////////////////////////////////////////////////////////
// Displayausgaben

// Display nur alle X-Sekunden löschen ... vermeidet Flackern!
  if (millis() >= UpdateDisplay){
    lcd.clear();
    UpdateDisplay = millis() + 1000;      // 1000 = 1 Sekunde
  }

  
  lcd.setCursor(0,0); lcd.print("PP_calc: "); lcd.print(PP_calc); lcd.setCursor(14,0); lcd.print("PP: "); lcd.print(PP);  
  lcd.setCursor(0,1); lcd.print("Connected?: "); lcd.print(Connected);
//  lcd.setCursor(0,2); lcd.print("rem_con   : "); lcd.print(rem_con); lcd.print(" ");lcd.print((remTime_con-millis())/1000);
//  lcd.setCursor(0,3); lcd.print("rem_discon: "); lcd.print(rem_discon); lcd.print(" ");lcd.print((remTime_discon-millis())/1000);


 
/*
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AI0: ");
  lcd.print(AI0);

  lcd.setCursor(0, 1);
  lcd.print("PP= ");
  switch (PP) {
    case 0:   lcd.print("Init Error!") ; break;    // Initialisierungsfehler
    case 1:   lcd.print(" No con!")    ; break;    // Nicht angeschlossen
    case 13:  lcd.print("13A / 1500")  ; break;    // 13A
    case 20:  lcd.print("20A / 680")   ; break;    // 20A
    case 32:  lcd.print("32A / 220")   ; break;    // 32A
    case 63:  lcd.print("63A / 100")   ; break;    // 63A
    case 99:  lcd.print("Kurzschluss") ; break;    // Kurzschluss
    default:  lcd.print("Script Err.") ; break;    // Ungenau
    }
*/

//  delay(200);
}
