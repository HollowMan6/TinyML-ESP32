#include <driver/i2s.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <arduinoFFT.h>

#include <Adafruit_NeoPixel.h>
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

const char * ssid = "Hollow_ESP32";
const char * password = "hollowman";


char webpage[] PROGMEM = R"=====(
<html>
<!-- Adding a data chart using Chart.js -->
<head>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js'></script>
</head>
<body onload="javascript:init()">

<div>
  <canvas id="chart" width="600" height="400"></canvas>
</div>
<!-- Adding a websocket to the client (webpage) -->
<script>
  var webSocket, dataPlot;
  var maxDataPoints = 20;
  const maxValue = 128;
  const maxLow = maxValue * 0.5;
  const maxMedium = maxValue * 0.2;
  const maxHigh = maxValue * 0.3;
  function init() {
    webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
    dataPlot = new Chart(document.getElementById("chart"), {
      type: 'bar',
      data: {
        labels: [],
        datasets: [{
          data: [],
          label: "L",
          backgroundColor: "#D6E9C6"
        },
        {
          data: [],
          label: "M",
          backgroundColor: "#FAEBCC"
        },
        {
          data: [],
          label: "H",
          backgroundColor: "#EBCCD1"
        },
        ]
      }, 
      options: {
          responsive: false,
          animation: false,
          scales: {
              xAxes: [{ stacked: true }],
              yAxes: [{
                  display: true,
                  stacked: true,
                  ticks: {
                    beginAtZero: true,
                    steps: 1000,
                    stepValue: 500,
                    max: maxValue
                  }
              }]
           }
       }
    });
    webSocket.onmessage = function(event) {
      var data = JSON.parse(event.data);
      dataPlot.data.labels = [];
      dataPlot.data.datasets[0].data = [];
      dataPlot.data.datasets[1].data = [];
      dataPlot.data.datasets[2].data = [];

      data.forEach(function(element) {
        dataPlot.data.labels.push(element.bin);
        var lowValue = Math.min(maxLow, element.value);
        dataPlot.data.datasets[0].data.push(lowValue);

        var mediumValue = Math.min(Math.max(0, element.value - lowValue), maxMedium);
        dataPlot.data.datasets[1].data.push(mediumValue);

        var highValue = Math.max(0, element.value - lowValue - mediumValue);
        dataPlot.data.datasets[2].data.push(highValue);
      });
      dataPlot.update();
    }
  }
</script>
</body>
</html>
)=====";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);


void setup() {
  Serial.begin(115200);
  i2s_init();
  i2s_setpin();
  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  pixels.begin(); 
  pixels.clear(); //清除颜色
  pixels.show(); //打印
  pixels.setPixelColor(0, pixels.Color(0,0,0)); //调颜色
  pixels.show(); //打印
  delay(1000);
  server.on("/",[](){
    server.send_P(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();
}


void loop() {
  webSocket.loop();
  server.handleClient();
  int num_bytes_read = i2s_read_bytes(I2S_PORT, 
                                      (void *)samples, 
                                      BLOCK_SIZE*2,     
                                      portMAX_DELAY); // no timeout

  FFT_Operation();
  constrain128();
  Check_Start();
  Send_Msg();
}

int bsum=0;
int Threshold_HIGH=40;
//int Threshold_HIGH=0; 用于搜集环境噪音
int Threshold_LOW=20;
int smooth_count=0;
int record_count=-1;

void Check_Start(){
    bsum=0;
    for(int j=0;j<8;j++){
      bsum=bsum+bands[j];
    }

    if(bsum>=Threshold_HIGH&&record_count==-1){
    data_count++;
    if(data_count==n){
      pixels.setPixelColor(0, pixels.Color(255,0,0)); //调颜色
      pixels.show(); //打印
    }else if(data_count>n){
      pixels.setPixelColor(0, pixels.Color(0,0,255)); //调颜色
      pixels.show(); //打印
    }    
    record_count=0;
  }

  if(record_count<record_num&&record_count!=-1)//收集30条数据
  {
       record_125[record_count]= bands[0];
       record_250[record_count]= bands[1];
       record_500[record_count]= bands[2];
       record_1k[record_count]= bands[3];
       record_2k[record_count]= bands[4];
       record_4k[record_count]= bands[5];
       record_8k[record_count]= bands[6];
       record_16k[record_count]= bands[7];
       record_count++;
  }

  if(record_count==record_num)//收集完成一次声音的30个元组数据
    {
        for(int k=0;k<record_count;k++){
          Serial.print(record_125[k]);
          Serial.print(",");
          Serial.print(record_250[k]);
          Serial.print(",");
          Serial.print(record_500[k]);
          Serial.print(",");
          Serial.print(record_1k[k]);
          Serial.print(",");
          Serial.print(record_2k[k]);
          Serial.print(",");
          Serial.print(record_4k[k]);
          Serial.print(",");
          Serial.print(record_8k[k]);
          Serial.print(",");
          Serial.print(record_16k[k]);
          Serial.println("");
        }
        record_count=-1;
    }

}

void constrain128(){
      for(int j=0;j<8;j++){
      bands[j]=int(bands[j]*(128.0/100000));
      bands[j]=constrain(bands[j],0,128);
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

void Send_Msg() {
  String json = "[";
  for (int i = 0; i < 8; i++) {
    if (i > 0) {
      json +=", ";
    }
    json += "{\"bin\":";
    json += "\"" + labels[i] + "\"";
    json += ", \"value\":";
    json += String(bands[i]);
    json += "}"; 
  }
  json += "]";
  webSocket.broadcastTXT(json.c_str(), json.length());
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
