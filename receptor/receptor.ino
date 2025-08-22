#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

String inString = "";
String MyMessage = "";

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

#define BAND 915E6

#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

int rssi;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void startOLED() {
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA SENDER");
}

void startLoRA() {
  int counter = 0;
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    Serial.println("Starting LoRa failed!");
  }
  display.setCursor(0, 10);
  display.clearDisplay();
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

void setup() {
  Serial.begin(9600);
  startOLED();
  startLoRA();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    String newMessage = ""; // Inicializar la variable antes de entrar al bucle
    
    while (LoRa.available()) {
      int inChar = LoRa.read();
      if (isDigit(inChar) || inChar == '.') {
        inString += (char)inChar;
        newMessage = inString;
      }
    }
    
    inString = "";
    rssi = LoRa.packetRssi();
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Recibiendo mensaje:");
    display.setCursor(0, 10);
    display.print(newMessage);

    display.setCursor(0, 20);
    display.print("RSSI:");
    display.setCursor(0, 30);
    display.print(rssi);

    display.display();
    
    if (!newMessage.isEmpty()) {
      if (newMessage != MyMessage) {
        MyMessage = newMessage;
        Serial.println(MyMessage);
      }
    }
  }
  delay(2000);
  
}
