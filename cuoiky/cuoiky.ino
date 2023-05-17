#include  <WiFi.h>
#include  <WiFiClient.h>
#include  <FirebaseESP32.h>

#include <DHT.h>
#define dhttype DHT11
#define dhtpin 4
String path ="/";
const int ENA = 27;
const int IN1 = 12;
const int IN2 = 14;
#define IN_den 5
#define IN_quat 2
int TTden, TTquat;
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

  Serial.begin(9600);
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
  pinMode(IN_quat, OUTPUT);
}

void loop() {
//Gán biến đo nhiệt độ, độ ẩm
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Nhiet do: ");
  Serial.print(t);
  Serial.print("*C  ");
  Serial.print("Do am: ");
  Serial.print(h);
  Serial.println("%  ");

  Firebase.setFloat( fbcontrol,path + "/NHIỆT ĐỘ", t);
  Firebase.setFloat( fbcontrol,path + "/ĐỘ ẨM", h);
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
//Trạng thái Quạt
  if(Firebase.getInt( fbcontrol,"/fan")== 1 )
  {
    int TTquat = fbcontrol.intData();
    Serial.print("Trang thai Quạt: ");
    Serial.print(TTquat);
    Serial.print(" ");
     if(TTquat == 1)
     {
       digitalWrite(IN1,HIGH);
       digitalWrite(IN2,LOW);
      ledcWrite(1, 50);
       Serial.println("QUẠT: ON");
     }
     else if(TTquat == 0)
     {
       digitalWrite(IN1,HIGH);
       digitalWrite(IN2,LOW);
       ledcWrite(1, 0);
       Serial.println("QUẠT: OFF");
     }
  }
//Đọc tốc độ quạt
  if(Firebase.getInt( fbcontrol,"/speedfan")== true)
  {
    int TDquat = fbcontrol.intData();
    Serial.print("Tốc độ quạt: ");
    Serial.println(TDquat);
   /*if(TTquat == 1)
    {
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      ledcWrite(1, TDquat);
    }
    else if(TTquat == 0)
    {
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
      ledcWrite(1, 0);
    }*/
    
  }
//Trạng thái Đèn
  if(Firebase.getInt( fbcontrol,"/lamp")== true)
  {
    int TTden = fbcontrol.intData();
    Serial.print("Trang thai den: ");
    Serial.print(TTden);
    Serial.print(" ");
    if(TTden == 1)
    {
      digitalWrite(IN_den,HIGH);
      Serial.println("ĐÈN: ON");
    }
    else if(TTden == 0)
    {
       
       digitalWrite(IN_den,0);
        Serial.println("ĐÈN: OFF");
    }
  }
  delay(100);
}