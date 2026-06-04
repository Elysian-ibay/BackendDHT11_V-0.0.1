const express = require("express");
const cors = require("cors");

const app = express();

// ============================================================
//  MIDDLEWARE
// ============================================================
app.use(cors()); // Izinkan akses dari frontend/domain mana saja
app.use(express.json()); // Parsing JSON body dari ESP32

// ============================================================
//  CONSTANTS
// ============================================================
const API_VERSION = "0.0.1";

// ============================================================
//  IN-MEMORY DATA STORE
//  Catatan: Data akan hilang setiap kali server restart.
//  Untuk produksi, ganti dengan database (MongoDB, PostgreSQL, dll).
// ============================================================
let sensorData = [];
let connectedDevices = {}; // Registry perangkat ESP32 yang pernah ping/kirim data

// ============================================================
//  ROUTES
// ============================================================

/**
 * GET /api/ping
 * Endpoint untuk ESP32 mengecek apakah server API merespon.
 * ESP32 bisa hit endpoint ini saat boot untuk verifikasi koneksi.
 * Query opsional: ?device_id=ESP32_001
 *
 * Response: { success: true, message: "pong", ... }
 */
app.get("/api/ping", (req, res) => {
  const deviceId = req.query.device_id || "unknown";
  const now = new Date().toISOString();

  // Register/update device di registry
  connectedDevices[deviceId] = {
    last_ping: now,
    ip: req.ip || req.headers["x-forwarded-for"] || "unknown",
    user_agent: req.headers["user-agent"] || "unknown",
  };

  console.log(`[${now}] PING dari device: ${deviceId} (IP: ${connectedDevices[deviceId].ip})`);

  return res.status(200).json({
    success: true,
    message: "pong",
    server_time: now,
    device_id: deviceId,
    api_version: API_VERSION,
  });
});

/**
 * GET /api/devices
 * Menampilkan daftar perangkat ESP32 yang pernah terhubung.
 */
app.get("/api/devices", (req, res) => {
  const deviceList = Object.entries(connectedDevices).map(([id, info]) => ({
    device_id: id,
    ...info,
  }));

  return res.status(200).json({
    success: true,
    message: `${deviceList.length} perangkat terdaftar.`,
    data: deviceList,
  });
});

/**
 * GET /api/sensor
 * Mengambil seluruh data sensor, diurutkan dari yang terbaru.
 * Query parameter opsional:
 *   ?latest=true  → hanya mengembalikan 1 data terbaru
 *   ?limit=N      → mengembalikan N data terbaru
 */
app.get("/api/sensor", (req, res) => {
  const { latest, limit } = req.query;

  // Urutkan dari terbaru ke terlama
  const sorted = [...sensorData].reverse();

  // Jika ?latest=true, kirim hanya 1 data terbaru
  if (latest === "true") {
    if (sorted.length === 0) {
      return res.status(200).json({
        success: true,
        message: "Belum ada data sensor.",
        data: null,
      });
    }
    return res.status(200).json({
      success: true,
      message: "Data sensor terbaru berhasil diambil.",
      data: sorted[0],
    });
  }

  // Jika ?limit=N, batasi jumlah data yang dikembalikan
  if (limit) {
    const n = parseInt(limit, 10);
    if (isNaN(n) || n < 1) {
      return res.status(400).json({
        success: false,
        message: "Parameter 'limit' harus berupa angka positif.",
      });
    }
    return res.status(200).json({
      success: true,
      message: `${Math.min(n, sorted.length)} data sensor berhasil diambil.`,
      total: sorted.length,
      data: sorted.slice(0, n),
    });
  }

  // Default: kirim semua data
  return res.status(200).json({
    success: true,
    message: `${sorted.length} data sensor berhasil diambil.`,
    total: sorted.length,
    data: sorted,
  });
});

/**
 * POST /api/sensor
 * Menerima data sensor dari ESP32.
 * Body JSON yang diharapkan:
 *   { "suhu": 28.5, "kelembapan": 65.2 }
 *   atau dengan device_id:
 *   { "suhu": 28.5, "kelembapan": 65.2, "device_id": "ESP32_001" }
 *
 * Server akan menambahkan id dan timestamp secara otomatis.
 */
app.post("/api/sensor", (req, res) => {
  const { suhu, kelembapan, device_id } = req.body;

  // ---------- VALIDASI ----------
  if (suhu === undefined || kelembapan === undefined) {
    return res.status(400).json({
      success: false,
      message: "Field 'suhu' dan 'kelembapan' wajib diisi.",
    });
  }

  if (typeof suhu !== "number" || typeof kelembapan !== "number") {
    return res.status(400).json({
      success: false,
      message: "Field 'suhu' dan 'kelembapan' harus bertipe number.",
    });
  }

  // ---------- SIMPAN DATA ----------
  const now = new Date().toISOString();
  const deviceName = device_id || "unknown";

  const newEntry = {
    id: sensorData.length + 1,
    device_id: deviceName,
    suhu,
    kelembapan,
    timestamp: now,
  };

  sensorData.push(newEntry);

  // Update device registry
  connectedDevices[deviceName] = {
    last_data: now,
    ip: req.ip || req.headers["x-forwarded-for"] || "unknown",
    user_agent: req.headers["user-agent"] || "unknown",
  };

  console.log(`[${now}] Data diterima dari ${deviceName} → Suhu: ${suhu}°C | Kelembapan: ${kelembapan}%`);

  return res.status(201).json({
    success: true,
    message: "Data sensor berhasil disimpan.",
    data: newEntry,
  });
});

/**
 * DELETE /api/sensor
 * Menghapus seluruh data sensor (berguna saat testing).
 */
app.delete("/api/sensor", (req, res) => {
  const count = sensorData.length;
  sensorData = [];

  return res.status(200).json({
    success: true,
    message: `${count} data sensor berhasil dihapus.`,
  });
});

/**
 * GET /api
 * Health-check endpoint — menampilkan info server & daftar endpoint.
 */
app.get("/api", (req, res) => {
  res.status(200).json({
    success: true,
    message: "🌤️ Stasiun Cuaca Mini API is running!",
    version: API_VERSION,
    server_time: new Date().toISOString(),
    total_data: sensorData.length,
    connected_devices: Object.keys(connectedDevices).length,
    endpoints: {
      "GET    /api":                     "Health check & info server",
      "GET    /api/ping?device_id=X":    "Ping test untuk ESP32 (cek koneksi)",
      "GET    /api/devices":             "Daftar perangkat yang pernah terhubung",
      "POST   /api/sensor":              "Kirim data sensor dari ESP32",
      "GET    /api/sensor":              "Ambil semua data sensor",
      "GET    /api/sensor?latest=true":  "Ambil data sensor terbaru",
      "GET    /api/sensor?limit=N":      "Ambil N data sensor terbaru",
      "DELETE /api/sensor":              "Hapus semua data sensor",
    },
  });
});

// ============================================================
//  EXPORT untuk Vercel Serverless Functions
//  Vercel membutuhkan module.exports = app (TANPA app.listen)
// ============================================================
module.exports = app;
