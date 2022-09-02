#include <ESP8266WiFi.h>

// Перезапуск розеток(системы), если интернет недоступен
// Для определения наличия интернета пингуется ресурс на gitHub (https://raw.githack.com/MyasnikovIA/WebSputnik/main/index.html)

const char* ssid     = "ELTEX-87A2";
const char* password = "**********";

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
int _ON_ = 0;    //  Включение реле
int _OFF_ = 255;    //  Выключение реле


void setup() {
  Serial.begin(115200);
  delay(10);

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
   
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

bool ExistConnectInternet(){
  char* host = "raw.githack.com"; 
  WiFiClientSecure clientSocketHttps;
  const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
  clientSocketHttps.setInsecure(); 
  if (!clientSocketHttps.connect(host, httpPort)) {
     return false;
  }
  String url = "/MyasnikovIA/WebSputnik/main/index.html";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  clientSocketHttps.println(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  String line = "";
  while (clientSocketHttps.available()) {
      // String line = clientSocket.readStringUntil('\n');
      // Serial.print(line);
      char nextChar = clientSocketHttps.read();
      line += nextChar;
  }
  clientSocketHttps.stop();
  if (line.indexOf("HTTP/1.1 200 OK") != 0) {
     return false;   
  }else{
     return true;
  }
}

int ind = 0;
void loop() {
  delay(60000*10);// проверяем наличие интеренета 1 раз в 10 минут
  ind = ind+1;
  Serial.print(ind);
  if (ExistConnectInternet() == false){
      Serial.println(" Error connect");
      //analogWrite(pin[0], _ON_);
      analogWrite(pin[1], _ON_);
      delay(60000);
      //analogWrite(pin[0], _OFF_);
      analogWrite(pin[1], _OFF_);
      ind = 0;
  } else {
      Serial.println(" Connect OK ");
      //analogWrite(pin[0], _OFF_);
      analogWrite(pin[1], _OFF_);
  }
}
