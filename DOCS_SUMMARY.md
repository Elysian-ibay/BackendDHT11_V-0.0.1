# 📋 PROJECT SUMMARY — Stasiun Cuaca Mini API

> **Dokumen ini dibuat agar bisa dilampirkan ke AI (Gemini/ChatGPT) untuk melanjutkan progress proyek.**  
> **Terakhir diupdate:** 2026-06-05  
> **Versi saat ini:** v0.0.1-update2  
> **Repository:** https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1

---

## 🎯 Deskripsi Proyek

Proyek IoT **"Stasiun Cuaca Mini"** menggunakan **ESP32 + sensor DHT11** untuk membaca suhu dan kelembapan, kemudian mengirim data tersebut ke **REST API Backend** yang dibangun dengan **Node.js + Express.js**.

API ini di-deploy sebagai **Vercel Serverless Function**.

> ⚠️ **Perubahan Arsitektur (v0.0.1-update2):** ESP32 **TIDAK lagi mengirim data otomatis** setiap 5 detik. Sekarang menggunakan **Command Queue** — ESP32 hanya kirim data saat ada perintah `"send"` dari server via `POST /api/command`.

---

## 🏗️ Arsitektur Sistem (Command-Based)

```
  ESP32 + DHT11 + OLED + Buzzer
       │
       │ [Boot] GET /api/ping (cek koneksi)
       │
       │ [Tiap 10s] POST /api/sensor/live (update live reading)
       │
       │ [Tiap 5s] GET /api/command (polling: ada command?)
       │            ├── "none" → OLED: "Standby"
       │            └── "send" → POST /api/sensor (kirim data!)
       ▼
  ┌──────────────────────────────┐
  │  REST API Backend            │
  │  Node.js + Express.js       │
  │  Hosted: Vercel Serverless   │
  │  Repo: GitHub                │
  └──────────────┬───────────────┘
                 │
                 ▼
  ┌──────────────────────────────┐
  │  Frontend / Postman / User   │
  │  POST /api/command           │ ← Trigger "send"
  │  GET /api/sensor/live        │ ← Monitor real-time
  │  GET /api/sensor             │ ← Ambil data tersimpan
  └──────────────────────────────┘
```

---

## 📁 Struktur Folder

```
BackendDHT11_V-0.0.1/
├── api/
│   └── index.js          ← Express app utama (Vercel entry point)
├── SketchArduinoIDE/
│   └── DHT11-ESP32/
│       └── sketch_jun5a/
│           └── sketch_jun5a.ino  ← Firmware ESP32 (command-based)
├── server.js             ← Local dev server (app.listen terpisah)
├── package.json          ← Dependencies: express, cors
├── vercel.json           ← Routing config Vercel
├── .gitignore
├── README.md             ← Dokumentasi API lengkap
├── USER_GUIDE_API.MD     ← Panduan testing Postman (untuk teman)
└── DOCS_SUMMARY.md       ← File ini
```

---

## 📡 Daftar Endpoint API (v0.0.1-update2)

| # | Method | Endpoint | Fungsi | Status |
|---|--------|----------|--------|--------|
| 1 | `GET` | `/api` | Health check, info server, versi API | ✅ Done |
| 2 | `GET` | `/api/ping?device_id=X` | Ping/heartbeat — ESP32 cek koneksi ke server | ✅ Done |
| 3 | `GET` | `/api/devices` | Daftar perangkat ESP32 yang pernah terhubung | ✅ Done |
| 4 | `POST` | `/api/sensor` | Terima data `{suhu, kelembapan, device_id}` dari ESP32 **(on-demand)** | ✅ Done |
| 5 | `GET` | `/api/sensor` | Ambil semua data sensor (terbaru duluan) | ✅ Done |
| 6 | `GET` | `/api/sensor?latest=true` | Ambil 1 data sensor paling baru | ✅ Done |
| 7 | `GET` | `/api/sensor?limit=N` | Ambil N data sensor terbaru | ✅ Done |
| 8 | `DELETE` | `/api/sensor` | Hapus semua data sensor (untuk testing) | ✅ Done |
| 9 | `POST` | `/api/command` | **Kirim perintah ke ESP32** `{device_id, command: "send"}` | ✅ **BARU** |
| 10 | `GET` | `/api/command?device_id=X` | **ESP32 polling** cek command pending (consume sekali pakai) | ✅ **BARU** |
| 11 | `POST` | `/api/sensor/live` | **ESP32 update live reading** (tidak disimpan permanen) | ✅ **BARU** |
| 12 | `GET` | `/api/sensor/live` | **Ambil live reading** terbaru (per device atau semua) | ✅ **BARU** |

---

## 📦 Payload JSON

### ESP32 → API (POST /api/sensor) — On-Demand
```json
{
  "suhu": 28.5,
  "kelembapan": 65.2,
  "device_id": "ESP32_ALAT_01"
}
```

### API → Response (201)
```json
{
  "success": true,
  "message": "Data sensor berhasil disimpan.",
  "data": {
    "id": 1,
    "device_id": "ESP32_ALAT_01",
    "suhu": 28.5,
    "kelembapan": 65.2,
    "timestamp": "2026-06-05T01:15:00.000Z"
  }
}
```

### User → API (POST /api/command) — Trigger Send
```json
{
  "device_id": "ESP32_ALAT_01",
  "command": "send"
}
```

### API → ESP32 (GET /api/command) — Response
```json
{
  "success": true,
  "command": "send",
  "timestamp": "2026-06-05T02:00:00.000Z"
}
```
> Setelah ESP32 mengambil command, response berubah menjadi `"command": "none"` (consumed).

### ESP32 → API (POST /api/sensor/live) — Live Reading
```json
{
  "suhu": 28.5,
  "kelembapan": 65.2,
  "device_id": "ESP32_ALAT_01"
}
```

---

## 🔑 Keputusan Teknis yang Sudah Dibuat

| Keputusan | Detail | Alasan |
|-----------|--------|--------|
| **Framework** | Express.js v4 | Ringan, banyak dokumentasi, cocok untuk IoT backend |
| **Storage** | In-memory Array | Sementara untuk testing via Postman. Belum pakai database |
| **Hosting** | Vercel Serverless | Gratis, mudah deploy, auto-scaling |
| **Struktur** | `api/index.js` + `server.js` terpisah | Kompatibel Vercel (tanpa app.listen) + bisa run lokal |
| **CORS** | Enabled untuk semua origin | Agar bisa diakses dari frontend mana saja |
| **Validasi** | Cek field wajib + tipe data number | Mencegah data kotor dari sensor |
| **Device Tracking** | `connectedDevices` registry in-memory | Tahu perangkat mana saja yang pernah terhubung |
| **Ping Endpoint** | `GET /api/ping` → `"pong"` | ESP32 bisa verifikasi koneksi ke server saat boot |
| **Command Queue** | `pendingCommands` in-memory per device | ESP32 tidak kirim otomatis, hanya saat ada command "send". Command bersifat sekali pakai (consumed). |
| **Live Reading** | `liveReadings` in-memory per device | Frontend bisa monitor suhu real-time tanpa trigger "kirim ke API". Data di-overwrite terus, tidak disimpan permanen. |
| **WiFi Config** | **Hardcoded** di firmware ESP32 | ESP32 harus konek WiFi dulu sebelum bisa akses API (chicken-and-egg). Remote config via API tidak possible untuk WiFi. |
| **Polling Interval** | 5 detik (loop) + 10 detik (live reading) | Cukup responsif untuk demo, tidak terlalu berat untuk Vercel serverless. |
| **Version Control** | GitHub repo + Vercel auto-deploy | Push ke GitHub → Vercel auto-deploy |

---

## 🔧 Konfigurasi ESP32 (Hardcoded)

Semua konfigurasi ESP32 di-hardcode langsung di firmware. Untuk mengubah, harus edit kode lalu re-flash ke board.

| Konfigurasi | Default | Cara Ubah |
|-------------|---------|-----------|
| WiFi SSID | `"12346"` | Edit kode → re-flash |
| WiFi Password | `".@nd4l4n!#"` | Edit kode → re-flash |
| Server URL | `"https://backend-dht-11-v-0-0-1.vercel.app"` | Edit `serverBase` → re-flash |
| Device ID | `"ESP32_ALAT_01"` | Edit `device_id` → re-flash |
| Loop delay | `5000` ms (5 detik) | Edit `delay()` di `loop()` → re-flash |
| Live interval | `10000` ms (10 detik) | Edit `LIVE_INTERVAL` → re-flash |
| Sensor pin | GPIO `4` | Edit `DHTPIN` → re-flash |
| Sensor type | `DHT11` | Edit `DHTTYPE` → re-flash |
| Buzzer pin | GPIO `5` | Edit `BUZZER_PIN` → re-flash |
| OLED address | `0x3C` | Edit `SCREEN_ADDRESS` → re-flash |

---

## ✅ Yang Sudah Selesai (Progress)

### Backend API
- [x] Inisialisasi proyek Node.js + Express.js
- [x] Middleware: cors + express.json
- [x] `POST /api/sensor` — Terima data dari ESP32 (on-demand)
- [x] `GET /api/sensor` — Ambil data (semua / ?latest=true / ?limit=N)
- [x] `DELETE /api/sensor` — Hapus semua data testing
- [x] `GET /api/ping` — Heartbeat/ping test untuk ESP32
- [x] `GET /api/devices` — Daftar perangkat yang pernah terhubung
- [x] `GET /api` — Health check + versi + info server
- [x] `POST /api/command` — Command queue (trigger ESP32 kirim data)
- [x] `GET /api/command` — ESP32 polling cek command (consume sekali pakai)
- [x] `POST /api/sensor/live` — Update live reading (tidak simpan permanen)
- [x] `GET /api/sensor/live` — Ambil live reading terbaru
- [x] Input validation (field wajib + tipe data)
- [x] Struktur Vercel-compatible (api/index.js + server.js terpisah)
- [x] vercel.json routing config
- [x] Testing semua endpoint lokal berhasil ✅

### Dokumentasi
- [x] README.md lengkap (endpoint, Postman guide, ESP32 code, deploy steps)
- [x] DOCS_SUMMARY.md (file ini — untuk AI lanjutan)
- [x] USER_GUIDE_API.MD (panduan testing Postman untuk teman)
- [x] Changelog v0.0.1 + v0.0.1-update2

### Kode ESP32 (Arduino IDE)
- [x] Firmware ESP32 + DHT11 + OLED + Buzzer (command-based)
- [x] Fungsi `cekCommandDariServer()` — GET /api/command polling
- [x] Fungsi `kirimDataSensor()` — POST /api/sensor (on-demand)
- [x] Fungsi `kirimLiveReading()` — POST /api/sensor/live (berkala)
- [x] OLED: "Standby" saat idle, "Mengirim API" saat kirim
- [x] Buzzer notifikasi (beep saat kirim, nada ceria saat sukses)
- [x] Konfigurasi hardcoded (SSID, password, URL, delay, pin)
- [ ] Belum di-flash ke board ESP32 (perlu dilakukan secara manual)

### Deployment
- [x] Repository GitHub: https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1
- [x] Push ke GitHub ✅
- [x] Deploy ke Vercel (auto-deploy dari GitHub) ✅

---

## 🔲 Yang Belum Dibuat (TODO / Next Steps)

| Prioritas | Task | Keterangan |
|-----------|------|------------|
| 🔴 High | **Database Integration** | Ganti in-memory array → MongoDB Atlas / Supabase. Data saat ini hilang setiap cold-start Vercel. |
| 🟡 Medium | **API Key Authentication** | Tambah header `X-API-Key` agar endpoint tidak bisa diakses sembarang orang |
| 🟡 Medium | **Rate Limiting** | Batasi request per menit per IP/device |
| 🟡 Medium | **Remote Config** | Endpoint `GET/PUT /api/config/:device_id` untuk ubah delay/settings ESP32 tanpa re-flash |
| 🟡 Medium | **Multiple Commands** | Tambah command selain "send" (misal: "reset", "config", "buzzer") |
| 🟢 Low | **Data Agregasi** | Endpoint rata-rata suhu/kelembapan per jam/hari |
| 🟢 Low | **Frontend Dashboard** | Web dashboard React/Next.js untuk visualisasi data real-time + tombol "Send" |
| 🟢 Low | **WebSocket** | Real-time push ke dashboard tanpa polling |
| 🟢 Low | **WiFi Manager** | ESP32 AP Mode + Captive Portal untuk config WiFi tanpa hardcode |

---

## 🧰 Tech Stack

| Komponen | Teknologi |
|----------|-----------|
| **Mikrokontroler** | ESP32 |
| **Sensor** | DHT11 (suhu + kelembapan) |
| **Display** | OLED SSD1306 128x64 (I2C) |
| **Notifikasi** | Buzzer Pasif (GPIO 5) |
| **Backend** | Node.js + Express.js v4 |
| **Middleware** | cors, express.json |
| **Hosting** | Vercel Serverless Functions |
| **Repository** | GitHub — `Elysian-ibay/BackendDHT11_V-0.0.1` |
| **Storage (saat ini)** | In-memory Array + Object |
| **Storage (planned)** | MongoDB Atlas / Supabase |
| **Frontend (planned)** | React / Next.js |

---

## 💡 Cara Melanjutkan Proyek dengan AI

Lampirkan/paste seluruh isi file ini, lalu berikan instruksi seperti:

> *"Saya melanjutkan proyek Stasiun Cuaca Mini. Progress sudah sampai v0.0.1-update2 (backend API + command queue + live reading selesai, sudah di-deploy Vercel). ESP32 sekarang kirim data on-demand via command system. Sekarang saya ingin [INSTRUKSI]."*

**Contoh instruksi lanjutan:**
1. *"Tambahkan MongoDB Atlas sebagai database pengganti in-memory array"*
2. *"Buatkan frontend dashboard dengan React untuk visualisasi data sensor + tombol Send"*
3. *"Tambahkan API key authentication untuk keamanan"*
4. *"Buatkan endpoint remote config agar bisa ubah delay ESP32 dari dashboard"*
5. *"Tambahkan command baru selain 'send' (misal: 'reset', 'buzzer')"*

---

> 📌 **Repository:** https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1  
> 📌 **File API utama:** `api/index.js`  
> 📌 **Firmware ESP32:** `SketchArduinoIDE/DHT11-ESP32/sketch_jun5a/sketch_jun5a.ino`  
> 📌 **Dokumentasi detail:** `README.md`  
> 📌 **Panduan testing:** `USER_GUIDE_API.MD`
