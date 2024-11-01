/*************************************************** RUTINAS DE IMPRESION EN PANTALLA. */
/********************************************
*
*     Revisión de los eventos
*
*********************************************/
void displayRevisar()
{
  evento current;
  current = Evento.getEvento(idEventoRevisar);    
  Oled.setTextSize(1);   
  Oled.clearDisplay();
  Oled.setCursor(0, 0);
  Oled.print("EVENTO: ");
  Oled.print(idEventoRevisar);
  Oled.setCursor(0, 20);  
  Oled.print((int)current.dia);
  Oled.print("-");
  Oled.print((int)current.mes);
  Oled.print("-");
  Oled.print((int)current.ano);
  Oled.print(" ");    
  Oled.print((int)current.hora);
  Oled.print(":");
  Oled.print((int)current.minuto);
  Oled.print(":");
  Oled.print((int)current.segundo);

  Oled.setCursor(0, 10);
if ( current.codigo == 0)
    Oled.print("PUESTA EN MARCHA");  
  if ( current.codigo == 10)
    Oled.print("INICIO DE PARADA");
  if ( current.codigo == 20)
    {    
    Oled.print("ADRENALINA (");
    Oled.print( current.argumento );   
    Oled.print(")"); 
    }
  if ( current.codigo == 30)
    Oled.print("FIN DE PARADA");  
  
  Oled.display();
}
/********************************************
 * 
 * 
 * 
*********************************************/
void msgBorradoEprom()
{
  Oled.setTextSize(1);   
  Oled.clearDisplay();
  Oled.setCursor(0, 0);
  Oled.print("Borrando memoria.");
  Oled.display();
  delay(2000);
}
/********************************************
*
*     Impresión del tiempo de parada en la matriz de leds
*
*********************************************/
void displayParada()
{

 static DateTime last_shown =  (reloj.now() - TimeSpan(0,0,parada.minute(), parada.second()));      // ultimo mostrado. Inicializamos con 0
 char uni_sec, dec_sec, uni_min, dec_min;                                                           // para evitar parpadeos, sólo imprimiremos cuando cambie la información a mostrar
 static char refractar = 0;
 if (reloj.now() - TimeSpan(0,0,parada.minute(), parada.second()) != last_shown)                    // cambia el tiempo. Asumimos que ha pasado un segundo
  { 
    uni_sec =  '0' + (last_shown.second()%10);                          // cálculo de los digitos a imprimir en pantalla
    dec_sec =  '0' + (last_shown.second()/10);
    uni_min =  '0' + (last_shown.minute()%10);
    dec_min =  '0' + (last_shown.minute()/10);
    lDisplay.clear();
    lDisplay.setChar(6,uni_sec);                                        // impresión en pantalla
    lDisplay.setChar(12,dec_sec);
    lDisplay.setChar(16,':');
    lDisplay.setChar(23,uni_min);
    lDisplay.setChar(29,dec_min);
    last_shown =  (reloj.now() - TimeSpan(0,0,parada.minute(), parada.second()));   // actualización de la última  impresión en pantalla 
    if (adrenalina_flag)
      {    
      adr_time--;      
      refractar++;
      if (refractar == 5)
       {
        refractar = 0;
        adr_refractario = false;        
       }
      }      
  }
  displayIdle();
}
/********************************************
*
*     Impresion de la hora o la temperatura
*
*********************************************/
void displayIdle()
{
  char buffer[] = "hh:mm:ss";
  char Dbuffer[] = "DD/MM/YYYY";  
  uint8_t temperatura;
   
  now = reloj.now();
  temperatura = reloj.getTemperature();
  Oled.setTextSize(1); 
  Oled.clearDisplay();
  Oled.setCursor(0, 0);
  Oled.print(now.toString(Dbuffer));  
  //Oled.print(now.year());       
  Oled.setCursor(0, 10); 
  Oled.print(now.toString(buffer));          
  Oled.setCursor(0, 20);
 // Oled.print(Evento.primerEvento);
 // Oled.print("-");
 // Oled.print(Evento.ultimoEvento);
 // Oled.print("     ");  
  Oled.print(temperatura);
  Oled.print(" Grados");
  Oled.setCursor(0, 40);
  Oled.print(now.toString(buffer));
  Oled.display();
  if (parada_flag)
    ssDParada();
  else
    ssDNoParada();
 
}
void ssDParada()
{

  int8_t  minutos, segundos;
  minutos = adr_time / 60;
  segundos = adr_time % 60;
  if (adr_time < 0)
  {
   ssD1.clear();
    
  }

  // Ponemos el tiempo restante para la adrenalina en el primer display  
  ssD1.showNumberDecEx(minutos*100+segundos,64,true);  
  
  ssD2.showNumberDecEx(adr_count,0,false);  
  // y el contador en el segundo.
  
}

void ssDNoParada()
{

  uint8_t temperatura, hora, minuto,dospuntos; 
  uint8_t data[4];  // declaramos array para imprimir

  
  temperatura = reloj.getTemperature(); 
  hora = now.hour();
  minuto = now.minute();
  dospuntos = (now.second()%2 ) * 64; // Tomará el valor 0 cuando sea un segundo par, 64 si impar. 
                                      // Esto causará que los dos puntos del display se parpadeen con un ciclo de 1 Hz
  // Ponemos la hora en el primer display
  ssD1.showNumberDecEx(hora*100+minuto,dospuntos,true);  
  // Componemos el contenido del segundo display  (temperatura)
  data[0] = ssD2.encodeDigit(temperatura/10);
  data[1] = ssD2.encodeDigit(temperatura%10);
  data[2] = GRADO;
  data[3] = CENTIGRADO;
  ssD2.setSegments(data); 
}
/*******************************************
*
*   Display Oled mostrando las opciones de menú.
*
********************************************/
void displayMenu()
{
  int8_t  f,g;
  static char  opciones[IDLE+1][13];              // obligamos a que IDLE es siempre la última opción (salir). Entonces, ese es el número de entradas de menú.
  strcpy(opciones[SETUP_TIME], "Aj. hora");
  strcpy(opciones[REVISAR], "Revisar");
  strcpy(opciones[IDLE], "Salir");
  Oled.clearDisplay();
  Oled.setTextSize(1); 
  for(f=0; f<=IDLE; f++)                        // tantas pasadas de bucle como opciones
    {
      g=f+opc_actual;      
      if (g>IDLE)
        g=g-IDLE-1;        
      if (g<0)
        g++;  
      Oled.setCursor(0, 10*f);
      Oled.print(opciones[g]);
    }  
  Oled.display();
}
// Se ejecuta cuando se está ajustando la hora
void displaySetupTime()
{
   switch (setup_time_opcion)
    {
      case HORA:
        xdisplaySetupTime("hora",(uint16_t) now.hour());
        break;
      case MINUTOS:
        xdisplaySetupTime("minuto ",(uint16_t) now.minute());
        break;
      case DIA:
        xdisplaySetupTime("dia ",(uint16_t) now.day());
        break;
      case MES:
        xdisplaySetupTime("mes ",(uint16_t) now.month());
        break;
      case ANNO:
        xdisplaySetupTime("ano  ",now.year());      
        break;        

    }   
   
}
// Funcion auxiliar a la anterior
void xdisplaySetupTime(const String& item,uint16_t valor )
{
  Oled.clearDisplay();

  Oled.setTextSize(1); 
  Oled.setCursor(0, 0);
  Oled.print(item);

  Oled.setCursor(0, 10);
  Oled.print(valor);
  Oled.display();  
}
