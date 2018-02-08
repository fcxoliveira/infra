#include "fsm_config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>


#define DHTPIN D4          // what pin we're connected to
#define DHTTYPE DHT22     // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
const int buttonPin = D5;    // definicao do pino utilizado pelo botao
const int ledPin = D7;       // definicao do pino utilizado pelo led
const char* ssid = "augusto-Aspire-E5-571"; // id da rede
const char* password = "sLPCwZqX"; // senha da rede
const char* mqtt_server = "things.ubidots.com"; // server


int buttonState = LOW;             // armazena a leitura atual do botao
int lastButtonState = LOW;         // armazena a leitura anterior do botao
unsigned long lastDebounceTime = 0;  // armazena a ultima vez que a leitura da entrada variou
unsigned long debounceDelay = 50;    // tempo utilizado para implementar o debounce
float umidade;  //Stores humidity value
float temperatura; //Stores temperature value
float button;
float timePass;
String hum = "{\"value\":";
String temp = "{\"value\":";
String but = "{\"value\":";
String time1 = "{\"value\":";
char humChar[50];
char tempChar[50];
char butChar[50];
char timeChar[50];

WiFiClient espClient;
PubSubClient client(espClient);
int lastMsg = 10000;
int now;



// definicao das funcoes relativas a cada estado
event mqqt_state(void){

    if (client.connect("ESP8266Client","A1E-KjZv9M3SE1Zkom6cZM1HS3bcUabMLF","")) {
      Serial.println("connected");
      client.subscribe("inTopic");
      return next;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      return back;
    }
  }

event _question_state(void){
  //delay(5000);
 
  now = millis();
  if(now-lastMsg > 10000){
    Serial.println("Por tempo");
    lastMsg = now + 10000;
    return next;
  }
  
  if(read_button()){
    Serial.println("Por botão");
    digitalWrite(ledPin, HIGH);
    lastMsg = now + 10000;
    return first;
  }
  Serial.println(now - lastMsg);
  Serial.println("Resetando o estado");
  return back;
}
event send_button_state(void) {


  if (client.connected()){
    if (send_data()){
      timePass = 0;
      button = 1;
      time1.concat(timePass);
      time1.concat("}");
      but.concat(button);
      but.concat("}");
      time1.toCharArray(timeChar, 50);
      but.toCharArray(butChar, 50);
      String time1 = "{\"value\":";
      String but = "{\"value\":";
      client.publish("/v1.6/devices/wemos/time", timeChar);
      client.publish("/v1.6/devices/wemos/button", butChar);   
      return back; 
    }
  }
  else{
    return first;
  }
}

event send_time_state(void) {
  // Loop until we're reconnected

  if (client.connected()){
    if (send_data()){
      timePass = 1;
      button = 0;
      time1.concat(timePass);
      time1.concat("}");
      but.concat(button);
      but.concat("}");
      time1.toCharArray(timeChar, 50);
      but.toCharArray(butChar, 50);
      String time1 = "{\"value\":";
      String but = "{\"value\":";
      client.publish("/v1.6/devices/wemos/time", timeChar);
      client.publish("/v1.6/devices/wemos/button", butChar);      
      return back; 
    }
  }
  else{
    return first;
  }
}

event int_connect_state(void) {
   delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  delay(5000);
  if (WiFi.status()== WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return next;
  }
  else{
    Serial.println("Wifi não conectada");
    return back;
  }

}


// variaveis que armazenam estado atual, evento atual e funcao de tratamento do estado atual
state cur_state = ENTRY_STATE;
event cur_evt;
event (* cur_state_function)(void);

// implementacao de funcoes auxiliares
int read_button() {


  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  lastButtonState = reading;

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        return true;
      }
    }
  }
  return false;
}



int send_data() {
  dht.begin();
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();
  umidade = 4;
  temperatura = 26;
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(umidade);
  Serial.print(" %, Temp: ");
  Serial.print(temperatura);
  Serial.println(" Celsius");
  temp.concat(temperatura);
  temp.concat("}");
  hum.concat(umidade);
  hum.concat("}");
  temp.toCharArray(tempChar, 50);
  hum.toCharArray(humChar, 50);
  String hum = "{\"value\":";
  String temp = "{\"value\":";
  client.publish("/v1.6/devices/wemos/umidade", humChar);
  client.publish("/v1.6/devices/wemos/temp", tempChar);
  
  delay(10000); //Delay 2 sec.
  return true;
}

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  client.setServer(mqtt_server, 1883);
  Serial.begin(9600);

}

void loop() {
  cur_state_function = state_functions[cur_state];
  cur_evt = (event) cur_state_function();
  if (EXIT_STATE == cur_state)
    return;
  cur_state = lookup_transitions(cur_state, cur_evt);
}
