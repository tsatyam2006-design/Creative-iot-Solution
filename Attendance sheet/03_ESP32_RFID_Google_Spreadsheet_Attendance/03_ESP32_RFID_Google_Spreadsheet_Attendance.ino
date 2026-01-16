//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 03_ESP32_RFID_Google_Spreadsheet_Attendance
//----------------------------------------Including the libraries.
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "WiFi.h"
#include <HTTPClient.h>
//----------------------------------------

// Defines SS/SDA PIN and Reset PIN for RFID-RC522.
#define SS_PIN  5  
#define RST_PIN 4

// Defines the button PIN.
#define BTN_PIN 15

// LED pins
#define GREEN_LED 25
#define RED_LED   26

//----------------------------------------SSID and PASSWORD of your WiFi network.
const char* ssid = "Slog";  //--> Your wifi name
const char* password = "slog@123"; //--> Your wifi password
//----------------------------------------

// Google script Web_App_URL.
String Web_App_URL = "https://script.google.com/macros/s/AKfycbyQnKt3F8gHkNCY8VqfvW4aTy4RayvgFv4jGNa5-Y_nYe-9G-3vpra46zKbkyuuK47Q6g/exec";

String reg_Info = "";

String atc_Info = "";
String atc_Name = "";
String atc_Date = "";
String atc_Time_In = "";
String atc_Time_Out = "";

// Variables for the number of columns and rows on the LCD.
int lcdColumns = 20;
int lcdRows = 4;

// Variable to read data from RFID-RC522.
int readsuccess;
char str[32] = "";
String UID_Result = "--------";

String modes = "atc";

// Create LiquidCrystal_I2C object as "lcd"
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// Create MFRC522 object as "mfrc522"
MFRC522 mfrc522(SS_PIN, RST_PIN);  

//________________________________________________________________________________http_Req()
void http_Req(String str_modes, String str_uid) {
  if (WiFi.status() == WL_CONNECTED) {
    String http_req_url = "";

    //----------------------------------------Create links to make HTTP requests to Google Sheets.
    if (str_modes == "atc") {
      http_req_url  = Web_App_URL + "?sts=atc";
      http_req_url += "&uid=" + str_uid;
    }
    if (str_modes == "reg") {
      http_req_url = Web_App_URL + "?sts=reg";
      http_req_url += "&uid=" + str_uid;
    }
    //----------------------------------------

    Serial.println();
    Serial.println("-------------");
    Serial.println("Sending request to Google Sheets...");

    Serial.print("URL : ");
    Serial.println(http_req_url);
     
    HTTPClient http;
    http.begin(http_req_url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload);  
    }
    
    Serial.println("-------------");
    http.end();

    String sts_Res = getValue(payload, ',', 0);

    if (sts_Res == "OK") {
      //..................
      if (str_modes == "atc") {
        atc_Info = getValue(payload, ',', 1);
        
        if (atc_Info == "TI_Successful") {
          atc_Name = getValue(payload, ',', 2);
          atc_Date = getValue(payload, ',', 3);
          atc_Time_In = getValue(payload, ',', 4);

          // Green LED ON for 2 seconds
          digitalWrite(GREEN_LED, HIGH);
          delay(2000);
          digitalWrite(GREEN_LED, LOW);

          //::::::::::::::::::LCD
          int name_Lenght = atc_Name.length();
          int pos = 0;
          if (name_Lenght > 0 && name_Lenght <= lcdColumns) {
            pos = map(name_Lenght, 1, lcdColumns, 0, (lcdColumns / 2) - 1);
            pos = ((lcdColumns / 2) - 1) - pos;
          } else if (name_Lenght > lcdColumns) {
            atc_Name = atc_Name.substring(0, lcdColumns);
          }

          lcd.clear();
          delay(500);
          lcd.setCursor(pos,0);
          lcd.print(atc_Name);
          lcd.setCursor(0,1);
          lcd.print("Date    : ");
          lcd.print(atc_Date);
          lcd.setCursor(0,2);
          lcd.print("Time IN :   ");
          lcd.print(atc_Time_In);
          lcd.setCursor(0,3);
          lcd.print("Time Out:   ");
          delay(5000);
          lcd.clear();
          delay(500);
        }

        if (atc_Info == "TO_Successful") {
          atc_Name = getValue(payload, ',', 2);
          atc_Date = getValue(payload, ',', 3);
          atc_Time_In = getValue(payload, ',', 4);
          atc_Time_Out = getValue(payload, ',', 5);

          // Red LED ON for 2 seconds
          digitalWrite(RED_LED, HIGH);
          delay(2000);
          digitalWrite(RED_LED, LOW);

          //::::::::::::::::::LCD
          int name_Lenght = atc_Name.length();
          int pos = 0;
          if (name_Lenght > 0 && name_Lenght <= lcdColumns) {
            pos = map(name_Lenght, 1, lcdColumns, 0, (lcdColumns / 2) - 1);
            pos = ((lcdColumns / 2) - 1) - pos;
          } else if (name_Lenght > lcdColumns) {
            atc_Name = atc_Name.substring(0, lcdColumns);
          }

          lcd.clear();
          delay(500);
          lcd.setCursor(pos,0);
          lcd.print(atc_Name);
          lcd.setCursor(0,1);
          lcd.print("Date    : ");
          lcd.print(atc_Date);
          lcd.setCursor(0,2);
          lcd.print("Time IN :   ");
          lcd.print(atc_Time_In);
          lcd.setCursor(0,3);
          lcd.print("Time Out:   ");
          lcd.print(atc_Time_Out);
          delay(5000);
          lcd.clear();
          delay(500);
        }

        if (atc_Info == "atcInf01") {
          lcd.clear();
          delay(500);
          lcd.setCursor(1,0);
          lcd.print("You have completed");
          lcd.setCursor(2,1);
          lcd.print("your  attendance");
          lcd.setCursor(2,2);
          lcd.print("record for today");
          delay(5000);
          lcd.clear();
          delay(500);
        }

        if (atc_Info == "atcErr01") {
          lcd.clear();
          delay(500);
          lcd.setCursor(6,0);
          lcd.print("Error !");
          lcd.setCursor(4,1);
          lcd.print("Your card or");
          lcd.setCursor(4,2);
          lcd.print("key chain is");
          lcd.setCursor(3,3);
          lcd.print("not registered");
          delay(5000);
          lcd.clear();
          delay(500);
        }

        atc_Info = "";
        atc_Name = "";
        atc_Date = "";
        atc_Time_In = "";
        atc_Time_Out = "";
      }
      //..................

      if (str_modes == "reg") {
        reg_Info = getValue(payload, ',', 1);
        
        if (reg_Info == "R_Successful") {
          lcd.clear();
          delay(500);
          lcd.setCursor(2,0);
          lcd.print("The UID of your");
          lcd.setCursor(0,1);
          lcd.print("card or keychain has");
          lcd.setCursor(1,2);
          lcd.print("been successfully");
          lcd.setCursor(6,3);
          lcd.print("uploaded");
          delay(5000);
          lcd.clear();
          delay(500);
        }

        if (reg_Info == "regErr01") {
          lcd.clear();
          delay(500);
          lcd.setCursor(6,0);
          lcd.print("Error !");
          lcd.setCursor(0,1);
          lcd.print("The UID of your card");
          lcd.setCursor(0,2);
          lcd.print("or keychain has been");
          lcd.setCursor(5,3);
          lcd.print("registered");
          delay(5000);
          lcd.clear();
          delay(500);
        }

        reg_Info = "";
      }
    }
  } else {
    lcd.clear();
    delay(500);
    lcd.setCursor(6,0);
    lcd.print("Error !");
    lcd.setCursor(1,1);
    lcd.print("WiFi disconnected");
    delay(3000);
    lcd.clear();
    delay(500);
  }
}
//________________________________________________________________________________

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int getUID() {  
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
  
  byteArray_to_string(mfrc522.uid.uidByte, mfrc522.uid.size, str);
  UID_Result = str;
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  return 1;
}

void byteArray_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len*2] = '\0';
}

void setup(){
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(500);

  SPI.begin();      
  mfrc522.PCD_Init(); 
  delay(500);

  lcd.setCursor(5,0);
  lcd.print("ESP32 RFID");
  lcd.setCursor(3,1);
  lcd.print("Google  Sheets");
  lcd.setCursor(1,2);
  lcd.print("Attendance  System");
  lcd.setCursor(4,3);
  lcd.print("by  UTEH STR");
  delay(3000);
  lcd.clear();

  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");

  Serial.println();
  Serial.println("------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int connecting_process_timed_out = 20; 
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    lcd.setCursor(0,0);
    lcd.print("Connecting to SSID");
    delay(250);

    lcd.clear();
    delay(250);
    
    if (connecting_process_timed_out > 0) connecting_process_timed_out--;
    if (connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("------------");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi connected");
  delay(2000);
  lcd.clear();
  delay(500);
}

void loop(){
  int BTN_State = digitalRead(BTN_PIN);

  if (BTN_State == LOW) {
    lcd.clear();
    
    if (modes == "atc") {
      modes = "reg";
    } else if (modes == "reg") {
      modes = "atc";
    }
    
    delay(500);
  }

  readsuccess = getUID();

  if (modes == "atc") {
    lcd.setCursor(5,0);
    lcd.print("ATTENDANCE");
    lcd.setCursor(0,2);
    lcd.print("Please tap your card");
    lcd.setCursor(4,3);
    lcd.print("or key chain");

    if (readsuccess){
      lcd.clear();
      delay(500);
      lcd.setCursor(4,0);
      lcd.print("Getting  UID");
      lcd.setCursor(4,1);
      lcd.print("Successfully");
      lcd.setCursor(3,3);
      lcd.print("Please wait...");
      delay(1000);

      http_Req(modes, UID_Result);
    }
  }

  if (modes == "reg") {
    lcd.setCursor(4,0);
    lcd.print("REGISTRATION");
    lcd.setCursor(0,2);
    lcd.print("Please tap your card");
    lcd.setCursor(4,3);
    lcd.print("or key chain");

    if (readsuccess){
      lcd.clear();
      delay(500);
      lcd.setCursor(0,0);
      lcd.print("Getting  UID");
      lcd.setCursor(0,1);
      lcd.print("Successfully");
      lcd.setCursor(0,2);
      lcd.print("UID : ");
      lcd.print(UID_Result);
      lcd.setCursor(0,3);
      lcd.print("Please wait...");
      delay(1000);

      http_Req(modes, UID_Result);
    }
  }

  delay(10);
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
