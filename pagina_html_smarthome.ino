#include <SPI.h>
#include <Ethernet.h>
#include <nRF24L01.h>
#include <RF24.h>
#include<dht.h>
dht DHT;
#define DHT11_PIN 6
long previousMillis_dht11 = 0;
long interval_dht11 = 2000;
//declarare comunicatie radio
RF24 radio(7, 8); // CNS, CE
const byte address[6] = "00001";
//declarare alarma de inundatie
int WaterAlarm = A2;
int Led_water = 23;
int Buzzer_w =22;
int tones[] = {261, 277, 293, 311, 329, 349, 369, 392, 415, 440, 466, 493, 523 ,554};
//              1    2    3    4    5    6    7    8    9    10   11   12   13   14
int water; 
long previousMillis_water = 0;     
long interval_water = 1000; 
//declarare senzor de lumina
int light_sensor_1 = A0;
int light_reading_1;
long previousMillis_light_1 =0;
long interval_light_1=1000;
//declarare alarma de gaz
int GasSensor = A3;
int Led_gas = 24;
int Buzzer_g = 25;
long previousMillis_gas=0;
long interval_gas=1000;
int gas;
//declarare senzor prezenta
int pirSensor=30;
int pir_reading;
long previousMillis_pir = 0;
long interval_pir =1000;
//declarare relee
int releu_1=34;
int releu_2=35;
int releu_3=36;
int releu_4=37;
//declarare adresa mac si adresa ip 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,100, 120);
EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  //configurare comunicatie radio
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  //configurare adresa ip
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("adresa IP a server-ului este: ");
  Serial.println(Ethernet.localIP());
    pinMode (WaterAlarm, INPUT);
    pinMode (GasSensor, INPUT);
    pinMode (Buzzer_w, OUTPUT);
    pinMode (Buzzer_g, OUTPUT);
    pinMode (Led_water, OUTPUT);
    pinMode (Led_gas, OUTPUT);
    pinMode(pirSensor, INPUT);
    pinMode(releu_1, OUTPUT);
    pinMode(releu_2, OUTPUT);
    pinMode(releu_3, OUTPUT);
    pinMode(releu_4, OUTPUT);
}
void loop() {
  //initializare server
    EthernetClient client = server.available();
    if (client) {
      Serial.println("conectare client nou");
      boolean currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          if (c == '\n' && currentLineIsBlank) {
            //creare pagina web
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<meta http-equiv=\"refresh\" content=\"5\">");
            client.println("<style>");
            client.println("h1{ color: blue; font-family: verdana; font-size: 180%;}");
            client.println("table{ font-family: arial, sans-serif; border-collapse: collapse; width: 50%;}");
            client.println("td,th{ border: 2px solid #dddddd; text-align:center; padding:10px;}");
            client.println("tr:nth-child(even){background-color: arial;}");
            client.println("</style>");
            client.println("<body>");
            client.println("<h1 style=\"text-align:center;\">ARDUINO SMART HOME & GARDEN</h1>");
            client.println("<table align=\"center\">");
            client.println("<tr>");
            client.println("<th>Senzori</th>");
            client.println("<th>Rezultat</th>");
            client.println("<th>Releu</th>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<th>Temperatura_IN</th>");
            client.println("<th>");
            client.println(DHT.temperature,1);
            client.println("</th>");
            client.println("<th>");
            if(DHT.temperature>22){
              digitalWrite(releu_3,LOW);
              client.println("ON");
          }
          else
          {
            digitalWrite(releu_3,HIGH);
            client.println("OFF");
          }
          if(DHT.temperature<16){
            digitalWrite(releu_4,LOW);
            client.println("ON");
          }
          else
          {
            digitalWrite(releu_4,HIGH);
            client.println("OFF");
          }
          client.println("</th>");
          client.println("</tr>");
          client.println("<tr>");
          client.println("<th>Umiditate</th>");
          client.println("<th>");
          client.println(DHT.humidity,1);
          client.println("</th>");
          client.println("<th></th>");
          client.println("</tr>");
          client.println("<tr>");
          client.println("<th>Luminozitate_IN</th>");
          client.println("<th>");
          if (light_reading_1 < 10) {
            client.println(" Intuneric");
            }
            else if (light_reading_1 < 200) {
            client.println(" Lumina Slaba ");
            }
            else if (light_reading_1 < 400){ 
            client.println(" Lumina normala");
            }
            else if (light_reading_1 < 800){ 
            client.println(" Luminos");
            }
            else {
            client.println(" Foarte Luminos");
            }
          client.println("</th>");
          client.println("<th>");
          if (light_reading_1 < 10) {
            client.println("ON");
            }
            else if (light_reading_1 < 200) {
            client.println("ON");
            }
            else if (light_reading_1 < 400){ 
            client.println("OFF");
            }
            else if (light_reading_1 < 800){ 
            client.println("OFF");
            }
            else {
            client.println("OFF");
            }
          client.println("</tr>");
          client.println("<tr>");
          client.println("<th>Alarma APA</th>");
          client.println("<th>");
          if(water>50){
            client.println("Alerta APA!!!");
          }
          else{
            client.println("APA OFF");
          }  
          client.println("</th>");
          client.println("<th></th>");
          client.println("</tr>");
          client.println("<tr>");
          client.println("<th>Alarma GAZ</th>");
          client.println("<th>");
          if(gas>800)
           {
            client.println("Alerta GAZ!!!");
           }
           else{
            client.println("GAZ OFF");
           }
          client.println("</th>");
          client.println("<th>");
          client.println("</th>");
          client.println("</tr>");
          client.println("<tr>");
          client.println("<th>Senzor Miscare</th>");
          client.println("<th>");
          if(pir_reading == 0){
              digitalWrite(releu_1, HIGH);
              client.println("Prezenta nedetectata");
          }
          else{
            digitalWrite(releu_1, LOW);
            client.println("Prezenta activa");            
            }
          client.println("</th>");
          client.println("<th>");
          if(pir_reading == 0){
              client.println("OFF");
            }
            else{           
            client.println("ON");
            }
          client.println("</th>");
          client.println("</tr>");
/*Comunicatia Radio*/
if (radio.available()) {
    char text[32] = "";
    int moisture;
    int rain_sensor;
    int light_reading_2;
    float flowRate;
    int totalMilliLitres;
    int celsius;
    radio.read(&moisture,sizeof(moisture));
    Serial.println(moisture);
    radio.read(&light_reading_2, sizeof(light_reading_2));
    client.println("<tr>");
    client.println("<th>Luminozitate_OUT</th>");
    client.println("<th>");
    int light_eth = light_reading_2;
    if (light_eth < 10) {
      client.println(" Intuneric");
      }
      else if (light_eth < 200) {
        client.println(" Lumina Slaba ");
        }
        else if (light_eth < 400){ 
          client.println(" Lumina normala");
          }
          else if (light_eth < 800){ 
            client.println(" Luminos");
            }
            else {
            client.println(" Foarte Luminos");
            }
            client.println("</th>");
            client.println("<th></th>");
            client.println("</tr>");
            radio.read(&celsius,sizeof(celsius));
            Serial.println(celsius);
            client.println("<tr>");
            client.println("<th>Temperatura_OUT</th>");
            client.println("<th>");
            client.println(celsius);
            client.println("</th>");
            client.println("<th></th>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<th>Umiditate_SOL</th>");
            client.println("<th>");
            if(moisture <= 100){
              client.println("In aer");
              }
              else if ((moisture <= 310)&&(moisture>100)) {
                client.println("Uscat");
              }
              else if (moisture <= 710) {
                client.println("Umed ");
              } 
              else {
                client.println("In Apa");
              }
              client.println("</th>");
              client.println("<th>");
              if(moisture <= 100){
                client.println("ON");
              }
              else if ((moisture <= 310)&&(moisture>100)) {
                client.println("ON");
              }
              else if (moisture <= 710) {
                client.println("OFF");
              } 
              else {
                client.println("OFF");
              }          
              client.println("</th>");
              client.println("</tr>");      
}
client.println("</table>");
client.println("</body>");            
unsigned long currentMillis = millis();
// DHT 11- temperatura, umiditate 
if(currentMillis - previousMillis_dht11 > interval_dht11) {
  previousMillis_dht11 = currentMillis; 
  int chk = DHT.read11(DHT11_PIN);
  }
  if(currentMillis - previousMillis_pir > interval_pir) {
    previousMillis_pir = currentMillis;
    pir_reading=digitalRead(pirSensor);
    if(pir_reading == 0){
      digitalWrite(releu_1, HIGH);
      }
      }
      client.println("<br />");
      //Alarma de apa
      if(currentMillis - previousMillis_water >interval_water){
        previousMillis_water = currentMillis; 
        water= analogRead(WaterAlarm);
        Serial.println("Water:");
        Serial.println(WaterAlarm);
        if(water>50){
          client.println("Alerta APA!!!");
          client.println("<br />");
          digitalWrite(Led_water, HIGH);
          digitalWrite(Buzzer_w, HIGH);
          tone(Buzzer_w, tones[6]);
          }
          else{
            digitalWrite(Led_water, LOW);
            digitalWrite(Buzzer_w, LOW); 
            noTone(Buzzer_w);
            }  
          }
          client.println("<br />");
          // Senzor de Lumina 
          if(currentMillis - previousMillis_light_1 > interval_light_1) {
            previousMillis_light_1 = currentMillis;
            light_reading_1=analogRead (light_sensor_1);   
            if (light_reading_1 < 10) {
              Serial.println(" - Intuneric");
              digitalWrite(releu_2, LOW);
              Serial.println("Releu 2 pornit");
              }
              else if (light_reading_1 < 200) {
                Serial.println(" - Lumina Slaba ");
                digitalWrite(releu_2, LOW);
                Serial.println("Releu 2 pornit");
                }
                else if (light_reading_1 < 400){ 
                  Serial.println(" - Lumina normala");
                  digitalWrite(releu_2, HIGH);
                  Serial.println("Releu 2 oprit");
                }
                else if (light_reading_1 < 800){ 
                  Serial.println(" - Luminos");
                  digitalWrite(releu_2, HIGH);
                  Serial.println("Releu 2 oprit");
                  }
                  else {
                    Serial.println(" - Foarte Luminos");
                    digitalWrite(releu_2, HIGH);
                    Serial.println("Releu 2 oprit");
                    }
                    }
                    // Senzor Gas/Smoke
                    if(currentMillis - previousMillis_gas > interval_gas){
                      previousMillis_gas = currentMillis;
                      gas =analogRead(GasSensor);
                      if(gas>800){
                        client.println("Alerta GAZ!!!");
                        client.println("<br />");
                        digitalWrite(Led_gas, HIGH);
                        digitalWrite(Buzzer_g,HIGH);
                        tone(Buzzer_g, tones[10]);
                        }
                        else{
                          digitalWrite(Led_gas, LOW);
                          digitalWrite(Buzzer_g, LOW); 
                          noTone(Buzzer_g);
                          }
                          }
                          client.println("</html>");
                          break;
                          }
                          if (c == '\n') {
                            currentLineIsBlank = true;
                            }
                            else if (c != '\r') {
                              currentLineIsBlank = false;
                              }
                              }
                              }
                              delay(1);
                              client.stop();
                              Serial.println("clientul a incheiat sesiunea");
                              }
                              }
