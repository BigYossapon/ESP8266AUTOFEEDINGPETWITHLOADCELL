#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <time.h>

int timezone = 7 * 3600;                    //ตั้งค่า TimeZone ตามเวลาประเทศไทย
int dst = 0;                                //กำหนดค่า Date Swing Time


File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = D8;

#define calibration_factor 370000 // เอาค่าที่ได้จากการปรับ มาใส่ตรงนี้
#define DOUT  16
#define CLK  0

//****SERVO MOTOR****
Servo myservo; //ประกาศตัวแปรแทน Servo

//******BLYNK******
char auth[] = "PAUzjJQmds_Jy-pqWdaGorA0uQ0zRTAz";

HX711 scale(DOUT, CLK);
float w=(scale.get_units(), 2);
// Set the LCD address to 0x27 for a 16 chars and 2 line display
// Arduino UNO => PIN 4 = SCL, PIN 5 = SDA
// NodeMCU Dev Kit => D1 = SCL, D2 = SDA
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  int s;
  // initialize the LCD
  Serial.begin(9600);
  Serial.println("ArduinoAll Calibrating...");
  scale.set_scale(calibration_factor); // ปรับค่า calibration factor
  scale.tare(); //รีเซตน้ำหนักเป็น 0
  Serial.println("OK Start :");

  myservo.attach(2);// กำหนดขา 2 ควบคุม Servo
  
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();

   Blynk.begin(auth, "AndroidAP", "0947168060");

  while (WiFi.status() != WL_CONNECTED) { //รอจนกว่าจะเชื่อมต่อสำเร็จ 
    Serial.print("."); //แสดง ... ไปเรื่อยๆ จนกว่าจะเชื่อมต่อได้
    delay(500);
  } //ถ้าเชื่อมต่อสำเร็จก็จะหลุก loop while ไปค่ะ

  Serial.println(""); 
  Serial.println("Wi-Fi connected"); //แสดงว่าเชื่อมต่อ Wi-Fi ได้แล้ว
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP()); //แสดง IP ของบอร์ดที่ได้รับแจกจาก AP

  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");     //ดึงเวลาจาก Server
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
       Serial.print(".");
       delay(1000);
    }
  //****Sd_card****
  while (!Serial) {
  ; // รอจนกระทั่งเชื่อมต่อกับ Serial port แล้ว สำหรับ Arduino Leonardo เท่านั้น
  }
  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
  Serial.println("initialization failed!");
  return;
  }
  Serial.println("initialization done.");

}



void loop()
{
  
  Serial.print("Reading: ");
  Serial.print(scale.get_units()); //แสดงผลน้ำหนัก 2 ตำแหน่ง
  Serial.println(" kg");
  lcd.setCursor(0, 0); // กำหนดให้ เคอร์เซอร์ อยู่ตัวอักษรตำแหน่งที่0 แถวที่ 1 เตรียมพิมพ์ข้อความ
  lcd.print("WEIGHT(KG)"); //พิมพ์ข้อความ "LCD1602 I2c Test"
  lcd.setCursor(0,1);
  float w=scale.get_units();
  
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");    //ดีงเวลาปัจจุบันจาก Server อีกครั้ง
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  delay(500);
  
  

  if(w<=0.05){
   
      
        myservo.write(1450);
        w=scale.get_units();
        lcd.setCursor(0, 0); // กำหนดให้ เคอร์เซอร์ อยู่ตัวอักษรตำแหน่งที่0 แถวที่ 1 เตรียมพิมพ์ข้อความ
        lcd.print("WEIGHT(KG)"); //พิมพ์ข้อความ "LCD1602 I2c Test"
         lcd.setCursor(0,1);
          lcd.print(scale.get_units());
          Serial.print(scale.get_units()); //แสดงผลน้ำหนัก 2 ตำแหน่ง
        Serial.println(" kg");
     
     
    }
    
  
  if((w>0.05&&w<=1.0)){
    
  while(w<0.2&&w>0.05){
  
  //******SERVO MOTOR*****
  myservo.write(360); // สั่งให้ Servo หมุนไปองศาที่ 360
  delay(100);
  w=scale.get_units();
  Serial.print(scale.get_units()); //แสดงผลน้ำหนัก 2 ตำแหน่ง
  Serial.println(" kg");
  lcd.setCursor(0, 0); // กำหนดให้ เคอร์เซอร์ อยู่ตัวอักษรตำแหน่งที่0 แถวที่ 1 เตรียมพิมพ์ข้อความ
  lcd.print("WEIGHT(KG)"); //พิมพ์ข้อความ "LCD1602 I2c Test"
  lcd.setCursor(0,1);
  lcd.print(scale.get_units());
  Blynk.run();
  }
  myservo.write(1450);
  if(w>=0.2){
    myFile = SD.open("test.txt", FILE_WRITE); // เปิดไฟล์ที่ชื่อ test.txt เพื่อเขียนข้อมูล โหมด FILE_WRITE
    if (myFile) {
   
    Serial.print("Writing to test.txt...");
    myFile.print(p_tm->tm_hour);
    myFile.print(":");
    myFile.print(p_tm->tm_min);
    myFile.print("  ");
    myFile.print(p_tm->tm_mday); 
    myFile.print("/");
    myFile.print(p_tm->tm_mon+1); 
    myFile.print("/");
    myFile.print(p_tm->tm_year+1900);
    myFile.println("Feeding pet success from weight sensor"); // สั่งให้เขียนข้อมูล
    
    myFile.close(); // ปิดไฟล์
   
    }
     else {
      // ถ้าเปิดไฟลืไม่สำเร็จ ให้แสดง error
        Serial.println("error opening test.txt");
        }
 }
  }
  
   Blynk.run();
}
BLYNK_WRITE(V1){
  myservo.write(360);
  w=scale.get_units();
 
}
BLYNK_WRITE(V2){
  myservo.write(1450);
  w=scale.get_units();  
}
BLYNK_WRITE(V3){
  Serial.println(param.asInt());
  
  if(param.asInt() == 0)
  {
    myservo.write(1450);
    w=scale.get_units();  
  }
  if(param.asInt() == 1)
  {
    while(w<=0.2){
    myservo.write(360);
    w=scale.get_units();  
  } myservo.write(1450);
  }
}
BLYNK_WRITE(V4){
 Serial.println(param.asInt());
  
  if(param.asInt() == 0)
  {
    myservo.write(1450);
    w=scale.get_units();
    Serial.println(param.asInt());  
  }
  if(param.asInt() == 1)
  {
    while(w<=0.2){
    myservo.write(360);
    w=scale.get_units();
    Serial.println(param.asInt());  
  } myservo.write(1450);
  }
}
BLYNK_WRITE(V6){
  Serial.println(param.asInt());
  
  if(param.asInt() == 0)
  {
    myservo.write(1450);
    w=scale.get_units();  
  }
  if(param.asInt() == 1)
  {
    while(w<=0.2){
    myservo.write(360);
    w=scale.get_units();  
  } myservo.write(1450);
  }
}
BLYNK_WRITE(V7){
  Serial.println(param.asInt());
   float s=param.asInt();
  if(param.asInt()==1){
      myservo.write(1450);
    
    } 
  
  }
  BLYNK_WRITE(V8){
  Serial.println(param.asInt());
   
  
  }
  

  
