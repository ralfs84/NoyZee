
#include <driver/i2s.h>
#include <math.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "homepage.h"

;
const char* ssid = "Bruh";
const char* password = "passwordpass";
int x = 0;
int prev;
WebServer server(80);

#define I2S_PORT I2S_NUM_0

// MAX98357
#define MAX_DIN 33   
#define MAX_LRC 32   
#define MAX_BCLK 25  

// Audio parameters
#define SAMPLE_RATE 44100
float TONE_FREQ =  261.6 ;
#define AMPLITUDE 20000  // Max 32767

#define RIGHT true

// Buffer size (frames, not samples)
#define BUFFER_LEN 256

// Stereo buffer: Left, Right
int16_t samples[BUFFER_LEN * 2];

void handleRoot() {
  String message = homePagePart;
  server.send(200, "text/html", message);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }


  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  Serial.println("ESP32 Touch Test");

  server.begin();
  Serial.println("HTTP server started");


  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_LEN,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = MAX_BCLK,
    .ws_io_num = MAX_LRC,
    .data_out_num = MAX_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
 
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks

  //Serial.println(touchRead(27));  

  static float phase = 0.0;
  const float phaseIncrement = 2.0 * PI * TONE_FREQ / SAMPLE_RATE;

  for (int i = 0; i < BUFFER_LEN; i++) {
    //int16_t sound = (int16_t)(AMPLITUDE * sin(phase));
    int16_t silence = 0;
      int16_t sound = 0;
    //samples[2 * i] = (LEFT) ? sound : silence;       // Left channel

    if(touchRead(4) < 400)
    {
      TONE_FREQ = 440;
      const float phaseIncrement = 2.0 * PI * TONE_FREQ / SAMPLE_RATE;
      sound = (int16_t)(AMPLITUDE * sin(phase));
    }
    else if(touchRead(27) < 400)
    {
      TONE_FREQ = 493.883;
      const float phaseIncrement = 2.0 * PI * TONE_FREQ / SAMPLE_RATE;
      sound = (int16_t)(AMPLITUDE * sin(phase));
    }
    else
    {
      sound = 0;
    }
    samples[2 * i + 1] = (RIGHT) ? sound : silence;  // Right channel

    phase += phaseIncrement;
    if (phase >= 2.0 * PI) phase -= 2.0 * PI;
  }

  size_t bytes_written;
  i2s_write(I2S_PORT, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
  
}
