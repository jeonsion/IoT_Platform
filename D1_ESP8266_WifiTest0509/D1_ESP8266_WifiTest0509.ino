#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String cnt = "Led";
const String ae = "justin"; // 모비우스에서 사용할 AE 이름
const String cin = "1"; // 전송할 데이터

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
    String payload = "{\"m2m:cin\": {\"con\": \"" + cin + "\"}}"; // 전송할 데이터

    WiFiClient client;
    HTTPClient http;

    // HTTP POST 요청 설정
    http.begin(client, "http://" + String(server) + ":" + String(port) + "/Mobius/justin/" + cnt);
    http.addHeader("Content-Type", "application/vnd.onem2m-res+json; ty=4");
    http.addHeader("Accept", "application/json");
    http.addHeader("X-M2M-RI", "12345");
    http.addHeader("X-M2M-Origin", ae);
    int httpCode = http.POST(payload); // HTTP POST 요청 전송
    String response = http.getString(); // 서버 응답 수신
    http.end(); // HTTP 연결 종료

    if (httpCode == 201) { // HTTP 응답 코드가 201인 경우
      Serial.println("Data sent to Mobius");
      Serial.println(response);
    } else {
      Serial.print("HTTP POST failed: ");
      Serial.println(httpCode);
      Serial.println(response);
    }
  } else {
    Serial.println("Wi-Fi disconnected");
  }

  delay(5000); // 5초마다 데이터 전송
}
