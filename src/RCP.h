
#define ADRTIME 180               // tiempo entre inyecciones de adrenalina
// ************************* Librerías para el RTC
#include "RTClib.h"

// ************************ Librerias para el Display OLED I2C

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ************************ Librerías para la matriz de leds

#include <MD_MAX72xx.h>

// ************************ librerías de acceso a memoria no volátil
#include <EEPROM.h>

// ************************ librería para los displays de 7 segmentos

#include <TM1637Display.h>

// ******** Definir constantes para display OLED
#define ANCHO_PANTALLA 128 // ancho pantalla OLED
#define ALTO_PANTALLA 32 // alto pantalla OLED


// ******** Definir constantes para encoder
#define ENC_DOWN   3 
#define ENC_UP    4
#define ENC_SW    2 // pulsador del encoder

// ********* Definir constantes para matriz LED
#define CLK_PIN   13
#define DIN_PIN  10
#define CS_PIN    11
#define MAT_COUNT 4
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// ********* Definir constantes para los interruptores
#define INT_PARADA 9
#define INT_ADR    12

// ********* Definir constantes para los displays de 7 segmentos

#define SSCLK1 8
#define SSDIO1 7
#define SSCLK2 6
#define SSDIO2 5

const uint8_t GRADO       =  SEG_A | SEG_B | SEG_F | SEG_G ;
const uint8_t CENTIGRADO  =  SEG_A | SEG_D | SEG_E | SEG_F ;

// Instanciación de objetos de ámbito global

// Objeto de la clase Adafruit_SSD1306
Adafruit_SSD1306 Oled(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

// objeto de la clase RCT__DS3231
RTC_DS3231 reloj;

// Objeto matriz de leds
MD_MAX72XX lDisplay = MD_MAX72XX(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAT_COUNT);

// Objetos 7 segmentos display

TM1637Display ssD1(SSCLK1, SSDIO1);
TM1637Display ssD2(SSCLK2, SSDIO2);

/*********************************************************
* Definir constantes para el funcionamiento del programa *
**********************************************************/
// opciones de menu
#define SETUP_TIME  0
#define REVISAR     1
#define IDLE        2
#define MENU        3
// campos del ajuste de la hora
#define HORA      0
#define MINUTOS   1
#define DIA       2
#define MES       3
#define ANNO      4

// Variables globales de control

DateTime now;                                          // hora actual para ser mostrada en el reloj
//DateTime parada,adrenalina;                            // momentos estáticos en los que se pulsan los botones de evento

DateTime parada,adrenalina;  

 struct   evento       // definición de la estructura. La clase Evento hará uso de ella.
  {
   uint8_t id;             // identificador de evento
   uint8_t codigo;         // codigo de identificacion de evento: 10 = pulsado botón de parada
                        //                                     20 = pulsado el botón de adrenalina
                        //                                     30 = Fin de parada
   uint8_t argumento;      // El evento puede tener un argumento. De entrada, sólo tiene sentido para los shots de adrenalina, y se indica el ordinal
   uint8_t dia;            // fecha y hora del evento
   uint8_t mes;
   uint16_t ano;
   uint8_t hora;
   uint8_t minuto;
   uint8_t segundo;
  };
class Persistente // esta clase va a abstraer toda la lógica de los eventos registrados en memoria no volátil.
{
           
 public:
           uint16_t tam;            // número de eventos máximo
           uint16_t primerEvento = 0, ultimoEvento = 0;               // índice del primer y último elemento (real) del array 
           evento actual;           
               
                     
  Persistente() // constructor
  {
    tam = EEPROM.length()/sizeof(evento)-1;    // calculamos el tamaño del array
  } 

  void setMaxMin()
  {
  uint16_t f;
  
  DateTime fechaMenor, fechaMayor, fecha;                         // para calcular los tiempos
  
  EEPROM.get(0,actual);                                           // leemos el primer registro
  fechaMayor = toDT(actual);
  fechaMenor = toDT(actual);
  for (f = 1; f <= tam ; f++)
    {
      EEPROM.get(f*sizeof(evento),actual);                        // leemos el siguiente registro
      fecha = toDT(actual);
#ifdef __DEBUG__
  Serial.print("Leyendo evento.");
  Serial.println((uint16_t)f);
#endif
      
      if (! fecha.isValid())
          continue;        
                    
#ifdef __DEBUG__
  Serial.print("Validando evento.");
  Serial.println((uint16_t)f);
#endif

      if ( fecha < fechaMenor )
        {
          fechaMenor = fecha;
          primerEvento = f;
        }        
      if (fecha > fechaMayor)
        {
          fechaMayor = fecha;
          ultimoEvento = f;          
        }
    }    

  }
void iniciar_eeprom()
  {
    uint16_t f;
    
    for  (f = 0; f <= EEPROM.length(); f++)
    {
    EEPROM.put(f,(char) -1);  
    } 
  setEvent(0,0,true);    
  }     
 DateTime toDT(evento actual)                 // devuelve un DateTime a partir de una struct evento
  {
  return DateTime(actual.ano, actual.mes, actual.dia, actual.hora, actual.minuto, actual.segundo);
  }  
  evento getEvento(uint8_t id)       // devuelve una estructura evento correspondiente al id enviado
  {
   evento toReturn; // para devolver
   EEPROM.get(id*sizeof(evento),toReturn);
   return toReturn;
  }  
  void setEvent(char code, char arg = 0, bool force = false)
  {
    uint16_t next;
    if (! force)
      next = getNextPointer();
    else
      next = 0;
    EEPROM.get(ultimoEvento*sizeof(evento),actual);            // guardamos en actual el contenido del evento más reciente.       
    actual.id         = next;
    actual.codigo     = code;
    actual.argumento  = arg;
    actual.dia        = now.day();
    actual.mes        = now.month();
    actual.ano        = now.year();
    actual.hora       = now.hour();
    actual.minuto     = now.minute();
    actual.segundo    = now.second();     // todos los campos están asignados
 
    EEPROM.put(next*sizeof(evento),actual);            // guardamos en actual el contenido del evento más reciente.    
    ultimoEvento = next;
#ifdef __DEBUG__
  Serial.println("Grabando evento.");
  Serial.print("Code ");
  Serial.println((uint8_t)actual.codigo);  
  Serial.print("Hora ");  
  Serial.println((uint8_t)actual.hora);
  Serial.print("Minuto ");
  Serial.println((uint8_t)actual.minuto);
#endif    
  }
uint16_t getNextPointer()  // devuelve el siguiente puntero disponible
  {
  return  (ultimoEvento == tam ? 0 : ultimoEvento + 1);       //si hemos llegado al final de la memoria eeprom, el próximo puntero a usar es el principio de la memoria
  } 
void chkLastEvent()
  {
   evento actual;
   if(primerEvento == 0 and ultimoEvento == 0)
    return;     
   EEPROM.get(ultimoEvento*sizeof(evento),actual);          // leemos el último evento
   if (actual.codigo != 30 and actual.codigo !=0)  
   {                                                        // 30 es fin de parada
    now = reloj.now();                                   // Fecha del evento = momento actual  
    setEvent(30) ;                                           // grabamos un evento de fin de parada
   }
  }   
};


uint8_t accion = IDLE;                                 // regula el flujo mayor del programa
int8_t opc_actual = 0,setup_time_opcion = HORA;        // opcion cuando la accion = MENU. Inicializa la primera entrada a ajustar cuando se ajuste la hora
bool time_adjust = false;                              // flag para el ajuste de la hora
bool parada_flag = false, adrenalina_flag = false;     // flag para el status de parada y adrenalina
Persistente Evento;                                    // instanciamos la clase Evento
uint16_t idEventoRevisar;                               // identificador del evento en revisión. 
int16_t adr_time = ADRTIME, adr_count = 0;             // segundos entre shots de adrenalina y contador de shots
bool adr_refractario = false;

/********************************************** LISTA DE FUNCIIONES ***************************************/
void MatrizSetup();
void RelojSetup();
void OledSetup();
void ssDisplaySetup();
void doEncode();
void doSwitch();
void displayParada();
void displayIdle();
void displayMenu();
void displaySetupTime();
void displayRevisar();
void ssDParada();
void ssDNoParada();
void xdisplaySetupTime(const String& ,uint16_t);
void menuSelect();
void setupTimeSelect();
void revisar();