#include <WiFi.h>
#include <PubSubClient.h>
#include "Receiver.h"

//This is a program to control RX5808 or any video reciever with SPI control using an MQTT server.
//The esp32 connects over wifi and then connects to an open MQTT server.  No username or password.
//MQTT frequencies should be in 4 digit format.

 //user defined parameters. Between "" add in your wifi ssid, password and the RH server IP address.
 //Make sure to make it a reserved static IP or you will have to change this often
const char* ssid = "XXXXXX";
const char* password =  "YYYYYYYYY";
const char* mqtt_server = "ZZZ.ZZZ.Z.ZZZ";
const int mqtt_port = 1883;

//These are the topics the reveivers need to subscribe to. The / separates the topic sequence.
#define RECEIVER_1_FREQ "receiver/1"
#define RECEIVER_2_FREQ "receiver/2"
#define RECEIVER_3_FREQ "receiver/3"
#define RECEIVER_4_FREQ "receiver/4"
#define RECEIVER_5_FREQ "receiver/5"
#define RECEIVER_6_FREQ "receiver/6"
#define RECEIVER_7_FREQ "receiver/7"
#define RECEIVER_8_FREQ "receiver/8"
#define ESP32_PRESENCE_PUBLISH "presence/ESP32"

//Initial frequencies for each receiver.  You can change these if you want but they are here to initialize the program.
//Once connected to the MQTT server it will change the receivers automatically.
int rx_freq_1 = 5880;
int rx_freq_2 = 5880;
int rx_freq_3 = 5880;
int rx_freq_4 = 5880;
int rx_freq_5 = 5880;
int rx_freq_6 = 5880;
int rx_freq_7 = 5880;
int rx_freq_8 = 5880;

//Don't change these.  These are to initialize the past frequency numbers. Inside the loop there is a check to see if
//something new has changed.  If something did change it runs the freq change code.
int rx_freq_1_old = rx_freq_1;
int rx_freq_2_old = rx_freq_2;
int rx_freq_3_old = rx_freq_3;
int rx_freq_4_old = rx_freq_4;
int rx_freq_5_old = rx_freq_5;
int rx_freq_6_old = rx_freq_6;
int rx_freq_7_old = rx_freq_7;
int rx_freq_8_old = rx_freq_8;

//Pin definitions. Reciever count is redundant but is left in there.
#define RECEIVER_COUNT 8
#define RECEIVER_SPI_CLK_PIN 18
#define RECEIVER_SPI_DATA_PIN 23
#define RECEIVER_SPI_SS_1 13
#define RECEIVER_SPI_SS_2 12
#define RECEIVER_SPI_SS_3 14
#define RECEIVER_SPI_SS_4 27
#define RECEIVER_SPI_SS_5 26
#define RECEIVER_SPI_SS_6 25
#define RECEIVER_SPI_SS_7 33
#define RECEIVER_SPI_SS_8 32

Receiver receivers[RECEIVER_COUNT];

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  delay(15);  
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
    pinMode(RECEIVER_SPI_CLK_PIN, OUTPUT);
  pinMode(RECEIVER_SPI_DATA_PIN, OUTPUT);
  digitalWrite(RECEIVER_SPI_CLK_PIN, LOW);
  digitalWrite(RECEIVER_SPI_DATA_PIN, LOW);
  pinMode(RECEIVER_SPI_SS_1, OUTPUT);
  digitalWrite(RECEIVER_SPI_SS_1, HIGH);
  pinMode(RECEIVER_SPI_SS_2, OUTPUT);
  digitalWrite(RECEIVER_SPI_SS_2, HIGH);
  pinMode(RECEIVER_SPI_SS_3, OUTPUT);
  digitalWrite(RECEIVER_SPI_SS_3, HIGH);
  pinMode(RECEIVER_SPI_SS_4, OUTPUT);
  digitalWrite(RECEIVER_SPI_SS_4, HIGH);
  pinMode(RECEIVER_SPI_SS_5, OUTPUT);
  digitalWrite(RECEIVER_SPI_SS_5, HIGH);
  pinMode(RECEIVER_SPI_SS_6, OUTPUT);
  digitalWrite(RECEIVER_SPI_SS_6, HIGH);
  pinMode(RECEIVER_SPI_SS_7, OUTPUT);
  digitalWrite(RECEIVER_SPI_SS_7, HIGH);
  pinMode(RECEIVER_SPI_SS_8, OUTPUT);
  digitalWrite(RECEIVER_SPI_SS_8, HIGH);
      receivers[1].init(
          RECEIVER_SPI_CLK_PIN,
          RECEIVER_SPI_DATA_PIN,
          RECEIVER_SPI_SS_1
      );
       receivers[2].init(
          RECEIVER_SPI_CLK_PIN,
          RECEIVER_SPI_DATA_PIN,
          RECEIVER_SPI_SS_2
      );
        receivers[3].init(
          RECEIVER_SPI_CLK_PIN,
          RECEIVER_SPI_DATA_PIN,
          RECEIVER_SPI_SS_3
      );
       receivers[4].init(
          RECEIVER_SPI_CLK_PIN,
          RECEIVER_SPI_DATA_PIN,
          RECEIVER_SPI_SS_4
      );
       receivers[5].init(
          RECEIVER_SPI_CLK_PIN,
          RECEIVER_SPI_DATA_PIN,
          RECEIVER_SPI_SS_5
      );
       receivers[6].init(
          RECEIVER_SPI_CLK_PIN,
          RECEIVER_SPI_DATA_PIN,
          RECEIVER_SPI_SS_6
      );
       receivers[7].init(
          RECEIVER_SPI_CLK_PIN,
          RECEIVER_SPI_DATA_PIN,
          RECEIVER_SPI_SS_7
      );
       receivers[8].init(
          RECEIVER_SPI_CLK_PIN,
          RECEIVER_SPI_DATA_PIN,
          RECEIVER_SPI_SS_8
      );
  receivers[1].setFrequency(rx_freq_1);
  receivers[2].setFrequency(rx_freq_2);
  receivers[3].setFrequency(rx_freq_3);
  receivers[4].setFrequency(rx_freq_4);
  receivers[5].setFrequency(rx_freq_5);
  receivers[6].setFrequency(rx_freq_6);
  receivers[7].setFrequency(rx_freq_7);
  receivers[8].setFrequency(rx_freq_8);
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish(ESP32_PRESENCE_PUBLISH, "Wat Sup");
      // ... and resubscribe
      client.subscribe(RECEIVER_1_FREQ);
      client.subscribe(RECEIVER_2_FREQ);
      client.subscribe(RECEIVER_3_FREQ);
      client.subscribe(RECEIVER_4_FREQ);
      client.subscribe(RECEIVER_5_FREQ);
      client.subscribe(RECEIVER_6_FREQ);
      client.subscribe(RECEIVER_7_FREQ);
      client.subscribe(RECEIVER_8_FREQ);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//MQTT Callback and turning messages into strings.
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic,RECEIVER_1_FREQ)==0){
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    rx_freq_1 = atoi((char *)payload);
  }
  if (strcmp(topic,RECEIVER_2_FREQ)==0){
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    rx_freq_2 = atoi((char *)payload);
  }
  if (strcmp(topic,RECEIVER_3_FREQ)==0){
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    rx_freq_3 = atoi((char *)payload);
  }
  if (strcmp(topic,RECEIVER_4_FREQ)==0){
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    rx_freq_4 = atoi((char *)payload);
  }
  if (strcmp(topic,RECEIVER_5_FREQ)==0){
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    rx_freq_5 = atoi((char *)payload);
  }
  if (strcmp(topic,RECEIVER_6_FREQ)==0){
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    rx_freq_6 = atoi((char *)payload);
  }
  if (strcmp(topic,RECEIVER_7_FREQ)==0){
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    rx_freq_7 = atoi((char *)payload);
  }
  if (strcmp(topic,RECEIVER_8_FREQ)==0){
    payload[length] = '\0'; // Make payload a string by NULL terminating it.
    rx_freq_8 = atoi((char *)payload);
  }
} 

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(ESP32_PRESENCE_PUBLISH, serialData);
}

//Loop looks at callback function and then does a check to see if any frequencies change.  If they do then the set frequency function executes.
void loop() {
   client.loop();
   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     publishSerialData(mun);
   }
   if (rx_freq_1 != rx_freq_1_old) {
    receivers[1].setFrequency(rx_freq_1);
    rx_freq_1_old = rx_freq_1;
   }
   if (rx_freq_2 != rx_freq_2_old) {
    receivers[2].setFrequency(rx_freq_2);
    rx_freq_2_old = rx_freq_2;
   }
   if (rx_freq_3 != rx_freq_3_old) {
    receivers[3].setFrequency(rx_freq_3);
    rx_freq_3_old = rx_freq_3;
   }
   if (rx_freq_4 != rx_freq_4_old) {
    receivers[4].setFrequency(rx_freq_4);
    rx_freq_4_old = rx_freq_4;
   }
   if (rx_freq_5 != rx_freq_5_old) {
    receivers[5].setFrequency(rx_freq_5);
    rx_freq_5_old = rx_freq_5;
   }
   if (rx_freq_6 != rx_freq_6_old) {
    receivers[6].setFrequency(rx_freq_6);
    rx_freq_6_old = rx_freq_6;
   }
   if (rx_freq_7 != rx_freq_7_old) {
    receivers[7].setFrequency(rx_freq_7);
    rx_freq_7_old = rx_freq_7;
   }
   if (rx_freq_8 != rx_freq_8_old) {
    receivers[8].setFrequency(rx_freq_8);
    rx_freq_8_old = rx_freq_8;
   }
   delay(20);
 }
