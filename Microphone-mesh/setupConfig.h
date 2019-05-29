// UPDATE with your connection
#define WIFISSID "KabelBox-A210"
#define WIFIPASS "14237187131701431551"

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

