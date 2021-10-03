#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h" //온도 습도 헤더 파일 주입 선언
#include <MQUnifiedsensor.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include "ThingSpeak.h" //IOT WIFI 헤더 파일 주입 
#include "secrets.h" //IOT API 값 헤더 파일 주입 
#include <ESP8266WiFi.h> //WIFI 칩 헤더 파일 주입 
#include <Fonts/FreeMono12pt7b.h> //OLED 글꼴 및 폰트 설정
#define DHTTYPE DHT22 //온습도계 종류 설정
 
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
 
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
 

uint8_t DHTPin = D5; //온습도 센서 데이터 핀
DHT dht(DHTPin, DHTTYPE); //온습도 센서 헤더파일 선언
float humidity = dht.readHumidity(); //온도 변수 선언
float temperature = dht.readTemperature(); //습도 변수 선언
int sound = D0; //스피커
int numTones = 8;
int tones[]={261, 294, 330, 349, 392, 440, 494, 523}; // 기획중인 스피커 음 배열변수 선언
   
#define SCREEN_WIDTH 128 // OLED display width, in pixels, OLED 높이 픽셀 지정
#define SCREEN_HEIGHT 64 // OLED display height, in pixels, OLED 넓이 픽셀 지정

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //OLED 헤더 파일 주입

void setup() {
  Serial.begin(115200);  // Initialize serial
  dht.begin(); //온습도 센서 시작
  pinMode(DHTPin,INPUT); //온습도 센서 입출력핀
  WiFi.mode(WIFI_STA); //WIFI 설정 
  ThingSpeak.begin(client);  // Initialize ThingSpeak, IOT API 설정 
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(("SSD1306 allocation failed"));
    for(;;);
  } // 만약에 OLED가 정상적으로 작동되지 않는다면 시리얼 프린터에 경고문 발생
    digitalWrite(sound,OUTPUT); //스피커 입출력 선언

   display.clearDisplay(); //디스플레이 초기화
 // display.setTextSize(8); //디스플레이 텍스트 사이즈 선언(위의 글꼴 헤더파일에서 설정)
  display.setTextColor(WHITE);//디스플레이 색상 설정
  display.setCursor(0, 10);// 디스플레이 글꼴 시작 위치)
  display.setFont(&FreeMono12pt7b);//디스플레이 글꼴 설정
  
  // Display static text
  display.print("TEMP: ");
  display.println(dht.readTemperature()); //온도계 값
  display.print("HUM: ");
  display.println(dht.readHumidity()); // 습도계 값
  display.display(); 

}
 
void loop() { 

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print("."); 
      delay(5000);     
    } 
    Serial.println("\nConnected."); //WIFI 연결이 성공적으로 작동하면 시리얼 모니터에 선언
  }
               
  // set the fields with the values
  ThingSpeak.setField(1, dht.readTemperature()); //IOT API에 온습도 센서의 온도값 전달
  ThingSpeak.setField(2, dht.readHumidity());    //IOT API에 읍슨도 센서의 습도값 전달

  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  } 
  
  //if (dht.readTemperature(D0) >= 30){ //경계 온도 시점
  //  for(int i=0;i<numTones;i++){
 // tone(sound,tones[i]);//
 //   }
 // }
  delay(20000); // Wait 20 seconds to update the channel again, 20초마다 API에게 온습도 정보 갱신
}


  
