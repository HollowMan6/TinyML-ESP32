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
}

void loop(){
  dirty=0;
  while(dirty!=1){
    measure();
    analysis();
  }
  Serial.print("aX:");Serial.print(aX);
  Serial.print("aY:");Serial.print(aY);
  Serial.print("aZ:");Serial.print(aZ);
  Serial.print("gX:");Serial.print(gX);
  Serial.print("gY:");Serial.print(gY);
  Serial.print("gZ:");Serial.print(gZ);
  Serial.print("yX:");Serial.print(yX);
  Serial.print("yY:");Serial.print(yY);
  Serial.print("yZ:");Serial.print(yZ);
  Serial.println(""); 
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
