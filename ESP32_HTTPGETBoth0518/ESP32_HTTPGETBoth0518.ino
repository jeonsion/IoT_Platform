//Description : HTTP GET 메소드를 이용하여 Fan과 Led 컨테이너의 cin 값을 0.3초마다 읽어들이는 코드입니다.
//비동기 방식이 아니므로 Time Delay가 있을 수 있습니다.
//ver 1.0


#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String ae = "sionjeon"; // 모비우스에서 사용할 AE 이름

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Fan 데이터 요청
    HTTPClient httpFan;
    String cntFan = "Fan";
    httpFan.begin("http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cntFan + "/la");
    httpFan.addHeader("Accept", "application/json");
    httpFan.addHeader("X-M2M-RI", "12345");
    httpFan.addHeader("X-M2M-Origin", ae);
    int httpCodeFan = httpFan.GET();


     // LED 데이터 요청
    HTTPClient httpLed;
    String cntLed = "Led";
    httpLed.begin("http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cntLed + "/la");
    httpLed.addHeader("Accept", "application/json");
    httpLed.addHeader("X-M2M-RI", "12345");
    httpLed.addHeader("X-M2M-Origin", ae);
    int httpCodeLed = httpLed.GET();

    // Fan, LED 데이터 응답 처리
    if (httpCodeFan > 0 && httpCodeLed > 0) {
            String response1 = httpFan.getString();
            String response2 = httpLed.getString();
            DynamicJsonDocument doc1(1024);
            DynamicJsonDocument doc2(1024);
            deserializeJson(doc1, response1);
            deserializeJson(doc2, response2);

            String ledValue = doc1["m2m:cin"]["con"].as<String>();
            String fanValue = doc2["m2m:cin"]["con"].as<String>();

            Serial.println("LED Value: " + ledValue + ", " +"Fan Value: " + fanValue);

          }
          else {
            Serial.println("Error on HTTP request");
          }


    // Fan 데이터 요청 종료
    httpFan.end();
    // LED 데이터 요청 종료
    httpLed.end();
  }

  delay(300);
}
