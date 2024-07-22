#include <WiFi.h>
#include <FirebaseESP32.h>

// Replace with your network credentials
const char* ssid = "kicik";
const char* password = "12345678";
int urut = 1;

// Firebase settings
#define FIREBASE_HOST "https://sensor-b15c7-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyDjHeDbeIotvRUuF0XZLeiRFjKgfv-6KHo"

// Define Firebase Data object
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Sensor settings
const int ledHijau = 32;
const int ledKuning = 33;
const int sampleWindow = 50;  
const int AMP_PIN = 35;      
const float MIC_SENSITIVITY_dBV = 1.0; // Sensitivity of MAX9814 microphone in dBV (typical value)
const float REF_VOLTAGE = 3.3;  // Reference voltage of your ADC in volts

unsigned int sample;

void setup() {
  Serial.begin(9600);
  pinMode(ledKuning, OUTPUT);
  pinMode(ledHijau, OUTPUT);

  // Connect to Wi-Fi
  digitalWrite(ledKuning, HIGH);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to Wi-Fi");
    digitalWrite(ledKuning, LOW);
  } else {
    Serial.println("Failed to connect to Wi-Fi");
    return;
  }

  // Configure Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  digitalWrite(ledHijau, HIGH);
  unsigned long startMillis = millis(); 
  unsigned int peakToPeak = 0;         

  unsigned int signalMax = 0;
  unsigned int signalMin = 4095;    

  // collect data for 50 mS and then process data
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(AMP_PIN);
    if (sample < 4095) { 
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


  float sensitivity_Vrms = pow(10, MIC_SENSITIVITY_dBV / 20);  // Convert dBV to Vrms

  float pressure = voltageRMS / sensitivity_Vrms;

  // Convert pressure to decibels
  float dB = 20 * log10(pressure / 0.00002); 

  Serial.println(dB);
  String soundLevel = "/sound Sensor/"; //+ String(urut);
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