# 🌤️ Stasiun Cuaca Mini — REST API Backend

> **Versi: v0.0.1** | Node.js + Express.js | Vercel-Ready Serverless

[![GitHub](https://img.shields.io/badge/GitHub-BackendDHT11__V--0.0.1-181717?logo=github)](https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1)

REST API Backend untuk proyek IoT **Stasiun Cuaca Mini** menggunakan ESP32 + sensor DHT11.  
API ini menerima data suhu & kelembapan dari ESP32 **on-demand** (hanya saat diperintahkan), menyimpannya, dan menyediakannya untuk frontend/dashboard.

> ⚠️ **Perubahan Penting:** ESP32 **TIDAK lagi mengirim data otomatis** setiap 5 detik.  
> Sekarang menggunakan sistem **Command Queue** — ESP32 hanya kirim data saat ada perintah `"send"` dari server.

---

## 📋 Daftar Isi

- [Fitur](#-fitur)
- [Cara Kerja (Command-Based)](#-cara-kerja-command-based)
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
| Command Queue — ESP32 kirim data hanya saat diminta | ✅ |
| Live Reading — monitoring real-time tanpa simpan permanen | ✅ |
| Terima data sensor dari ESP32 (POST on-demand) | ✅ |
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

## 🔄 Cara Kerja (Command-Based)

```
┌─────────────────┐                  ┌───────────────────────┐
│   ESP32 + DHT11 │                  │  Backend API (Vercel) │
│   + OLED + Buzz │                  │  Node.js + Express.js │
└────────┬────────┘                  └───────────┬───────────┘
         │                                       │
         │  [Setiap 5 detik - LOOP]              │
         │                                       │
         │  1. Baca sensor DHT11                 │
         │     → Tampilkan di OLED               │
         │                                       │
         │  2. POST /api/sensor/live              │
         │     → Update live reading (tiap 10s)  │
         │ ─────────────────────────────────────> │
         │                                       │
         │  3. GET /api/command?device_id=X       │
         │     → Polling: ada command "send"?     │
         │ ─────────────────────────────────────> │
         │                                       │
         │     Jika command = "none":             │
         │     OLED: "Status: Standby"            │
         │                                       │
         │     Jika command = "send":             │
         │  4. POST /api/sensor                   │
         │     → Kirim data sensor ke API         │
         │ ─────────────────────────────────────> │
         │     OLED: "Sukses Ngirim API!"         │
         │     Buzzer: 🔔 beep beep               │
         │                                       │

  ┌──────────────────────┐
  │  Frontend / Postman  │
  │  POST /api/command   │ ← User trigger "send"
  │  {device_id, "send"} │
  └──────────────────────┘
```

**Ringkasan:**
1. **Sensor selalu aktif** — baca suhu & kelembapan, tampil di OLED
2. **Live reading** dikirim ke server tiap 10 detik (tidak disimpan permanen)
3. **ESP32 polling** command dari server tiap 5 detik
4. **Data hanya dikirim** ke `POST /api/sensor` kalau ada command `"send"`
5. **Command sekali pakai** — setelah diambil ESP32, otomatis terhapus dari server

---

## 🛠️ Tech Stack

- **Runtime:** Node.js
- **Framework:** Express.js v4
- **Middleware:** cors, express.json (body-parser)
- **Hosting:** Vercel Serverless Functions
- **Sensor:** ESP32 + DHT11
- **Display:** OLED SSD1306 128x64
- **Notifikasi:** Buzzer Pasif

---

## 📁 Struktur Folder

```
DHT11/
├── api/
│   └── index.js          ← Express app (entry point Vercel)
├── SketchArduinoIDE/
│   └── DHT11-ESP32/
│       └── sketch_jun5a/
│           └── sketch_jun5a.ino  ← Firmware ESP32 (command-based)
├── server.js             ← Entry point development lokal (app.listen)
├── package.json          ← Dependencies & scripts
├── vercel.json           ← Konfigurasi routing Vercel
├── .gitignore
├── README.md             ← File ini
├── USER_GUIDE_API.MD     ← Panduan testing Postman (untuk teman)
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

**Base URL:** `http://localhost:3000` (lokal) atau `https://backend-dht-11-v-0-0-1.vercel.app` (produksi)

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

### 4. Kirim Data Sensor — On-Demand (dari ESP32)

```
POST /api/sensor
Content-Type: application/json
```

> ⚠️ ESP32 sekarang hanya mengirim ke endpoint ini **saat ada command `"send"`** dari server.

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

### 7. 🎮 Command Queue — Perintahkan ESP32 Kirim Data (BARU!)

#### A. Kirim Perintah ke ESP32

```
POST /api/command
Content-Type: application/json
```

**Body:**
```json
{
  "device_id": "ESP32_ALAT_01",
  "command": "send"
}
```

**Response (200):**
```json
{
  "success": true,
  "message": "Command 'send' berhasil dijadwalkan untuk ESP32_ALAT_01.",
  "data": {
    "command": "send",
    "timestamp": "2026-06-05T02:00:00.000Z"
  }
}
```

> ⚠️ Command bersifat **sekali pakai**. Setelah ESP32 mengambilnya, command otomatis terhapus.

**Command yang tersedia:**

| Command | Fungsi |
|---------|--------|
| `"send"` | Perintahkan ESP32 mengirim data sensor terkini ke `POST /api/sensor` |

**Error Responses:**

| Kasus | Status | Response |
|-------|--------|----------|
| Field kosong `{}` | 400 | `"Field 'device_id' dan 'command' wajib diisi."` |
| Command tidak valid | 400 | `"Command 'xyz' tidak valid. Gunakan: send"` |

#### B. Cek Command Pending (Dipakai ESP32)

```
GET /api/command?device_id=ESP32_ALAT_01
```

**Response — Ada command (200):**
```json
{
  "success": true,
  "command": "send",
  "timestamp": "2026-06-05T02:00:00.000Z"
}
```

**Response — Tidak ada command (200):**
```json
{
  "success": true,
  "command": "none"
}
```

---

### 8. 📡 Live Reading — Monitoring Real-Time (BARU!)

ESP32 mengirim live reading setiap ~10 detik. Data ini **TIDAK disimpan permanen** — hanya di-overwrite sebagai "pembacaan terkini".

#### A. Update Live Reading (Dipakai ESP32)

```
POST /api/sensor/live
Content-Type: application/json
```

**Body:**
```json
{
  "suhu": 28.5,
  "kelembapan": 65.2,
  "device_id": "ESP32_ALAT_01"
}
```

**Response (200):**
```json
{
  "success": true,
  "message": "Live reading updated."
}
```

#### B. Ambil Live Reading Device Tertentu

```
GET /api/sensor/live?device_id=ESP32_ALAT_01
```

**Response (200):**
```json
{
  "success": true,
  "data": {
    "device_id": "ESP32_ALAT_01",
    "suhu": 28.5,
    "kelembapan": 65.2,
    "updated_at": "2026-06-05T02:00:00.000Z"
  }
}
```

#### C. Ambil Semua Live Reading

```
GET /api/sensor/live
```

**Response (200):**
```json
{
  "success": true,
  "data": [
    { "device_id": "ESP32_ALAT_01", "suhu": 28.5, "kelembapan": 65.2, "updated_at": "..." }
  ]
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
| 1 | `GET` | `/api` | — | Health check OK |
| 2 | `GET` | `/api/ping?device_id=ESP32_ALAT_01` | — | `"message": "pong"` |
| 3 | `POST` | `/api/sensor/live` | `{"suhu": 28.5, "kelembapan": 65.2, "device_id": "ESP32_ALAT_01"}` | `"Live reading updated."` |
| 4 | `GET` | `/api/sensor/live?device_id=ESP32_ALAT_01` | — | Lihat live reading |
| 5 | `GET` | `/api/command?device_id=ESP32_ALAT_01` | — | `"command": "none"` |
| 6 | `POST` | `/api/command` | `{"device_id": "ESP32_ALAT_01", "command": "send"}` | Command dijadwalkan |
| 7 | `GET` | `/api/command?device_id=ESP32_ALAT_01` | — | `"command": "send"` (consumed!) |
| 8 | `GET` | `/api/command?device_id=ESP32_ALAT_01` | — | `"command": "none"` (sudah consumed) |
| 9 | `POST` | `/api/sensor` | `{"suhu": 28.5, "kelembapan": 65.2, "device_id": "ESP32_ALAT_01"}` | Status `201` |
| 10 | `GET` | `/api/sensor?latest=true` | — | 1 data terbaru |
| 11 | `GET` | `/api/devices` | — | Perangkat terdaftar |
| 12 | `DELETE` | `/api/sensor` | — | Data dihapus |

> **Tip:** Di Postman, pastikan pilih tab **Body → raw → JSON** saat mengirim POST request.

### Flow Lengkap Testing (Simulasi ESP32):

```
1. GET /api                         → Pastikan server hidup
2. POST /api/sensor/live            → Simulasi ESP32 kirim live reading
3. GET /api/sensor/live             → Cek live reading masuk
4. GET /api/command?device_id=...   → Cek: belum ada command → "none"
5. POST /api/command                → Kirim perintah {"command":"send"}
6. GET /api/command?device_id=...   → ESP32 ambil command → "send" (consumed!)
7. POST /api/sensor                 → ESP32 kirim data sensor (karena ada command)
8. GET /api/sensor?latest=true      → Verifikasi data tersimpan
```

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
| Polling interval | Firmware ESP32 (5 detik) | Edit `delay()` di `loop()` → re-flash |
| Live reading interval | Firmware ESP32 (10 detik) | Edit `LIVE_INTERVAL` → re-flash |
| Sensor pin (GPIO) | Firmware ESP32 | Edit `DHTPIN` → re-flash |

### Kode Arduino IDE (ESP32 + DHT11 + OLED + Buzzer)

File sketch lengkap tersedia di: `SketchArduinoIDE/DHT11-ESP32/sketch_jun5a/sketch_jun5a.ino`

```cpp
// === RINGKASAN FUNGSI UTAMA ===

// Cek apakah ada command "send" dari server
bool cekCommandDariServer() {
  // GET /api/command?device_id=ESP32_ALAT_01
  // Return true jika command = "send"
}

// Kirim data sensor ke API (hanya saat diminta)
bool kirimDataSensor(float suhu, float kelembapan) {
  // POST /api/sensor
}

// Kirim live reading (update berkala, tidak disimpan permanen)
void kirimLiveReading(float suhu, float kelembapan) {
  // POST /api/sensor/live
}

void loop() {
  // 1. Baca sensor DHT11 → Tampilkan di OLED (SELALU)
  // 2. Kirim live reading tiap 10 detik
  // 3. Polling GET /api/command
  //    → Jika "send" → POST /api/sensor → Buzzer beep
  //    → Jika "none" → OLED: "Status: Standby"
}
```

### Alur Kerja ESP32 → API (Command-Based)

```
┌─────────────┐         ┌──────────────────┐         ┌──────────────┐
│   ESP32     │         │   Backend API    │         │  User/       │
│  + DHT11    │         │  (Express.js)    │         │  Frontend    │
│  + OLED     │         │  Vercel          │         │  Postman     │
└──────┬──────┘         └────────┬─────────┘         └──────┬───────┘
       │                         │                          │
       │  [Boot]                 │                          │
       │  GET /api/ping          │                          │
       │ ───────────────────────>│  ← Cek server hidup     │
       │         "pong" 200      │                          │
       │ <───────────────────────│                          │
       │                         │                          │
       │  [Setiap 10 detik]      │                          │
       │  POST /api/sensor/live  │                          │
       │  {suhu, kelembapan}     │                          │
       │ ───────────────────────>│  ← Update live reading   │
       │                         │                          │
       │  [Setiap 5 detik]       │                          │
       │  GET /api/command       │                          │
       │ ───────────────────────>│                          │
       │    "command": "none"    │                          │
       │ <───────────────────────│                          │
       │  OLED: "Standby"       │                          │
       │                         │                          │
       │                         │  POST /api/command       │
       │                         │  {"command": "send"}     │
       │                         │ <────────────────────────│ ← User trigger!
       │                         │                          │
       │  GET /api/command       │                          │
       │ ───────────────────────>│                          │
       │    "command": "send"    │  ← Command consumed!     │
       │ <───────────────────────│                          │
       │                         │                          │
       │  POST /api/sensor       │                          │
       │  {suhu, kelembapan}     │                          │
       │ ───────────────────────>│  ← Data disimpan!        │
       │         201 Created     │                          │
       │ <───────────────────────│                          │
       │  OLED: "Sukses!"       │                          │
       │  Buzzer: 🔔 beep beep  │                          │
       │                         │                          │
```

---

## 📤 Push ke GitHub

**Repository:** https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1

```bash
# Stage semua perubahan
git add .

# Commit
git commit -m "feat: command-based API - ESP32 kirim data hanya saat diminta"

# Push ke GitHub → Vercel auto-deploy
git push origin main
```

---

## ☁️ Deploy ke Vercel

### Opsi A: Via GitHub Integration (Recommended)

1. Buka [vercel.com](https://vercel.com) dan login
2. Klik **"Add New Project"**
3. Import repository **`Elysian-ibay/BackendDHT11_V-0.0.1`** dari GitHub
4. Vercel akan otomatis mendeteksi `vercel.json`
5. Klik **"Deploy"** — selesai!
6. Setiap kali push ke GitHub, Vercel **auto-deploy** ✅ (tidak perlu build ulang manual)

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
const char* serverBase = "https://backend-dht-11-v-0-0-1.vercel.app";
```

> ⚠️ **Penting:** In-memory storage akan reset setiap kali Vercel cold-start.  
> Untuk data persistent, upgrade ke database (lihat Roadmap).

---

## 📝 Changelog

### v0.0.1-update2 — 2026-06-05
**Command-Based Architecture**
- ✅ `POST /api/command` — Kirim perintah "send" ke ESP32
- ✅ `GET /api/command` — ESP32 polling cek command (consume sekali pakai)
- ✅ `POST /api/sensor/live` — Update live reading real-time (tidak disimpan permanen)
- ✅ `GET /api/sensor/live` — Ambil live reading terbaru
- ✅ ESP32 sketch diubah ke mode command-based (tidak kirim otomatis)
- ✅ OLED menampilkan "Standby" saat idle, "Mengirim API" saat kirim
- ✅ Buzzer notifikasi saat kirim data
- ✅ `USER_GUIDE_API.MD` — Panduan testing Postman

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
