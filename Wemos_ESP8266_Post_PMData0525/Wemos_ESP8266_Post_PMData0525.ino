#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "PMS.h"
#include <SoftwareSerial.h>

const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String cnt = "Dust";
const String ae = "sionjeon"; // 모비우스에서 사용할 AE 이름

SoftwareSerial nockanda(D3, D4); // tx=3, rx=4
PMS pms(nockanda);
PMS::DATA data;

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);

  Serial.print("Wi-Fi에 연결 중");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Wi-Fi에 연결되었습니다.");

  nockanda.begin(9600);
  pms.passiveMode();
}

void loop() {
  pms.requestRead();

  if (pms.readUntil(data)) {
    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);

    String payload = "{\"m2m:cin\": {\"con\": \"" + String(data.PM_AE_UG_2_5) + "\"}}";

    WiFiClient client;
    HTTPClient http;

    // HTTP POST 요청 설정
    http.begin(client, "http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cnt);
    http.addHeader("Content-Type", "application/vnd.onem2m-res+json; ty=4");
    http.addHeader("Accept", "application/json");
    http.addHeader("X-M2M-RI", "12345");
    http.addHeader("X-M2M-Origin", ae);
    int httpCode = http.POST(payload); // HTTP POST 요청 전송
    String response = http.getString(); // 서버 응답 수신
    http.end(); // HTTP 연결 종료

    if (httpCode == 201) { // HTTP 응답 코드가 201인 경우
      Serial.println("데이터를 Mobius로 전송하였습니다.");
      Serial.println(response);
    } else {
      Serial.print("HTTP POST 실패: ");
      Serial.println(httpCode);
      Serial.println(response);
    }
  } else {
    Serial.println("데이터를 읽을 수 없습니다.");
  }

  delay(5000); // 5초마다 전송
}
