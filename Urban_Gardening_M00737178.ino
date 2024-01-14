#include "Arduino_LED_Matrix.h"
#include "WiFiS3.h"
#include <DHT.h>

#define LED1 2
#define LED2 4

#define MOISTURE_SENSOR_A0 A0
#define MOISTURE_SENSOR_A1 A1

#define LIGHT_SENSOR_2 A2
#define LIGHT_SENSOR_3 A3

#define DHTTYPE DHT11  // Define the type of DHT sensor as DHT11

#define DHTPIN4 A4  // DHT sensor 1 connected to A4
#define DHTPIN5 A5  // DHT sensor 2 connected to A5

DHT dht4(DHTPIN4, DHTTYPE);  // Initialize DHT sensor 4 as DHT11
DHT dht5(DHTPIN5, DHTTYPE);  // Initialize DHT sensor 5 as DHT11


//Global Variable Declaration
int moistureValue = 0;
int moistureValueA1 = 0;
int lightValue1 = 0;
int lightValue2 = 0;
float temp4 = 0.0;
float hum4 = 0.0;
float temp5 = 0.0;
float hum5 = 0.0;


ArduinoLEDMatrix matrix;

const uint32_t hi[] = {
  0xcdfcdfcc,
  0x4fc4fc4c,
  0xc4cdfcdf,
  66
};

char ssid[] = "SKY1F9EE";    // Enter your WIFI SSID
char pass[] = "RQDWLCRNLQ";  // Enter your WIFI password

String outputLED1 = "off";
String outputLED2 = "off";
String output = "off";
String header;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  matrix.begin();

  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, HIGH);  // Start with LED1 off

  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, HIGH);  // Start with LED2 off

  dht4.begin();  // Initialize the DHT sensor connected to A4
  dht5.begin();  // Initialize the DHT sensor connected to A5
  delay(2000);   // Delay for sensor stabilization

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();
  printWifiStatus();
}


void loop() {
  moistureValue = analogRead(MOISTURE_SENSOR_A0);
  moistureValueA1 = analogRead(MOISTURE_SENSOR_A1);

  lightValue1 = analogRead(LIGHT_SENSOR_2);
  lightValue2 = analogRead(LIGHT_SENSOR_3);

  temp4 = dht4.readTemperature();
  hum4 = dht4.readHumidity();

  if (isnan(temp4) || isnan(hum4)) {
    Serial.println("Failed to read from DHT sensor 4!");
    // Handle error...
  }


  temp5 = dht5.readTemperature();
  hum5 = dht5.readHumidity();

  if (isnan(temp5) || isnan(hum5)) {
    Serial.println("Failed to read from DHT sensor 5!");
    // Handle error...
  }


  webServer();
  LEDMatrix();
}

void LEDMatrix() {
  matrix.loadFrame(hi);
}

void webServer() {
  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    unsigned long currentTime = millis();
    unsigned long previousTime = currentTime;
    const long timeoutTime = 2000;

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /LED1/on") >= 0) {
              outputLED1 = "on";
              digitalWrite(LED1, LOW);
            } else if (header.indexOf("GET /LED1/off") >= 0) {
              outputLED1 = "off";
              digitalWrite(LED1, HIGH);
            }

            if (header.indexOf("GET /LED2/on") >= 0) {
              outputLED2 = "on";
              digitalWrite(LED2, LOW);
            } else if (header.indexOf("GET /LED2/off") >= 0) {
              outputLED2 = "off";
              digitalWrite(LED2, HIGH);
            }


            ////////////////////////////////////////////////////////START//////////////////////////////////////////////////////////////////////////
            //webServer() function, HTML and CSS part
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>");
            client.println("body { margin: 0; font-family: Arial, sans-serif; background-color: #d8f3dc; }");
            client.println(".navbar { display: flex; justify-content: center; align-items: center; background-color: #52b788; padding: 10px; }");
            client.println(".navbar h1 { color: white; }");
            client.println(".main { display: flex; flex-wrap: wrap; justify-content: space-evenly; gap: 20px; padding: 20px; }");
            client.println(".card { background-color: #b7e4c7; border: 2px solid #40916c; border-radius: 10px; padding: 20px; width: calc(50% - 40px); box-sizing: border-box; text-align: center; }");
            client.println(".card h3 { font-weight: bold; color: #1b4332; }");
            client.println(".card button { background-color: #95d5b2; color: #081c15; border: 1px solid #2d6a4f; border-radius: 5px; padding: 10px 20px; margin: 10px; cursor: pointer; }");
            client.println("</style>");
            client.println("</head>");

            client.println("<body>");

            client.println("<div class=\"navbar\">");
            client.println("<h1>URBAN GARDENING</h1>");
            client.println("</div>");

            // Inside webServer() function:

            client.println("<div class=\"main\">");

            // Group LED 1 and LED 2 in one card
            client.println("<div class=\"card\">");
            client.println("<h3>LED Controls</h3>");
            client.println("<div>LED 1: ");
            if (outputLED1 == "off") {
              client.println("<a href=\"/LED1/on\"><button>ON</button></a>");
            } else {
              client.println("<a href=\"/LED1/off\"><button>OFF</button></a>");
            }
            client.println("</div><div>LED 2: ");
            if (outputLED2 == "off") {
              client.println("<a href=\"/LED2/on\"><button>ON</button></a>");
            } else {
              client.println("<a href=\"/LED2/off\"><button>OFF</button></a>");
            }
            client.println("</div></div>");

            // Group Moisture Sensors in one card
            int averageMoisture = (moistureValue + moistureValueA1) / 2;
            client.println("<div class=\"card\">");
            client.println("<h3>Moisture Levels</h3>");
            client.println("<p>A0 Value: " + String(moistureValue) + "</p>");
            client.println("<p>A1 Value: " + String(moistureValueA1) + "</p>");
            client.println("<p>Average: " + String(averageMoisture) + "</p>");
            client.println("</div>");

            // Group Light Sensors in one card
            client.println("<div class=\"card\">");
            client.println("<h3>Light Levels</h3>");
            client.println("<p>Light Sensor 1: " + String(lightValue1) + "</p>");
            client.println("<p>Light Sensor 2: " + String(lightValue2) + "</p>");
            client.println("</div>");

            // Group Temperature & Humidity Sensors in one card
            client.println("<div class=\"card\">");
            client.println("<h3>Temp & Humidity</h3>");
            client.println("<p>Sensor 4 - Temp: " + String(temp4) + " C, Humidity: " + String(hum4) + "%</p>");
            client.println("<p>Sensor 5 - Temp: " + String(temp5) + " C, Humidity: " + String(hum5) + "%</p>");
            client.println("</div>");

            client.println("</div>");  // Close the main div

            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }
}
////////////////////////////////////////////////////////END//////////////////////////////////////////////////////////////////////////
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.print("To control the IOT open this URL --> http://");
  Serial.println(ip);
}
