#include <HTTPClient.h>
#include <M5StickC.h>

#define PIN_CLK  0
#define PIN_DATA 34
#define READ_LEN (1 * 1024)
uint8_t BUFFER[READ_LEN] = {0};

uint16_t oldx[160];
uint16_t oldy[160];
uint16_t *adcBuffer = NULL;
HTTPClient http;  

#include "setupConfig.h"

void mic_record_task() //void* arg
{   

    i2s_read_bytes(I2S_NUM_0, (char*) BUFFER, READ_LEN, (100 / portTICK_RATE_MS));
    adcBuffer = (uint16_t *)BUFFER;
    
    int signalLevel = 0;   
    for ( int i = 0; i < READ_LEN; i++ ) {
       signalLevel += adcBuffer[i];
    }
    signalLevel = ((signalLevel/READ_LEN)/100)-200;
   Serial.print(signalLevel);Serial.println();
 
   http.begin("http://192.168.0.75/device_request");    // Specify destination for HTTP request
   http.addHeader("Content-Type", "application/json");  // Specify content-type headers
   
   // Replace this with your Light ID after making a mesh_info call:
   http.addHeader("Mesh-Node-Mac", "3c71bf9d6ab4,3c71bf9d6980");
   
   String json = "{\"request\": \"set_status\",\"characteristics\": [{\"cid\": 1,\"value\": "+String(signalLevel)+"}]}";
   int httpResponseCode = http.POST(json);
    
    if(httpResponseCode>0){
    String response = http.getString();                       //Get the response to the request
    //Serial.println(httpResponseCode);   //Print return code
    //Serial.println(response);           //Print request answer
 
   }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
   }
   http.end();
   // Let's focus first on sending the Light signal fast
   //showSignal(); 
   //vTaskDelay(100 / portTICK_RATE_MS);
}

void showSignal(){
  int x, y;
  for (int n = 0; n < 160; n++){
    x = n;
    y = map(adcBuffer[n], 0, 65535, 10, 70);
    M5.Lcd.drawPixel(oldx[n], oldy[n],WHITE);
    M5.Lcd.drawPixel(x,y,BLACK);
    oldx[n] = x;
    oldy[n] = y;
  }
}

void loop() {
  mic_record_task();
  delay(10);
}
