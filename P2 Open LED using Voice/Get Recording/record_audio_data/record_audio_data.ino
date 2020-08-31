#include <driver/i2s.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#define I2S_PORT I2S_NUM_1

#define I2S_WS 18
#define I2S_SD 32
#define I2S_SCK 14

const int BLOCK_SIZE = 512;
int16_t samples[BLOCK_SIZE];

const char * ssid = "Hollow_ESP32";
const char * password = "hollowman";



char webpage[] PROGMEM = R"=====(
<html>
<head>
  <script src='https://cdn.staticfile.org/jquery/1.10.2/jquery.min.js'></script>
</head>
<body onload="javascript:init()">

<p></p>


<script>
  var webSocket
  var data
  var count=0
  var voice=new Array()
  var show=""
  function init() {
    webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
    webSocket.onmessage = function(event) {
      data = JSON.parse(event.data);
      voice=voice.concat(data.data)
      show=JSON.stringify(voice)
      $("p").html(show); 
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

  String json="";
  DynamicJsonDocument jsonobj(1024);
  JsonArray data = jsonobj.createNestedArray("data");

  for (int i = 0; i < BLOCK_SIZE; i++) {
    data.add(samples[i]);
  }

  serializeJson(jsonobj, json);

  webSocket.broadcastTXT(json.c_str(), json.length());
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
