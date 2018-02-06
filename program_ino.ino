#include "fsm_config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>;

#define DHTPIN D3          // what pin we're connected to
#define DHTTYPE DHT22     // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
const int buttonPin = D5;    // definicao do pino utilizado pelo botao
const int ledPin = D7;       // definicao do pino utilizado pelo led
const char* ssid = "GVT-AE52"; // id da rede
const char* password = "jualabsufrpe"; // senha da rede
const char* mqtt_server = "things.ubidots.com"; // server


int buttonState = LOW;             // armazena a leitura atual do botao
int lastButtonState = LOW;         // armazena a leitura anterior do botao
unsigned long lastDebounceTime = 0;  // armazena a ultima vez que a leitura da entrada variou
unsigned long debounceDelay = 50;    // tempo utilizado para implementar o debounce
float umidade;  //Stores humidity value
float temperatura; //Stores temperature value

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[60];
char msg1[60];



// definicao das funcoes relativas a cada estado
event mqqt_state(void){
    if (client.connect("ESP8266Client","A1E-KjZv9M3SE1Zkom6cZM1HS3bcUabMLF","")) {
      Serial.println("connected");
      client.subscribe("inTopic");
      return question;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      return server;
    }
  }

event question_state(void){
  now = millis();
  if (readButton()){
    digitalWrite(led, HIGH);
    return send_button;
  }

  else if(now- lastMsg >= 10000){
    digitalWrite(led, HIGH);
    lastMsg = now
    return send_time;
  }
  else{
    digitalWrite(led, LOW);
    return question;
  }
}

event send_button_state(void) {
  if (send_data()){
    return question;
  }
  else{
    return send_button;
  }
}

event send_time_state(void) {
  // Loop until we're reconnected
  if (send_data()){
    return question;
  }
  else{
    return send_time;
  }
}

event int_connect_state(void) {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  if (WiFi.status()== WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return server;
  }
  else{
    Serial.println("Wifi não conectada")
    return internet_c;
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
int send_data(void) {
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();
  snprintf (msg, 60, "{\"value\":%d}", dtostrf(temperatura, 6, 2, NULL));
  snprintf (msg1, 60, "{\"value\":%d}", dtostrf(umidade, 6, 2, NULL));
  client.publish("/v1.6/devices/wemos/umidade", msg1);
  client.publish("/v1.6/devices/wemos/temp", msg);
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(umidade);
  Serial.print(" %, Temp: ");
  Serial.print(temperatura);
  Serial.println(" Celsius");
  delay(10000); //Delay 2 sec.
  return true
}

void setup() {
  client.setServer(mqtt_server, 1883);
  dht.begin();
  pinMode(buttonPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
}

void loop() {
  cur_state_function = state_functions[cur_state];
  cur_evt = (event) cur_state_function();
  if (EXIT_STATE == cur_state)
    return;
  cur_state = lookup_transitions(cur_state, cur_evt);
}
