#include <HardwareSerial.h>
HardwareSerial mySerial(1);
#define RECORD_LEN 30

#include <WiFi.h>
#include <WebServer.h>
#include "index.h"

const char * ssid = "Hollow_ESP32";
const char * password = "hollowman";
WebServer server(80);
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
WebSocketsServer webSocket = WebSocketsServer(81);

#include <Adafruit_NeoPixel.h>
#define PIN 12
#define NUM 1 //灯个数
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM, PIN, NEO_GRB + NEO_KHZ800);
int skip_cnt = 0;

#include <TensorFlowLite_ESP32.h>
#include <tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h>
#include <tensorflow/lite/experimental/micro/micro_error_reporter.h>
#include <tensorflow/lite/experimental/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>
#include "model.h"
tflite::MicroErrorReporter tflErrorReporter;
tflite::ops::micro::AllOpsResolver tflOpsResolver;
const tflite::Model *tflModel = nullptr;
tflite::MicroInterpreter *tflInterpreter = nullptr;
TfLiteTensor *tflInputTensor = nullptr;
TfLiteTensor *tflOutputTensor = nullptr;
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize];
const char *GESTURES[] = {
    "skip",
    "mess"};
#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

int count = 0;
unsigned char sign = 0;
unsigned char Re_buf[25], counter = 0;
float record_aX[RECORD_LEN];
float record_aY[RECORD_LEN];
float record_aZ[RECORD_LEN];

float record_gX[RECORD_LEN];
float record_gY[RECORD_LEN];
float record_gZ[RECORD_LEN];
int16_t YPR[3];
int16_t gyro[3] = {0};
int16_t acc[3] = {0};
// uint32_t timer;
float aX, aY, aZ, gX, gY, gZ, yX, yY, yZ;
int fresh = 0;
int isRecord = 0;
void setup()
{
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 15, 21);
  delay(500);
  mySerial.write(0XA5);
  mySerial.write(0X55);
  mySerial.write(0X57); //初始化GY25Z,输出陀螺和欧拉角
  mySerial.write(0X51);
  delay(100);
  mySerial.write(0XA5);
  mySerial.write(0X56); //初始化GY25Z,连续输出模式
  mySerial.write(0X02); //初始化GY25Z,连续输出模式
  mySerial.write(0XFD);

  pixels.begin();
  pixels.clear();                                  //清除颜色
  pixels.show();                                   //打印
  pixels.setPixelColor(0, pixels.Color(10, 0, 0)); //调颜色
  pixels.show();

  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION)
  {
    Serial.println("Model schema mismatch!");
    while (1);
  }
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);
  tflInterpreter->AllocateTensors();
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);

  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.on("/",[](){
    server.send_P(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();
  delay(1000);
}

void loop()
{ server.handleClient();
  webSocket.loop();
  fresh = 0;
  while (!fresh)
  {
    measure();
    analysis();
  }
  float sum = fabs(aX) + fabs(aY) + fabs(aZ);
  if (sum > 2.8)
  {
    isRecord = 1;
  }
  if (isRecord)
  {
    if (count < RECORD_LEN)
    {
      record_aX[count] = aX;
      record_aY[count] = aY;
      record_aZ[count] = aZ;
      record_gX[count] = gX;
      record_gY[count] = gY;
      record_gZ[count] = gZ;
      count++;
    }
    else
    {
      isRecord = 0;
      count = 0;
      for (int k = 0; k < RECORD_LEN; k++)
      {
        tflInputTensor->data.f[k * 6 + 0] = (record_aX[k] + 2.0) / 4.0;
        tflInputTensor->data.f[k * 6 + 1] = (record_aY[k] + 2.0) / 4.0;
        tflInputTensor->data.f[k * 6 + 2] = (record_aZ[k] + 2.0) / 4.0;
        tflInputTensor->data.f[k * 6 + 3] = (record_gX[k] + 2000.0) / 4000.0;
        tflInputTensor->data.f[k * 6 + 4] = (record_gY[k] + 2000.0) / 4000.0;
        tflInputTensor->data.f[k * 6 + 5] = (record_gZ[k] + 2000.0) / 4000.0;
      }
      TfLiteStatus invokeStatus = tflInterpreter->Invoke();
      if (invokeStatus != kTfLiteOk)
      {
        Serial.println("Invoke failed!");
        while (1);
      }

      if (tflOutputTensor->data.f[0] > 0.94)
      {
        Serial.println(++skip_cnt);
        webSocket.broadcastTXT("k", 1);
      }

    }
  }
  delay(5);
  // timer = micros();
}

void measure()
{
  fresh = 0;
  while (mySerial.available())
  {
    Re_buf[counter] = (unsigned char)mySerial.read();
    if (counter == 0 && Re_buf[0] != 0x5A)
    {
      return;
    }
    counter++;
    if (counter == 25) //接收到数据
    {
      counter = 0; //重新赋值，准备下一帧数据的接收
      sign = 1;
    }
  }
}
void analysis()
{
  if (sign)
  {
    sign = 0;
    if (Re_buf[0] == 0x5A && Re_buf[1] == 0x5A) //检查帧头，帧尾
    {
      fresh = 1;
      acc[0] = (Re_buf[4] << 8 | Re_buf[5]);
      acc[1] = (Re_buf[6] << 8 | Re_buf[7]);
      acc[2] = (Re_buf[8] << 8 | Re_buf[9]);

      gyro[0] = (Re_buf[10] << 8 | Re_buf[11]);
      gyro[1] = (Re_buf[12] << 8 | Re_buf[13]);
      gyro[2] = (Re_buf[14] << 8 | Re_buf[15]);

      YPR[0] = (Re_buf[16] << 8 | Re_buf[17]);
      YPR[1] = (Re_buf[18] << 8 | Re_buf[19]);
      YPR[2] = (Re_buf[20] << 8 | Re_buf[21]);

      aX = acc[0] / 16383.5;
      aY = acc[1] / 16383.5;
      aZ = acc[2] / 16383.5;
      gX = gyro[0] / 16.3835;
      gY = gyro[1] / 16.3835;
      gZ = gyro[2] / 16.3835;
      yX = YPR[0] / 100.0;
      yY = YPR[1] / 100.0;
      yZ = YPR[2] / 100.0;
    }
  }
}
