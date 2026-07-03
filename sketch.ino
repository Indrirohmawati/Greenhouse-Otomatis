#define BLYNK_TEMPLATE_ID "TMPL6M2E7EkP1"
#define BLYNK_TEMPLATE_NAME "Greenhouse Otomatis dengan Kontrol Kipas dan Lampu"
#define BLYNK_AUTH_TOKEN "gjxTQuyXkMuF4MI9q5psaq0g8K0I2jGO"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define DHTPIN 15
#define DHTTYPE DHT22

#define SERVO_PIN 18

#define LED_MERAH   2
#define LED_HIJAU   16
#define LED_PUTIH    19

#define LDR_PIN 34

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
Servo servo;

BlynkTimer timer;

void kirimData(){

  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  int cahaya = analogRead(LDR_PIN);

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Sensor DHT Error");
    return;
  }

  // ===== LCD =====
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(suhu,1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.setCursor(10,0);
  lcd.print("H:");
  lcd.print((int)kelembapan);
  lcd.print("% ");

  lcd.setCursor(0,1);
  lcd.print("Cahaya:");
  lcd.print(cahaya);
  lcd.print("    ");

  // ===== Kontrol Kipas =====
  if(suhu >= 30){

    servo.write(90);

    digitalWrite(LED_MERAH,HIGH);
    digitalWrite(LED_HIJAU,LOW);

    Blynk.virtualWrite(V3,1);

  }else{

    servo.write(0);

    digitalWrite(LED_MERAH,LOW);
    digitalWrite(LED_HIJAU,HIGH);

    Blynk.virtualWrite(V3,0);

  }

 // ===== Kontrol Lampu =====
if (cahaya < 2000) {

    digitalWrite(LED_PUTIH, HIGH);   // Lampu ON
    Blynk.virtualWrite(V4, 1);

} else {

    digitalWrite(LED_PUTIH, LOW);    // Lampu OFF
    Blynk.virtualWrite(V4, 0);

}

  // ===== Serial Monitor =====
  Serial.println("=======================");
  Serial.print("Suhu : ");
  Serial.print(suhu);
  Serial.println(" C");

  Serial.print("Kelembapan : ");
  Serial.print(kelembapan);
  Serial.println(" %");

  Serial.print("Cahaya : ");
  Serial.println(cahaya);

  // ===== Kirim ke Blynk =====
  Blynk.virtualWrite(V0,suhu);
  Blynk.virtualWrite(V1,kelembapan);
  Blynk.virtualWrite(V2,cahaya);

}

void setup(){

  Serial.begin(115200);

  pinMode(LED_MERAH, OUTPUT);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_PUTIH, OUTPUT);

  pinMode(LDR_PIN,INPUT);

  dht.begin();

  servo.attach(SERVO_PIN);
  servo.write(0);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("GREENHOUSE");
  lcd.setCursor(0,1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();

  WiFi.begin(ssid,pass);

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  timer.setInterval(1000L,kirimData);

}

void loop(){

  Blynk.run();
  timer.run();

}