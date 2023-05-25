/*
위 코드는 ESP32 기반의 Arduino 프로그램으로, WiFi를 통해 모비우스 서버에서 LED와 Fan 데이터를 가져와 제어하는 예제입니다. 주요 기능과 동작은 다음과 같습니다:

WiFi 연결: 주어진 SSID와 비밀번호를 사용하여 WiFi 네트워크에 연결합니다.
LED 데이터 요청: 주기적으로 모비우스 서버에 LED 상태를 요청합니다. HTTP GET 요청을 통해 LED 상태 값을 가져옵니다.
Fan 데이터 요청: 주기적으로 모비우스 서버에 Fan 값(예: 먼지 농도)을 요청합니다. HTTP GET 요청을 통해 Fan 값을 가져옵니다.
LED 제어: 가져온 LED 값에 따라 LED를 켜거나 끕니다. LED 제어용 GPIO 핀을 사용합니다.
DC 모터 제어: 가져온 Fan 값이 15 이상인 경우 DC 모터를 켭니다. 그 외의 경우에는 모터를 끕니다. DC 모터 제어용 GPIO 핀을 사용합니다.
값 초기화: LED와 Fan 값을 사용한 후에는 값을 초기화하여 다음 데이터 요청에 대비합니다.
따라서, 이 코드는 WiFi를 통해 서버와 통신하여 LED와 Fan을 제어하는 기능을 수행합니다.
*/

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

const int ledPin = 12; // LED 제어용 GPIO 핀
const int motorPin = 13; // DC 모터 제어용 GPIO 핀

String ledValue;
int fanValue = 0;
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
    String cntDust = "Dust";
    httpFan.begin("http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cntDust + "/la");
    httpFan.addHeader("Accept", "application/json");
    httpFan.addHeader("X-M2M-RI", "12345");
    httpFan.addHeader("X-M2M-Origin", ae);
    int httpCodeFan = httpFan.GET();

    if (httpCodeFan > 0) {
      String response = httpFan.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);
      fanValue = doc["m2m:cin"]["con"].as<int>();
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

  pinMode(ledPin, OUTPUT); // LED 제어용 핀을 출력으로 설정
  pinMode(motorPin, OUTPUT); // DC 모터 제어용 핀을 출력으로 설정

  // LED 데이터 요청을 위한 스레드 생성
  xTaskCreatePinnedToCore(taskLedRequest, "taskLed", 10000, NULL, 1, &taskLed, 1);
  
  // Fan 데이터 요청을 위한 스레드 생성
  xTaskCreatePinnedToCore(taskFanRequest, "taskFan", 10000, NULL, 1, &taskFan, 1);
}

void loop() {
  if (ledValueReady && fanValueReady) {
    // LED와 Fan 값 출력
    Serial.println("LED Value: " + ledValue + ", Fan Value: " + fanValue);

    // LED 제어
    if (ledValue == "1") {
      digitalWrite(ledPin, HIGH); // LED 켜기
    } else {
      digitalWrite(ledPin, LOW); // LED 끄기
    }

    // DC 모터 제어
    if (fanValue >= 15) {
      digitalWrite(motorPin, HIGH); // DC 모터 켜기
    } else {
      digitalWrite(motorPin, LOW); // DC 모터 끄기
    }

    // 값 사용 후 초기화
    ledValueReady = false;
    fanValueReady = false;
  }
}
