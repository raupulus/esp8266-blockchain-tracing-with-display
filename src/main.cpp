#include <Arduino.h>
#include <string>
#include <api.cpp>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Parámetros para el modo hibernación
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
//#define TIME_TO_SLEEP  108000    /* Time ESP32 will go to sleep (in seconds) */
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */

int bootCount = 0;

// Para https
BearSSL::WiFiClientSecure client;


// Usual address 0x27 | 0x20
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display

/*
 * Realiza la conexión al wifi en caso de no estar conectado.
 */
bool wifiConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando al WiFi..");
    WiFi.begin(AP_NAME, AP_PASSWORD);

    delay(5000);

    Serial.println(WiFi.status());

    // Si en este punto no ha conectado, puede haber un problema.
    if(WiFi.status() != WL_CONNECTED) {
      return false;
    }

    Serial.println("Se ha conectado al wifi correctamente..");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  return true;
}

/**
 * Comprueba si el wifi está conectado.
 */ 
bool wifiIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void serialLog() {
  bool wifiStatus = wifiIsConnected();
  
  if (wifiStatus) {
    Serial.println("WLAN → ON");
  } else {
    Serial.println("WLAN → OFF");
  }
}

unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}

String urldecode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}

float getAverageFromApi(String symbol) {
  auto payload = "";
  DynamicJsonDocument doc(2048);
  float average = 0.0;


  // Compruebo si está conectado a la red antes de iniciar la subida.
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Obteniendo datos de la API");
    HTTPClient http;

    //uint8_t cert[20] = {184,120,14,91,203,255,205,98,26,130,214,144,95,34,236,197,210,208,31,245};
    //client.setFingerprint(cert);
    client.setInsecure();
    //client.connect(API_DOMAIN, (String) 443);

    String endpoint = "avgPrice?symbol=" + (String)symbol;
    String url = (String)API_DOMAIN + ":" +(String)API_PORT + "/" + (String)API_PATH + "/" + endpoint;
    String urlEncode = urlencode(url);

    Serial.print("Url completa de la petición: ");
    Serial.println(url);


    http.begin(client, url);
    //http.begin(url);
    //http.addHeader("Content-Type", "application/json");
    
    if (API_TOKEN_BEARER) {
      //http.addHeader("Authorization", API_TOKEN_BEARER);
    }

    //http.addHeader("Accept", "*/*");

    // Realiza la petición a la API
    int httpCode = http.GET();

    Serial.print("Código http de respuesta:");
    Serial.println(httpCode);

    // Respuesta de la API
    auto payload = http.getString();

    Serial.print("Stream:");
    Serial.println(http.getStream());
    Serial.print("Response:");
    Serial.println(payload);

    Serial.print("Código de respuesta de la API: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
      Serial.println("Http code es mayor a 0");
      // Parse response
      deserializeJson(doc, payload);
      if (doc.size()) {
        return (float) doc["price"];
      } else {
        average = 0.0;
      }
    } else {
      average = 0.0;
    }

    // Indica que ha terminado de transmitirse.
    http.end();
  } else {
      Serial.println("No se ha conectado al WIFI, no se inicia la descarga de la API");
      average = 0.0;
  }

  return average;
}

/**
 * Search i2c devices (DEBUG)
 */ 
void scani2c() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
}

void testScreen() {
  lcd.setCursor(0,0);
  lcd.println("INITIALIZED SCREEN");
  lcd.setCursor(0,1);
  lcd.println("LOAD API DATA...");
  lcd.setCursor(0,2);
  lcd.println("BLOCKCHAIN MONITOR");
  lcd.setCursor(0,3);
  lcd.println("CREATED BY RAUL CARO");
}

void setup() {
  // Delay para prevenir posible cuelgue al despertar de hibernación.
  delay(500);


  // Initialize the lcd 
  lcd.init();
  lcd.rightToLeft();

  // Back Light ON.
  lcd.backlight();
  
  lcd.clear();


  Serial.begin(115200);
  Serial.println("ESP8266 STARTED");

  //testScreen();

  // Conectando al wifi
  wifiConnect();
}

void showDisplay(int line, float price, String name) {
  
  lcd.setCursor(0, line);
  lcd.print((String) price + (String) " " + (String) name);
  //lcd.printRight("BTC/EUR");
}

void loop() {
  //scani2c();
  delay(2000);

  Serial.println("");
  Serial.println("---------------------------------------");
  Serial.println("Comienza el loop");
  
  // Compruebo si está conectado a la red Wireless y descargo datos.
  //wifiConnect();
  
  delay(1000);

  // Media del Bitcoin
  auto btc = getAverageFromApi("BTCEUR");
  showDisplay(0, btc, "BTC/EUR");
  Serial.print("BTC:");
  Serial.println(btc);
  delay(3000);

  // Media del Ethereum
  auto eth = getAverageFromApi("ETHEUR");
  showDisplay(1, eth, "ETH/EUR");
  Serial.print("ETH:");
  Serial.println(eth);
  delay(3000);

  // Media del Ethereum
  auto ltc = getAverageFromApi("LTCEUR");
  showDisplay(2, ltc, "LTC/EUR");
  Serial.print("LTC:");
  Serial.println(ltc);
  delay(3000);

  // Media del Doge
  auto doge = getAverageFromApi("DOGEEUR");
  showDisplay(3, doge, "DOGE/EUR");
  Serial.print("DOGE:");
  Serial.println(doge);
  delay(3000);


  delay(120000);



  Serial.println("Termina el loop");
  Serial.println("---------------------------------------");
  Serial.println("");

  // Habilito y establezco hibernación para ahorrar baterías.
  ++bootCount;
  Serial.print("Contador de veces despierto: ");
  Serial.println(bootCount);
  delay(500);
  //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //esp_deep_sleep_start();
}

