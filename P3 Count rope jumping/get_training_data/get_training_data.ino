#include <HardwareSerial.h>
#define RECORD_LEN 30

// 添加RGB灯控制
#include <Adafruit_NeoPixel.h>
#define PIN 12
#define NUM 1 //灯个数
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM, PIN, NEO_GRB + NEO_KHZ800);

HardwareSerial mySerial(1);

int count = 0;
int data_cnt = 0;
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
  pixels.clear();                                 //清除颜色
  pixels.show();                                  //打印
  pixels.setPixelColor(0, pixels.Color(10, 0, 0)); //调颜色
  pixels.show();
  delay(100);
}

void loop()
{
  fresh = 0;
  while (!fresh)
  {
    measure();
    analysis();
  }
  float sum = fabs(aX) + fabs(aY) + fabs(aZ);
  if (sum > 2.8)
  {
    isRecord = 18;
  }
  else if (sum < 1.8 && isRecord > 0)
  {
    isRecord--;
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
    }
  }
  else
  {
    if (count == RECORD_LEN)
    {
      data_cnt++;
      for (int k = 0; k < RECORD_LEN; k++) // 按照csv规则输出
      {
        Serial.print(record_aX[k]);
        Serial.print(",");
        Serial.print(record_aY[k]);
        Serial.print(",");
        Serial.print(record_aZ[k]);
        Serial.print(",");
        Serial.print(record_gX[k]);
        Serial.print(",");
        Serial.print(record_gY[k]);
        Serial.print(",");
        Serial.print(record_gZ[k]);
        Serial.println("");
      }
      if (data_cnt == 100) // 采集数量
      {
        pixels.setPixelColor(0, pixels.Color(0, 10, 0)); //改变颜色
        pixels.show();
        while (1)
        {
        }
      }
    }
    count = 0;
  }

  delay(10);
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
      //添加↓
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
