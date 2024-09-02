#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <DHT.h>

// Constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define DHTPIN 6
#define DHTTYPE DHT11 // DHT11 or DHT22
#define SOS_BUTTON_PIN 2
#define VIBRATION_PIN 3
#define HEARTBEAT_PIN A0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;
DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;
SoftwareSerial gpsSerial(4, 5); // GPS RX, TX
SoftwareSerial gsmSerial(10, 11); // GSM RX, TX

void setup() {
  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(VIBRATION_PIN, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  rtc.begin();
  dht.begin();
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);
  
  display.clearDisplay();
}

void loop() {
  DateTime now = rtc.now();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Display Time, Temperature, and Humidity
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(now.hour());
  display.print(':');
  display.print(now.minute());
  display.print(':');
  display.print(now.second());
  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(temperature);
  display.print(" C");
  display.setCursor(0, 32);
  display.print("Hum: ");
  display.print(humidity);
  display.print(" %");
  display.display();
  
  // Read Pulse Sensor
  int heartRate = analogRead(HEARTBEAT_PIN); // Simulated heart rate reading
  display.setCursor(0, 48);
  display.print("Heart Rate: ");
  display.print(heartRate);
  display.display();
  
  // Check for SOS button press
  if (digitalRead(SOS_BUTTON_PIN) == LOW) {
    sendSOS();
  }
  
  delay(1000);
}

void sendSOS() {
  String location = "";
  if (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
    if (gps.location.isUpdated()) {
      location = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
    }
  }

  // Send SOS via GSM
  gsmSerial.print("AT+CMGF=1\r");
  delay(100);
  gsmSerial.print("AT+CMGS=\"+1234567890\"\r"); // Replace with actual phone number
  delay(100);
  gsmSerial.print("SOS! Help needed at location: ");
  gsmSerial.print(location);
  gsmSerial.write(26); // Ctrl+Z to send SMS
  
  // Vibration feedback
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(500);
  digitalWrite(VIBRATION_PIN, LOW);
}
