#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

//Web page String with backwards slash (\) skipped and using single quotes (') instead of double (")
const char* webpage = "<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <title>Auto Tool</title> <style> * { user-select: none; -webkit-tap-highlight-color: rgba(255, 255, 255, 0) !important; -webkit-focus-ring-color: rgba(255, 255, 255, 0) !important; outline: none !important; } body { background-color: #1d1c21; color: #EEEEEE; font-family: Arial, Helvetica, sans-serif; margin: 0; } header { display: flex; justify-content: center; } main { display: flex; align-items: flex-start; justify-items: center; flex-wrap: wrap; padding: 10px; } main .select-t { flex-grow: 1; display: flex; flex-direction: column; align-items: center; padding: 10px; } .tool-box { display: flex; flex-direction: column; color: #1d1c21; border-radius: 20px; width: 100%; height: 30vh; padding: 5px; } main .select-t > div { background-color: #c0dedd; } .sub-btn { margin: 10px; border: 0; padding: 5px; text-decoration: none; cursor: pointer; border-radius: 10px; } main .select-t > input { background-color: #e6dff1; } main .select-t > input:active { border: 2px solid #e6dff1; } .sub-btn:active { margin: 8px; background-color: transparent; color: #EEEEEE; outline: none !important; } main .current-t { flex-grow: 1; display: flex; flex-direction: column; align-items: center; padding: 10px; } main .current-t > div { background-color: #e6dff1; } main .current-t > input { background-color: #c0dedd; } main .current-t > input:active { border: 2px solid #c0dedd; } main .station-dir { width: 100%; display: flex; flex-direction: column; align-items: center; } .disabled label{ color: #950101; text-decoration: line-through; } </style></head><body> <header> <h1>Auto Tool</h1> </header> <main> <div class='select-t'> <h2>Select Tools</h2> <div id='select-tools' class='tool-box'> <!-- <div> <input type='checkbox' id='Screwdriver1' value='Screwdriver'> <label for='Screwdriver1'>Screwdriver</label> </div> <div class='disabled'> <input disabled='disabled' type='checkbox' id='Screw box1' value='Screw box'> <label for='Screw box1'>Screw box</label> </div> <div> <input type='checkbox' id='Hammer1' value='Hammer'> <label for='Hammer1'>Hammer</label> </div> --> </div> <input class='sub-btn' type='submit' value='Add Tools' onclick='addTools()'> </div> <div class='current-t'> <h2>Current Tools</h2> <div id='current-tools' class='tool-box'> <!-- <ul> <li>Sopa</li> <li>Sopa</li> </ul> --> </div> <input class='sub-btn' type='submit' value='Return'> </div> <div class='station-dir'> <h2>Station Number</h2> <input type='number'> <input type='submit' value='Request'> </div> </main> <script> var dataJSON = {}; function toolWriter(){ var output = ''; for(var i = 0;i <= (dataJSON.tools.length - 1); i++){ if(dataJSON.tools[i].available){ output += '<div><input type=\\'checkbox\\' id=\\''+dataJSON.tools[i].id+'\\' value=\\''+dataJSON.tools[i].id+'\\'></input><label for=\\''+dataJSON.tools[i].id+'\\'>'+dataJSON.tools[i].name+'</label></div>'; } else { output += '<div class=\\'disabled\\'><input disabled=\\'disabled\\' type=\\'checkbox\\' id=\\''+dataJSON.tools[i].id+'\\' value=\\''+dataJSON.tools[i].id+'\\'></input><label for=\\''+dataJSON.tools[i].id+'\\'>'+dataJSON.tools[i].name+'</label></div>'; } } document.getElementById('select-tools').innerHTML = ''; document.getElementById('select-tools').innerHTML = output; output = ''; } var Socket; function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function processCommand(event) { dataJSON = JSON.parse(event.data); toolWriter(); } function addTools() { output = ''; for(i = 0; i <= (dataJSON.tools.length - 1); i++) { if(document.getElementById(dataJSON.tools[i].id).checked && dataJSON.tools[i].available){ output += '<li id=\\'AT'+dataJSON.tools[i].id+'\\'>'+dataJSON.tools[i].name+'</li>' } } document.getElementById('current-tools').innerHTML = ''; document.getElementById('current-tools').innerHTML = '<ul>'+output+'</ul>'; } window.onload = function(event) { init(); } </script></body></html>";

//Defines the JSON transimitt template in a char array 
const char* JSON_tx_st = "{ \"tools\" : [ { \"id\":\"00000001\", \"name\":\"Screwdriver\", \"available\":false, \"usedBy\":\"Mario\" }, { \"id\":\"00000002\", \"name\":\"Hammer\", \"available\":true, \"usedBy\":null }, { \"id\":\"00000003\", \"name\":\"Screwdriver\", \"available\":true, \"usedBy\":null }, { \"id\":\"00000004\", \"name\":\"Nails\", \"available\":false, \"usedBy\":\"José\" } ]}";
StaticJsonDocument<512> JSON_tx; //Defines the JSON object with a memory of 512

const char* JSON_rx_st = "{ \"stationID\":{ \"path\":\"121121000\", \"username\":\"Mario\" }, \"tools\" : [ { \"id\":\"00000001\" }, { \"id\":\"00000002\" }, { \"id\":\"00000003\" }, { \"id\":\"00000004\" } ]}";
StaticJsonDocument<512> JSON_rx;

WebServer server(80); //defines a webserver object called server in port 80
WebSocketsServer webSocket = WebSocketsServer(81); //defines a WebSocketsServer object called webSocket in port 81

const int btn_tool_1 = 5; //Defines the pin used for tool 1 PULL-UP
const int btn_tool_2 = 19;//Defines the pin used for tool 2 PULL-UP
const int btn_tool_3 = 18;//Defines the pin used for tool 3 PULL-UP
const int btn_tool_4 = 23;//Defines the pin used for tool 4 PULL-UP

void setup() {
  Serial.begin(115200);//Initialize serial commuinications with PC at 115200 baud

  jsonTxSetup();//Deserialzie the transmit JSON string in to the JSON object
  jsonRxSetup();

  wifiAPSetup();//Select either wifiSetup() or wifiAPSetuo() for broadcasting or connecting to a wifi signal    **WiFi manager pending for WiFi connection
  serverSetup();//Defines what to do when a client connects, intializes the webServer and the webSocketsServer

  pinMode(btn_tool_1, INPUT_PULLUP);//Tool 1 input PULL-UP, switch must be connected to GND
  pinMode(btn_tool_2, INPUT_PULLUP);//Tool 2 input PULL-UP, switch must be connected to GND
  pinMode(btn_tool_3, INPUT_PULLUP);//Tool 3 input PULL-UP, switch must be connected to GND
  pinMode(btn_tool_4, INPUT_PULLUP);//Tool 4 input PULL-UP, switch must be connected to GND
  
}

void loop() {
  server.handleClient();//handles client every loop
  webSocket.loop();//handles webSocket requests every loop

  toolJSONWriter();
}
void serverSetup() {
  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();
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


void jsonRxSetup() {

  DeserializationError error = deserializeJson(JSON_rx, JSON_rx_st);

  if(error) {
    Serial.print("ERROR: ");
    Serial.println(error.c_str());
  }
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

  String JSON_tx_out = "";
  serializeJson(JSON_tx, JSON_tx_out);

  if(btn_previous_1 != digitalRead(btn_tool_1)) {
    webSocket.broadcastTXT(JSON_tx_out);
    Serial.println("Data Sent;");
    btn_previous_1 = digitalRead(btn_tool_1);
  }
  if(btn_previous_2 != digitalRead(btn_tool_2)) {
    webSocket.broadcastTXT(JSON_tx_out);
    Serial.println("Data Sent;");
    btn_previous_2 = digitalRead(btn_tool_2);
  }
  if(btn_previous_3 != digitalRead(btn_tool_3)) {
    webSocket.broadcastTXT(JSON_tx_out);
    Serial.println("Data Sent;");
    btn_previous_3 = digitalRead(btn_tool_3);
  }
  if(btn_previous_4 != digitalRead(btn_tool_4)) {
    webSocket.broadcastTXT(JSON_tx_out);
    Serial.println("Data Sent;");
    btn_previous_4 = digitalRead(btn_tool_4);
  }
}