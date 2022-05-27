//Libreria DHT11
#include <dht.h>
// GPIO's de entrada: Sensores
dht DHT;
#define DHT11_PIN 8
#define SENSOR_HUMEDAD A0
#define SENSOR_AGUA A1

// GPIO's de entrada: botones
#define LUZ_BOTTON 7
#define BOTTON_IMPRIMIR 6
#define BOTTON_REGAR 5
#define BOTTON_SUBIR 4
#define BOTTON_BAJAR 3

// GPIO's de salida actuadores
#define ALARMA_BUZZER 13
#define CONTROL_RELAY 12
#define CONTROL_REGAR 11

// actuador LCD:
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// dISPLAY DE 2 LINEAS CON 16 CARACTERES
LiquidCrystal_I2C lcd(0x27,16,2);

/*
Porcentaje de agua:
La alarma sonara si el sensor detecta un porcentaje de agua menor a esta variable
*/
float PORCENTAJE_AGUA_UMBRAL = 30.0;
float PORCENTAJE_HUMEDAD_UMBRAL = 50.0;

// Declarar varaibles para guardar los valores de los sensores
float LECTURA_AGUA = 0.0,LECTURA_HUMEDAD = 0.0,LECTURA_TEMPERATURA = 0.0;
byte LECTURA_BOTON_LUZ, LECTURA_BOTON_IMPRIMIR, LECTURA_BOTON_REGAR, LECTURA_BOTON_SUBIR, LECTURA_BOTON_BAJAR; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  
  pinMode(ALARMA_BUZZER,OUTPUT);
  pinMode(CONTROL_RELAY,OUTPUT);
  pinMode(CONTROL_REGAR,OUTPUT);

  pinMode(LUZ_BOTTON,INPUT_PULLUP);
  pinMode(BOTTON_IMPRIMIR,INPUT_PULLUP);
  pinMode(BOTTON_REGAR,INPUT_PULLUP);
  pinMode(BOTTON_SUBIR,INPUT_PULLUP);
  pinMode(BOTTON_BAJAR,INPUT_PULLUP);
  
}

// Lectura de la Temperatura
void lectura_temperatura(void) {
  DHT.read11(DHT11_PIN);
  LECTURA_TEMPERATURA = DHT.temperature;
  delay(500);
  }
  
// Tomar lectura del sensor de temperatura y normalizar 
void lectura_nivel_agua(void) {
  LECTURA_AGUA = analogRead(SENSOR_AGUA);
  LECTURA_AGUA = LECTURA_AGUA * 100.0/1024.0; // Normalizar a 100& (Regla de 3)
  delay(500);
}

// Hacer solar la alarma si el porcentaje de agua esta por debajo del umbral 
void alarmas(void) {
  // activar la alarma si el nivel de agua es menor al porcentaje definido arriba.
  if(LECTURA_AGUA < PORCENTAJE_AGUA_UMBRAL) {
    lcd.clear();
    lcd.print("Nivel de");
    lcd.setCursor(0,1);
    lcd.print("agua bajo!!!");
    lcd.blink();
    tone(ALARMA_BUZZER,2000,2000);
    delay(1000);
  }
}

// tomar lecturas sensor
void lectura_humedad_suelo(void) {
  LECTURA_HUMEDAD = analogRead(SENSOR_HUMEDAD);
  LECTURA_HUMEDAD = LECTURA_HUMEDAD * 100.0/1024.0;
  LECTURA_HUMEDAD = 100-LECTURA_HUMEDAD;
}


void menu_principal(void) {
  lcd.clear();
  lcd.print("1: Luz | 2: Stat");
  lcd.setCursor(0,1);
  lcd.print("3: Reg | 4: Hum");
  lcd.blink();
  delay(500);
}

// Lectura del Boton de luz 
void lectura_botones(void) {
  LECTURA_BOTON_LUZ = digitalRead(LUZ_BOTTON);
  LECTURA_BOTON_IMPRIMIR = digitalRead(BOTTON_IMPRIMIR);
  LECTURA_BOTON_REGAR = digitalRead(BOTTON_REGAR);
  LECTURA_BOTON_SUBIR = digitalRead(BOTTON_SUBIR);
  LECTURA_BOTON_BAJAR = digitalRead(BOTTON_BAJAR);
}

void asignar_funcion(void) {
  if(LECTURA_BOTON_LUZ == 0) {
    control_luz();
  }
  else if(LECTURA_BOTON_IMPRIMIR == 0) {
    imprimir_variables();
  }
  else if(LECTURA_BOTON_REGAR == 0) {
    regar();
  }
  else if(LECTURA_BOTON_SUBIR == 0) {
    ajustar_humedad();
  } 
}

void control_luz(void) {
  digitalWrite(CONTROL_RELAY,!digitalRead(CONTROL_RELAY));
  delay(300);   
}

void regar(void) {
  digitalWrite(CONTROL_REGAR,!digitalRead(CONTROL_REGAR));
  delay(300);  
}

void ajustar_humedad(void) {
  String imprimir;
  while(digitalRead(BOTTON_REGAR)==1) {
      imprimir = "Umbral: ";
      imprimir += PORCENTAJE_HUMEDAD_UMBRAL;
      lcd.clear();
      lcd.print(imprimir);
      lcd.blink();
      delay(500);
      if(digitalRead(BOTTON_SUBIR) == 0){
        PORCENTAJE_HUMEDAD_UMBRAL = PORCENTAJE_HUMEDAD_UMBRAL + 5.0;
        }
      else if(digitalRead(BOTTON_BAJAR) == 0){
        PORCENTAJE_HUMEDAD_UMBRAL = PORCENTAJE_HUMEDAD_UMBRAL - 5.0;
        }
      
      if(PORCENTAJE_HUMEDAD_UMBRAL > 90.0){
        PORCENTAJE_HUMEDAD_UMBRAL = 80.0;
        break;
        }
      else if(PORCENTAJE_HUMEDAD_UMBRAL < 10.0){
        PORCENTAJE_HUMEDAD_UMBRAL = 20.0;
        break;
        }
    }
  
  }

void imprimir_variables(void){
  String renglon1 = "Temp: ";
  String renglon2 = "Hum: ";
  renglon1 += LECTURA_TEMPERATURA;
  renglon2 += LECTURA_HUMEDAD;
  
  lcd.clear();
  lcd.print(renglon1);
  lcd.setCursor(0,1);
  lcd.print(renglon2);
  lcd.blink();
  delay(3000);
  }

 void control_humedad(void) {
  if(LECTURA_HUMEDAD < PORCENTAJE_HUMEDAD_UMBRAL){
    String hum = "Humedad: ";
    hum += LECTURA_HUMEDAD;
    lcd.clear();
    lcd.print("Regando...");
    lcd.setCursor(0,1);
    lcd.print(hum);
    lcd.blink();    
    if(digitalRead(CONTROL_REGAR) == 0){
      digitalWrite(CONTROL_REGAR,HIGH);
      }
    delay(5000);
    digitalWrite(CONTROL_REGAR,LOW);
    }
  }

void loop() {
  // put your main code here, to run repeatedly:
  // tomar lecturas de los sensores
  lectura_nivel_agua();
  lectura_temperatura();
  lectura_humedad_suelo();

  // saltar alarmas
  alarmas();
  
  // Menu principal
  menu_principal();
  lectura_botones();
  asignar_funcion();
  
  // control humedad
  control_humedad();
  
}
