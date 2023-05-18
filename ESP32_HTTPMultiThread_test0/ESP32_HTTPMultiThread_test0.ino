#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String ae = "sionjeon"; // 모비우스에서 사용할 AE 이름

String ledValue;
String fanValue;
bool ledValueReady = false;
bool fanValueReady = false;

TaskHandle_t taskLed, taskFan;

// LED 데이터 요청
void taskLedRequest(void* pvParameters) {
  while (true) {
    HTTPClient httpLed;
    String cntLed = "Led";
    httpLed.begin("http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cntLed + "/la");
    httpLed.addHeader("Accept", "application/json");
    httpLed.addHeader("X-M2M-RI", "12345");
    httpLed.addHeader("X-M2M-Origin", ae);
    int httpCodeLed = httpLed.GET();

    if (httpCodeLed > 0) {
      String response = httpLed.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);
      ledValue = doc["m2m:cin"]["con"].as<String>();
      ledValueReady = true;
    } else {
      Serial.println("Error on LED HTTP request");
    }

    httpLed.end();
    vTaskDelay(pdMS_TO_TICKS(300)); // 0.3초 딜레이
  }
}

// Fan 데이터 요청
void taskFanRequest(void* pvParameters) {
  while (true) {
    HTTPClient httpFan;
    String cntFan = "Fan";
    httpFan.begin("http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cntFan + "/la");
    httpFan.addHeader("Accept", "application/json");
    httpFan.addHeader("X-M2M-RI", "12345");
    httpFan.addHeader("X-M2M-Origin", ae);
    int httpCodeFan = httpFan.GET();

    if (httpCodeFan > 0) {
      String response = httpFan.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);
      fanValue = doc["m2m:cin"]["con"].as<String>();
      fanValueReady = true;
    } else {
      Serial.println("Error on Fan HTTP request");
    }

    httpFan.end();
    vTaskDelay(pdMS_TO_TICKS(300)); // 0.3초 딜레이
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  // LED 데이터 요청을 위한 스레드 생성
  xTaskCreatePinnedToCore(taskLedRequest, "taskLed", 10000, NULL, 1, &taskLed, 1);
  
  // Fan 데이터 요청을 위한 스레드 생성
  xTaskCreatePinnedToCore(taskFanRequest, "taskFan", 10000, NULL, 1, &taskFan, 1);
}

void loop() {
  if (ledValueReady && fanValueReady) {
    // LED와 Fan 값 출력
    Serial.println("LED Value: " + ledValue + ", Fan Value: " + fanValue);

    // 값 사용 후 초기화
    ledValueReady = false;
    fanValueReady = false;
  }
}
