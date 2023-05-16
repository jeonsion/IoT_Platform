#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // JSON 파싱을 위한 라이브러리



const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String cnt = "Led";
const String ae = "justin"; // 모비우스에서 사용할 AE 이름


void setup() {
  
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
  
}
  
void loop() {
  
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
  
    HTTPClient http;
    
    
    // HTTP GET 요청 설정
    http.begin("http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cnt + "/" + "la"); //Specify the URL
    http.addHeader("Accept", "application/json");
    http.addHeader("X-M2M-RI", "12345");
    http.addHeader("X-M2M-Origin", ae);
    int httpCode = http.GET();                                        //Make the request
  
    if (httpCode > 0) { //Check for the returning code
  
        String response = http.getString();
        //Serial.println(httpCode);
        //Serial.println(payload);
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);

      String con = doc["m2m:cin"]["con"].as<String>();
      Serial.println(con);      // 나중에 이 값만 사용

      }
  
    else {
      Serial.println("Error on HTTP request");
    }
  
    http.end(); //Free the resources
  }
  
  delay(300);
  
}