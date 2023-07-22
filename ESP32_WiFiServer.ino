/* Code adapted from Tom Igoes "Simple WiFi Server", which he created for arduino 25 Nov 2012 as
   a WiFi Web Server to remotely turn an LED on or off.
   Jan Hendrik Berlin ported it for the sparkfun esp32 on 1 Jan 2017 
   19 July 2023, John Best added communication to an external uP to expand the IO 
   capability (and speed, etc) of the ESP32 based Heltec Lora/Wifi OLED board.   
    
    This sketch will print the IP address of your WiFi Shield (once connected)
    to the Serial monitor. From there, you can open that address in a web browser
    to turn on and off the LED.

     This example is written for a network using WPA encryption. For
     WEP or WPA, change the Wifi.begin() call accordingly.
 */

#include <WiFi.h>
#include "heltec.h" // alias for `#include "SSD1306Wire.h"`

const char* ssid     = "Your router name here";
const char* password = "Your password here";
int led = 25;          // LED pin on the Heltec LoRa/WiFi/OLED dev board
float fReturns[5], tempValue;
WiFiServer server(80);

void setup()
{
  int connectTries = 0;
    Serial.begin(115200);                     // USB serial for local debugging
    Serial2.begin(115200, SERIAL_8N1,12,13);  // 12=Rx, 13=Tx,  
    pinMode(led, OUTPUT);     // set the LED pin mode
    
    Heltec.begin(true);
    Heltec.display->init();
    //Heltec.display->flipScreenVertically();
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0,0,"Heltec WiFi/Lora ESP32");
    Heltec.display->drawString(0,15,"Open serial monitor for");
    Heltec.display->drawString(0,26,"status and debug");
    Heltec.display->drawString(0,44,"Testing LED for 10 seconds");
    Heltec.display->display();
    Serial.println("Test the LED... 10 seconds blink.");  
    for (int n=10; n>0; n--){  // Test LED
      digitalWrite(led, HIGH);    delay(500); 
      digitalWrite(led, LOW);     delay(500); 
    }
    Heltec.display->init();
    delay(10);

    // We start by connecting to a WiFi network
    Serial.println();        Serial.print("Connecting to ");      Serial.println(ssid);
    //WiFi.begin(ssid, password);  
    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);  
        delay(500);
        Serial.print(connectTries++);
        Serial.print("  ");
        Serial.print(ssid);
        Serial.print(" - ");
        Serial.println(password);
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Heltec.display->init();
    //char IPaddr = WiFi.localIP();
    Heltec.display->drawString(0,0, "*");  // WiFi.localIP());
    Heltec.display->display();
    server.begin();

}

int dataType =0;
float TempC = 123.456;
String inchar;
void loop(){
  // Simple command/responce over serial UART link to Teensy.
  // Sends a number 1-4 for the parameter you want the Teensy to send back.
  for (dataType = 1; dataType <= 4; dataType++){
    Serial.print(dataType);  //This one for testing.
    Serial2.print(dataType);  //This one to elicit data from Teeensy
    delay(100); // To give Teensy time to reply
    inchar = "";  
    while(  Serial2.available()  ){  inchar += char(Serial2.read());  } 
    if (inchar.length() >= 1){
      Serial.print("  Rcvd: ");   Serial.println(inchar);  
      tempValue = inchar.toFloat();
    }
    fReturns[dataType] = tempValue;
    sendToOLED();          delay(10);   //Display the data on the OLED if changed. 
  } Serial.println(" ");   //delay(100); 
  
 // now serve up the data just acquired.
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port

    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn LED on board ON.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn LED on board OFF.<br>");
            
            // 230719 JWB inserted to diaplay temperature
            client.print("<br>");
            client.print("Current Temp: ");   client.print(fReturns[1]); client.print("<br>");
            client.print("Current Press: ");  client.print(fReturns[2]); client.print("<br>");
            client.print("Current Alt: ");    client.print(fReturns[3]); client.print("<br>");
            client.print("Current Humid: ");  client.print(fReturns[4]); client.print("<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) { digitalWrite(led, HIGH);   }
        if (currentLine.endsWith("GET /L")) { digitalWrite(led, LOW);    }
      }
    }
    // close the connection:
    //client.stop();
    //Serial.println("Client Disconnected.");
  }
}

void sendToOLED(){   
      String outStr, labels[5] = {"","C","hPa","m","%"};
      static float oldParms[5] = {0,0,0,0,0};   //To keep OLED from updating if the data hasn't changed.
      static int yCoord[5] = {0, 15, 25, 35, 45};
      int refresh = 0;
      for (int n=1; n<5; n++){
        if ( oldParms[n] != fReturns[n] ){  
          oldParms[n] = fReturns[n]; refresh = 1;    //At least one parm has changed, so update OLED.
        }
      }
      if (refresh != 0){
        Heltec.display->clear();
        Heltec.display->drawString(0, yCoord[0], "Current");
        for (int n=1; n<5; n++){
          outStr = fReturns[n]; 
          Heltec.display->drawString(10, yCoord[n],  outStr);
          Heltec.display->drawString(45, yCoord[n],  labels[n]);
        }
        Heltec.display->display();  refresh = 0;
      }
  return;
}

 
