//********************************************** I S R 

/*************************************************************
*
* Rutina de servicio de la interrupción del giro del encoder
*
**************************************************************/
void doEncode()
{
  long ms;
  ms = micros();
while(micros()-ms < 1000);
  if(accion == IDLE)
    return;    
    
if (accion == MENU)
  menuSelect();
if (accion == SETUP_TIME)
  setupTimeSelect();
  
if (accion == REVISAR)
  revisar();
}  
/*************************************************************
*
* Rutina de servicio de la interrupción del pulsador del encoder
*
**************************************************************/  
void doSwitch()
{
  long ms;
  ms = micros();
while(micros()-ms < 1000);  
if (parada_flag)              // si hay una parada en curso, se ignora la pulsación del encoder
  return;  
  
switch (accion)
 {
  case IDLE:
    accion = MENU;
    break;
  case MENU:
    accion = opc_actual;
    break;
  case SETUP_TIME:
    setup_time_opcion++;
    if (setup_time_opcion > ANNO )
      {
      time_adjust = true;
      accion = IDLE;
      setup_time_opcion = HORA;
      }      
    break;
  case REVISAR:
    accion = IDLE;
  }
}
// se ejecuta dentro de la interrupcion del encoder, sólo cuando está seleccionada la opción del menu 'ajuste de hora'
void setupTimeSelect()
{
switch (setup_time_opcion)  
  {
int8_t current;
int16_t yr;
  case  HORA:  
    current = now.hour();
    if  (digitalRead(ENC_UP) == digitalRead(ENC_DOWN))  // subiendo
    {    
    current ++;                    // incrementamos una hora
    if(current== 24)               // si hemos llegado al final
      current =  0;                // son las 0 h.
    } 
    else                              // bajando
    {
    current --;                   // decrementamos una hora
    if(current == -1)             // si hemos llegado al principio
      current =  23;              // volvemos a las 23h
    }    
    now = DateTime(now.year(),now.month(), now.day(), current,now.minute(), now.second());
    break;
  case  MINUTOS:  
    current = now.minute();
    if  (digitalRead(ENC_UP) == digitalRead(ENC_DOWN))  // subiendo
    {    
    current ++;                    // incrementamos un minuto
    if(current== 60)               // si hemos llegado al final
      current =  0;                // 0 m.
    } 
    else                              // bajando
    {
    current --;                   // decrementamos un minuto
    if(current == -1)             // si hemos llegado al principio
      current =  59;              // volvemos a los 59m
    }    
    now = DateTime(now.year(),now.month(), now.day(), now.hour(),current, now.second());
    break;    
  case  DIA:  
    current = now.day();
    if  (digitalRead(ENC_UP) == digitalRead(ENC_DOWN))  // subiendo
    {    
    current ++;                    // incrementamos un dia
    if(current > 31)               // si hemos llegado al final
      current =  1;                // dia 1 
    } 
    else                              // bajando
    {
    current --;                   // decrementamos un dia
    if(current < 1)               // si hemos llegado al principio
      current =  31;              // volvemos al 31
    }    
    now = DateTime(now.year(),now.month(), current, now.hour(),now.minute(), now.second());    
    break;    
  case  MES:  
    current = now.month();
    if  (digitalRead(ENC_UP) == digitalRead(ENC_DOWN))  // subiendo
    {    
    current ++;                    // incrementamos un mes
    if(current > 12)               // si hemos llegado al final
      current =  1;                // volvemos a enero
    } 
    else                              // bajando
    {
    current --;                   // decrementamos un mes
    if(current < 1)               // si hemos llegado al principio
      current =  12;              // volvemos a diciembre
    }    
    now = DateTime(now.year(),current, now.day(), now.hour(),now.minute(), now.second());    
    break;    
  case  ANNO: 
    yr = now.year();
#ifdef __DEBUG__
 Serial.print ("leidoaño: ");
 Serial.println((uint16_t) yr);
#endif
    
    if  (digitalRead(ENC_UP) == digitalRead(ENC_DOWN))  // subiendo
      yr ++;                    // incrementamos un año
    else                             // bajando
      yr --;                    // decrementamos un año      
    now = DateTime(yr, now.month(), now.day(), now.hour(),now.minute(), now.second());
#ifdef __DEBUG__
 Serial.print ("Establecido año: ");
 Serial.println((uint16_t) yr);
#endif


    
    break;        
  }
}
// Se ejecuta dentro de la interrupción asociada al encoder cuando está activado el menu
void menuSelect()
{
  if  (digitalRead(ENC_UP) == digitalRead(ENC_DOWN))
    {    
    opc_actual++;
    if(opc_actual >IDLE )
      opc_actual = 0;
    } 
  else
    {
    opc_actual--;      
    if(opc_actual<0)
      opc_actual = IDLE;
    }    
}
// se ejecuta dentro de la interrupcion del encoder, sólo cuando está seleccionada la opción del menu 'revisar'
void revisar()
{
  if  (digitalRead(ENC_UP) == digitalRead(ENC_DOWN))
    {    
    idEventoRevisar++;
    if(idEventoRevisar > Evento.tam )
      idEventoRevisar = 0;
    } 
  else
    {
    idEventoRevisar--;      
    if(idEventoRevisar < 0)
      idEventoRevisar = Evento.tam;
    }    
}