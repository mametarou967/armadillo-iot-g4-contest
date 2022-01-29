// ボタンを押すと、
// 1000ppm以下だったらゲートが一定時間開
// 1000ppm以上だったらゲートが開かない

#include "M5StickC_GateControllerForCo2.h"
#include <M5StickC.h>
#include "MHZ19_uart.h"
#include <Ticker.h>

// CO2
#define RX_PIN 36 // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 0 // Tx pin which the MHZ19 Rx pin is attached to
#define CO2_UPDATE_INTERVAL 5000 // msec

MHZ19_uart mhz19; // Constructor for library

Ticker ticker;
int ppmValue = 0;

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


// the setup routine runs once when M5StickC starts up
void setup(){
  // Initialize the M5StickC object
  M5.begin();
  // 画面を横向きに
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextFont(4);

  // MHZ19のSETUP
  mhz19.begin(RX_PIN, TX_PIN);
  mhz19.setAutoCalibration(false);
  delay(3000);
  // 定期更新の設定
  ticker.attach(5, updatePpm); //5秒ごとにCO2の状態を更新
  
  // LCD display
  M5.Lcd.print("Hello World");
}

// the loop routine runs over and over again forever
void loop() {
  // Buttonクラスを利用するときには必ずUpdateを呼んで状態を更新する
  M5.update();
  
  // ホームボタンを押したか？（1度だけ取得可能）
  if ( M5.BtnA.wasPressed() ) {
    Serial.printf("co2:%d\n",ppmValue);
  }
  
  // 1秒Wait
  delay(1000);
}
