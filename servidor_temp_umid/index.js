
const express = require("express");
const bodyParser = require("body-parser");

const app = express();
const port = 3000;

// Middleware para tratar JSON
app.use(bodyParser.json());

// Rota para receber dados do ESP32
app.post("/data", (req, res) => {
  const { temperature, humidity } = req.body;
  if (temperature !== undefined && humidity !== undefined) {
    sensorData = { temperature, humidity };
    console.log(`Dados recebidos: Temp=${temperature}°C, Umidade=${humidity}%`);
    res.send("Dados recebidos com sucesso!");
  } else {
    res.status(400).send("Dados inválidos!");
  }
});

// Rota para exibir a página web
app.get("/", (req, res) => {
  res.send(`
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Monitor de Temperatura</title>
      <style>
        body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
        h1 { color: #333; }
        .data { font-size: 1.5em; margin-top: 20px; }
      </style>
    </head>
    <body>
      <h1>Monitor de Temperatura e Umidade</h1>
      <div class="data">
        <p>Temperatura: <span id="temperature">${sensorData.temperature}</span>°C</p>
        <p>Umidade: <span id="humidity">${sensorData.humidity}</span>%</p>
      </div>
      <script>
        setInterval(async () => {
          const response = await fetch("/data.json");
          const data = await response.json();
          document.getElementById("temperature").textContent = data.temperature;
          document.getElementById("humidity").textContent = data.humidity;
        }, 5000); // Atualiza a cada 5 segundos
      </script>
    </body>
    </html>
  `);
});

// Rota para enviar dados em formato JSON
app.get("/data.json", (req, res) => {
  res.json(sensorData);
});

app.listen(port, () => {
    console.log(`Servidor rodando em http://localhost:${port}`);
  });