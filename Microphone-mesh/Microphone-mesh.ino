#include <HTTPClient.h>
#include <M5StickC.h>

#define PIN_CLK  0
#define PIN_DATA 34
#define READ_LEN (1 * 100)
uint8_t BUFFER[READ_LEN] = {0};

uint16_t oldx[160];
uint16_t oldy[160];
uint16_t *adcBuffer = NULL;
HTTPClient http;  
uint16_t lastSignalLevel = 0;
#include "setupConfig.h"

#include <WiFi.h>
#include <driver/i2s.h>

void i2sInit()
{
   i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate =  44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
    .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 128,
   };

   i2s_pin_config_t pin_config;
   pin_config.bck_io_num   = I2S_PIN_NO_CHANGE;
   pin_config.ws_io_num    = PIN_CLK;
   pin_config.data_out_num = I2S_PIN_NO_CHANGE;
   pin_config.data_in_num  = PIN_DATA;
  
   
   i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
   i2s_set_pin(I2S_NUM_0, &pin_config);
   i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void mic_record_task() 
{   

    i2s_read_bytes(I2S_NUM_0, (char*) BUFFER, READ_LEN, (100 / portTICK_RATE_MS));
    adcBuffer = (uint16_t *)BUFFER;
    
    int signalLevel = 0;   
    for ( int i = 0; i < READ_LEN; i++ ) {
       signalLevel += adcBuffer[i];
    }
    signalLevel = ((signalLevel/READ_LEN)/100 -290)*3;
    
   if (signalLevel>=0 && signalLevel<360 && (signalLevel!=lastSignalLevel)) {
    Serial.println(signalLevel);
     http.begin("http://192.168.0.59/device_request");    // Specify destination for HTTP request
     http.addHeader("Content-Type", "application/json");  // Specify content-type headers
     
     // Replace this with your Light ID after making a mesh_info call:
     http.addHeader("Mesh-Node-Mac", "3c71bf9d6ab4,3c71bf9d6980");
     
     String json = "{\"request\": \"set_status\",\"characteristics\": [{\"cid\": 1,\"value\": "+String(signalLevel)+"}]}";
     int httpResponseCode = http.POST(json);
      M5.Lcd.fillScreen(WHITE);
      M5.Lcd.setCursor(10, 20);
      
      if(httpResponseCode>0){
      String response = http.getString();                       //Get the response to the request
      Serial.println(httpResponseCode);Serial.println();   //Print return code
      //Serial.println(response);           //Print request answer 
      M5.Lcd.print(String(signalLevel));
      lastSignalLevel = signalLevel;
     }
     http.end();
   }
   showSignal(); 
   
   //vTaskDelay(100 / portTICK_RATE_MS);
   //vTaskDelete(NULL);
}

void showSignal(){
  int x, y;
  for (int n = 0; n < READ_LEN; n++){
    x = n;
    y = map(adcBuffer[n], 0, 65535, 10, 70);
    M5.Lcd.drawPixel(oldx[n], oldy[n],WHITE);
    M5.Lcd.drawPixel(x,y,BLACK);
    oldx[n] = x;
    oldy[n] = y;
  }
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  
 // TODO: Get IP via mDNS from : esp32_mesh.local

 
  M5.Lcd.println("Connect WIFI:");
  M5.Lcd.println(WIFISSID);

  WiFi.begin(WIFISSID, WIFIPASS); 
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) { 
    M5.Lcd.print(".");delay(100);
    if (count>100) ESP.restart();
    count++;
  }
 
  M5.Lcd.println("Connected to WiFi");delay(500);
  
  M5.Axp.ScreenBreath(8); // Brightness (min and visible 7 - 10 max)
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setTextColor(BLACK, WHITE);

  i2sInit();
  
}

void loop() {
  mic_record_task();
  
  delay(15);
}
