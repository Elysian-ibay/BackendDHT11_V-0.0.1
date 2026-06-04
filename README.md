# 🌤️ Stasiun Cuaca Mini — REST API Backend

> **Versi: v0.0.1** | Node.js + Express.js | Vercel-Ready Serverless

[![GitHub](https://img.shields.io/badge/GitHub-BackendDHT11__V--0.0.1-181717?logo=github)](https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1)

REST API Backend untuk proyek IoT **Stasiun Cuaca Mini** menggunakan ESP32 + sensor DHT11.  
API ini menerima data suhu & kelembapan dari ESP32, menyimpannya, dan menyediakannya untuk frontend/dashboard.

---

## 📋 Daftar Isi

- [Fitur](#-fitur)
- [Tech Stack](#-tech-stack)
- [Struktur Folder](#-struktur-folder)
- [Instalasi & Setup](#-instalasi--setup)
- [Daftar Endpoint API](#-daftar-endpoint-api)
- [Panduan Testing Postman](#-panduan-testing-postman)
- [Integrasi ESP32](#-integrasi-esp32)
- [Deploy ke Vercel](#-deploy-ke-vercel)
- [Changelog](#-changelog)

---

## ✨ Fitur

| Fitur | Status |
|-------|--------|
| Terima data sensor dari ESP32 (POST) | ✅ |
| Ambil data sensor untuk dashboard (GET) | ✅ |
| Ping/heartbeat test untuk ESP32 | ✅ |
| Device registry (tracking perangkat) | ✅ |
| Validasi input (tipe data & field wajib) | ✅ |
| CORS enabled (akses dari frontend mana saja) | ✅ |
| Vercel serverless compatible | ✅ |
| In-memory storage (untuk testing) | ✅ |
| Database persistent (MongoDB/PostgreSQL) | 🔲 Planned |
| Authentication/API Key | 🔲 Planned |

---

## 🛠️ Tech Stack

- **Runtime:** Node.js
- **Framework:** Express.js v4
- **Middleware:** cors, express.json (body-parser)
- **Hosting:** Vercel Serverless Functions
- **Sensor:** ESP32 + DHT11

---

## 📁 Struktur Folder

```
DHT11/
├── api/
│   └── index.js          ← Express app (entry point Vercel)
├── server.js             ← Entry point development lokal (app.listen)
├── package.json          ← Dependencies & scripts
├── vercel.json           ← Konfigurasi routing Vercel
├── .gitignore
├── README.md             ← File ini
└── DOCS_SUMMARY.md       ← Ringkasan proyek untuk AI lanjutan
```

> **Catatan Penting:**  
> `api/index.js` mengekspor `app` **tanpa** `app.listen()`.  
> `server.js` mengimpor `app` dan menjalankan `app.listen()` — hanya untuk lokal.  
> Pemisahan ini wajib agar kompatibel dengan Vercel.

---

## 🚀 Instalasi & Setup

### Prasyarat
- [Node.js](https://nodejs.org/) v18+ terinstall
- [Postman](https://www.postman.com/) untuk testing (opsional)

### Langkah-langkah

```bash
# 1. Clone repository
git clone https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1.git
cd BackendDHT11_V-0.0.1

# 2. Install dependencies
npm install

# 3. Jalankan server lokal
npm start
```

**Output yang diharapkan:**
```
🌤️  Stasiun Cuaca Mini API
   Server berjalan di: http://localhost:3000
   Health check:       http://localhost:3000/api
   Sensor endpoint:    http://localhost:3000/api/sensor
```

---

## 📡 Daftar Endpoint API

**Base URL:** `http://localhost:3000` (lokal) atau `https://nama-proyek.vercel.app` (produksi)

### 1. Health Check

```
GET /api
```

**Response (200):**
```json
{
  "success": true,
  "message": "🌤️ Stasiun Cuaca Mini API is running!",
  "version": "0.0.1",
  "server_time": "2026-06-05T01:15:00.000Z",
  "total_data": 0,
  "connected_devices": 0,
  "endpoints": { ... }
}
```

---

### 2. Ping / Heartbeat (Test Koneksi ESP32)

```
GET /api/ping?device_id=ESP32_001
```

ESP32 bisa hit endpoint ini saat boot untuk memastikan server merespon.

**Response (200):**
```json
{
  "success": true,
  "message": "pong",
  "server_time": "2026-06-05T01:15:00.000Z",
  "device_id": "ESP32_001",
  "api_version": "0.0.1"
}
```

---

### 3. Daftar Perangkat Terhubung

```
GET /api/devices
```

**Response (200):**
```json
{
  "success": true,
  "message": "1 perangkat terdaftar.",
  "data": [
    {
      "device_id": "ESP32_001",
      "last_ping": "2026-06-05T01:15:00.000Z",
      "ip": "192.168.1.10",
      "user_agent": "ESP32HTTPClient"
    }
  ]
}
```

---

### 4. Kirim Data Sensor (dari ESP32)

```
POST /api/sensor
Content-Type: application/json
```

**Body:**
```json
{
  "suhu": 28.5,
  "kelembapan": 65.2,
  "device_id": "ESP32_001"
}
```

> `device_id` bersifat opsional. Jika tidak dikirim, akan diset `"unknown"`.

**Response (201):**
```json
{
  "success": true,
  "message": "Data sensor berhasil disimpan.",
  "data": {
    "id": 1,
    "device_id": "ESP32_001",
    "suhu": 28.5,
    "kelembapan": 65.2,
    "timestamp": "2026-06-05T01:15:00.000Z"
  }
}
```

**Error Responses:**

| Kasus | Status | Response |
|-------|--------|----------|
| Field kosong `{}` | 400 | `"Field 'suhu' dan 'kelembapan' wajib diisi."` |
| Tipe salah `{"suhu": "panas"}` | 400 | `"Field 'suhu' dan 'kelembapan' harus bertipe number."` |
| Sebagian field `{"suhu": 28.5}` | 400 | `"Field 'suhu' dan 'kelembapan' wajib diisi."` |

---

### 5. Ambil Data Sensor

```
GET /api/sensor              → Semua data (terbaru duluan)
GET /api/sensor?latest=true  → 1 data paling baru
GET /api/sensor?limit=5      → 5 data terbaru
```

**Response — Semua Data (200):**
```json
{
  "success": true,
  "message": "3 data sensor berhasil diambil.",
  "total": 3,
  "data": [
    { "id": 3, "device_id": "ESP32_001", "suhu": 32.0, "kelembapan": 55.3, "timestamp": "..." },
    { "id": 2, "device_id": "ESP32_001", "suhu": 30.1, "kelembapan": 70.5, "timestamp": "..." },
    { "id": 1, "device_id": "ESP32_001", "suhu": 28.5, "kelembapan": 65.2, "timestamp": "..." }
  ]
}
```

**Response — Data Terbaru (200):**
```json
{
  "success": true,
  "message": "Data sensor terbaru berhasil diambil.",
  "data": {
    "id": 3,
    "device_id": "ESP32_001",
    "suhu": 32.0,
    "kelembapan": 55.3,
    "timestamp": "2026-06-05T01:17:00.000Z"
  }
}
```

---

### 6. Hapus Semua Data (Testing)

```
DELETE /api/sensor
```

**Response (200):**
```json
{
  "success": true,
  "message": "3 data sensor berhasil dihapus."
}
```

---

## 🧪 Panduan Testing Postman

### Step 1 — Jalankan server
```bash
npm start
```

### Step 2 — Buka Postman, ikuti urutan berikut:

| # | Method | URL | Body (raw JSON) | Expected |
|---|--------|-----|------------------|----------|
| 1 | `GET` | `http://localhost:3000/api` | — | Health check OK |
| 2 | `GET` | `http://localhost:3000/api/ping?device_id=ESP32_001` | — | `"message": "pong"` |
| 3 | `POST` | `http://localhost:3000/api/sensor` | `{"suhu": 28.5, "kelembapan": 65.2, "device_id": "ESP32_001"}` | Status `201` |
| 4 | `POST` | `http://localhost:3000/api/sensor` | `{"suhu": 30.1, "kelembapan": 70.5, "device_id": "ESP32_001"}` | Status `201` |
| 5 | `POST` | `http://localhost:3000/api/sensor` | `{"suhu": 32.0, "kelembapan": 55.3, "device_id": "ESP32_001"}` | Status `201` |
| 6 | `GET` | `http://localhost:3000/api/sensor` | — | 3 data |
| 7 | `GET` | `http://localhost:3000/api/sensor?latest=true` | — | 1 data terbaru |
| 8 | `GET` | `http://localhost:3000/api/devices` | — | 1 perangkat |
| 9 | `POST` | `http://localhost:3000/api/sensor` | `{}` | Status `400` |
| 10 | `DELETE` | `http://localhost:3000/api/sensor` | — | Data dihapus |

> **Tip:** Di Postman, pastikan pilih tab **Body → raw → JSON** saat mengirim POST request.

---

## 🔌 Integrasi ESP32

> ⚠️ **Konfigurasi WiFi (SSID & Password) di-hardcode langsung di firmware ESP32.**  
> Ini karena ESP32 harus terhubung ke WiFi terlebih dahulu sebelum bisa mengakses API.  
> Untuk versi mendatang, bisa ditambahkan fitur WiFi Manager (AP Mode + Captive Portal).

### Konfigurasi yang Di-Hardcode di ESP32

| Konfigurasi | Lokasi | Cara Ubah |
|-------------|--------|-----------|
| WiFi SSID | Firmware ESP32 | Edit kode → re-flash |
| WiFi Password | Firmware ESP32 | Edit kode → re-flash |
| Server URL | Firmware ESP32 | Edit kode → re-flash |
| Device ID | Firmware ESP32 | Edit kode → re-flash |
| Delay interval | Firmware ESP32 | Edit kode → re-flash |
| Sensor pin (GPIO) | Firmware ESP32 | Edit kode → re-flash |

### Kode Arduino IDE (ESP32 + DHT11)

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ===== KONFIGURASI HARDCODED — EDIT SESUAI KEBUTUHAN =====
#define DHTPIN 4              // Pin GPIO sensor DHT11
#define DHTTYPE DHT11         // Tipe sensor
#define DELAY_MS 10000        // Interval kirim data (ms) — 10 detik

const char* ssid         = "NAMA_WIFI_KAMU";       // ← Ganti!
const char* password     = "PASSWORD_WIFI_KAMU";    // ← Ganti!
const char* deviceId     = "ESP32_001";             // ← Ganti sesuai device

// ===== GANTI URL INI =====
// Lokal  : "http://192.168.1.x:3000"
// Vercel : "https://nama-proyek.vercel.app"
const char* serverBase   = "http://192.168.1.x:3000"; // ← Ganti!

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // ----- Koneksi WiFi -----
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected!");
  Serial.print("   IP Address: ");
  Serial.println(WiFi.localIP());

  // ----- Ping Test ke Server -----
  pingServer();
}

void loop() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("❌ Gagal membaca sensor DHT11!");
    delay(5000);
    return;
  }

  Serial.printf("📡 Suhu: %.1f°C | Kelembapan: %.1f%%\n", suhu, kelembapan);
  kirimData(suhu, kelembapan);

  delay(DELAY_MS); // Interval kirim data (default: 10 detik)
}

// ===== FUNGSI: Ping ke server untuk cek koneksi =====
void pingServer() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String pingURL = String(serverBase) + "/api/ping?device_id=" + deviceId;

  http.begin(pingURL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    Serial.println("✅ Server merespon: PONG!");
    Serial.println("   Response: " + http.getString());
  } else {
    Serial.printf("❌ Server tidak merespon. HTTP Code: %d\n", httpCode);
  }

  http.end();
}

// ===== FUNGSI: Kirim data sensor ke API =====
void kirimData(float suhu, float kelembapan) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi terputus!");
    return;
  }

  HTTPClient http;
  String sensorURL = String(serverBase) + "/api/sensor";

  http.begin(sensorURL);
  http.addHeader("Content-Type", "application/json");

  // Buat JSON payload
  String jsonPayload = "{\"suhu\":" + String(suhu, 1) +
                       ",\"kelembapan\":" + String(kelembapan, 1) +
                       ",\"device_id\":\"" + deviceId + "\"}";

  int httpCode = http.POST(jsonPayload);

  if (httpCode == 201) {
    Serial.println("✅ Data berhasil dikirim ke server!");
    Serial.println("   Response: " + http.getString());
  } else {
    Serial.printf("❌ Gagal kirim data. HTTP Code: %d\n", httpCode);
    if (httpCode > 0) {
      Serial.println("   Response: " + http.getString());
    }
  }

  http.end();
}
```

### Alur Kerja ESP32 → API

```
┌─────────────┐         ┌──────────────────┐
│   ESP32     │         │   Backend API    │
│  + DHT11    │         │  (Express.js)    │
└──────┬──────┘         └────────┬─────────┘
       │                         │
       │  1. GET /api/ping       │
       │ ───────────────────────>│  ← Cek server hidup
       │         "pong" 200      │
       │ <───────────────────────│
       │                         │
       │  2. POST /api/sensor    │
       │  {suhu, kelembapan}     │
       │ ───────────────────────>│  ← Kirim data sensor
       │         201 Created     │
       │ <───────────────────────│
       │                         │
       │     (ulangi setiap      │
       │      10 detik)          │
       │                         │
```

---

## 📤 Push ke GitHub

**Repository:** https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1

```bash
# 1. Inisialisasi git (jika belum)
git init

# 2. Tambahkan remote repository
git remote add origin https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1.git

# 3. Stage semua file
git add .

# 4. Commit
git commit -m "v0.0.1 — Initial release: REST API Stasiun Cuaca Mini"

# 5. Rename branch ke main (jika perlu)
git branch -M main

# 6. Push ke GitHub
git push -u origin main
```

---

## ☁️ Deploy ke Vercel

### Opsi A: Via GitHub Integration (Recommended)

1. Buka [vercel.com](https://vercel.com) dan login
2. Klik **"Add New Project"**
3. Import repository **`Elysian-ibay/BackendDHT11_V-0.0.1`** dari GitHub
4. Vercel akan otomatis mendeteksi `vercel.json`
5. Klik **"Deploy"** — selesai!
6. Setiap kali push ke GitHub, Vercel auto-deploy ✅

### Opsi B: Via Vercel CLI

```bash
# 1. Install Vercel CLI
npm install -g vercel

# 2. Login
vercel login

# 3. Deploy (preview)
vercel

# 4. Deploy ke production
vercel --prod
```

### Setelah Deploy

Ganti `serverBase` di kode ESP32 dengan URL Vercel:
```cpp
const char* serverBase = "https://nama-proyek.vercel.app";
```

> ⚠️ **Penting:** In-memory storage akan reset setiap kali Vercel cold-start.  
> Untuk data persistent, upgrade ke database (lihat Roadmap).

---

## 📝 Changelog

### v0.0.1 — 2026-06-05
**Initial Release — Foundation**
- ✅ `POST /api/sensor` — Terima data suhu & kelembapan dari ESP32
- ✅ `GET /api/sensor` — Ambil data sensor (semua / terbaru / limit)
- ✅ `DELETE /api/sensor` — Hapus semua data (testing)
- ✅ `GET /api/ping` — Ping/heartbeat test untuk ESP32
- ✅ `GET /api/devices` — Daftar perangkat terhubung
- ✅ `GET /api` — Health check endpoint
- ✅ CORS enabled
- ✅ Input validation
- ✅ Vercel serverless compatible
- ✅ In-memory storage (Array)

### Roadmap
- 🔲 v0.1.0 — Integrasi MongoDB/PostgreSQL untuk persistent storage
- 🔲 v0.2.0 — API Key authentication untuk keamanan
- 🔲 v0.3.0 — WebSocket untuk real-time data streaming
- 🔲 v1.0.0 — Frontend dashboard (React/Next.js)

---

## 📄 Lisensi

ISC License
