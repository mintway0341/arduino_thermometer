#include "DM_G_I2C.h" 
#include <SPI.h> 
#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>  
#include <DallasTemperature.h> 
#include <SoftwareSerial.h>
#include <pitches.h>

//DS18B20 온도 센서의 데이터선인 가운데 핀을 아두이노 3번에 연결
#define ONE_WIRE_BUS 3

//1-wire 디바이스와 통신하기 위한 준비  
OneWire oneWire(ONE_WIRE_BUS);
    
// oneWire선언한 것을 sensors 선언시 참조함.  
DallasTemperature sensors(&oneWire);
    
//다비아스 주소를 저장할 배열 선언  
DeviceAddress insideThermometer;


int Tx=6; //전송
int Rx=7; //수신
int speakerOut = 4;

SoftwareSerial btSerial(Tx, Rx);

float measuredTemperature = 0.0;

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);        
  G_I2C_Scanner(); delay(1000); 
  OLED_SETUP();

  pinMode(speakerOut, OUTPUT); 
  
  Serial.println("Dallas Temperature IC Control Library Demo");  
    
  //1-wire 버스 초기화  
  sensors.begin();  

  Serial.println("Begin");
      
  //발견한 디바이스 갯수  
  Serial.print("Found ");  
  Serial.print(sensors.getDeviceCount(), DEC);  
  Serial.println(" devices.");  
    
  // parasite power 모드일 때에는  2핀(GND와 DQ 핀)만 연결하면 됨.  
  Serial.print("Parasite power is: ");   
  if (sensors.isParasitePowerMode()) Serial.println("ON");  
  else Serial.println("OFF");  
      
     
  //버스에서 첫번째 장치의 주소를 가져온다.  
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");   
      
  //버스에서 발견한 첫번째 장치의 주소 출력  
  Serial.print("Device 0 Address: ");  
  printAddress(insideThermometer);  
  Serial.println();  
    
  //데이터시트에서 확인결과 9~12비트까지 설정 가능  
  sensors.setResolution(insideThermometer, 10);  
     
  Serial.print("Device 0 Resolution: ");  
  Serial.print(sensors.getResolution(insideThermometer), DEC);   
  Serial.println();

  OLED_Display_clear_display();
}



// 온도를 출력하는 함수  
float printTemperature(DeviceAddress deviceAddress)  
{  
  //섭씨 온도를 가져옴  
  float  tempC = sensors.getTempC(deviceAddress);
  measuredTemperature = tempC;

  Serial.print("Temp C: ");  
  Serial.print(tempC);

  btSerial.println(tempC);

  return tempC;
}


    
//디바이스 주소를 출력하는 함수  
void printAddress(DeviceAddress deviceAddress)  
{  
  for (uint8_t i = 0; i < 8; i++)  
  {  
    if (deviceAddress[i] < 16) Serial.print("0");  
        Serial.print(deviceAddress[i], HEX);  
  }  
}  
  

void loop() { 
  byte receive = '0';
  if (btSerial.available()) {  
    receive = btSerial.read();     
    Serial.write(receive);
  }
  if (Serial.available()) {         
    btSerial.write(Serial.read());
  }

  //센서에서 읽어온 온도를 출력
  if(receive == '1') {
    Serial.println("Success");
    Serial.print("Requesting temperatures...");
    sensors.requestTemperaturesByIndex(0);
    float tempC = printTemperature(insideThermometer);

    char tmp[] = "hello";
    dtostrf(tempC, 5, 2, tmp);
    OLED_Display_clear_display();
    OLED_Display_add(2,0,0, tmp);
    
    if (measuredTemperature >= 30.0) {
      tone(speakerOut, NOTE_C6, 150);
      delay(250);
      tone(speakerOut, NOTE_C6, 150);
      delay(250);
      tone(speakerOut, NOTE_C6, 150);
      delay(250);
      tone(speakerOut, NOTE_C6, 150);
      delay(250);
      tone(speakerOut, NOTE_C6, 150);
      delay(250);
    }
    else {
      tone(speakerOut, NOTE_C6, 450);
      delay(400);
      tone(speakerOut, NOTE_E6, 450);
      delay(400);
      tone(speakerOut, NOTE_G6, 450);
      delay(400);
    }
  }
  delay(100); 
}
