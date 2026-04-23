const express = require('express');
const sqlite3 = require('sqlite3').verbose();

const app = express();
const PORT = 3000;

app.use(express.static('public'));

app.get('/api/metrics', (req, res) => {
    // Connect to the shared Docker volume database
    const db = new sqlite3.Database('/app/data/metrics.db', sqlite3.OPEN_READONLY, (err) => {
        if (err) return res.status(500).json({ error: "DB not ready" });
    });

    // Fetch the latest 10 metrics
    db.all("SELECT * FROM telemetry ORDER BY timestamp DESC LIMIT 10", [], (err, rows) => {
        if (err) {
            res.status(500).json({ error: err.message });
            return;
        }
        res.json(rows.reverse());
    });

    db.close();
});

app.listen(PORT, () => {
    console.log(`Dashboard API running on port ${PORT}`);
});