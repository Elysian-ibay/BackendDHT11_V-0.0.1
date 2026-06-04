#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h> // Library untuk mengambil waktu dari internet

// --- 1. Konfigurasi WiFi & Waktu ---
const char* ssid = "12346";
const char* password = ".@nd4l4n!#";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600; // Zona waktu WIB (UTC+7)
const int   daylightOffset_sec = 0;

// --- 2. Konfigurasi API Vercel ---
const char* serverBase = "https://backend-dht-11-v-0-0-1.vercel.app";
String device_id = "ESP32_ALAT_01"; 

// --- 3. Konfigurasi Komponen ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define BUZZER_PIN 5 // Pin untuk Buzzer Pasif

// --- 4. Variabel Global ---
unsigned long lastLiveUpdate = 0;
const unsigned long LIVE_INTERVAL = 10000; // Kirim live reading tiap 10 detik

// Fungsi untuk mendapatkan Waktu Format Indonesia
String dapatkanWaktuIndo() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "Waktu blm sync";
  }
  // Array nama hari
  const char* namaHari[] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
  
  char timeStringBuff[50];
  // Format: "Hari Jam:Menit Tgl/Bulan" (Contoh: Jumat 14:30 05/06)
  sprintf(timeStringBuff, "%s %02d:%02d %02d/%02d", 
          namaHari[timeinfo.tm_wday], 
          timeinfo.tm_hour, timeinfo.tm_min, 
          timeinfo.tm_mday, timeinfo.tm_mon + 1);
          
  return String(timeStringBuff);
}

// ============================================================
//  Cek apakah ada command "send" dari server
//  GET /api/command?device_id=ESP32_ALAT_01
//  Return: true jika command = "send", false jika "none"
// ============================================================
bool cekCommandDariServer() {
  HTTPClient http;
  String url = String(serverBase) + "/api/command?device_id=" + device_id;
  http.begin(url);
  http.setTimeout(5000);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    http.end();
    
    // Cek sederhana apakah response mengandung "send"
    // Format response: {"success":true,"command":"send","timestamp":"..."}
    if (payload.indexOf("\"send\"") > -1) {
      Serial.println("[CMD] Command 'send' diterima dari server!");
      return true;
    }
    return false; // command = "none"
  }
  
  Serial.printf("[CMD] Gagal cek command, HTTP code: %d\n", httpCode);
  http.end();
  return false;
}

// ============================================================
//  Kirim data sensor ke POST /api/sensor (hanya saat diminta)
// ============================================================
bool kirimDataSensor(float suhu, float kelembapan) {
  HTTPClient http;
  String url = String(serverBase) + "/api/sensor";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);

  String body = "{\"suhu\":" + String(suhu) + ",\"kelembapan\":" + String(kelembapan) + ",\"device_id\":\"" + device_id + "\"}";
  int httpCode = http.POST(body);
  http.end();

  if (httpCode > 0) {
    Serial.printf("[SEND] Data terkirim! HTTP code: %d\n", httpCode);
    return true;
  }
  Serial.printf("[SEND] Gagal kirim data! HTTP code: %d\n", httpCode);
  return false;
}

// ============================================================
//  Kirim live reading ke POST /api/sensor/live (update saja, tidak disimpan permanen)
// ============================================================
void kirimLiveReading(float suhu, float kelembapan) {
  HTTPClient http;
  String url = String(serverBase) + "/api/sensor/live";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(3000);

  String body = "{\"suhu\":" + String(suhu) + ",\"kelembapan\":" + String(kelembapan) + ",\"device_id\":\"" + device_id + "\"}";
  int httpCode = http.POST(body);
  http.end();

  if (httpCode > 0) {
    Serial.println("[LIVE] Live reading updated.");
  } else {
    Serial.printf("[LIVE] Gagal update live reading. HTTP: %d\n", httpCode);
  }
}

void setup() {
  Serial.begin(115200);

  // Inisialisasi Sensor & Buzzer Pasif
  dht.begin();
  pinMode(BUZZER_PIN, OUTPUT);

  // Inisialisasi Layar OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("OLED gagal!"));
    for(;;); 
  }

  // Tampilkan pesan pembuka
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 10);
  display.println("Stasiun Cuaca");
  display.setCursor(25, 25);
  display.println("ALAT IoT Mini");
  
  // --- Proses Koneksi WiFi ---
  display.setCursor(0, 45);
  display.print("Konek WiFi...");
  display.display();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Sinkronisasi waktu internet (NTP) setelah WiFi Konek
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  display.clearDisplay();
  display.setCursor(15, 25);
  display.println("WiFi Terhubung!");
  display.display();
  
  // Bunyi beep pendek tanda alat siap
  tone(BUZZER_PIN, 2000, 150); // Nada 2000Hz selama 150ms
  delay(2000); 
}

void loop() {
  delay(5000); // Tunggu 5 detik antar siklus

  // ============================
  //  STEP 1: Baca sensor DHT11
  // ============================
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 

  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca DHT!");
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Error Baca DHT11");
    display.display();
    return;
  }

  // ============================
  //  STEP 2: Tampilkan di OLED (SELALU)
  // ============================
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Suhu  : ");
  display.print(t, 1);
  display.print(" C");
  
  display.setCursor(0, 15);
  display.print("Lembap: ");
  display.print(h, 1);
  display.print(" %");

  // ============================
  //  STEP 3: Cek WiFi & Polling Command
  // ============================
  if (WiFi.status() == WL_CONNECTED) {
    
    // --- Kirim live reading secara berkala (tidak simpan permanen) ---
    unsigned long now = millis();
    if (now - lastLiveUpdate >= LIVE_INTERVAL) {
      kirimLiveReading(t, h);
      lastLiveUpdate = now;
    }

    // --- Cek apakah ada command "send" dari server ---
    Serial.println("[POLL] Mengecek command dari server...");
    bool adaCommand = cekCommandDariServer();

    if (adaCommand) {
      // ===== ADA PERINTAH "SEND" → KIRIM DATA KE API =====
      display.setCursor(0, 32);
      display.print(">> Mengirim API <<");
      display.display();
      
      // BUZZER: 1x Bunyi saat mulai ngirim (Nada 1500Hz)
      tone(BUZZER_PIN, 1500, 50); 

      bool sukses = kirimDataSensor(t, h);

      // Bersihkan area bawah layar untuk status
      display.fillRect(0, 30, 128, 34, SSD1306_BLACK); 
      display.setCursor(0, 32);

      if (sukses) {
        display.println("Sukses Ngirim API!");
        display.setCursor(0, 48);
        display.print(dapatkanWaktuIndo()); 

        // BUZZER: 2x Nada cepat (Nada ceria)
        tone(BUZZER_PIN, 2000, 100);
        delay(150);
        tone(BUZZER_PIN, 2500, 150);
      } else {
        display.print("Gagal ngirim! (Err)");
        // BUZZER: Nada rendah panjang (Error)
        tone(BUZZER_PIN, 500, 1000); 
      }

    } else {
      // ===== TIDAK ADA COMMAND → STANDBY =====
      display.setCursor(0, 35);
      display.print("Status: Standby");
      display.setCursor(0, 50);
      display.print(dapatkanWaktuIndo());
    }

  } else {
    display.setCursor(0, 35);
    display.print("WiFi Terputus!");
  }
  
  display.display();
}