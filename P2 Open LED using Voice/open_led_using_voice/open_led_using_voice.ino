#include <driver/i2s.h>
#include <arduinoFFT.h>

#include <Adafruit_NeoPixel.h>

#include <TensorFlowLite_ESP32.h>
#include <tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h>
#include <tensorflow/lite/experimental/micro/micro_error_reporter.h>
#include <tensorflow/lite/experimental/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

#include "model.h"

#define MAX_STEP             5
#define SAMPLE_NUMS      30

#define PIN            12 
#define NUM      1 //灯个数
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM, PIN, NEO_GRB + NEO_KHZ800);

#define I2S_PORT I2S_NUM_1

#define I2S_WS 18
#define I2S_SD 32
#define I2S_SCK 14

const int BLOCK_SIZE = 512;
int16_t samples[BLOCK_SIZE];

arduinoFFT FFT = arduinoFFT();
double vReal[BLOCK_SIZE];
double vImag[BLOCK_SIZE];
String labels[] = {"125", "250", "500", "1K", "2K", "4K", "8K", "16K"};
int bands[8] = {0, 0, 0, 0, 0, 0, 0, 0};
const uint8_t amplitude = 150;

const int record_num=30;
float record_125[record_num];
float record_250[record_num];
float record_500[record_num];
float record_1k[record_num];
float record_2k[record_num];
float record_4k[record_num];
float record_8k[record_num];
float record_16k[record_num];

int data_count=0;
int n=100; //声音的次数

tflite::MicroErrorReporter tflErrorReporter;
tflite::ops::micro::AllOpsResolver tflOpsResolver;
const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize];

int sign=0;
int Inputindex=0;
int Inputbands[SAMPLE_NUMS][8]={0};
int steps=MAX_STEP;

const char* VOICES[] = {
  "silence",
  "open",
  "close"
};

#define NUM_VOICES (sizeof(VOICES) / sizeof(VOICES[0]))


void setup() {
  Serial.begin(115200);
  i2s_init();
  i2s_setpin();
  pixels.begin(); 
  pixels.clear(); //清除颜色
  pixels.show(); //打印
  pixels.setPixelColor(0, pixels.Color(0,0,0)); //调颜色
  pixels.show(); //打印
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);
  tflInterpreter->AllocateTensors();
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
  delay(1000);
}


void loop() {
  int num_bytes_read = i2s_read_bytes(I2S_PORT, 
                                      (void *)samples, 
                                      BLOCK_SIZE*2,     
                                      portMAX_DELAY); // no timeout

  FFT_Operation();
  constrain128();
  move_Inputindex();

  if(sign==1){
    detection();
    application();    
    sign=0;
  }
}

int bsum=0;
int Threshold_HIGH=0;
int Threshold_LOW=20;
int smooth_count=0;
int record_count=-1;

void constrain128(){
      for(int j=0;j<8;j++){
      bands[j]=int(bands[j]*(128.0/100000));
      bands[j]=constrain(bands[j],0,128);
    }
}

void move_Inputindex(){
  if(Inputindex<SAMPLE_NUMS){
    for(int bands_index=0;bands_index<8;bands_index++){
      Inputbands[Inputindex][bands_index]=(int)bands[bands_index];
    }
    Inputindex=Inputindex+1;
    if(Inputindex==SAMPLE_NUMS){
      sign=1;
    }

  }else{

    steps=steps+1;
    if(steps==MAX_STEP+1){
      for(int mo=0;mo<SAMPLE_NUMS-MAX_STEP;mo++){
        for(int bands_index=0;bands_index<8;bands_index++){
          Inputbands[mo][bands_index]=Inputbands[mo+MAX_STEP][bands_index];
        }
      }   
      steps=0; 
    }
    else{
        for(int bands_index=0;bands_index<8;bands_index++){
          Inputbands[SAMPLE_NUMS-1-MAX_STEP+steps][bands_index]=(int)bands[bands_index];
        }
        if(steps==MAX_STEP){
          sign=1;
        }

    }
  }
}

void detection(){
  for(int i=0;i<SAMPLE_NUMS;i++){
    for(int j=0;j<8;j++){
      tflInputTensor->data.f[i * 8 + j] = Inputbands[i][j]/ 128.0;
    }
  }

    TfLiteStatus invokeStatus = tflInterpreter->Invoke();
    if (invokeStatus != kTfLiteOk) {
      Serial.println("Invoke failed!");
      while (1);
      return;
    }
}


void application(){
  float  silence=tflOutputTensor->data.f[0];
  float  open=tflOutputTensor->data.f[1];
  float  close=tflOutputTensor->data.f[2];


  if(silence==1){

  }else{
        for (int i = 0; i < NUM_VOICES; i++) {
            Serial.print(VOICES[i]);
            Serial.print(": ");
            Serial.println(tflOutputTensor->data.f[i], 6);
        }
    if(open>close&&open>0.6){
      Serial.print("open");
      Serial.println(open);
      Serial.println(close);
      pixels.clear(); //调颜色
      pixels.show(); //打印
      pixels.setPixelColor(0, pixels.Color(255,255,255)); //调颜色
      pixels.show(); //打印
    }else if(close>open&&close>0.6){
      Serial.print("close");
      Serial.println(close);
      Serial.println(open); 
      pixels.clear(); //调颜色
      pixels.show(); //打印
      pixels.setPixelColor(0, pixels.Color(0,0,0)); //调颜色
      pixels.show(); //打印
    }
  }

}

void FFT_Operation(){
  for (uint16_t i = 0; i < BLOCK_SIZE; i++) {
    vReal[i] = samples[i] << 8;
    vImag[i] = 0.0; 
  }

  FFT.Windowing(vReal, BLOCK_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, BLOCK_SIZE, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, BLOCK_SIZE);
  for (int i = 0; i < 8; i++) {
    bands[i] = 0;
  }

  //0赫兹不存在，另外由于FFT结果的对称性，通常我们只使用前半部分的结果，即小于采样频率一半的结果。
  for (int i = 1; i < (BLOCK_SIZE/2); i++){ 
    if (vReal[i] > 2000) { 
      if (i<2 )              bands[0] = max(bands[0], (int)(vReal[i]/amplitude)); // 125Hz
      if (i >=2  && i<=4 )   bands[1] = max(bands[1], (int)(vReal[i]/amplitude)); // 250Hz
      if (i >4   && i<=8 )   bands[2] = max(bands[2], (int)(vReal[i]/amplitude)); // 500Hz
      if (i >8   && i<=15 )  bands[3] = max(bands[3], (int)(vReal[i]/amplitude)); // 1000Hz
      if (i >15  && i<=35 )  bands[4] = max(bands[4], (int)(vReal[i]/amplitude)); // 2000Hz
      if (i >35  && i<=58 )  bands[5] = max(bands[5], (int)(vReal[i]/amplitude)); // 4000Hz
      if (i >58  && i<=128 ) bands[6] = max(bands[6], (int)(vReal[i]/amplitude)); // 8000Hz
      if (i >128           ) bands[7] = max(bands[7], (int)(vReal[i]/amplitude)); // 16000Hz
    }
  }
}

void i2s_init(){
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = true
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin(){
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}
