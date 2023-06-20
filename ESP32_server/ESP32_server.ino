#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
HardwareSerial SerialPort(0);

//Web page String with backwards slash (\) skipped and using single quotes (') instead of double (")
const char* webpage = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><link rel=\"stylesheet\"href=\"https://fonts.googleapis.com/css2?family=Bree+Serif\"><title>Auto Tool</title><style>* {user-select: none;-webkit-tap-highlight-color: rgba(255, 255, 255, 0) !important;-webkit-focus-ring-color: rgba(255, 255, 255, 0) !important;outline: none !important;}body {background-color: #1d1c21;color: #EEEEEE;font-family: Arial, Helvetica, sans-serif;margin: 0;}header {display: flex;justify-content: space-between;align-items: center;padding: 10px;}header svg {height: 50px;width: 50px;}header h1 {margin-right: auto;margin-left: auto;position: absolute;left: 50%;transform: translateX(-50%);font-family: \"Bree Serif\", sans-serif;font-weight: 400;}.battery-container {position: relative;display: flex;width: 30px;height: 15px;background-color: transparent;border: 2px solid #BBD6B8;border-radius: 5px;align-items: center;padding: 5px;}.battery-level {height: 100%;width: 30%;border-radius: 5px;background-color: #BBD6B8;}.battery-text {position: absolute;left: 50%;transform: translateX(-50%);font-family: Arial, Helvetica, sans-serif;color: white;font-size: 10px;mix-blend-mode: difference;}main {display: flex;align-items: flex-start;justify-items: center;flex-wrap: wrap;padding: 10px;}main .select-t {flex-grow: 1;display: flex;flex-direction: column;align-items: center;padding: 10px;}.tool-box {display: flex;flex-direction: column;color: #1d1c21;border-radius: 20px;width: 100%;height: 30vh;padding: 5px;}main .select-t > div {background-color: #c0dedd;}.sub-btn {margin: 10px;border: 0;padding: 5px;text-decoration: none;cursor: pointer;border-radius: 10px;}main .select-t > input {background-color: #F7C8E0;}main .select-t > input:active {border: 2px solid #F7C8E0;}main .current-t {flex-grow: 1;display: flex;flex-direction: column;align-items: center;padding: 10px;}main .current-t > div {background-color: #F7C8E0;}main .current-t > input {background-color: #c0dedd;}main .current-t > input:active {border: 2px solid #c0dedd;}main .station-dir {width: 100%;display: flex;flex-direction: column;align-items: center;}.disabled label{color: #950101;text-decoration: line-through;}#stat-num {border-radius: 10px;padding: 4px;input::-webkit-outer-spin-button,input::-webkit-inner-spin-button {-webkit-appearance: none;margin: 0;}}#rqst-btn {background-color: white;}#rqst-btn:active {border: 2px solid white;}.sub-btn:active {margin: 8px;background-color: transparent !important;color: #EEEEEE;outline: none !important;}</style></head><body><header><svg   width=\"44.928436mm\"   height=\"39.936905mm\"   viewBox=\"0 0 44.928436 39.936905\"   version=\"1.1\"   id=\"svg5\"   xml:space=\"preserve\"   xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"   xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"   xmlns=\"http://www.w3.org/2000/svg\"   xmlns:svg=\"http://www.w3.org/2000/svg\"><sodipodi:namedview     id=\"namedview7\"     pagecolor=\"#505050\"     bordercolor=\"#ffffff\"     borderopacity=\"1\"     inkscape:showpageshadow=\"0\"     inkscape:pageopacity=\"0\"     inkscape:pagecheckerboard=\"1\"     inkscape:deskcolor=\"#505050\"     inkscape:document-units=\"mm\"     showgrid=\"false\"     inkscape:zoom=\"1.1893044\"     inkscape:cx=\"338.01271\"     inkscape:cy=\"409.48306\"     inkscape:window-width=\"1920\"     inkscape:window-height=\"1009\"     inkscape:window-x=\"-8\"     inkscape:window-y=\"-8\"     inkscape:window-maximized=\"1\"     inkscape:current-layer=\"layer1\" /><defs     id=\"defs2\" /><g     inkscape:label=\"Layer 1\"     inkscape:groupmode=\"layer\"     id=\"layer1\"     transform=\"translate(-94.604937,-69.019461)\"><path       style=\"fill:#f7c8e0;fill-opacity:1;stroke:none;stroke-width:0.264583px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"       d=\"M 124.52701,108.84295 H 94.604931 l 4.115676,-7.11901 h 15.850913 l 2.83648,-5.116787 z\"       id=\"path2418\" /><path       style=\"fill:#c0dedd;fill-opacity:1;stroke:none;stroke-width:0.264583px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"       d=\"m 110.47047,96.391287 h -9.16327 l 15.55396,-27.371823 22.67221,39.936906 H 130.5864 L 116.93982,84.888042 Z\"       id=\"path2420\" /></g></svg><h1>Auto-Tools</h1><div><div id=\"battery-container\" class=\"battery-container\"><div id=\"battery-level\" class=\"battery-level\"></div><div id=\"battery-text\" class=\"battery-text\"></div><div class=\"battery-unavailable\"></div></div></div></header><main><div class=\"select-t\"><h2>Select Tools</h2><div id=\"select-tools\" class=\"tool-box\"></div><input class=\"sub-btn\" type=\"submit\" value=\"Add Tools\" onclick=\"addTools()\"></div><div class=\"current-t\"><h2>Current Tools</h2><div id=\"current-tools\" class=\"tool-box\"></div><input class=\"sub-btn\" type=\"submit\" value=\"Return\" onclick=\"returnTools()\"></div><div class=\"station-dir\"><h2>Station Number</h2><input id=\"stat-num\" type=\"number\"><input id=\"rqst-btn\" class=\"sub-btn\" type=\"submit\" value=\"Request\" onclick=\"requestTools()\"></div></main><script>var dataJSON = {};function toolWriter(){var output = \"\";for(i = 0; i < dataJSON.tools.length; i++){if(dataJSON.tools[i].available){output += \"<div><input type=\\\"checkbox\\\" id=\\\"\"+dataJSON.tools[i].id+\"\\\" value=\\\"\"+dataJSON.tools[i].id+\"\\\"><label for=\\\"\"+dataJSON.tools[i].id+\"\\\">\"+dataJSON.tools[i].name+\"</label></div>\";}else {output += \"<div class=\\\"disabled\\\"><input disabled=\\\"disabled\\\" type=\\\"checkbox\\\" id=\\\"\"+dataJSON.tools[i].id+\"\\\" value=\\\"\"+dataJSON.tools[i].id+\"\\\"><label for=\\\"\"+dataJSON.tools[i].id+\"\\\">\"+dataJSON.tools[i].name+\"</label></div>\";}}if(output != document.getElementById(\"select-tools\").innerHTML){document.getElementById(\"select-tools\").innerHTML = \"\";document.getElementById(\"select-tools\").innerHTML = output;document.getElementById(\"current-tools\").innerHTML = \"\";}}var batteryLevel = 0;var batteryLevelElement = document.getElementById('battery-level');var batteryTextElement = document.getElementById('battery-text');setInterval(main, 1);function main(){if(dataJSON.batteryVoltage < 9){batteryLevelElement.style.display = \"none\";document.getElementById(\"battery-container\").style.borderColor = \"red\";batteryTextElement.textContent = \"X\";batteryTextElement.style.color = \"red\";}else if (dataJSON.batteryVoltage >= 9 && dataJSON.batteryVoltage <= 11.1){document.getElementById(\"battery-level\").style.display = \"block\";document.getElementById(\"battery-container\").style.borderColor = \"#BBD6B8\";batteryTextElement.textContent = Math.round(((dataJSON.batteryVoltage - 9) * 47.61)) + \"%\";batteryTextElement.style.color = \"white\";batteryLevelElement.style.width = Math.round(((dataJSON.batteryVoltage - 9) * 47.61)) + '%';}else if (dataJSON.batteryVoltage > 11.1){batteryAnimation();}}function batteryAnimation(){if(batteryLevel < 100){document.getElementById(\"battery-level\").style.display = \"block\";document.getElementById(\"battery-container\").style.borderColor = \"#BBD6B8\";batteryLevel += 1;batteryLevelElement.style.width = batteryLevel + '%';batteryTextElement.textContent = \"\";}else {batteryLevel = 0;}}var Socket;function init() {Socket = new WebSocket(\"ws://\" + window.location.hostname + \":81/\");Socket.onmessage = function(event) {processCommand(event);};}function processCommand(event) {dataJSON = JSON.parse(event.data);toolWriter();}function addTools() {output = \"\";for(i = 0; i < dataJSON.tools.length; i++) {if(document.getElementById(dataJSON.tools[i].id).checked && dataJSON.tools[i].available){output += \"<li id=\\\"AT\"+dataJSON.tools[i].id+\"\\\">\"+dataJSON.tools[i].name+\"</li>\"}document.getElementById(dataJSON.tools[i].id).checked = false;}document.getElementById(\"current-tools\").innerHTML = \"\";document.getElementById(\"current-tools\").innerHTML = \"<ul id=\\\"ul-tools\\\">\"+output+\"</ul>\";}function returnTools() {document.getElementById(\"current-tools\").innerHTML = \"\";for(i = 0; i < dataJSON.tools.length; i++){document.getElementById(dataJSON.tools[i].id).checked = false;}}function requestTools(){sendJSON = {stationID : {path : document.getElementById(\"stat-num\").value,username : \"Jose\"},tools : []};for(i = 0; i < document.getElementById(\"ul-tools\").getElementsByTagName(\"li\").length; i++){sendJSON.tools[i] = {id : document.getElementById(\"ul-tools\").getElementsByTagName(\"li\")[i].getAttribute(\"id\").slice(2)};}if(sendJSON.stationID.path >= 0 && sendJSON.stationID.path.length <= 8){console.log(sendJSON);Socket.send(JSON.stringify(sendJSON));}else {alert(\"Invalid path\");}}window.onload = function(event) {init();}</script></body></html>";

//Defines the JSON transimitt template in a char array 
const char* JSON_tx_st = "{\"tools\" : [{\"id\":\"00000001\",\"name\":\"Screwdriver\",\"available\":false,\"usedBy\":\"Mario\"},{\"id\":\"00000002\",\"name\":\"Hammer\",\"available\":true,\"usedBy\":null},{\"id\":\"00000003\",\"name\":\"Screwdriver\",\"available\":true,\"usedBy\":null},{\"id\":\"00000004\",\"name\":\"Nails\",\"available\":false,\"usedBy\":\"José\"}],\"batteryVoltage\" : 0}";
StaticJsonDocument<512> JSON_tx; //Defines the JSON object with a memory of 512

const char* JSON_rx_st = "";
StaticJsonDocument<512> JSON_rx;

WebServer server(80); //defines a webserver object called server in port 80
WebSocketsServer webSocket = WebSocketsServer(81); //defines a WebSocketsServer object called webSocket in port 81

const int btn_tool_1 = 5; //Defines the pin used for tool 1 PULL-UP
const int btn_tool_2 = 19;//Defines the pin used for tool 2 PULL-UP
const int btn_tool_3 = 18;//Defines the pin used for tool 3 PULL-UP
const int btn_tool_4 = 23;//Defines the pin used for tool 4 PULL-UP

const int BATTERY_VOLTAGE_PIN = 36;
float BATTERY_VOLTAGE;
int BATTERY_STATUS;

void setup() {
  Serial.begin(115200);//Initialize serial commuinications with PC at 115200 baud
  SerialPort.begin(115200, SERIAL_8N1, 16, 17);

  jsonTxSetup();//Deserialzie the transmit JSON string in to the JSON object

  wifiAPSetup();//Select either wifiSetup() or wifiAPSetuo() for broadcasting or connecting to a wifi signal    **WiFi manager pending for WiFi connection
  serverSetup();//Defines what to do when a client connects, intializes the webServer and the webSocketsServer

  pinMode(btn_tool_1, INPUT_PULLUP);//Tool 1 input PULL-UP, switch must be connected to GND
  pinMode(btn_tool_2, INPUT_PULLUP);//Tool 2 input PULL-UP, switch must be connected to GND
  pinMode(btn_tool_3, INPUT_PULLUP);//Tool 3 input PULL-UP, switch must be connected to GND
  pinMode(btn_tool_4, INPUT_PULLUP);//Tool 4 input PULL-UP, switch must be connected to GND

  pinMode(BATTERY_VOLTAGE_PIN, INPUT);
  
}

void loop() {
  server.handleClient();//handles client every loop
  webSocket.loop();//handles webSocket requests every loop
  
  batteryCheck();

  toolJSONWriter();
}
void serverSetup() {
  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();

  webSocket.onEvent(webSocketEvent);
}
void wifiAPSetup() {
  const char* ssid = "Auto Tools";
  const char* password = "tpassword";

  Serial.println("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}
void wifiSetup() {
  const char* ssid = "Tienda el Puerto";
  const char* password = "mayalaikahormiga";

  WiFi.begin(ssid, password);
  Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));
 
  while (WiFi.status() != WL_CONNECTED) {  
    delay(1000);
    Serial.print(".");
  }
  Serial.print("Connected to network with IP address: ");
  Serial.println(WiFi.localIP()); 
}

void jsonTxSetup() {

  DeserializationError error = deserializeJson(JSON_tx, JSON_tx_st);

  if(error) {
    Serial.print("ERROR: ");
    Serial.println(error.c_str());
  }
}
bool btn_previous_1 = !digitalRead(btn_tool_1);
bool btn_previous_2 = !digitalRead(btn_tool_2);
bool btn_previous_3 = !digitalRead(btn_tool_3);
bool btn_previous_4 = !digitalRead(btn_tool_4);

void toolJSONWriter() {
  JSON_tx["tools"][0]["available"] = !digitalRead(btn_tool_1);
  JSON_tx["tools"][1]["available"] = !digitalRead(btn_tool_2);
  JSON_tx["tools"][2]["available"] = !digitalRead(btn_tool_3);
  JSON_tx["tools"][3]["available"] = !digitalRead(btn_tool_4);

  JSON_tx["batteryVoltage"] = BATTERY_VOLTAGE;

  String JSON_tx_out = "";
  serializeJson(JSON_tx, JSON_tx_out);

  webSocket.broadcastTXT(JSON_tx_out);
}

void batteryCheck(){
  BATTERY_VOLTAGE = analogRead(BATTERY_VOLTAGE_PIN) * 0.003174;
  if (BATTERY_VOLTAGE < 9){
    BATTERY_STATUS = 0;
  }
  else if (BATTERY_VOLTAGE >= 9 && BATTERY_VOLTAGE <= 11.1){
    BATTERY_STATUS = 1;
  }
  else if (BATTERY_STATUS > 11.1){
    BATTERY_STATUS = 2;
  }
}

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("Client " + String(num) + " connected");
      break;
    case WStype_TEXT:
      DeserializationError error = deserializeJson(JSON_rx, payload);
      if(error) {
        Serial.print("ERROR: ");
        Serial.println(error.c_str());
        return;
      }
      else{
        Serial.print("Station path ="); Serial.println(JSON_rx["stationID"]["path"].as<const char*>());
        SerialPort.print("DR"); SerialPort.println(JSON_rx["stationID"]["path"].as<const char*>());
      }
      break;
    }
}