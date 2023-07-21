/*  Adapted from Adafruit example for the BME280 temperature sensor.  
 *  I added the bit that sends a parameter over a UART to an external
 *  device which requests the parameter it wants by sening a single number
 *  corresponding to the requested parameter.
 *  
 *  This project uses a Teensy 4.0 for whatever sort of data acquisition and reduction is needed,
 *  then, the ESP32 WiFi processor is free to format the data and serve it over the network.  
 *  The basic strategy is to provide the Teensy platform because of it's I/O and speed, the 
 *  ESP32 is just a connectivity port.   The Heltec ESP32 V2 used in this project has 
 *  Lora capability in addition to WiFi and BT.
 */
 #include <Wire.h>      
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Servo.h>
#define SEALEVELPRESSURE_HPA (1013.25)
#define PIN_Teensy_Led (13)   
Servo Tservo;   // Temperature indicating servo
Adafruit_BME280 bme; // I2C    //Adafruit_BME280 bme(BME_CS); // hardware SPI  //Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
String ESP32_inStr;
float TempC, TempF, Press, Humid, Altit, PressInHg, PressInPSI; 
int printMode = 0;

void setup() {
  pinMode(PIN_Teensy_Led, OUTPUT);    digitalWrite(PIN_Teensy_Led, LOW);
  Tservo.attach(2);
  Serial.begin(115200);  Serial2.begin(115200); //For COM to the ESP32, which will handle the WiFi
  Serial.println(F("BME280 V-1"));    
  bool status;
  int I2Caddr = 0x76;    // default I2C 0x76 addr   
  status = bme.begin(I2Caddr);  //was originally 0x76,  jwb 230716
  delay(10);    Serial.println();
  
  Serial.print("Going to 0, pausing a second, then to 180");   //temp code to set the pointer
  for (int n=90;  n>0;    n--){    Tservo.write(n);  delay(10);   }
  for (int n=0;   n<180;  n++){    Tservo.write(n);  delay(10);  }
  for (int n=180; n>90;   n--){    Tservo.write(n);  delay(10);  }
  for (int n=10; n>0; n--){  //to check the pointer mounting 
    Serial.print("At 90 degrees for ");    Serial.print(n);   Serial.println("  more seconds."); 
    delay(1000);
  }
}


void loop() { // This loop needs to conplete faster than the data request rate of the ESP32
  int servoDegrees;   
  readBME();  // Get the full set of parameters from the sensor, but will only send the ones requested.
  printMode = 0;  //inhibits printing 
  ESP32_inStr = "";  // Clear string before fetching and appending
  while (Serial2.available()){  ESP32_inStr += char(Serial2.read() );    }
  if ( ESP32_inStr.length() >= 1){
    printMode = ESP32_inStr.toInt();   //Convert to an integer to select printing action.  
    // Send to USB for verification while developing.   Serial.print(ESP32_inStr);   Serial.print(" -> ");     Serial.println(printMode);
  } 
  
  printValuesUSB();             // Sends all the data to the serial monitor
  printValuesUART(printMode);   // Sends just the data requested by printMode
  delay(10);   
  servoDegrees = -3*TempC + 135;  
  Tservo.write(servoDegrees);
  //  Serial.print("C ");  Serial.print(TempC); Serial.print(" yields "); Serial.println(servoDegrees);
}

void printValuesUART(int whichParameter){
  //Serial.println(whichParameter);
  switch (whichParameter){
    case 0: break;  //This case is used as the inhibitor from printing.
    case 1: Serial2.print(TempC);   break;
    case 2: Serial2.print(Press);   break; 
    case 3: Serial2.print(Altit);   break;
    case 4: Serial2.print(Humid);   break;
    default: break;
  }
  return;
}

void printValuesUSB() {
  Serial.print("Temperature = ");       Serial.print(TempC);        Serial.print(" *C");    
  Serial.print("       ");              Serial.print(TempF);        Serial.println(" *F");
  Serial.print("Pressure = ");          Serial.print(Press);        Serial.print(" hPa   ");
                                        Serial.print(PressInHg);    Serial.print(" In Hg   "); 
                                        Serial.print(PressInPSI);   Serial.println(" PSI");
  Serial.print("Approx. Altitude = ");  Serial.print(Altit);        Serial.println(" m");
  Serial.print("Humidity = ");          Serial.print(Humid);        Serial.println(" %");
  Serial.println();
}

void readBME(){  // Also calculates alternate units
  TempC =  bme.readTemperature();
  TempF = 1.8 * TempC + 32;
  Press = bme.readPressure() / 100.0F;
  Altit = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Humid = bme.readHumidity();
  PressInHg = Press / 33.8638; 
  PressInPSI = Press * 0.014504;
  return;
}
