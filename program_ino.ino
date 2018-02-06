#include "fsm_config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int buttonPin = D5;    // definicao do pino utilizado pelo botao
const int ledPin = D7;       // definicao do pino utilizado pelo led
const char* ssid = "jualabs"; // id da rede
const char* password = "jualabsufrpe"; // senha da rede
const char* mqtt_server = "things.ubidots.com"; // server

int buttonState = LOW;             // armazena a leitura atual do botao
int lastButtonState = LOW;         // armazena a leitura anterior do botao
unsigned long lastDebounceTime = 0;  // armazena a ultima vez que a leitura da entrada variou
unsigned long debounceDelay = 50;    // tempo utilizado para implementar o debounce

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int i = 0;


// definicao das funcoes relativas a cada estado

event btn_pressed_state(void) {
  digitalWrite(ledPin, HIGH);
  if(read_button())

    return btn_pressed;
}
event mqqt(void){
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  return time_pass;
}

event question_state(void){
  now = millis();
  if (readButton()){
    digitalWrite(led, HIGH);
    send_data();
    return send_button;
  }
  else if(now-millis() == 10000){
    now = millis()
    sendData();
    return send_time;
  }
  else{
    return question_state;
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

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
}

void int_connect_state() {

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
    return mqqt;
  }
  else{
    Serial.println("Wifi não conectada")
    return int_connect;
  }


}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void send_data() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client","A1E-KjZv9M3SE1Zkom6cZM1HS3bcUabMLF","")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("/v1.6/devices/wemos/contador", "{\"value\":3}");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/*void loop() {
  cur_state_function = state_functions[cur_state];
  cur_evt = (event) cur_state_function();
  if (EXIT_STATE == cur_state)
    return;
  cur_state = lookup_transitions(cur_state, cur_evt);
}
*/
void loop () {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    if (i==0){
    i  = i + 1;
    client.publish("/v1.6/devices/wemos/contador", "{\"value\":100}");
  }
  }
}
