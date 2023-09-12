/*
  LoRa Duplex communication

  Sends a message every 20 seconds, and polls continually
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.
 
  Uses readString() from Stream class to read payload. The Stream class'
  timeout may affect other functuons, like the radio's callback.
*/

#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>


//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6
//packet counter
int readingID = 0;  
int counter = 0;
String up;
String down;
String outgoing;              // outgoing message
const int lamp = 25;
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xCC;     // address of this device. Merci de mettre une adresse différente des autres
byte destination = 0xBB;      // destination to send to. Merci de voir l'adresse de votre collègue. OxFF adresse pour tous le monde
long lastSendTime = 0;        // last send time
int interval = 5000;          // interval between sends
String h;
String t;
String c;
String valeur;
const char* ssid = "Development-DGNI";
const char* password = "DGNI@123";
const char* mqtt_server = "10.0.10.112";
int message;
String statut;
const char* mqtt_username = "tahri";
const char* mqtt_password = "houssam";
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  Serial.print(messageTemp);
  Serial.println();
  if(String(topic) == "room/lamp"){
      Serial.print("Changing Room lamp to ");
      Serial.print(topic);
      if(messageTemp == "true"){
        statut="up";
        digitalWrite(lamp, HIGH);
        Serial.print("On");
        Serial.print(" statut: "+String(statut));
      }
      else if(messageTemp == "false"){
        statut="down";
        digitalWrite(lamp, LOW);
        Serial.print("Off");
        Serial.print(" statut: "+statut);
      }
      Serial.println();
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
    
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("room/lamp");
      client.subscribe("room/humidity");
      client.subscribe("room/temperature");
      client.subscribe("room/humidity_sol");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  
  //pinMode(lamp, OUTPUT);
  Serial.begin(9600);                   // initialize serial
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!Serial);

  Serial.println("LoRa Duplex");

   //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!");
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP32Client, mqtt_username, mqtt_password");
    
  if (millis() - lastSendTime > interval) {
    sendMessage(statut);
    Serial.println("Sending " + message);
    lastSendTime = millis();            // timestamp the message
 //   interval = random(10000) + 1000;    // 10-11 seconds
  }

  // parse for a packet, and call onReceive with the result:
  onReceive();
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive() {
//  if (packetSize == 0) return;          // if there's no packet, return
//
//  // read packet header bytes:
//  int recipient = LoRa.read();          // recipient address
//  byte sender = LoRa.read();            // sender address
//  byte incomingMsgId = LoRa.read();     // incoming msg ID
//  byte incomingLength = LoRa.read();    // incoming msg length
//
//  String incoming = "";
//
//  while (LoRa.available()) {
//    incoming += (char)LoRa.read();
//  }
//
//  if (incomingLength != incoming.length()) {   // check length for error
//    Serial.println("error: message length does not match length");
//    return;                             // skip rest of function
//  }

  // if the recipient isn't this device or broadcast,
//  if (recipient != localAddress && recipient != 0xFF) {
//    Serial.println("This message is not for me.");
//    return;                             // skip rest of function
//  }

  // if message is for this device, or broadcast, print details:
//  Serial.println("Received from: 0x" + String(sender, HEX));
//  Serial.println("Sent to: 0x" + String(recipient, HEX));
//  Serial.println("Message ID: " + String(incomingMsgId));
//  Serial.println("Message length: " + String(incomingLength));
//  Serial.println("Message: " + incoming);
//  Serial.println("RSSI: " + String(LoRa.packetRssi()));
//  Serial.println("Snr: " + String(LoRa.packetSnr()));
//  Serial.println();
   int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    //Serial.print("Received packet ");
    //read packet
    while (LoRa.available()) {
      h = LoRa.readStringUntil('|');
      //Serial.print(currentTemperature);
      t = LoRa.readStringUntil('|');
      valeur = LoRa.readStringUntil('|'); 
      c = LoRa.readStringUntil('|');
      
      client.publish("room/humidity", (String(h).c_str()));
      client.publish("room/temperature", (String(t).c_str()));
      client.publish("room/humidity_sol", (String(valeur).c_str()));
      
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print("°C ");
    Serial.print("Humidity de sol: ");
    Serial.print(valeur);
    Serial.print("%");
    Serial.print(" Received packet N°: ");
    Serial.print(c);
    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());
  
    }
    
  }
  
}

              
