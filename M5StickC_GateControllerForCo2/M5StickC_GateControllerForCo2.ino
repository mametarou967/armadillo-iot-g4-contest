// ボタンを押すと、
// 1000ppm以下だったらゲートが一定時間開
// 1000ppm以上だったらゲートが開かない

#include "M5StickC_GateControllerForCo2.h"
#include <M5StickC.h>
#include "MHZ19_uart.h"
#include "esp32-hal-ledc.h"
#include <WiFi.h>
#include "Ambient.h"
#include <ArduinoJson.h>

// CO2
#define RX_PIN 36 // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 0 // Tx pin which the MHZ19 Rx pin is attached to
#define CO2_UPDATE_INTERVAL 5000 // msec
MHZ19_uart mhz19; // Constructor for library

int ppmValue = 0;

// Gate
#define TIMER_WIDTH 16
#define COUNT_LOW 1500
#define COUNT_HIGH 6500

// ambient
WiFiClient client;
Ambient ambient;

bool ExistHuman()
{
  char buf[180] = {0};
  if(ambient.read(buf, 180))
  {
    // 受信ログ
    Serial.print("ambient read scusess \n");
    Serial.printf("%s\n",buf);
    
    // json解析
    const size_t capacity = 500;
    DynamicJsonDocument doc(capacity);
    DeserializationError err = deserializeJson(doc, buf);
    int value[1] = { 0 };
    value[0] = doc[0]["d1"];
    
    // 画面表示
    if(value[0] > 0){
      Serial.print("human Exist \n");
      return true;
    }else{
      Serial.print("human not Exist \n");
    }
  }else{
    Serial.print("ambient read failed \n");
  }
  return false;
}

void updatePpm()
{
  // 値の更新
  ppmValue = mhz19.getPPM();

  // 表示の更新
  int height = M5.Lcd.height();
  int width = M5.Lcd.width();

  M5.Lcd.fillRect(0, 0, width, height, BLACK);
  
  M5.Lcd.setCursor(20, 10);
  M5.Lcd.setTextSize(2);
  
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("%4d",ppmValue);

  M5.Lcd.setCursor(90, 55);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.printf("ppm");
}

void gateUp()
{
  for(int i = COUNT_LOW; i < COUNT_HIGH; i = i + 100){
    Serial.println(i);
    ledcWrite(1, i);
    delay(30);
  }
}

void gateDown()
{
  for(int i = COUNT_HIGH; i >= COUNT_LOW; i = i - 100){
    Serial.println(i);
    ledcWrite(1, i);
    delay(30);
  }
}

void openGate()
{
  gateDown();
  delay(5000);
  gateUp();
}


// the setup routine runs once when M5StickC starts up
void setup(){
  // Initialize the M5StickC object
  M5.begin();
  // 画面を横向きに
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextFont(4);

  // ambientのsetup
  WiFi.begin(ssid, password);  //  Wi-Fi APに接続
  while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
      delay(100);
  }

  Serial.print("WiFi connected\r\nIP address: ");
  Serial.println(WiFi.localIP());

  // ambient
  ambient.begin(channelId, writeKey, &client,readKey); // チャネルIDとライトキーを指定してAmbientの初期化

  // MHZ19のSETUP
  mhz19.begin(RX_PIN, TX_PIN);
  mhz19.setAutoCalibration(false);
  delay(3000);

  // lecd
  ledcSetup(1, 50, TIMER_WIDTH);
  ledcAttachPin(32, 1);
  
  // LCD display
  M5.Lcd.print("Hello World");
}

// the loop routine runs over and over again forever
void loop() {
  // Buttonクラスを利用するときには必ずUpdateを呼んで状態を更新する
  M5.update();

  updatePpm();
  
  // ホームボタンを押したか？（1度だけ取得可能）
  if ( M5.BtnA.wasPressed() || ExistHuman()) {
    Serial.printf("co2:%d\n",ppmValue);
    if(ppmValue > 1000){
      Serial.printf("since co2 is upper 1000, so gate wont move\n",ppmValue);
    }else{
      Serial.printf("gate move\n",ppmValue);
      openGate();
    }
  }
  /*
  if ( M5.BtnA.wasPressed()) {
      openGate();
  }
  */
  
  // 1秒Wait
  delay(5000);
}
