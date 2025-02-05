
const express = require("express");
const bodyParser = require("body-parser");
const path = require('path');

const app = express();
const port = 3000;

//configura o EJS como motor de views
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));

// Middleware para tratar JSON
app.use(bodyParser.json());

//vars para o sensor.
let sensorData = {temperature: "N/A", humidity: "N/A"};
app.post("/data", (req, res)=> {
  const {temperature, humidity} = req.body;
  if(temperature !== undefined && humidity !== undefined){
    sensorData = {temperature, humidity};
    console.log(`Dados recebidos: Temp=${temperature}°C, Umidade=${humidity}%`);
    res.send("dados recebidos com sucesso!");
  }else{
    res.status(400).send("Dados Inválidos");
  }
});

//rota para exibir a página web.
app.get("/", (req, res) => {
  res.render('index', {sensorData}); //renderiza a pag web index.ejs
});

//rota para os dados em formato json
app.get('/data.json', (req, res) => {
  res.json(sensorData);
});


app.listen(port, () => {
    console.log(`Servidor rodando em http://localhost:${port}`);
  });