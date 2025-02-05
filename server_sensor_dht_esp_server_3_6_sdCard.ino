/*
  * Medidor de temperatura e Umidade, enviando dados para um servidor em Node.js V 3.6.1
  * Informações de IP, porta, rota, SSID e senha alimentados dinâmicamente via Arq. txt do SD card
  * Bruno Patrocinio
*/  
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>

// Pinos sensor temp e umid
#define DHTPIN 27      // pino onde o sensor está conectado
#define DHTTYPE DHT11  // tipo de sensor (se nao funcionar, colocar DHT22)

const int chipSelect = 5; // pino do chipSelect SD Card

//var global inicial de IP. (pra ser dinâmica)
char protocolo[8] = "http://";
char sinal[2] = ":";
char porta[7] = "";
char rota[15] = "";

char ip[16];
char* urlServ[] = { protocolo, ip, sinal, porta, rota };

char resultado[50] = "";

bool cardOk = true; //var de controle sd card

LiquidCrystal_I2C lcd(0x27, 16, 2);  //cria o objeto lcd passando como parâmetros o endereço, o nº de colunas e o nº de linhas

float temperature = 0.0f;
float humidity = 0.0f;
SemaphoreHandle_t dataSemaphore;

// obj do sensor
DHT dht(DHTPIN, DHTTYPE);

// Configuração Wi-Fi Nome da Rede e senha
char *ssid = new char[11];
char *password = new char[20];

String line1, line2, line3, line4, line5;
int lineCount = 0;
const char* fileName = "/data.txt"; //nome do arquivo na raiz do cartão

void setup() {
  Serial.begin(38400);
  dht.begin();
  //seta as confg do display 16x2
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.print("Iniciando...");
  delay(1000);
  lcd.clear();

  lcd.println("Lendo SD-card...");
  if(!SD.begin(chipSelect)){
    Serial.println("Erro na leitura do SD card, não existe um SD card ou módulo não conectado.");
    lcd.clear();
    lcd.println("Erro leitura.");
    cardOk = false;
    return;
  }
  delay(2000);
  lcd.clear();
  lcd.println("Leu SD-card.");
  delay(2000);
  lcd.clear();
  carregaDados();
  // Conecta ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi...");
  delay(1000);
  lcd.clear();
  lcd.print("Conectando ao WiFi...");
  lcd.clear();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    lcd.print(".");
    lcd.clear();
  }
  Serial.println("\nConectado ao WiFi!");
  lcd.clear();
  lcd.println("Conectou ao WiFi");
  delay(2000);
  lcd.clear();

  // Criação do semáforo
  dataSemaphore = xSemaphoreCreateMutex();
  if (dataSemaphore == NULL) {
    Serial.println("Erro ao criar o semáforo!");
    while (1)
      ;
  }
  // Criação das tarefas de obter os dados do sensor e enviar para o servidor em node.js, usando freeRTOS
  xTaskCreatePinnedToCore(taskGetData, "GetDataTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskSendData, "SendDataTask", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // O Sist. op. do Free RTOS que gerencia as tarefas....não precisa de loop
}

void taskGetData(void* parameter) {
  while (true) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("Falha ao ler do sensor DHT!");
    } else {
      // protege o acesso as variáveis globais
      if (xSemaphoreTake(dataSemaphore, portMAX_DELAY)) {
        temperature = temp;
        humidity = hum;
        xSemaphoreGive(dataSemaphore);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1000));  // Aguarda 1 segundo
  }
}  //fim de taskGetData()

void taskSendData(void* parameter) {
  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      // protege o acesso as variáveis globais
      if (xSemaphoreTake(dataSemaphore, portMAX_DELAY)) {
        float temp = temperature;
        float hum = humidity;
        xSemaphoreGive(dataSemaphore);

        HTTPClient http;
        http.begin(resultado); //passa a url completada
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = String("{\"temperature\":") + temp + ",\"humidity\":" + hum + "}";
        int httpResponseCode = http.POST(jsonPayload);
        lcd.clear();
        lcd.print("Temp: ");lcd.print(temp);lcd.print(" c");
        lcd.setCursor(0, 1);
        lcd.print("Umid: ");lcd.print(hum);

        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println("Resposta do servidor: " + response);
        } else {
          Serial.println("Erro ao enviar dados: " + String(httpResponseCode));
        }
        http.end();
      }
    } else {
      Serial.println("Wi-Fi desconectado!");
    }

    vTaskDelay(pdMS_TO_TICKS(5000));  // aguarda 5 segundos antes de enviar novamente
  }
}  // fim de taskSendData()

void carregaDados(){
  char aux[6];
  File file = SD.open(fileName);
  if(!file){
    Serial.println("Erro ao abrir arquivo txt");
    lcd.clear();
    lcd.print("Erro Arq.");
    return;
  }
  while(file.available() && lineCount < 5){
    String line = file.readStringUntil('\n');
    line.trim();
    switch(lineCount){
      case 0: line1 = line; break;
      case 1: line2 = line; break;
      case 2: line3 = line; break;
      case 3: line4 = line; break;
      case 4: line5 = line; break;
    }
    lineCount++;
  }
  file.close(); // fecha o arquivo depois de usar
  //prints na serial só pra verificar
  Serial.print("Linha 1: ");Serial.println(line1);
  Serial.print("Linha 2: ");Serial.println(line2);
  Serial.print("Linha 3: ");Serial.println(line3);
  Serial.print("Linha 4: ");Serial.println(line4);
  Serial.print("Linha 5: ");Serial.println(line5);
  //copia para os ponteiros char e vetores de char as strings
  strcpy(ssid, line1.c_str());
  strcpy(password, line2.c_str());
  strcpy(ip, line3.c_str());
  strcpy(porta, line4.c_str());
  strcpy(rota, line5.c_str());
  //responsável por montar a URL pra enviar
  int n = sizeof(urlServ) / sizeof(urlServ[0]);
  for (int i = 0; i < n; i++) {
       strcat(resultado, urlServ[i]);  //garante aqui que seja inserido o IP digitado na URL passada para a requisição
  }
}//fim de carregaDados()