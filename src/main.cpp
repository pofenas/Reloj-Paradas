
//#define __DEBUG__
//#define BORRAR_EEPROM


#include <RCP.h>
#include <setup.h>
#include <ISR.h>
#include <display.h>

void setup() 
{


#ifdef __DEBUG__
  Serial.begin(9600);
#endif


// OCULUS:   el orden de inicialización de los elementos resulta ser crítico. Si inicializamos la matriz de leds despues de los dispositivos I2C, deja de funcionar.
// Inicializamos la matriz de leds
 MatrizSetup();
  
  // Iniciamos el Reloj en tiempo real
  RelojSetup();
  // Oled setup
  OledSetup();
  // Encoder setup
  ssDisplaySetup();
  pinMode(ENC_DOWN, INPUT_PULLUP); 
  pinMode(ENC_UP, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_DOWN), doEncode, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_SW), doSwitch, FALLING);
  // Interruptores setup
  pinMode(INT_PARADA, INPUT_PULLUP);
  pinMode(INT_ADR, INPUT_PULLUP);
  now = reloj.now();   
  #ifdef BORRAR_EEPROM
   Evento.iniciar_eeprom();
  #endif
  Evento.setMaxMin();
  Evento.chkLastEvent();  
  idEventoRevisar = Evento.ultimoEvento;

}

void loop()
 {
  if(digitalRead(INT_PARADA)==HIGH and ! parada_flag)    // se ha pulsado el boton de parada
   {
     parada_flag = true;                 // flag de parada. Solo pasara a false cuando se seleccione por menú
     now = reloj.now();               // fijamos la hora de inicio de parada  como hora actual para la grabacion
     parada = reloj.now();
     detachInterrupt(ENC_SW);          // deshabilitamos las interrupciones en el encoder. Sólo volverán a estar disponibles tras un reset.
     detachInterrupt(ENC_UP);
     detachInterrupt(ENC_DOWN);
     Evento.setEvent(10);          // grabamos un evento en la eeprom como que se ha pulsado el boton de parada
    // accion = IDLE;
   }   
    if(digitalRead(INT_ADR)==HIGH and  parada_flag and ! adr_refractario)    // se ha pulsado el boton de adrenalina durante una parada
    {
      adrenalina_flag = true;
      adr_count++;
      adr_time = ADRTIME;
      adr_refractario = true;
      Evento.setEvent(20,adr_count);
    }    
  if (time_adjust)
  {
    reloj.adjust(now);  
    time_adjust = false;
  } 
     
 if (parada_flag)
  displayParada();
    
else
 {
 if(accion == IDLE)
  displayIdle();
 if (accion == MENU)
  displayMenu();
 if (accion == SETUP_TIME)
  displaySetupTime();
 if ( accion == REVISAR)
  displayRevisar();
 }
 
 }
 

