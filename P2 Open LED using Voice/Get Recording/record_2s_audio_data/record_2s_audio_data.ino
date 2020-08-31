#include <driver/i2s.h>

#define I2S_WS 18
#define I2S_SD 32
#define I2S_SCK 14

const int BLOCK_SIZE = 16000;
int16_t samples[BLOCK_SIZE];

int samples_count=0;
int start_time=0;

#define I2S_PORT I2S_NUM_1

void setup(){
  Serial.begin(115200);
  i2s_init();
  i2s_setpin();
  delay(1000);
}

void loop(){
  int16_t sample=0;
  int bytes=i2s_pop_sample(I2S_PORT,(void *)&sample,portMAX_DELAY);
  if(bytes>0&&samples_count<BLOCK_SIZE){
    if(samples_count==0){
      start_time=millis();
    }
    samples[samples_count]=sample;
    samples_count=samples_count+1;
  }else if(samples_count==BLOCK_SIZE){
      int fin_time=millis()-start_time;
      Serial.println(fin_time);
      for(int i=0;i<BLOCK_SIZE;i++){
        Serial.print(samples[i]);
        Serial.print(", ");
      }
      samples_count=samples_count+1;
  }
}

void i2s_init(){
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 8000,
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
