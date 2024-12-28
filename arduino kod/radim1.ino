
#define dataa 2
long iducaProvjera;
#include <ESP8266HTTPClient.h>
#include <UrlEncode.h>
#include <Arduino.h>
#include <WiFiClient.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "-STUPCI-"
#define WIFI_PASSWORD "267212800"

// Insert Firebase project API Key
#define API_KEY "AIzaSyA4XEgeeSx_xej89twvRUCu931cVCVt9Y8"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://nodemcu-prviprojekat-default-rtdb.europe-west1.firebasedatabase.app/" 

//phone details
String brojTelefona = "38763738758";
String whatsAppApi = "4283038";
 //WhatsApp
void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + brojTelefona + "&apikey=" + whatsAppApi + "&text=" + urlEncode(message);
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}



int sistemUkljucen = 1; // po defaultu je ukljucen
const int pirPin = 2;
//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

// buzzer
const int buzzerPin = D6;
int noteDuration = 500;

void setup(){
  //device state 
    // pinMode(relayPin, OUTPUT);
    // digitalWrite(relayPin, LOW); // Default state OFF


  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print("WIFI nije povezan");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Serial.println();

  // Slanje poruke na wapp
  sendMessage("Kretnja uocena!!!");

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  // Postavljanje pin-a za buzzer
  pinMode(buzzerPin, OUTPUT);

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
      //  if(Firebase.RTDB.getInt(&fbdo, "/data/sistemUkljucen")){
      //   if(fbdo.dataType() == "int"){
      //     sistemUkljucen = fbdo.intData();
      //  }
    }



      //int motionStatus = digitalRead(pirPin); gledanje stanje sistema preko serial monitora
      String kretnja = sistemUkljucen ? "Kretnja uocena!!" : "Mirno je";
      String MotionPath = "/motionStatus";

    // Ucitaj stanje sistema iz firebase
  if (Firebase.RTDB.getString(&fbdo, "/SystemState")) {
      String state = fbdo.stringData();
      Serial.println("Stanje uređaja: " + state);
      if (state == "UKLJUCEN") 
      {
          digitalWrite(pirPin, HIGH); //  ukljuci
        
          if(millis() >= iducaProvjera) // ako je kretnja uocena, uradi ovo
          {

              if(Firebase.RTDB.setString(&fbdo,MotionPath.c_str(),kretnja  + String(millis())))
                {     // je li buzzzer ukljucen
                  if(Firebase.RTDB.getString(&fbdo, "/BuzzerOnOff"))
                    {
                      int buzzerState = fbdo.intData();
                      if(buzzerState == 1)
                        {
                            digitalWrite(buzzerPin, HIGH);
                             tone(buzzerPin,2000,noteDuration);
                            delay(800);
                        }
                      else{
                            digitalWrite(buzzerPin, LOW);
                          }
                    }
                  Serial.println("Kretnja uocena-c (" + String(millis()) + ")");
                        //buzzer
                       
                        //delay(800);
                    // Slanje poruke na wapp
                  // sendMessage("Kretnja uocena!!!");
                    iducaProvjera = millis() + 9000;
                    Serial.println("Iduca kretnja: " + String(iducaProvjera));
                }
                else
                {
                  Serial.println("Greska");
                  Serial.println(fbdo.errorReason());
                }
          }
      }else if (state == "ISKLJUCEN") {
          digitalWrite(pirPin, LOW);  //  iskljuci
          digitalWrite(buzzerPin, LOW);

      }
      else {
            Serial.println("Failed to read from Firebase");
            Serial.println(fbdo.errorReason());
          }

  }

  delay(1000);
  
}
