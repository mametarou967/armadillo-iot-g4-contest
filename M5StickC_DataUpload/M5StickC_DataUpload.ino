#include "M5StickC_GateController.h"
#include <M5StickC.h>
#include <WiFi.h>
#include "Ambient.h"
#include <ArduinoJson.h>

WiFiClient client;
Ambient ambient;

// the setup routine runs once when M5StickC starts up
void setup(){
  // Initialize the M5StickC object
  M5.begin();
  // 画面を横向きに
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextFont(4);

  // Wi-Fi接続
  Serial.println("Start");

  WiFi.begin(ssid, password);  //  Wi-Fi APに接続
  while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
      delay(100);
  }

  Serial.print("WiFi connected\r\nIP address: ");
  Serial.println(WiFi.localIP());

  // ambient
  ambient.begin(channelId, writeKey, &client,readKey); // チャネルIDとライトキーを指定してAmbientの初期化

  // LCD display
  M5.Lcd.print("Hello World");
}

// the loop routine runs over and over again forever
void loop() {
  // Buttonクラスを利用するときには必ずUpdateを呼んで状態を更新する
  M5.update();

  // ホームボタンを押したか？（1度だけ取得可能）
  if ( M5.BtnA.wasPressed() ) {
    char buf[180] = {0};
    
    if(ambient.read(buf, 180))
    {
      // 受信ログ
      Serial.print("ambient read scusess ");
      Serial.printf("%s",buf);

      // json解析
      const size_t capacity = 500;
      DynamicJsonDocument doc(capacity);
      DeserializationError err = deserializeJson(doc, buf);
      int value[1] = { 0 };
      value[0] = doc[0]["d1"];

      // 画面表示
      Serial.printf("Result:%d\n",value[0]);
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.print("Value1:"); M5.Lcd.println(value[0]);
    }else{
      Serial.print("ambient read failed ");
    }
  }
}
