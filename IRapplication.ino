#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <WiFiClient.h>

// WiFi Credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Static IP address configuration
IPAddress local_IP(192, 168, 2, 151); // Set your desired static IP here
IPAddress gateway(192, 168, 2, 254);    // Set your network gateway here
IPAddress subnet(255, 255, 255, 0);   // Set your network subnet mask here

ESP8266WebServer server(80);

// IRsend and IRrecv configuration
IRsend irsend(4);
const uint16_t kRecvPin = 14;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 15;
const uint8_t kTolerancePercentage = kTolerance;

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;

uint32_t lastReceivedCode = 0;  // Variable to store the last received IR code

void handleRoot() {
  String html = "<html>" \
                "<head><title>ESP8266 IR</title></head>" \
                "<script>" \
                "function updateCode() {" \
                "  var xhttp = new XMLHttpRequest();" \
                "  xhttp.onreadystatechange = function() {" \
                "    if (this.readyState == 4 && this.status == 200) {" \
                "      document.getElementById('irCode').innerHTML = this.responseText;" \
                "    }" \
                "  };" \
                "  xhttp.open('GET', '/get-code', true);" \
                "  xhttp.send();" \
                "}" \
                "function sendCommand(path) {" \
                "  var xhttp = new XMLHttpRequest();" \
                "  xhttp.open('GET', path, true);" \
                "  xhttp.send();" \
                "}" \
                "setInterval(updateCode, 100);" \
                "</script>" \
                "</head>" \
                "<body>" \
                  "<h1>NodeMCU IR Transmitter</h1>" \
                  "<p><a href=\"#\" onclick=\"sendCommand('/on-off')\">Send On/Off Command</a></p>" \
                  "<p><a href=\"#\" onclick=\"sendCommand('/auto')\">Send Auto Command</a></p>" \
                  "<p><a href=\"#\" onclick=\"sendCommand('/hdmi1')\">Send HDMI 1 Command</a></p>" \
                  "<p><a href=\"#\" onclick=\"sendCommand('/hdmi2')\">Send HDMI 2 Command</a></p>" \
                  "<p><a href=\"#\" onclick=\"sendCommand('/hdmi3')\">Send HDMI 3 Command</a></p>" \
                  "<p><a href=\"#\" onclick=\"sendCommand('/hdmi4')\">Send HDMI 4 Command</a></p>" \
                  "<p><a href=\"#\" onclick=\"sendCommand('/hdmi5')\">Send HDMI 5 Command</a></p>" \
                  "<h2>Live IR Signal</h2>" \
                  "<p id='irCode'>Last Received IR Code (Hex): 0x" + String(lastReceivedCode, HEX) + "<br>" \
                  "Last Received IR Code (Decimal): " + String(lastReceivedCode) + "</p>" \
                  "<p><a href=\"#\" onclick=\"sendCommand('/resend')\">Resend Last Received IR Code</a></p>" \
                  "<h3>Send Custom IR Code</h3>" \
                  "<form onsubmit=\"sendCommand('/send-custom?code=' + document.getElementById('customCode').value); return false;\">" \
                    "<input type=\"text\" id=\"customCode\" placeholder=\"Enter Decimal IR Code\">" \
                    "<input type=\"submit\" value=\"Send\">" \
                  "</form>" \
                "</body>" \
              "</html>";
  server.send(200, "text/html", html);
}

void handleGetCode() {
  String response = "Last Received IR Code (Hex): 0x" + String(lastReceivedCode, HEX) + "<br>" \
                    "Last Received IR Code (Decimal): " + String(lastReceivedCode);
  server.send(200, "text/plain", response);
}

void handleOnOff() {
  irsend.sendNEC(33441975, 32);  // Sending the On/Off IR code
  server.send(200, "text/plain", "Sent On/Off");
}

void handleAuto() {
  irsend.sendNEC(33454215, 32);  // Sending the Auto IR code
  server.send(200, "text/plain", "Sent Auto");
}

void handleHdmi1() {
  irsend.sendNEC(33464415, 32);  // Sending the HDMI 1 IR code
  server.send(200, "text/plain", "Sent HDMI 1");
}

void handleHdmi2() {
  irsend.sendNEC(33480735, 32);  // Sending the HDMI 2 IR code
  server.send(200, "text/plain", "Sent HDMI 2");
}

void handleHdmi3() {
  irsend.sendNEC(33427695, 32);  // Sending the HDMI 3 IR code
  server.send(200, "text/plain", "Sent HDMI 3");
}

void handleHdmi4() {
  irsend.sendNEC(33460335, 32);  // Sending the HDMI 4 IR code
  server.send(200, "text/plain", "Sent HDMI 4");
}

void handleHdmi5() {
  irsend.sendNEC(33478695, 32);  // Sending the HDMI 5 IR code
  server.send(200, "text/plain", "Sent HDMI 5");
}

void handleResend() {
  if (lastReceivedCode != 0) {
    irsend.sendNEC(lastReceivedCode, 32);  // Resend the last received IR code
  }
  server.send(200, "text/plain", "Resent Last Received IR Code");
}

void handleCustom() {
  if (server.hasArg("code")) {
    uint32_t customCode = server.arg("code").toInt();
    irsend.sendNEC(customCode, 32);  // Send the custom IR code
  }
  server.send(200, "text/plain", "Sent Custom IR Code");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();
  irrecv.enableIRIn();  // Initialize the IR receiver

  Serial.begin(kBaudRate);

  // Configuring static IP
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/get-code", handleGetCode);
  server.on("/on-off", handleOnOff);
  server.on("/auto", handleAuto);
  server.on("/hdmi1", handleHdmi1);
  server.on("/hdmi2", handleHdmi2);
  server.on("/hdmi3", handleHdmi3);
  server.on("/hdmi4", handleHdmi4);
  server.on("/hdmi5", handleHdmi5);
  server.on("/resend", handleResend);
  server.on("/send-custom", handleCustom);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();

  if (irrecv.decode(&results)) {
    lastReceivedCode = results.value;

    // Print the received code in hexadecimal format
    Serial.print("Code Received: 0x");
    Serial.println(lastReceivedCode, HEX);

    // Convert the hex code to decimal and print it
    Serial.print("Decimal Value: ");
    Serial.println(lastReceivedCode);

    irrecv.resume();  // Receive the next value
  }
}