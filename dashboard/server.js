const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const app = express();
const PORT = 3000;
app.use(express.static('public'));

app.get('/api/sysinfo', (req, res) => {
    const db = new sqlite3.Database('/app/data/metrics.db', sqlite3.OPEN_READONLY);
    db.get("SELECT * FROM sysinfo WHERE id = 1", (err, row) => {
        if (err) res.status(500).json({ error: err.message });
        else res.json(row || {});
        db.close();
    });
});

app.get('/api/metrics', (req, res) => {
    const db = new sqlite3.Database('/app/data/metrics.db', sqlite3.OPEN_READONLY);
    db.all("SELECT * FROM telemetry ORDER BY timestamp DESC LIMIT 25", [], (err, rows) => {
        if (err) res.status(500).json({ error: err.message });
        else res.json(rows.reverse());
        db.close();
    });
});

app.get('/api/stats', (req, res) => {
    const db = new sqlite3.Database('/app/data/metrics.db', sqlite3.OPEN_READONLY);
    // SQL query to calculate max and averages over the last 60 seconds
    const query = `
        SELECT 
            MAX(gpu_temp) as max_temp, ROUND(AVG(gpu_temp), 1) as avg_temp,
            MAX(gpu_util) as max_gutil, ROUND(AVG(gpu_util), 1) as avg_gutil,
            MAX(vram_usage) as max_vram, ROUND(AVG(vram_usage), 1) as avg_vram,
            MAX(cpu) as max_cpu, ROUND(AVG(cpu), 1) as avg_cpu,
            MAX(ram) as max_ram, ROUND(AVG(ram), 1) as avg_ram
        FROM telemetry 
        WHERE timestamp >= (strftime('%s','now') - 60)
    `;
    db.get(query, [], (err, row) => {
        if (err) res.status(500).json({ error: err.message });
        else res.json(row || {});
        db.close();
    });
});

app.listen(PORT, () => console.log(`Dashboard API running on port ${PORT}`));
