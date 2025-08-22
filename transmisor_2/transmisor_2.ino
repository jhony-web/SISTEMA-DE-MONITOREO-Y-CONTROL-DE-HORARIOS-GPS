#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial serialgps(2, 3);  // Pines RX (2) y TX (3)
TinyGPSPlus gps;

String MyMessage="";

String mensaje="";

const int ledAzulPin = 5;
const int ledRojoPin = 6;



const float puntos[][2] = {
  {-16.4671744,-71.5515102}, // paradero
                        
  {-16.4603719,-71.5563288}, // oriol 

  {-16.4555304,-71.5527112}, //alto alianza 
   
  {-16.4493842,-71.5528921}, // la 25 
  {-16.4431518,-71.5559043}, // brasilia 
  
  {-16.4396517,-71.5593734},  // bcp hunter 
  {-16.4315882,-71.5614924},  //iglecia tingo
  {-16.421347,-71.5532341}, //FERRREYROS
  {-16.4108863,-71.5439016},//TOTTUS
  {-16.4088863,-71.5370656},//Iglecia Nuestra Señora del Pilar
  {-16.4033207,-71.5288485}, //Paucarpata
  {-16.4088863,-71.5370656},//iglecia nuestra señora del pilar
  {-16.4108863,-71.5439016},//tottus
  {-16.421347,-71.5532341}, //FERRREYROS
  {-16.4315882,-71.5614924},  //Iglecia de tingo
  {-16.4396517,-71.5593734},  // BCP hunter 
  {-16.4431518,-71.5559043}, // brasilia 
  {-16.4493842,-71.5528921}, // la 25 
  {-16.4555304,-71.5527112}, //alto alianza 
  {-16.4603719,-71.5563288} // oriol 

  
}; 




int puntoActual = 1;
bool bucleEjecutado = false;  // Variable de control

//unsigned long distancia_final = 0;

const unsigned long tiempoEspera = 30000;  // Tiempo de espera en milisegundos (1 minuto)
unsigned long tiempoInicioMedicion = 0;
unsigned long tiempo2 = 30000;

const int d_max = 75;


// Radio de la Tierra en metros
const double R = 6371000.0;

// Función para convertir grados decimales a radianes
double toRadians(double grados) {
  return grados * PI / 180.0;
}

// Función para calcular la distancia haversine entre dos puntos geográficos
double haversine(double lat1, double lon1, double lat2, double lon2) {
  double dLat = toRadians(lat2 - lat1);
  double dLon = toRadians(lon2 - lon1);
  double a = sin(dLat / 2.0) * sin(dLat / 2.0) + cos(toRadians(lat1)) * cos(toRadians(lat2)) * sin(dLon / 2.0) * sin(dLon / 2.0);
  double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
  return R * c;  // R es el radio de la Tierra en metros (aproximadamente 6371000 metros)
}

void setup() {
  Serial.begin(9600);
  serialgps.begin(9600);

  while(!Serial);
  Serial.println("Nodo Transmisor");
  
  if (!LoRa.begin(915E6)){
    Serial.println("Starting Lora failed");
    while(1);
  }

  pinMode(ledAzulPin, OUTPUT);
  pinMode(ledRojoPin, OUTPUT);
}

void loop() {
  unsigned long tiempoInicio = millis();
  Serial.println(tiempoInicio);


    if (serialgps.available() > 0) {
      if (gps.encode(serialgps.read())) {
      //if (gps.location.isValid()) {
      
        
        // Verifica si ha pasado el tiempo de espera antes de realizar la medición

        if(tiempoInicio > (tiempoInicioMedicion + tiempoEspera)) {
          tiempoInicioMedicion = millis();

         

          unsigned long tiempoActual = millis();

          puntoActual %= (sizeof(puntos) / sizeof(puntos[0]));

          double distancia_final = 0.0;  // Variable para almacenar la distancia final
       
          while (millis() < (tiempoActual + tiempo2)) {

            
          
            if (!bucleEjecutado) {
              double sumaDistancias = 0.0;  // Variable para acumular las distancias
              
              for (int i = 0; i < 50; i++) {
                // Tu código aquí
                // Actualiza la ubicación del GPS cada vez que se recibe un nuevo dato
                double latitudActual = gps.location.lat();
                double longitudActual = gps.location.lng();

                // Imprime la distancia en metros
                Serial.println(gps.location.lat(), 5);
                Serial.println(gps.location.lng(), 5);
          
                // Utiliza las coordenadas del punto de destino (puntos[1])
                double latitudDestino = puntos[puntoActual][0];
                double longitudDestino = puntos[puntoActual][1];
            
                // Calcula la distancia haversine entre la ubicación actual y el punto de destino
                double distancia_1 = haversine(latitudActual, longitudActual, latitudDestino, longitudDestino);
            
                sumaDistancias += distancia_1;  // Acumula las distancias
                digitalWrite(ledAzulPin, HIGH);

                
              }
              
            
               // Calcula el promedio dividiendo la suma de distancias por la cantidad de iteraciones
              distancia_final = sumaDistancias / 50;
            
              Serial.print("Distancia final promedio: ");
              Serial.println(distancia_final);
              bucleEjecutado = true;  // Marcar que el bucle se ha ejecutado
              //Serial.println("Bucle ejecutado una vez");
            }
            digitalWrite(ledAzulPin, LOW);
            Serial.println("Distancia a punto de destino: " + String(distancia_final) + " metros");

            
            LoRa.beginPacket();
            LoRa.println(distancia_final, 2);
            LoRa.endPacket();
            Serial.println("Enviando mensaje");
          
            if (distancia_final > d_max) {
              Serial.println("La distancia es mayor a 75 metros");
              digitalWrite(ledRojoPin, HIGH);

              
            } if (distancia_final < d_max){

              
               digitalWrite(ledRojoPin, LOW);
              }
          
          }

           puntoActual++;
           bucleEjecutado = false;  // Restablecer la variable de control para la siguiente iteración
        }

        
      //}
    }
}
}
