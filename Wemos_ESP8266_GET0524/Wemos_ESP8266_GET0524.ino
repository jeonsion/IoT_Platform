#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> // JSON 파싱을 위한 라이브러리


const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String cnt = "earthquake";
const String ae = "KETIDGZ"; // 모비우스에서 사용할 AE 이름

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi"); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // HTTP GET 요청 설정
    http.begin(client, "http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cnt + "/" + "la");
    http.addHeader("Accept", "application/json");
    http.addHeader("X-M2M-RI", "12345");
    http.addHeader("X-M2M-Origin", ae);
    int httpCode = http.GET(); // HTTP GET 요청 전송
    String response = http.getString(); // 서버 응답 수신
    http.end(); // HTTP 연결 종료

    if (httpCode == 200) { // HTTP 응답 코드가 200인 경우
      Serial.println("Data received from Mobius:");
      
      //주석 처리해도 됨
      //Serial.println(response);


      // JSON 파싱
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);

      // con 값 추출 및 출력
      String con = doc["m2m:cin"]["con"].as<String>();
      //Serial.print("con: ");    //주석 처리해도 됨
      Serial.println(con);      // 나중에 이 값만 사용
    } else {
      Serial.print("HTTP GET failed: ");
      Serial.println(httpCode);
      Serial.println(response);
    }
  } else {
    Serial.println("Wi-Fi disconnected");
  }

  delay(1000); // 5초마다 데이터 요청
}