
# 🌤️ Panduan Pengujian API Stasiun Cuaca Mini

Halo! Berikut adalah panduan menggunakan API untuk proyek Stasiun Cuaca Mini (ESP32 + DHT11). API ini berbasis REST dan menerima/mengirim data dalam format **JSON**.

**🌐 Base URL API:** `https://backend-dht-11-v-0-0-1.vercel.app`

---

## ⚡ CARA KERJA (v0.0.1 Update)

ESP32 **TIDAK** lagi mengirim data otomatis ke API setiap 5 detik. Sekarang:

1. **Sensor DHT11 tetap aktif** membaca suhu & kelembapan dan menampilkan di OLED
2. **ESP32 polling** ke server setiap 5 detik untuk cek apakah ada **command "send"**
3. **Jika ada command "send"** → ESP32 kirim data sensor ke `POST /api/sensor`
4. **Jika tidak ada command** → OLED menampilkan status **"Standby"**
5. **Live reading** dikirim ke `/api/sensor/live` setiap 10 detik (tidak disimpan permanen, hanya untuk monitoring real-time)

**Untuk memerintahkan ESP32 kirim data**, gunakan endpoint `POST /api/command`.

---

### 📥 1. MENGAMBIL DATA (METHOD: GET)

Gunakan *method* ini di Postman tanpa perlu mengisi tab *Body*.

**A. Mengambil Semua Data Sensor**

* **Method:** `GET`
* **URL:** `{{base_url}}/api/sensor`
* **Fungsi:** Mengambil riwayat keseluruhan data yang dikirim oleh ESP32 (diurutkan dari yang paling baru).

**B. Mengambil Data Terakhir Saja (Latest)**

* **Method:** `GET`
* **URL:** `{{base_url}}/api/sensor?latest=true`
* **Fungsi:** Sangat berguna jika kamu hanya ingin menampilkan 1 data suhu paling *update* di dashboard/frontend.

**C. Mengambil N-Data Terakhir (Limit)**

* **Method:** `GET`
* **URL:** `{{base_url}}/api/sensor?limit=5`
* **Fungsi:** Mengambil data dalam jumlah tertentu (contoh di atas akan mengambil 5 data terakhir). Angka bisa diganti sesuai kebutuhan.

---

### 📤 2. MENGIRIM DATA MANUAL / SIMULASI (METHOD: POST)

Gunakan *method* ini jika kamu ingin berpura-pura menjadi ESP32 dan mengirim data suhu secara manual ke server.

* **Method:** `POST`
* **URL:** `{{base_url}}/api/sensor`
* **Headers:** `Content-Type: application/json`
* **Body:** Pilih tab **Body** -> pilih **raw** -> ubah dropdown Text menjadi **JSON**.
* **Isi Body (Copy-Paste ini):**

```json
{
  "suhu": 28.5,
  "kelembapan": 70.2,
  "device_id": "POSTMAN_TEMAN_01"
}

```

*Klik **Send**. Jika berhasil, kamu akan mendapat response kode `201 Created` dan datamu akan tersimpan di server!*

---

### 🎮 3. PERINTAH KE ESP32 (COMMAND SYSTEM) — BARU!

Fitur ini digunakan untuk **memerintahkan ESP32 mengirim data sensor** ke API. ESP32 tidak lagi mengirim otomatis!

**A. Memerintahkan ESP32 Kirim Data**

* **Method:** `POST`
* **URL:** `{{base_url}}/api/command`
* **Headers:** `Content-Type: application/json`
* **Body:**

```json
{
  "device_id": "ESP32_ALAT_01",
  "command": "send"
}
```

* **Fungsi:** Menjadwalkan perintah "send" untuk device tertentu. ESP32 akan mengambil command ini di polling berikutnya (maksimal 5 detik) dan langsung mengirim data sensor ke `POST /api/sensor`.

> ⚠️ Command bersifat **sekali pakai**. Setelah ESP32 mengambilnya, command otomatis terhapus dari server.

**B. Mengecek Command Pending (Biasanya dipakai ESP32)**

* **Method:** `GET`
* **URL:** `{{base_url}}/api/command?device_id=ESP32_ALAT_01`
* **Fungsi:** Mengecek apakah ada command yang dijadwalkan untuk device tersebut. Response: `"command": "send"` atau `"command": "none"`.

---

### 📡 4. LIVE READING (MONITORING REAL-TIME) — BARU!

ESP32 mengirim live reading setiap 10 detik. Data ini **TIDAK disimpan permanen** — hanya di-update terus sebagai "pembacaan terkini".

**A. Melihat Live Reading Device Tertentu**

* **Method:** `GET`
* **URL:** `{{base_url}}/api/sensor/live?device_id=ESP32_ALAT_01`
* **Fungsi:** Melihat suhu & kelembapan terkini dari device tanpa harus mengirim command "send".

**B. Melihat Semua Live Reading**

* **Method:** `GET`
* **URL:** `{{base_url}}/api/sensor/live`
* **Fungsi:** Melihat live reading dari semua device yang aktif.

---

### 🛠️ 5. ENDPOINT UTILITAS LAINNYA

**A. Mengecek Status Server (Health Check)**

* **Method:** `GET`
* **URL:** `{{base_url}}/api`
* **Fungsi:** Melihat apakah server dalam keadaan hidup, mengetahui jumlah data yang tersimpan, dan melihat daftar perangkat yang terhubung.

**B. Mengecek Perangkat Terhubung (Device Registry)**

* **Method:** `GET`
* **URL:** `{{base_url}}/api/devices`
* **Fungsi:** Menampilkan perangkat mana saja yang pernah *ping* atau mengirim data ke API ini beserta alamat IP-nya.

**C. Menghapus Semua Data (Reset Data)**

* **Method:** `DELETE`
* **URL:** `{{base_url}}/api/sensor`
* **Fungsi:** Mengosongkan memori server dari semua riwayat data sensor (biasanya digunakan sebelum memulai pengujian dari awal).

---

### ⚠️ CATATAN PENTING UNTUK TESTER

Saat ini API versi 0.0.1 di- *host* menggunakan layanan **Serverless Vercel** dengan metode *In-Memory Storage*.

**Apa artinya?**
Jika server tidak menerima *request* (nganggur/sleep) selama beberapa menit, memori akan di- *reset* oleh sistem Vercel. Jadi, wajar jika tiba-tiba saat kamu melakukan `GET`, datanya kembali menjadi kosong (`[]`). Nanti di versi selanjutnya, sistem ini akan disambungkan ke Database permanen (MongoDB).

### 🔄 FLOW PENGUJIAN BARU

```
1. GET /api                     → Pastikan server hidup
2. GET /api/sensor/live         → Lihat apakah ESP32 aktif (ada live reading?)
3. POST /api/command            → Kirim perintah {"device_id":"ESP32_ALAT_01","command":"send"}
4. (Tunggu ~5 detik, ESP32 akan mengambil command dan kirim data)
5. GET /api/sensor?latest=true  → Cek data yang baru dikirim ESP32
```

Selamat mencoba di Postman! 🚀
