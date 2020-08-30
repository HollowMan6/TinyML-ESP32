#include <Adafruit_NeoPixel.h>
#include <HardwareSerial.h>
HardwareSerial mySerial(1);

int conut=0;
unsigned char sign=0;
unsigned char Re_buf[25],counter=0;

int YPR[3];
int16_t gyro[3]={0};
int16_t acc[3]={0};
float aX, aY, aZ, gX, gY, gZ,yX,yY,yZ;
int dirty=0;

#define PIN            12 
#define NUM      1 //灯个数
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM, PIN, NEO_GRB + NEO_KHZ800);

void setup(){
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 15, 21);
  delay(500);
  mySerial.write(0XA5); 
  mySerial.write(0X55);
  mySerial.write(0X57);    //初始化GY25Z,输出陀螺和欧拉角
  mySerial.write(0X51); 
  delay(100); 
  mySerial.write(0XA5); 
  mySerial.write(0X56);    //初始化GY25Z,连续输出模式
  mySerial.write(0X02);    //初始化GY25Z,连续输出模式
  mySerial.write(0XFD);
  delay(100);
  pixels.begin(); 
  pixels.clear(); //清除颜色
  pixels.show(); //打印
  pixels.setPixelColor(0, pixels.Color(0,0,0)); //调颜色
  pixels.show(); //打印
}

void loop(){
  dirty=0;
  while(dirty!=1){
      measure();
      analysis();
  }
  int xSum = fabs(yX)*125/36000;
  int ySum = fabs(yY)*125/36000;
  int zSum = fabs(yZ)*125/36000;
  pixels.setPixelColor(0, pixels.Color(xSum,ySum,zSum)); //调颜色
  pixels.show(); //打印
}

void measure(){
  dirty=0;
  while (mySerial.available()) {
    Re_buf[counter]=(unsigned char)mySerial.read();
    if(counter==0&&Re_buf[0]!=0x5A){ return;}
    counter++;
    if(counter==25)                //接收到数据
    {
      counter=0;                 //重新赋值，准备下一帧数据的接收 
      sign=1;
    }
  }
}

void analysis(){
  if(sign)
  {
    sign=0;
    if(Re_buf[0]==0x5A&&Re_buf[1]==0x5A )        //检查帧头，帧尾
    { 
      acc[0]=(Re_buf[4]<<8|Re_buf[5]);   
      acc[1]=(Re_buf[6]<<8|Re_buf[7]);
      acc[2]=(Re_buf[8]<<8|Re_buf[9]);

      gyro[0]=(Re_buf[10]<<8|Re_buf[11]);   
      gyro[1]=(Re_buf[12]<<8|Re_buf[13]);
      gyro[2]=(Re_buf[14]<<8|Re_buf[15]);

      YPR[0]=(Re_buf[16]<<8|Re_buf[17]);   
      YPR[1]=(Re_buf[18]<<8|Re_buf[19]);
      YPR[2]=(Re_buf[20]<<8|Re_buf[21]);

      for(int j=0;j<3;j++){
        if(YPR[j]>46000){
          YPR[j]=YPR[j]-29535;
        }else{
        YPR[j]=YPR[j];
        }  
      }
      aX=acc[0]/16383.5;
      aY=acc[1]/16383.5;
      aZ=acc[2]/16383.5;
      gX=gyro[0]/16.3835;
      gY=gyro[1]/16.3835;
      gZ=gyro[2]/16.3835;
      yX=YPR[0];
      yY=YPR[1];
      yZ=YPR[2];
      dirty=1;
    }
  } 
}
