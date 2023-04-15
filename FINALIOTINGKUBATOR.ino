//library
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "DHT.h"
//inisialisasi pin
#define DHTPIN 23
#define RELAY 19
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
bool lampuOn = false; 

//set kolom lcd
int lcdColumns = 16;
int lcdRows = 2;

//lcd
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//wifi yng di gunakan
const char* ssid     = "--";
const char* password = "--";
const char* host = "192.168.218.102"; // your server




void setup()
{
    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();
    pinMode(RELAY,OUTPUT);
    Serial.begin(115200);
    Serial.println("DHT11 Output!");
    dht.begin();

    // memulai wifi

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    //menampilkan wifi berhasil koneksi dan ip di serial monitor
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
void loop()
{
  //membaca data dari dht11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();  
 
  if(isnan(temperature) || isnan(humidity)){
    Serial.println("Failed to read DHT11");
      }
  else
      {
    //menampilkn data temperature dan humidity di serial monitor
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
    delay(2000);
       }
       lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print((char)223); 
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");

  //delay(2000); // wait for 2 seconds before updating the LCD
  

  
  //pembacaan suhu kapan relay aktif dan mati
    if (temperature >= 38&& !lampuOn) {
    digitalWrite(RELAY, HIGH);
    Serial.println("Menghidupkan lampu");
    lampuOn = true;
    } 
    else if (temperature< 39 && lampuOn) {
    digitalWrite(RELAY, LOW);
    Serial.println("Mematikan lampu");
    lampuOn = false;
    }
    
    //koneksi ke ip
    Serial.print("connecting to ");
    Serial.println(host);

    // Gunakan kelas WiFiClient untuk membuat koneksi TCP
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    //kirim data kw server
     client.print(String("GET http://192.168.218.102/example/connect.php?") + 
                              ("temperature=") + temperature +
                              ("&humidity=") + humidity +
                              " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 1000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Baca semua baris balasan dari server dan cetak ke Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        
    }

    Serial.println();
    Serial.println("closing connection");
    lcd.clear(); // clears the display to print new readings
}
