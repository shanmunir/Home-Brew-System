#include <WiFi.h>  // Use <WiFi.h> for ESP32
#include <WiFiClientSecure.h>
#include <Base64.h>  // Base64 encoding library for authentication

const char* ssid = "YOUR_SSID_NAME";             // Replace with your network SSID
const char* password = "YOUR_SSID_PASSWORD";  // Replace with your network password

const char* server = "api.brewfather.app";  // Brewfather api end point
const char* apiPath = "/v2/batches";
const int port = 443;   // HTTPS port

const char* apiId = "YOUR_API_ID";
const char* apiKey = "YOUR_API_KEY";  // Replace with your Brewfather API key

// Root certificate in PEM format (example certificate)
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw"\
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU"\
"MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw"\
"MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp"\
"Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA"\
"A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo"\
"27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w"\
"Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw"\
"TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl"\
"qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH"\
"szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8"\
"Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk"\
"MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92"\
"wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p"\
"aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN"\
"VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID"\
"AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E"\
"FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb"\
"C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe"\
"QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy"\
"h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4"\
"7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J"\
"ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef"\
"MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/"\
"Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT"\
"6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ"\
"0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm"\
"2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb"\
"bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c" \
"-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(9600);
  delay(10);

  // Connect to Wi-Fi network using 
  Serial.print("\nConnecting to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Print IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Make a client request:
  ConnectToApi();
}

void ConnectToApi() {
  WiFiClientSecure client;

  // Trust all certificates (not secure, but okay for testing)
  //client.setInsecure(); // The sretInsecure() is for testing. Not recommended in production.

  // Set the root CA certificate for validation
  client.setCACert(rootCACertificate);

  // Base64 encode API key:
  String baseAuth = String(apiId) + ":" + String(apiKey);
  String encodedAuth = base64::encode(baseAuth);

  Serial.print("Connecting to Host: ");
  Serial.println(server);

  // POST request to Brewfather API
  if (client.connect(server, port)) {
    Serial.println("Connected to server");

    // Create the HTTP request manually (since we're using WiFiClientSecure)
    String postRequest = String("GET ") + 
                          "/v2/batches HTTP/1.1\r\n" + 
                          "Host: api.brewfather.app\r\n" + 
                          "Content-Type: application/json\r\n" + 
                          "Authorization: Basic " + encodedAuth + "\r\n" + 
                          "Connection: close\r\n";

    // Example JSON payload (modify as per Brewfather API requirements)
    String jsonData = "{\"name\":\"Test Brew Batch\", \"batchSize\": 20}";
    postRequest += "Content-Length: " + String(jsonData.length()) + "\r\n\r\n";
    postRequest += jsonData;

    // Send the POST request
    client.print(postRequest);

    // Wait for a response
    while (client.connected()) {
      String line = client.readStringUntil('\n');

      if (line.startsWith("HTTP/1.1")) {
        int responseCode = line.substring(9, 12).toInt();
        Serial.print("HTTP Response Code: ");
        Serial.println(responseCode);
      }

      if (line == "\r") {
        Serial.println("Headers received");
        break;
      }
    }

    // Read the rest of the response from the server
    String response = client.readString();
    Serial.println("Response: ");
    Serial.println(response);
  } else {
    Serial.println("Connection failed!");
  }

  client.stop();
}

void loop() {
  // nothing to do here
}
