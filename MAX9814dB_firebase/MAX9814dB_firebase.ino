#include <WiFi.h>
#include <FirebaseESP32.h>

// Replace with your network credentials
const char* ssid = "kicik";
const char* password = "12345678";
int urut = 1;

// Firebase settings
// #define FIREBASE_HOST "https://sensor-b15c7-default-rtdb.firebaseio.com/"
// #define FIREBASE_AUTH "AIzaSyDjHeDbeIotvRUuF0XZLeiRFjKgfv-6KHo"
#define FIREBASE_HOST "https://sensor-b15c7-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyDjHeDbeIotvRUuF0XZLeiRFjKgfv-6KHo"
// Define Firebase Data object
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Sensor settings
const int ledKuning = 4;
const int ledHijau = 2;
const int sampleWindow = 50;    // Sample window width in mS (50 mS = 20Hz)
const int AMP_PIN = 35;         // ADC1 channel 6 on ESP32 (GPIO 34)
const float MIC_SENSITIVITY_dBV = 22.0; // Sensitivity of MAX9814 microphone in dBV (typical value)
const float REF_VOLTAGE = 3.3;  // Reference voltage of your ADC in volts

unsigned int sample;

void setup() {
  Serial.begin(9600);
  pinMode(ledKuning, OUTPUT);
  pinMode(ledHijau, OUTPUT);


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  digitalWrite(ledKuning, HIGH);
  Serial.print("Connecting to Wi-Fi");
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ledKuning, LOW);
    Serial.println("Connected to Wi-Fi");
  } else {
    Serial.println("Failed to connect to Wi-Fi");
    return;  // Stop execution if Wi-Fi is not connected
  }

  // Configure Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  digitalWrite(ledHijau, HIGH);
  unsigned long startMillis = millis();  // Start of sample window
  unsigned int peakToPeak = 0;           // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 4095;         // ADC resolution on ESP32 is 12-bit (0-4095)

  // collect data for 50 mS and then process data
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(AMP_PIN);
    if (sample < 4095) {  // toss out spurious readings
      if (sample > signalMax) {
        signalMax = sample;  // save just the max levels
      } 
      if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

  // Convert peak-to-peak voltage to RMS voltage
  float voltageRMS = (peakToPeak * REF_VOLTAGE) / (2 * 4095 * sqrt(2));

  // Convert RMS voltage to pressure in Pascals using microphone sensitivity
  float sensitivity_Vrms = pow(10, MIC_SENSITIVITY_dBV / 20);  // Convert dBV to Vrms

  float pressure = voltageRMS / sensitivity_Vrms;

  // Convert pressure to decibels
  float dB = 20 * log10(pressure / 0.00002);  // 0.00002 is the reference pressure in Pascals (20 microPascals)

  Serial.println(dB);
  String soundLevel = "/soundLevel MAX9814/" ; //+ String(urut);
    if (Firebase.setFloat(firebaseData, soundLevel, dB)) {
      Serial.println("Data sent to Firebase");
    
  } else {
      Serial.println("Failed to send data to Firebase");
      Serial.println(firebaseData.errorReason());
  }


  // Send the dB value to Firebase
  // urut = urut+1;

  delay(1000);
}
