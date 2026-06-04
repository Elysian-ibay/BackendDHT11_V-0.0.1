# 📋 PROJECT SUMMARY — Stasiun Cuaca Mini API

> **Dokumen ini dibuat agar bisa dilampirkan ke AI (Gemini/ChatGPT) untuk melanjutkan progress proyek.**  
> **Terakhir diupdate:** 2026-06-05  
> **Versi saat ini:** v0.0.1  
> **Repository:** https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1

---

## 🎯 Deskripsi Proyek

Proyek IoT **"Stasiun Cuaca Mini"** menggunakan **ESP32 + sensor DHT11** untuk membaca suhu dan kelembapan, kemudian mengirim data tersebut ke **REST API Backend** yang dibangun dengan **Node.js + Express.js**.

API ini di-deploy sebagai **Vercel Serverless Function**.

---

## 🏗️ Arsitektur Sistem

```
  ESP32 + DHT11
       │
       │ 1. GET /api/ping (cek koneksi saat boot)
       │ 2. POST /api/sensor (kirim data berkala)
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
  │  Frontend / Dashboard        │
  │  (Belum dibuat — Planned)    │
  └──────────────────────────────┘
```

---

## 📁 Struktur Folder

```
BackendDHT11_V-0.0.1/
├── api/
│   └── index.js          ← Express app utama (Vercel entry point)
├── server.js             ← Local dev server (app.listen terpisah)
├── package.json          ← Dependencies: express, cors
├── vercel.json           ← Routing config Vercel
├── .gitignore
├── README.md             ← Dokumentasi API lengkap
└── DOCS_SUMMARY.md       ← File ini
```

---

## 📡 Daftar Endpoint API (v0.0.1)

| # | Method | Endpoint | Fungsi | Status |
|---|--------|----------|--------|--------|
| 1 | `GET` | `/api` | Health check, info server, versi API | ✅ Done |
| 2 | `GET` | `/api/ping?device_id=X` | Ping/heartbeat — ESP32 cek koneksi ke server | ✅ Done |
| 3 | `GET` | `/api/devices` | Daftar perangkat ESP32 yang pernah terhubung | ✅ Done |
| 4 | `POST` | `/api/sensor` | Terima data `{suhu, kelembapan, device_id}` dari ESP32 | ✅ Done |
| 5 | `GET` | `/api/sensor` | Ambil semua data sensor (terbaru duluan) | ✅ Done |
| 6 | `GET` | `/api/sensor?latest=true` | Ambil 1 data sensor paling baru | ✅ Done |
| 7 | `GET` | `/api/sensor?limit=N` | Ambil N data sensor terbaru | ✅ Done |
| 8 | `DELETE` | `/api/sensor` | Hapus semua data sensor (untuk testing) | ✅ Done |

---

## 📦 Payload JSON

### ESP32 → API (POST /api/sensor)
```json
{
  "suhu": 28.5,
  "kelembapan": 65.2,
  "device_id": "ESP32_001"
}
```

### API → Response (201)
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
| **WiFi Config** | **Hardcoded** di firmware ESP32 | ESP32 harus konek WiFi dulu sebelum bisa akses API (chicken-and-egg). Remote config via API tidak possible untuk WiFi. |
| **Delay / Config lain** | **Hardcoded** di firmware ESP32 | Untuk v0.0.1 disederhanakan. Remote config planned di versi mendatang. |
| **Version Control** | GitHub repo + Vercel auto-deploy | Push ke GitHub → Vercel auto-deploy |

---

## 🔧 Konfigurasi ESP32 (Hardcoded)

Semua konfigurasi ESP32 di-hardcode langsung di firmware. Untuk mengubah, harus edit kode lalu re-flash ke board.

| Konfigurasi | Default | Cara Ubah |
|-------------|---------|-----------|
| WiFi SSID | `"NAMA_WIFI_KAMU"` | Edit kode → re-flash |
| WiFi Password | `"PASSWORD_WIFI_KAMU"` | Edit kode → re-flash |
| Server URL | `"http://192.168.1.x:3000"` | Edit kode → re-flash |
| Device ID | `"ESP32_001"` | Edit kode → re-flash |
| Delay interval | `10000` ms (10 detik) | Edit `DELAY_MS` → re-flash |
| Sensor pin | GPIO `4` | Edit `DHTPIN` → re-flash |
| Sensor type | `DHT11` | Edit `DHTTYPE` → re-flash |

---

## ✅ Yang Sudah Selesai (Progress)

### Backend API
- [x] Inisialisasi proyek Node.js + Express.js
- [x] Middleware: cors + express.json
- [x] `POST /api/sensor` — Terima data dari ESP32 (suhu, kelembapan, device_id)
- [x] `GET /api/sensor` — Ambil data (semua / ?latest=true / ?limit=N)
- [x] `DELETE /api/sensor` — Hapus semua data testing
- [x] `GET /api/ping` — Heartbeat/ping test untuk ESP32
- [x] `GET /api/devices` — Daftar perangkat yang pernah terhubung
- [x] `GET /api` — Health check + versi + info server
- [x] Input validation (field wajib + tipe data)
- [x] Struktur Vercel-compatible (api/index.js + server.js terpisah)
- [x] vercel.json routing config
- [x] Testing semua endpoint lokal berhasil ✅

### Dokumentasi
- [x] README.md lengkap (endpoint, Postman guide, ESP32 code, deploy steps)
- [x] DOCS_SUMMARY.md (file ini — untuk AI lanjutan)
- [x] Changelog v0.0.1

### Kode ESP32 (Arduino IDE)
- [x] Kode firmware ESP32 + DHT11 sudah ditulis (ada di README.md)
- [x] Fungsi `pingServer()` — cek koneksi saat boot
- [x] Fungsi `kirimData()` — POST data sensor berkala
- [x] Konfigurasi hardcoded (SSID, password, URL, delay)
- [ ] Belum di-flash ke board ESP32 (perlu dilakukan secara manual)

### Deployment
- [x] Repository GitHub: https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1
- [ ] Push ke GitHub (langkah-langkah sudah disiapkan)
- [ ] Deploy ke Vercel (langkah-langkah sudah disiapkan)

---

## 🔲 Yang Belum Dibuat (TODO / Next Steps)

| Prioritas | Task | Keterangan |
|-----------|------|------------|
| 🔴 High | **Database Integration** | Ganti in-memory array → MongoDB Atlas / Supabase. Data saat ini hilang setiap cold-start Vercel. |
| 🔴 High | **Deploy ke Vercel** | Push ke GitHub → import di Vercel → auto-deploy |
| 🟡 Medium | **API Key Authentication** | Tambah header `X-API-Key` agar endpoint tidak bisa diakses sembarang orang |
| 🟡 Medium | **Rate Limiting** | Batasi request per menit per IP/device |
| 🟡 Medium | **Remote Config** | Endpoint `GET/PUT /api/config/:device_id` untuk ubah delay/settings ESP32 tanpa re-flash |
| 🟢 Low | **Data Agregasi** | Endpoint rata-rata suhu/kelembapan per jam/hari |
| 🟢 Low | **Frontend Dashboard** | Web dashboard React/Next.js untuk visualisasi data real-time |
| 🟢 Low | **WebSocket** | Real-time push ke dashboard tanpa polling |
| 🟢 Low | **WiFi Manager** | ESP32 AP Mode + Captive Portal untuk config WiFi tanpa hardcode |

---

## 🧰 Tech Stack

| Komponen | Teknologi |
|----------|-----------|
| **Mikrokontroler** | ESP32 |
| **Sensor** | DHT11 (suhu + kelembapan) |
| **Backend** | Node.js + Express.js v4 |
| **Middleware** | cors, express.json |
| **Hosting** | Vercel Serverless Functions |
| **Repository** | GitHub — `Elysian-ibay/BackendDHT11_V-0.0.1` |
| **Storage (saat ini)** | In-memory Array |
| **Storage (planned)** | MongoDB Atlas / Supabase |
| **Frontend (planned)** | React / Next.js |

---

## 💡 Cara Melanjutkan Proyek dengan AI

Lampirkan/paste seluruh isi file ini, lalu berikan instruksi seperti:

> *"Saya melanjutkan proyek Stasiun Cuaca Mini. Progress sudah sampai v0.0.1 (backend API selesai, in-memory storage, sudah di-deploy Vercel). Sekarang saya ingin [INSTRUKSI]."*

**Contoh instruksi lanjutan:**
1. *"Tambahkan MongoDB Atlas sebagai database pengganti in-memory array"*
2. *"Buatkan frontend dashboard dengan React untuk visualisasi data sensor"*
3. *"Tambahkan API key authentication untuk keamanan"*
4. *"Buatkan endpoint remote config agar bisa ubah delay ESP32 dari dashboard"*
5. *"Tambahkan endpoint data agregasi (rata-rata suhu per jam)"*

---

> 📌 **Repository:** https://github.com/Elysian-ibay/BackendDHT11_V-0.0.1  
> 📌 **File API utama:** `api/index.js`  
> 📌 **Dokumentasi detail:** `README.md`
