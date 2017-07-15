#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Set the name of the node and how often it checks temp/humidty
const String node_name = "outside";
const int refresh_seconds = 30;

// Wifi info
const char* ssid = "<changeme>";
const char* password = "<changeme>";

// Authenticated/SSL ES Host (ie. elastic cloud)
const char* host = "<changeme>";
const int httpsPort = 9243;

// ES User/Pass in Base64 for basic auth
// Usually in the format user:pass
const char* userpass = "<changeme";

String sensor_json() {

  // Read the 3 values from the sensor and return json doc
  float h = dht.readHumidity();
  float c = dht.readTemperature();
  float f = dht.readTemperature(true);
  String doc = "";

  if (isnan(h) || isnan(c) || isnan(f)) {
    return doc;
  } else {
    doc = "{\"location\":\"" + node_name + "\"," +
      "\"humidity\":" + h + "," +
      "\"temp_c\":" + c + "," +
      "\"temp_f\":" + f +
      "}";
  }
  
  return doc;
}

void setup() {
Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/temperature/sensor?pipeline=autots";
  Serial.print("requesting URL: ");
  Serial.println(url);

  String doc = sensor_json();

  Serial.println("Sending Document:");
  Serial.println(doc);
  Serial.println(doc.length());

  if(doc.length() > 0) {
    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: esp8266temp\r\n" +
                 "WWW-Authenticate: Basic realm=\"security\"\r\n" +
                 "Authorization: Basic " + userpass + "\r\n" +
                 "Connection: close\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + doc.length() + "\r\n\r\n" +
                 doc + "\r\n");
  
    //Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    String line = client.readStringUntil('\n');
    Serial.println("reply was:");
    Serial.println(line);
    delay(refresh_seconds * 1000);
  } else {
    delay(1000);
  }
}
