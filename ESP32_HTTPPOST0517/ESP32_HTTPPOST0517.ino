#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String cnt = "Led";
const String ae = "sionjeon"; // 모비우스에서 사용할 AE 이름
const String cin = "POSTING from ESP32"; // 전송할 데이터

void setup() {
  Serial.begin(115200);
  delay(4000); // Delay needed before calling the WiFi.begin

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { // Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to the WiFi network");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection status
    String payload = "{\"m2m:cin\": {\"con\": \"" + cin + "\"}}"; // 전송할 데이터
    HTTPClient http;

    http.begin("http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cnt); // Specify destination for HTTP request
    http.addHeader("Content-Type", "application/vnd.onem2m-res+json; ty=4");
    http.addHeader("Accept", "application/json");
    http.addHeader("X-M2M-RI", "12345");
    http.addHeader("X-M2M-Origin", ae);
    int httpResponseCode = http.POST(payload); // Send the actual POST request

    if (httpResponseCode > 0) {
      String response = http.getString(); // Get the response to the request

      Serial.println(httpResponseCode); // Print return code
      Serial.println(response); // Print request answer
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Free resources
  } else {
    Serial.println("Error in WiFi connection");
  }

  delay(3000); // Send a request every 10 seconds
}
