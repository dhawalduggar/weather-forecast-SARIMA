#include <DHT.h> // Include the DHT library
#include <ESP8266WiFi.h>

// Replace with your WiFi credentials
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// Replace with your ThingSpeak API key
String apiKey = "your_thingspeak_api_key";

#define DHTPIN 2 // DHT11 data pin connected to Arduino pin 2
#define DHTTYPE DHT11 // DHT11 sensor type

DHT dht(DHTPIN, DHTTYPE); // Initialize the DHT sensor

void setup() {
  Serial.begin(9600);
  dht.begin(); // Initialize the sensor
  WiFi.begin(ssid, password); // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  delay(2000); // Wait between measurements

  float humidity = dht.readHumidity(); // Read humidity
  float temperatureC = dht.readTemperature(); // Read temperature in Celsius
  float temperatureF = dht.readTemperature(true); // Read temperature in Fahrenheit

  if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
    Serial.println("Failed to read from DHT11 sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("% | ");
    Serial.print("Temperature (C): ");
    Serial.print(temperatureC);
    Serial.print("°C | ");
    Serial.print("Temperature (F): ");
    Serial.print(temperatureF);
    Serial.println("°F");

    // Send data to ThingSpeak
    String data = "field1=" + String(temperatureC) + "&field2=" + String(humidity);
    String url = "http://api.thingspeak.com/update?" + data + "&api_key=" + apiKey;
    Serial.println("Sending data to ThingSpeak...");
    Serial.println(url);

    // Use HTTP client to send data
    WiFiClient client;
    if (client.connect("api.thingspeak.com", 80)) {
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: api.thingspeak.com\r\n" +
                   "Connection: close\r\n\r\n");
      delay(1000);
      client.stop();
      Serial.println("Data sent successfully!");
    } else {
      Serial.println("Error sending data to ThingSpeak.");
    }
  }
}
