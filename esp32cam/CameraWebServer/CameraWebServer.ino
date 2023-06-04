#include "esp_camera.h"
#include <WiFi.h>
#include <AsyncUDP.h>
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

const char *ssid = "Redmi_K40";
const char *password = "15876376419";
#define maxcache 1024  //图像数据包的大小

//创建服务器端
WiFiServer server;
//创建客户端
WiFiClient client;
String  frame_begin = "FrameBegin"; //图像传输包头
String  frame_over = "FrameOverr";  //图像传输包尾

//.......................................................//

void setupLedFlash(int pin);

int initCamera()
{

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return 1;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  Serial.println("Camera Init OK!!!");
  return 0;
}

bool wifi_init(){
 WiFi.mode(WIFI_STA);                  //wifi初始化
  WiFi.setSleep(false); //关闭STA模式下wifi休眠，提高响应速度
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  Serial.println();
while (WiFi.status() != WL_CONNECTED && i++ < 20) {
    delay(500);
    Serial.print(".");

}
if (i == 21) {
  Serial.println();
  Serial.print("Could not connect to"); 
  Serial.println(ssid);

  return false;
}
 Serial.print("Connecting to wifi "); 
  Serial.print(ssid);
  Serial.println(" success!"); 
  return true;


}

void setup() //程序加电后初始化
{
Serial.begin(115200);
Serial.setDebugOutput(true);
wifi_init();
//启动server
server.begin(8080);
//关闭小包合并包功能，不会延时发送数据
server.setNoDelay(true);
Serial.print("Ready! TCP Server");
Serial.print(WiFi.localIP());
Serial.println(":8080 Running!");


  initCamera(); //相机初始化

  pinMode(4, OUTPUT);    //LED灯，GPIO4
  digitalWrite(4, HIGH); // turn the LED on by making the voltage HIGH
  delay(1000);
  digitalWrite(4, LOW); // turn the LED off by making the voltage LOW
}
void cssp(){
  camera_fb_t * fb = esp_camera_fb_get();
  uint8_t * temp = fb->buf; //这个是为了保存一个地址，在摄像头数据发送完毕后需要返回，否则会出现板子发送一段时间后自动重启，不断重复
  if (!fb)
  {
      Serial.println("Camera Capture Failed");
  }
  else
  { 
    //先发送Frame Begin 表示开始发送图片 然后将图片数据分包发送 每次发送1430 余数最后发送 
    //完毕后发送结束标志 Frame Over 表示一张图片发送完毕 
    client.print(frame_begin); //一张图片的起始标志
    // 将图片数据分段发送
    int leng = fb->len;
    int timess = leng/maxcache;
    int extra = leng%maxcache;
    for(int j = 0;j< timess;j++)
    {
      client.write(fb->buf, maxcache); 
      for(int i =0;i< maxcache;i++)
      {
        fb->buf++;
      }
    }
    client.write(fb->buf, extra);
    client.print(frame_over);      // 一张图片的结束标志
    //Serial.print("This Frame Length:");
    //Serial.print(fb->len);
    //Serial.println(".Succes To Send Image For TCP!");
    //return the frame buffer back to the driver for reuse
    fb->buf = temp; //将当时保存的指针重新返还
    esp_camera_fb_return(fb);  //这一步在发送完毕后要执行，具体作用还未可知。        
  }
  //delay(20);//短暂延时 增加数据传输可靠性        
}


void loop()
{
  if (server.hasClient()) {
    if ( client && client.connected()) {
      WiFiClient serverClient = server.available();
      serverClient.stop();
      Serial.println("Connection rejected!");
    }else{
      //分配最新的client
      client = server.available();
    
      Serial.println("Client is Connect!");
    }
  }
  
    if (client && client.connected()) {
      cssp();

      delay(50); //50毫秒循环一次,也就是每秒发送20次照片,在局域网内,这不算快,当然没有必要这么快了,你可以调大一些
      }
}
