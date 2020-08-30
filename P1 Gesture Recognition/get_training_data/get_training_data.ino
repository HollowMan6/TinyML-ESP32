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
const float accelerationThreshold_HIGH = 3.5; // 阈值为3.5倍重力

const float accelerationThreshold_LOW= 1.9;
int smooth_count = 0;
int record_count=0;

float record_aX[300];
float record_aY[300];
float record_aZ[300];

float record_gX[300];
float record_gY[300];
float record_gZ[300];

const int record_num=70;

int data_count=0;
int n=100; //动作的次数

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
  record_count=-1; //防止第一次数据的错误触发
}

void loop(){
  dirty=0;
  while(dirty!=1){
      measure();
      analysis();
  }
  float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

  if (aSum >= accelerationThreshold_HIGH&&record_count==-1)//动作开始
  {
    record_count=0;

    data_count++;
    if(data_count==n){
      pixels.setPixelColor(0, pixels.Color(255,0,0)); //调颜色
      pixels.show(); //打印
    }else if(data_count>n){
      pixels.setPixelColor(0, pixels.Color(0,255,0)); //调颜色
      pixels.show(); //打印
    }
  }

  if(record_count<record_num&&record_count!=-1)//收集70个元组数据
  {
    record_aX[record_count]=aX;
    record_aY[record_count]=aY;
    record_aZ[record_count]=aZ;
    record_gX[record_count]=gX;
    record_gY[record_count]=gY;
    record_gZ[record_count]=gZ;
    record_count++;
  }

  if(record_count==record_num)//收集完成一次动作的70个元组数据
  {
      for(int k=0;k<record_count;k++){
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
      record_count=-1;
  }
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
