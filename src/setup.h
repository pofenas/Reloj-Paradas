/*************************************** INICIALIZACIÓN DE ELEMENTOS *****************************************/

/***********************************************
*
*       Setup de la matriz de leds
*
************************************************/
void MatrizSetup()
{
  lDisplay.begin();
  lDisplay.control(MD_MAX72XX::TEST,true);
  delay(1000);
  lDisplay.control(MD_MAX72XX::TEST,false) ; 
}
/***********************************************
*
*       Setup de la pantallita Oled
*
************************************************/
void OledSetup()
{

#ifdef __DEBUG__

  Serial.println("Iniciando pantalla OLED.");
#endif
  // Iniciar pantalla OLED en la dirección 0x3C
  if (!Oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
#ifdef __DEBUG__
    Serial.println("No se encuentra la pantalla OLED.");
#endif
    while (true);
  }
  Oled.clearDisplay();
  // Tamaño del texto
  Oled.setTextSize(2);
  // Color del texto
  Oled.setTextColor(SSD1306_WHITE);
  Oled.display();  // pantalla borrada uy lista para trabajar
}
/***************************************************
*
*         Setup del reloj en tiempo real
*
****************************************************/
void RelojSetup()
{
#ifdef __DEBUG__
  Serial.println("Iniciando Reloj en tiempo real.");
#endif
  if (! reloj.begin())
  {
#ifdef __DEBUG__
    Serial.println("No se encuentra RTC.");
    Serial.flush();
#endif
    while (true);
  }
}

/***************************************************
*
*         Setup de los displays de 7 segmentos
*
****************************************************/
void ssDisplaySetup()

{
  ssD1.setBrightness(0x0f);
  ssD2.setBrightness(0x0f);
  ssD1.clear();
  ssD2.clear();
  
}