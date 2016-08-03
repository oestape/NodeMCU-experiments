#include <ESP8266WiFi.h>

const char* ssid = "NODEMCU";
const char* password = "1234";

int ledPin = 13; // GPIO13 -> Mapped to NodeMCU D7
WiFiServer server(80);

int ledValue = LOW;
int servoPos = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  //uncomment only one of those:
  //setupWifiRouter(); //connects to a router. You need to use the serial output to know the IP it has been assigned
  setupWifiAP(); //acts as an Access Point (no need for external routers) connect to http://192.168.4.1
}

void setupWifiRouter()
{
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void setupWifiAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  client.setNoDelay(false);

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request

int pos = 0;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, HIGH);
    ledValue = HIGH;
  } else if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, LOW);
    ledValue = LOW;
  } else if ((pos = request.indexOf("/SER=")) != -1) {
    //TODO
  } else if (request.indexOf("mystyle.css") != -1) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/css");
    client.println("Expires: Sat, 05 Apr 2025 00:00:00 GMT"); //we don't want to resend the css every time
    client.println(""); //  do not forget this one
    printCSS(client);
    return;

  }
  else { //many browsers request favicon.ico for example
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("-");

    return;
  }

  // Set ledPin according to the request
  //digitalWrite(ledPin, ledValue);

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one


  client.print("<!DOCTYPE HTML>");
  client.print("<html><head>");
  //client.print("<style>body {zoom: 3}</style>");
  //printCSS(client);
  client.print("<link rel=\"stylesheet\" type=\"text/css\" href=\"mystyle.css\">");
  client.print("</head><body>");

  client.print("<br><br>");
  client.print("<a href=\"/LED=ON\"");
  if (ledValue == HIGH) client.print(" class=\"bp\"");
  client.print(">ON</a> ");
  client.print("<a href=\"/LED=OFF\"");
  if (ledValue == LOW) client.print(" class=\"bp\"");
  client.print(">OFF</a> ");
  client.print("</body></html>");

  client.print("<br><br>");
  for (int i = 0; i < 9; i++)
  {
    client.print("<a href=\"/SER=");
    client.print((i - 4) * 45);
    client.print("\"");
    client.print(">");
    client.print((i - 4) * 45);
    client.print("</a> ");
  }

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}

void printCSS(WiFiClient client)
{
  //client.print("<style>");

  client.print(""
               //"body {zoom: 3}"
               "a {"
               "display: inline-block;"
               "padding: 15px 25px;"
               "font-size: 24px;"
               "cursor: pointer;"
               "text-align: center;"
               "text-decoration: none;"
               "outline: none;"
               "color: #fff;"
               "background-color: #4064bb;"
               "border: none;"
               "border-radius: 15px;"
               "box-shadow: 0 9px #999;"
               "font-family: Sans-Serif;"
               "}"

               "a:hover {background-color: #2c4480}"

               "a:active {"
               "background-color: #5a7ac7;"
               "box-shadow: 0 0px #666;"
               "transform: translateY(9px);"
               "}"

               ".bp {"
               "color: #000;"
               "background-color: #ffdb00;"
               "box-shadow: 0 3px #999;"
               "transform: translateY(6px);"
               "}"

               ".bp:hover {"
               "background-color: #cbae00;"
               "}"
               ".bp:active {"
               "background-color: #ffdb00;"
               "box-shadow: 0 0px #666;"
               "transform: translateY(9px);"
               "}");


  //client.print("</style>");
}

