/**
 * server.js — Entry point untuk development lokal.
 *
 * File ini HANYA digunakan untuk menjalankan server di localhost.
 * Saat di-deploy ke Vercel, file ini TIDAK digunakan.
 * Vercel langsung menggunakan api/index.js sebagai serverless function.
 */

const app = require("./api/index");

const PORT = process.env.PORT || 3000;

app.listen(PORT, () => {
  console.log(`\n🌤️  Stasiun Cuaca Mini API`);
  console.log(`   Server berjalan di: http://localhost:${PORT}`);
  console.log(`   Health check:       http://localhost:${PORT}/api`);
  console.log(`   Sensor endpoint:    http://localhost:${PORT}/api/sensor\n`);
});
