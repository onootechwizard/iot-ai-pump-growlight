#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <DHT.h>

//inisialisasi pin dan tipe data
#define SENSOR       A0  // UNTUK SENSOR PIN A0
#define RELAY1       D6  // UNTUK LAMPU PIN D6
#define RELAY2       D7  // UNTUK POMPA PIN D7
#define DurasiPompa 3000 // Durasi penyalaan pompa satuan ms
#define DHTPIN       D5  // UNTUK DHT PIN D5
#define DHTTYPE DHT11    // DHT 11

DHT dht(DHTPIN, DHTTYPE);
int val = 0;
int ON = LOW, OFF = HIGH;// untuk relay
bool Mode = 1; // 1 mode otomatis 0 mode manual


// Initialize Wifi connection to the router
char ssid[] = "Mystery.";     // diisi nama wifi
char password[] ="s4tud4n8"; // diisi password wifi

// Initialize Telegram BOT
#define BOTtoken "5407124593:AAHDcAY2kA9lk__Vo7k3RQMdt-QJ2GbcCSQ" // diisi Token Bot (Dapat dari Telegram Botfather)
#define chatid "5561934635" // diisi id (Dapat dari get id bot)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    //untuk menu silakan ganti kata didalam " "
    if (text == "/Siram" && Mode == 0) {
     digitalWrite(RELAY2, ON);
     delay (DurasiPompa);
     digitalWrite(RELAY2, OFF);
     bot.sendChatAction(chatid, "typing");
     bot.sendMessage(chatid, "Tanaman disiram secara Manual");
    }
    if (text == "/Manual") {
      Mode = 0;
      bot.sendChatAction(chatid, "typing");
      bot.sendMessage(chatid, "Mode penyiraman manual");
    }
    if (text == "/Otomatis") {
      Mode = 1;    
      bot.sendChatAction(chatid, "typing");
      bot.sendMessage(chatid, "Mode penyiraman Otomatis");
    }
    if (text == "/LedOn") {
      digitalWrite (RELAY1 , ON); 
      bot.sendChatAction(chatid, "typing"); 
      bot.sendMessage(chatid, "lampu dinyalakan");
    }
    if (text == "/LedOff") {
      digitalWrite (RELAY1 , OFF);  
      bot.sendChatAction(chatid, "typing");
      bot.sendMessage(chatid, "lampu dimatikan");
    }
    if (text == "/Cek") { 
      val = map(analogRead(SENSOR), 1024, 495, 0, 100);
      if(val<0)val=0;
      else if (val>100)val=100;
      float  t = dht.readTemperature();
      String welcome = "Data sensor untuk " + from_name + ".\nSoil moisture: ";
      welcome += val;
      welcome += "%.\nSuhu ";
      welcome += t;
      welcome += "C.\nRelay 1 (Lampu) dalam kondisi ";
      boolean a=digitalRead(RELAY1);
      if(a)welcome += "mati.";
      else welcome += "menyala.";
      welcome += "\nRelay 2 (Pompa) dalam kondisi ";
      boolean b=digitalRead(RELAY2);
      if(b)welcome += "mati.";
      else welcome += "menyala.";
      bot.sendChatAction(chatid, "typing");
      bot.sendMessage(chatid, welcome, "Markdown");
    }
    if (text == "/start"){
      String welcome = "Selamat datang di RANCANG BANGUN PERAWATAN TANAMAN DAN PEMANTAUAN DENGAN APLIKASI BOT TELEGRAM JARAK JAUH BERBASIS NODEMCU ESP8266 DAN OPENCV.\n";
      welcome += "/Manual   : Tekan untuk merubah mode otomatis menjadi manual\n";
      welcome += "/Siram    : Tekan untuk menyiram tanaman mode manual\n";
      welcome += "/Otomatis : Tekan untuk merubah mode manual menjadi otomatis\n";
      welcome += "/LedOn    : Tekan untuk menyalakan lampu\n";
      welcome += "/LedOff   : Tekan untuk mematikan lampu\n";
      welcome += "/Cek      : Tekan untuk mengecek data sensor\n";
     
      bot.sendChatAction(chatid, "typing");
      bot.sendMessage(chatid, welcome, "Markdown");
    }
  }
}

void setup() {

  Serial.begin(115200);
  dht.begin();
  // This is the simplest way of getting this working
  // if you are passing sensitive information, or controlling
  // something important, please either use certStore or at
  // least client.setFingerPrint
  client.setInsecure();

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);}
    
  pinMode (RELAY1, OUTPUT);
  pinMode (RELAY2, OUTPUT);
  digitalWrite(RELAY1, OFF);
  digitalWrite(RELAY2, OFF);
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());}

void loop() {
int sensorValue = analogRead(A0);
val = map(analogRead(SENSOR), 1024, 495, 0, 100);
if(val<0)val=0; 
else if (val>100)val=100;
Serial.print("Sensor Soil Moisture: ");
Serial.println(val);
Serial.print("Nilai ADC soil moisture: ");
Serial.println(sensorValue);
float  t = dht.readTemperature();
Serial.print("Sensor DHT11: ");
Serial.println(t);

  if (val < 80 && Mode == 1 ){
    digitalWrite(RELAY2, ON);
    bot.sendChatAction(chatid, "typing");
    bot.sendMessage(chatid, "Tanaman disiram secara otomatis");}
    
  if (t > 28 && Mode == 1 ){ 
    digitalWrite(RELAY2, ON);
    bot.sendChatAction(chatid, "typing");
    bot.sendMessage(chatid, "Tanaman disiram secara otomatis");} 
    
  else{
    digitalWrite(RELAY2, OFF);}  
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);}
    lastTimeBotRan = millis();}}

    
