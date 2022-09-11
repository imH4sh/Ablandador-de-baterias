#include <LiquidCrystal.h>
#include <EasyButton.h>
#define RELAY_ON LOW
#define RELAY_OFF HIGH
#define pinPulsador 12
//Variables globales
LiquidCrystal lcd(8,9,4,5,6,7);
int rele1 = 2;//rele del cargador definido como pin 2
int rele2 = 3;//rele del descargador o carga definido como pin 3
int alarma = 10;
int tiempodeCargaMinutos;
int tiempodeDescargaMinutos;
int estadosys[5] = {1, 2, 3, 4, 5}; //el 5 es programa terminado
int vecesBotonPresionado = 0;
int estadoBoton = 0;
int estadoPrevioBoton = 0;
int vecessys;
int pulsadorEst2 = 0;
float temperatura[4]; // Array temperatura de 4
float corriente;
float tension;
bool yaCorrio = false;
bool necesitaDescarga = false;
bool carga=false;
bool pullup = true;
bool descarga;
bool yaCorrioPulsador = false;
bool advCarga = false;
bool terminarBatCargadas = false;
unsigned long ultimaDescarga = 0;
unsigned long ultciclo = 0; //Ultima vez que se realizo el ciclo
unsigned long periodo = 5000; // Periodo
unsigned long periodo2 = 10000;//perioido2
unsigned long ultciclo2 = 0; //periodo3
unsigned long tiempocarga; //Cronometro de carga
unsigned long tiempodescarga; //Tiempo de descarga
unsigned long tiempodeinicio = millis();
unsigned long tiempocargaAlgo;
uint32_t debounce = 40;
EasyButton Pulsador(pinPulsador, debounce, pullup); //Instancia Pulsador de clase EasyButton


void setup(){
    Serial.begin(9600);
    lcd.begin(20,4);
    Pulsador.begin();
    pinMode(rele1, OUTPUT);
    pinMode(rele2, OUTPUT);
    pinMode(alarma, OUTPUT);

}

void loop(){
    if(yaCorrio == false){
        Serial.println("Iniciando...");
        lcd.setCursor(0,0);
        lcd.print("Cargando...");
        lcd.setCursor(0,1);
        lcd.print("Creado por EEST N1");
        lcd.setCursor(0,2);
        lcd.print("6to III - 2022");
        lcd.setCursor(0,3);
        lcd.print("bit.ly/proyecto");
        delay(10000);
        lcd.clear();
        yaCorrio = true;
        checkearTemperatura();
    }
    while(yaCorrioPulsador == false){
        if(advCarga==false){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Tiene 1 minuto para");
            lcd.setCursor(0,1);
            lcd.print("elegir veces a");
            lcd.setCursor(0,2);
            lcd.print("ciclear.");
            lcd.setCursor(0,3);
            lcd.print("0 veces = infinito.");
            delay(10000);
            advCarga = true;
            lcd.clear();
        }
        while(millis() - tiempodeinicio < 60000){
            Serial.println("Presione el boton para elegir cantidad de veces a ciclear. Veces seleccionadas:" + String(vecesBotonPresionado));
            lcd.setCursor(0,0);
            lcd.print("Utilize el boton y");
            lcd.setCursor(0,1);
            lcd.print("pulse las veces a");
            lcd.setCursor(0,2);
            lcd.print("ciclear. Tiene 1min.");
            lcd.setCursor(0,3);
            lcd.print("Veces:");
            lcd.setCursor(7,3);
            if(vecesBotonPresionado == 0){
             lcd.print("0"); 
            }
            estadoBoton = Pulsador.read();
            if(estadoBoton != estadoPrevioBoton){
                if(estadoBoton == LOW){
                    vecesBotonPresionado++;
                    lcd.setCursor(7,3);
                    lcd.print("  ");
                    lcd.setCursor(7,3);
                    lcd.print(vecesBotonPresionado);
                    }
                else{

                }
                delay(200);
            }
            estadoPrevioBoton = estadoBoton;
        }
        Serial.println("Antes del unsigned");
        unsigned long tiempoInicioEstadoFinal = millis();
        while(millis() - tiempoInicioEstadoFinal < 700000){
          Serial.println("Adentro");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("¿Terminar ciclo/s");
          lcd.setCursor(0,1);
          lcd.print("con bat cargadas?");
          lcd.setCursor(0,2);
          lcd.print("Presione para si,");
          lcd.setCursor(0,3);
          lcd.print("espere 10s para no.");
          pulsadorEst2 = Pulsador.read();
          if(pulsadorEst2 == HIGH){
            terminarBatCargadas = true;
            delay(5000);
            break;
            }
          else{
            }
        }
        yaCorrioPulsador = true;
    }

    if(vecesBotonPresionado == 0){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Ciclos elegidos:");
        lcd.setCursor(0,1);
        lcd.print("Infinitos.");
        delay(5000);
        vecessys = 1;
        for (;;){
            logicaPrincipal();
        }
        
    }
    else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Ciclos elegidos:");
        lcd.setCursor(0,1);
        lcd.print(vecesBotonPresionado);
        delay(5000);
        vecessys = 2;
        for(int vhecho = 0; vhecho < vecesBotonPresionado; vhecho++){
            logicaPrincipal();
        }

    }
    for(;;){
        Serial.println("Programa terminado. Reiniciar dispositivo para volver a empezar");
        funciondisplay(estadosys[4]);
    }



    

}

//Logica principal del programa.
void logicaPrincipal(){
    checkearTemperatura();
    lecturaTension();
    if(tension < 13.7){
        digitalWrite(rele1, RELAY_ON); //Carga conectada
        Serial.println("Cargador conectado");
        tiempocarga = millis();
        while (tension <17.1){
            lecturaTension();
            checkearTemperatura();
            funciondisplay(estadosys[0]);
            delay(5000);
        }
    }
    if(tension >= 17){
        digitalWrite(rele1, RELAY_OFF); //Cargador desconectado
        Serial.println("Cargador desconectado");
        unsigned long espera1 = millis();
        while((millis() - espera1)<1800000){
            lecturaTension();
            checkearTemperatura();
            funciondisplay(estadosys[2]);
            delay(5000);
        }
        digitalWrite(rele2, RELAY_ON); //Descargador conectado
        Serial.println("Descargador conectado");
        tiempodescarga = millis();
        lecturaTension();
        while(tension >= 10){
            lecturaTension();
            checkearTemperatura();
            funciondisplay(estadosys[1]);
            delay(5000);

        }
        digitalWrite(rele2, RELAY_OFF); //Descargador desconectado
        Serial.println("Descargador desconectado");
        unsigned long espera2 = millis();
        while((millis() - espera2)<1800000){
            lecturaTension();
            checkearTemperatura();
            funciondisplay(estadosys[2]);
            delay(5000);
        }
    }
    if (terminarBatCargadas == true){
            digitalWrite(rele1, RELAY_ON); // Carga conectada
            Serial.println("Cargador conectado");
            tiempocarga = millis();
            while (tension < 17.1){
                lecturaTension();
                checkearTemperatura();
                funciondisplay(estadosys[0]);
                delay(5000);
            }
            digitalWrite(rele1, RELAY_OFF); // Cargador desconectado
            Serial.println("Cargador desconectado");
            unsigned long esperaAlgo = millis();
            while ((millis() - esperaAlgo) < 1800000){
                lecturaTension();
                checkearTemperatura();
                funciondisplay(estadosys[2]);
                delay(5000);
                }
    }
}

// Lee, actualiza y comprueba temperatura. Si la temp es +=50 apaga todo y salta la alarma. 
void checkearTemperatura(){
    unsigned long sens1 = 0;
    unsigned long sens2 = 0;
    unsigned long sens3 = 0;
    unsigned long sens4 = 0;

    for (int i = 0; i < 100; i++) //100 lecturas de cada sensor
    {
        //sens1 += analogRead(A2);
        sens2 += analogRead(A3);
        //sens3 += analogRead(A4);
        //sens4 += analogRead(A5);
    }
    //temperatura[0] = (5.0 * (sens1 / 100) * 100.0) / 1024.0; //Promediacion de 100 y calculo temps finales 
    temperatura[0] = 25; //(5.0 * (sens2 / 100) * 100.0) / 1024.0; 
    temperatura[1] = 25; 
    temperatura[2] = 25;
    temperatura[3] = 25;
    //temperatura[3] = (5.0 * (sens4 / 100) * 100.0) / 1024.0; 
    if (temperatura[0] >= 50 || temperatura[1] >= 50 || temperatura[2] >= 50 || temperatura[3] >= 50){
        digitalWrite(rele1, RELAY_OFF);
        digitalWrite(rele2, RELAY_OFF);
        digitalWrite(alarma, HIGH);
        Serial.println("Alerta: Temperatura muy alta. Se detuvieron todos los procesos.");
        for ( ; ; ){
            funciondisplay(estadosys[3]);
            delay(57000);
        }
        


    }
    
}

void lecturaCorriente(){
    float sensibilidad = (-0.072);                         // Sensibilidad del sensor de corriente
    float voltajesensor = analogRead(A0) * (5.0 / 1023.0); // lectura del sensor (0-1023)
    corriente = (voltajesensor - 2.5) / sensibilidad;      // ecuacion par obtener corriente
    Serial.println(corriente);

}

void lecturaTension(){
  int valorSensorRaw;//valor leido por el sensor (0-1023)
  valorSensorRaw = analogRead(A1);// valor leido del sensor tension 
  tension = (float(valorSensorRaw) - 0) * (25.0 - 0.0) / (1023 - 0) + 0.0;// ecuacion para obtener tension
  }


//Tipo de dato string y la concha de su madre
String tiempotranscurrido(int minutos){
    int horas = int((minutos / 60));
    int min = minutos - (horas * 60);
    String tiempo = String(horas) + "h " + String(min) + "m ";
    return tiempo;

}

//Controla el display
void funciondisplay(int estado) {

  if (estado == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("V: ");
    lcd.setCursor(3, 0);
    lcd.print(tension);
    lcd.setCursor(9, 0);
    lcd.print("A: ");
    lcd.setCursor(12, 0);
    lcd.print(corriente);
    lcd.setCursor(0, 1);
    lcd.print("T1: ");
    lcd.setCursor(4, 1);
    lcd.print(String(temperatura[0]));
    lcd.setCursor(10, 1);
    lcd.print("T2: ");
    lcd.setCursor(15, 1);
    lcd.print(String(temperatura[1]));
    lcd.setCursor(0, 2);
    lcd.print("T3: ");
    lcd.setCursor(4, 2);
    lcd.print(String(temperatura[2]));
    lcd.setCursor(10, 2);
    lcd.print("T4: ");
    lcd.setCursor(15, 2);
    lcd.print(String(temperatura[3]));
    tiempodeCargaMinutos = int((millis() - tiempocarga) / 60000);
    String tiempodeCargaFinal = tiempotranscurrido(tiempodeCargaMinutos);
    lcd.setCursor(0, 3);
    lcd.print("TCarga: ");
    lcd.setCursor(8, 3);
    lcd.print(tiempodeCargaFinal);
    }
    

  if (estado == 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("V: ");
    lcd.setCursor(3, 0);
    lcd.print(tension);
    lcd.setCursor(9, 0);
    lcd.print("A: ");
    lcd.setCursor(13, 0);
    lcd.print(corriente);
    lcd.setCursor(0, 1);
    lcd.print("T1: ");
    lcd.setCursor(4, 1);
    lcd.print(String(temperatura[0]));
    lcd.setCursor(10, 1);
    lcd.print("T2: ");
    lcd.setCursor(15, 1);
    lcd.print(String(temperatura[1]));
    lcd.setCursor(0, 2);
    lcd.print("T3: ");
    lcd.setCursor(4, 2);
    lcd.print(String(temperatura[2]));
    lcd.setCursor(10, 2);
    lcd.print("T4: ");
    lcd.setCursor(14, 2);
    lcd.print(String(temperatura[3]));
    tiempodeDescargaMinutos = int((millis() - tiempodescarga) / 60000);
    String tiempodeDescargaFinal = tiempotranscurrido(tiempodeDescargaMinutos);
    lcd.setCursor(0, 3);
    lcd.print("TDescarga: ");
    lcd.setCursor(11, 3);
    lcd.print(tiempodeDescargaFinal);
    }

  if (estado == 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("V: ");
    lcd.setCursor(3, 0);
    lcd.print(tension);
    lcd.setCursor(9, 0);
    lcd.print("A: ");
    lcd.setCursor(13, 0);
    lcd.print(corriente);
    lcd.setCursor(0, 1);
    lcd.print("T1: ");
    lcd.setCursor(4, 1);
    lcd.print(String(temperatura[0]));
    lcd.setCursor(10, 1);
    lcd.print("T2: ");
    lcd.setCursor(14, 1);
    lcd.print(String(temperatura[1]));
    lcd.setCursor(0, 2);
    lcd.print("T3: ");
    lcd.setCursor(4, 2);
    lcd.print(String(temperatura[2]));
    lcd.setCursor(10, 2);
    lcd.print("T4: ");
    lcd.setCursor(14, 2);
    lcd.print(String(temperatura[3]));
    lcd.setCursor(0, 3);
    lcd.print("Estado: reposando.");
    }

  if (estado == 4) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sobrecalentamiento!");
    lcd.setCursor(0, 1);
    lcd.print("T1: ");
    lcd.setCursor(4, 1);
    lcd.print(String(temperatura[0]));
    lcd.setCursor(10, 1);
    lcd.print("T2: ");
    lcd.setCursor(14, 1);
    lcd.print(String(temperatura[1]));
    lcd.setCursor(0, 2);
    lcd.print("T3: ");
    lcd.setCursor(4, 2);
    lcd.print(String(temperatura[2]));
    lcd.setCursor(10, 2);
    lcd.print("T4: ");
    lcd.setCursor(14, 2);
    lcd.print(String(temperatura[3]));
    lcd.setCursor(0, 3);
    lcd.print("No se acerque.");
    }

  if (estado == 5) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Finalizado.");
    lcd.setCursor(0, 1);
    lcd.print("Para operar de nuevo");
    lcd.setCursor(0, 2);
    lcd.print("reinicie el");
    lcd.setCursor(0, 3);
    lcd.print("dispositivo.");
    }
}

  
