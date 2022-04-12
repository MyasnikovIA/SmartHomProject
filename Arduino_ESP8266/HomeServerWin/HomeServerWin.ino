//https://dl.espressif.com/dl/package_esp32_index.json
//http://arduino.esp8266.com/stable/package_esp8266com_index.json
#include <ArduinoJson.h> // https://arduinojson.org/v6/api/jsonobject/containskey/
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

/*

send:PowerSW_001
pass:********
method:push
status:1


send:robot
pass:********
status:1

on:1
off:1
pop


click:2
delay:100000


http://128.0.24.172:8200/signalchange.ru?from=test&pass=********&method=push&status=1
http://128.0.24.172:8200/signalchange.ru?from=test&pop=1
*/


WiFiClient clientSocket;
JsonObject *conf;
DynamicJsonDocument jsoonObj(2048);
DynamicJsonDocument clientPass(2048);

String DEVICE_NAME = "HomeServerWin"; // имя устройства
String SSIDName = "ELTEX-87A2";     // имя точки доступа
char* SSIDPass = "GP08004568";      // пароль точки доступа
String IP_SIGNAL_SERVER = "128.0.24.172";  // http://128.0.24.172:8200/signalchange.ru
int PORT_SIGNAL_SERVER = 8200;
String PassDevice = "********";      // пароль устройства

//int _ON_ = HIGH;    //  Включение реле
//int _OFF_ = LOW;    //  Выключение реле
int _ON_ = 0;    //  Включение реле
int _OFF_ = 255;    //  Выключение реле


String methSend = "send"; 
String fromDevice="";
String PassDeviceKey="";
int analogPin = -1;
int servoPin = -1;
Servo myservo; 

int pinStatus[] ={0,0,0,0,0,0,0,0,0,0,0};
int pin[] ={16,5,4,0,2,14,12,13,15,9,10};
int pinValueClick[] ={0,0,0,0,0,0,0,0,0,0,0};
const byte Rele0 = 0; // 
const byte Rele1 = 1; // 
const byte Rele2 = 2; // 
const byte Rele3 = 3; // 
const byte Rele4 = 4; // 
const byte Rele5 = 5; // 
const byte Rele6 = 6; // 
const byte Rele7 = 7; // 
const byte Rele8 = 8; // 




void sendClient(String msg) {
  clientSocket.println("{\""+methSend+"\":\""+fromDevice+"\",\"msg\":\""+msg+"\"}");
  clientSocket.println("");
  clientSocket.println("");
}

void setup(){
   //Serial.begin(115200);
   // pinMode(pin[strob1], INPUT); // конфигурируем порт как вход (кнопка)
   pinMode(pin[Rele0], OUTPUT); 
   pinMode(pin[Rele1], OUTPUT); 
   pinMode(pin[Rele2], OUTPUT); 
   pinMode(pin[Rele3], OUTPUT); 
   pinMode(pin[Rele4], OUTPUT); 
   pinMode(pin[Rele5], OUTPUT); 
   pinMode(pin[Rele6], OUTPUT); 
   pinMode(pin[Rele7], OUTPUT); 
   pinMode(pin[Rele8], OUTPUT); 

   analogWrite(pin[Rele0], _OFF_);  
   analogWrite(pin[Rele1], _OFF_);  
   analogWrite(pin[Rele2], _OFF_);  
   analogWrite(pin[Rele3], _OFF_);  
   analogWrite(pin[Rele4], _OFF_);  
   analogWrite(pin[Rele5], _OFF_);  
   analogWrite(pin[Rele6], _OFF_);  
   analogWrite(pin[Rele7], _OFF_);  
   analogWrite(pin[Rele8], _OFF_);  
   deserializeJson(clientPass, "{}");
}


void connectApp(){
    int Tnetwork=0;
    Tnetwork = WiFi.scanNetworks();
    int selApp = -1;
    while(selApp == -1){
       for (int i = 0; i < Tnetwork; ++i) {
          if (WiFi.SSID(i) == SSIDName ) {
              selApp = i;
              break;
          }
       }
    }
    WiFi.begin(WiFi.SSID(selApp), SSIDPass);
    //Serial.println("");    
    while (WiFi.status() != WL_CONNECTED) {
       delay(500);
      // Serial.println(".");    
    }
    //Serial.println(WiFi.localIP());    
}


///*******************************************************************************************************************
///*******************************************************************************************************************
///*******************************************************************************************************************

void mainLoop() {
    for (int pinNum = 0; pinNum < 10; ++pinNum) {
       if (pinValueClick[pinNum]>0) {
           digitalWrite(pin[pinNum], _ON_);          
           pinValueClick[pinNum] = pinValueClick[pinNum] - 1;
           if(pinValueClick[pinNum] == 0) {
              digitalWrite(pin[pinNum], _OFF_);          
           }
       }
    }
};

///*******************************************************************************************************************
///*******************************************************************************************************************
///*******************************************************************************************************************


int indStep=0;
bool onPing(){
    indStep++;
    if (indStep > 500000) {
       indStep=0;
       clientSocket.println("ping");
       clientSocket.println();
       clientSocket.println();
       return true;
   }
   return false;
}


String readWiFiData(){
    String line = "";
    while(clientSocket.available()){
        // String line = clientSocket.readStringUntil('\n');
        // Serial.print(line);
        char nextChar = clientSocket.read();
        line += nextChar;
    }
    return line;
}

bool testPass(){
   if (PassDevice.length() > 0) {
      if (!clientPass.containsKey(fromDevice)){
         sendClient("Error pass: client not found");
         return false;;
      }
      if (String(clientPass[fromDevice]).length() == 0){
         sendClient("Error pass: client not pass info");
         return false;
      }
      if (!PassDevice.equals(String(clientPass[fromDevice]))) {
         sendClient("Error pass");
         return false;;
      }
    }  
    return true;
}


String tmpStr="";
void loop() {
  connectApp();
  if (clientSocket.connect(String(IP_SIGNAL_SERVER),int(PORT_SIGNAL_SERVER))){
      clientSocket.println(DEVICE_NAME); // Регистрируем имя устройства на сигнальном сервере
      clientSocket.println();
      clientSocket.println();
      while (clientSocket.connected()) {
           if (onPing()) continue;
           mainLoop();
           if (!clientSocket.connected()){ return; }
           String line = readWiFiData();
           if (line.substring(0,4).equals("ping")) continue;
           if ((line.length() > 2) && (tmpStr != line)) {        
                tmpStr = line;
                //Serial.println("|"+line+"|");    
                DeserializationError error = deserializeJson(jsoonObj, line);
                if (error){  continue; }
                // Обработка входящих команд из JSON объекта
                if (jsoonObj.containsKey("from")) { 
                    fromDevice = String(jsoonObj["from"]);
                }
                if ((fromDevice.length() > 0) && (jsoonObj.containsKey("pass"))) { 
                    PassDeviceKey = String(jsoonObj["pass"]);
                    PassDeviceKey.replace("\r", "");
                    PassDeviceKey.replace("\n", "");
                    clientPass[fromDevice]=PassDeviceKey;
                }
                // -------- Проверка пароля --------------------------
                if (!testPass()){
                   continue;
                }
                if (jsoonObj.containsKey("ip")) {
                   sendClient(WiFi.localIP().toString());
                   continue;
                } 
                if (jsoonObj.containsKey("method")) {
                   String methSendTmp = String(jsoonObj["method"]);
                   if ((methSendTmp == "send")||(methSendTmp == "push")) {
                      methSend = methSendTmp;
                   }
                } 
                if (jsoonObj.containsKey("status")) {
                    String msg = "{\""+methSend+"\":\""+fromDevice+"\"";
                    for (int i = 0; i < 10; ++i) {
                       msg+=",\"Pin";
                       msg+=i;
                       msg+="\":\"";
                       msg+=pinStatus[i];
                       msg+="\"";
                    }
                    msg+="}";
                    clientSocket.println(msg);
                    clientSocket.println("");
                    clientSocket.println("");                    
                    continue;
                }
                // -------- Включение кнопки  на время ----------------
                if (jsoonObj.containsKey("click")) {
                    int pinNum =String(jsoonObj["click"]).toInt();
                    if (jsoonObj.containsKey("delay")){
                       int delayValue =String(jsoonObj["delay"]).toInt();
                       pinValueClick[pinNum] = delayValue;
                       String msg = "Click ";
                       msg = msg + pinNum;
                       msg = msg + " delay ";
                       msg = msg + delayValue;
                       sendClient(msg);
                   }
                }
                // ----------------------------------------------------
                if (jsoonObj.containsKey("on")) {
                    int pinNum =String(jsoonObj["on"]).toInt();
                    analogWrite(pin[pinNum], _ON_);
                    pinStatus[pinNum]=255;
                    String msg = "Pin ";
                    msg = msg + pinNum;
                    msg = msg + " on";
                    sendClient(msg);
                }
                // ------------------------------------------
                if (jsoonObj.containsKey("off")) {
                    int pinNum =String(jsoonObj["off"]).toInt();
                    analogWrite(pin[pinNum], _OFF_);
                    pinStatus[pinNum] = 0;
                    String msg = "Pin ";
                    msg = msg + pinNum;
                    msg = msg+" off";
                    sendClient(msg);
                }
                // ------- Включение аналогового -----------------------
                if (jsoonObj.containsKey("analog")) {
                    analogPin = String(jsoonObj["analog"]).toInt();
                    servoPin = -1;
                }
                if (jsoonObj.containsKey("value") && (analogPin !=-1)){
                   String msg = "Pin ";
                   msg = msg + analogPin;
                   int val =String(jsoonObj["value"]).toInt();
                   msg = msg + " value=";
                   msg = msg + val;
                   if (val == 0) {
                      digitalWrite(pin[analogPin], _OFF_);  
                      pinStatus[analogPin] = 0;
                   } else {
                      analogWrite(pin[analogPin], val);
                      pinStatus[analogPin] = val;
                   }
                   sendClient(msg);
                }
                
                // ---- Управление сервоприводом(надо проверять) -----------------                
                if (jsoonObj.containsKey("servo")) {
                    servoPin = String(jsoonObj["servo"]).toInt();
                    analogPin = -1;
                    myservo.attach(pin[servoPin]); 
                }
                if (jsoonObj.containsKey("value") && (servoPin !=-1)){
                   // val = analogRead(potpin); 
                   // val = map(val, 0, 1023, 0, 180);
                   // myservo.write(val); 
                   String msg = "Servo pin ";
                   msg = msg + servoPin;
                   int val =String(jsoonObj["value"]).toInt();
                   msg = msg + " value=";
                   msg = msg + val;
                   myservo.write(val);    
                   sendClient(msg);  
                }
                // ------------------------------------------  
            }
      }
  }

}  
