#include<Wire.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#include<RTClib.h>
#include<TinyGPS++.h>
#include<DHT.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RTC module
RTC_DS3231 rtc;

// GPS module
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // RX=4, TX=5

// GSM module
HardwareSerial gsmSerial(2); // RX=16, TX=17

// DHT22 sensor
#define DHTPIN 17
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Button and motor
#define SOS_BUTTON_PIN 27
#define VIBRATION_PIN 26

// Pulse sensor
#define PULSE_PIN 34

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }
  display.clearDisplay();
  display.display();
  
  // Initialize RTC
  if(!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while(true);
  }
  if(rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Initialize GPS Serial
  gpsSerial.begin(9600, SERIAL_8N1, 4, 5); // RX=4, TX=5
  
  // Initialize GSM Serial
  gsmSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  
  // Initialize DHT22
  dht.begin();
  
  // Initialize Button and Motor
  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(VIBRATION_PIN, LOW);
  
  // Initialize Pulse Sensor
  pinMode(PULSE_PIN, INPUT);
}

void loop() {
  // Update and display sensor data
  displaySensorData();
  
  // Check for SOS button press
  if(digitalRead(SOS_BUTTON_PIN) == LOW) {
    sendSOS();
    delay(1000); // Debounce delay
  }
  
  // Handle GPS data
  while(gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
  
  delay(1000); // Update every second
}

void displaySensorData() {
  DateTime now = rtc.now();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int pulse = analogRead(PULSE_PIN); // Placeholder for actual heart rate
  
  display.clearDisplay();
  
  // Display Time
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.printf("Time: %02d:%02d:%02d", now.hour(), now.minute(), now.second());
  
  // Display Temperature
  display.setCursor(0, 16);
  if(isnan(temperature)) {
    display.print("Temp: N/A");
  } else {
    display.printf("Temp: %.1f C", temperature);
  }
  
  // Display Humidity
  display.setCursor(0, 32);
  if(isnan(humidity)) {
    display.print("Hum: N/A");
  } else {
    display.printf("Hum: %.1f %%", humidity);
  }
  
  // Display Heart Rate
  display.setCursor(0, 48);
  display.printf("Heart: %d bpm", pulse); // Replace with actual calculation
  
  display.display();
}

void sendSOS() {
  // Get GPS Location
  String location = "Unknown";
  if(gps.location.isValid()) {
    location = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
  }
  
  // Construct SOS Message
  String message = "SOS! Help needed at Location: " + location;
  
  // Send SMS via GSM
  sendSMS(message);
  
  // Activate Vibration Motor
  activateVibration();
}

void sendSMS(String message) {
  gsmSerial.println("AT"); // Check communication
  delay(100);
  gsmSerial.println("AT+CMGF=1"); // Set SMS to text mode
  delay(100);
  gsmSerial.printf("AT+CMGS=\"+1234567890\"\r"); // Replace with your phone number
  delay(100);
  gsmSerial.print(message);
  delay(100);
  gsmSerial.write(26); // Ctrl+Z to send SMS
  delay(1000);
}

void activateVibration() {
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(500);
  digitalWrite(VIBRATION_PIN, LOW);
}
