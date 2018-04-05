#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>


// Update these with values suitable for your network.
IPAddress server(x, x, x, x);
const char *ssid =  "";    // cannot be longer than 32 characters!
const char *pass =  "";    //


//create variables
boolean lightState = false;
float temperature = 0;
float humidity = 0;
char buffer[25];
#define light D2
//setup the DHT22 sensor
#define DHTPIN D3 
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE); 
//setup the MQTT
WiFiClient wclient;
PubSubClient client(wclient);
//set the light on/off
void setLight(boolean state){
  if(state == true){
    digitalWrite(light, 1);
    lightState = state;
    Serial.print("Light on");
  }
  else{
    digitalWrite(light, 0);
    lightState = state;
    Serial.print("Light off");
  }
}
//send the state of the light over MQTT
void checkLightState(){
  if(lightState == true){
         client.publish("home/gustavoBedroom/light/bedroomLight", "1");
  }
  else{
         client.publish("home/gustavoBedroom/light/bedroomLight", "0");
  }
}
//check temperature and send it over MQTT
void checkTemperature(){
  temperature = dht.readTemperature();
  dtostrf(temperature,4, 1, buffer);
  client.publish("home/gustavoBedroom/temperature", buffer);
  Serial.print("Temperature: ");
  Serial.println(temperature);
}
//check humidity and send it over MQTT
void checkHumidity(){
  humidity = dht.readHumidity();
  dtostrf(humidity,4, 1, buffer);
  client.publish("home/gustavoBedroom/humidity",buffer);
  Serial.print("Humidity: ");
  Serial.println(humidity);
}
//interpret the messsage
void interpretMessage(String message){
    if(message == "1"){
      setLight(true);
    }
    else{
      setLight(false);
    }
  }

 void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  //copy the message to a String
  for (int i=0;i<length;i++) {
    message.concat((char)payload[i]);
    Serial.print((char)payload[i]);
  }
  //send it to a method to interpret it
  interpretMessage(message);
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("wclient")) {
      Serial.println("connected");
      //resubscribe
      client.subscribe("home/gustavoBedroom/light/set");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  //set the pins
   pinMode(D2, OUTPUT);
//set MQTT server
  client.setServer(server, 1883);
  client.setCallback(callback);
  //Connect to WIFI
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
//if not connected, then attempt a connection
{
  if (!client.connected()) {
    reconnect();
  }
  //send the current light state over MQTT
  checkLightState();
  //send the current temperature/humidity over MQTT
  checkTemperature();
  checkHumidity();
      client.loop();
      delay(2000);
}
