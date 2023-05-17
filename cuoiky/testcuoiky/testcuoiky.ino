#include  <WiFi.h>
#include  <WiFiClient.h>
#include  <FirebaseESP32.h>
#include <Arduino.h>
#include <DHT.h>

#define dhttype DHT11
#define dhtpin 32


float tempTem,humTem;
String path ="/";

const int IN1 = 12;
const int IN2 = 14;
#define ENA 17
#define IN_den 5


// cài đặt PWM
const int freq = 5000;  // tần số xung
const int ledChannel1 = 0; 
const int ledChannel2 = 1;// kênh PWM
const int resolution = 8; // độ phân giải 8bit
  
int ttDc, ttDen;
int tdDc,tdDen;
float hum,temp;
DHT dht(dhtpin, dhttype);


#define WIFI_SSID "iphone" //Tên Wifi kết nối
#define WIFI_PASSWORD "12121212" //Mật khẩu của Wifi
#define FIREBASE_HOST "nhom2android-default-rtdb.firebaseio.com/" //Địa chỉ Host của Project Firebase
#define FIREBASE_AUTH "L3gUjtu5ibc9CTusTad9t12W1WYSJowgAesSi9FP" // Mạt khẩu mã hoá của Project Firebase

String lampPath = "/lamp";
String fanPath = "/fan";
String humPath = "/ĐỘ ẨM";
String tempPath = "/NHIỆT ĐỘ";


FirebaseData fbcontrol;





void setup() {

  Serial.begin(115200);
  delay(1000);
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Dang ket noi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  dht.begin();
  Serial.println ("");
  Serial.println ("Da ket noi WiFi!");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // Thiết lập chân kết nối
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN_den, OUTPUT);

  
  // setup kênh pwm
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);
  
  // xuất pwm ra chân 16
  ledcAttachPin(IN_den, ledChannel1);
  ledcAttachPin(ENA, ledChannel2);

  testDc();
  testDen();
}

void loop() {
// Đọc và gửi nhiệt độ, độ ẩm
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  sendTempAndHum();
  
  


  readItemDatabase();
  testDen();
  testDc();
  delay(100);
}

void sendTempAndHum () {
    // Check dht 11
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    
    
    if(temp != tempTem) {
        Firebase.setFloat( fbcontrol,tempPath, temp);
         tempTem = temp;
        Serial.print("Nhiet do: ");
        Serial.print(temp);
        Serial.print("*C  ");
  }
    if(hum != humTem) {
        Firebase.setFloat( fbcontrol,humPath , hum);
        humTem = hum;
        Serial.print("Do am: ");
        Serial.print(hum);
        Serial.println("%  ");
  }
  }
}

void readItemDatabase() {
    if(Firebase.getInt( fbcontrol,"/fan")) {
        ttDc  = fbcontrol.intData();
   };
    if(Firebase.getInt( fbcontrol,"/speedfan")){
        tdDc  = fbcontrol.intData();
   };
   if(Firebase.getInt( fbcontrol,"/lamp")){
        ttDen = fbcontrol.intData();
   };
    if(Firebase.getInt( fbcontrol,"/lampbrightness")){
        tdDen = fbcontrol.intData();
   };
}
void testDen()
{
    Serial.print("Trang thai den: ");
    Serial.print(ttDen);
    Serial.print(" ");
    if(ttDen == 1)
    {
      ledcWrite(ledChannel1, tdDen*2.5);
      Serial.println("LAMP: ON");
      Serial.print("LightBrighttness: ");
      Serial.println(tdDen);
    }
    else if(ttDen == 0)
    {
       ledcWrite(ledChannel1, 0);
        Serial.println("LAMP: OFF");
    }
}

void testDc (){
    Serial.print("Trang thai quat: ");
    Serial.print(ttDc);
    Serial.print(" ");
    
   if(ttDc == 1)
    { 
      Serial.println("Fan: ON");
      ledcWrite(ledChannel2, tdDc*2.5);
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      Serial.print("Speed Fan: ");
      Serial.println(tdDc);
    }
    else if(ttDc == 0)
    {
      Serial.println("Fan: OFF");
      ledcWrite(ledChannel2, 0);   
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,LOW);      
    //   ledcWrite(1, 0);
    }
    
  }
