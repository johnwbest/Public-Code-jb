# Public-Code-jb
The JPG image (TemperatureThingAnot8.jpg) shows the project as of 21 July, 2023.  The code is in two parts, one for each uP.   It is a simple mash-up of modified Arduino examples.  One would create a dev environment under the Arduino IDE for the Teensy, per https://www.pjrc.com/store/teensy40.html.  The Heltec Lora dev board setup is here: https://docs.heltec.org/en/dev_kits/esp32_arduino/index.html.   A BME280 temperature sensor can be found at Adafruit, Sparkfun, or Amazon.   
   
Nine connections need be made, excluding power and ground to the Teensy and ESP32.  Those come from the USB hub, but the BME280 also needs referenced to the Teensy.   Note, against perfect practice, I did also connect their grounds locally through the BME280 ground creating a small loop.      
    From                             To      
1  BME280 Pwr --------------------- +3.3V on Teensy.   Not 5V!   BME280 is 3.3V
2  BME280 GND --------------------- Teensy ground.
3  BME280 I2C SCK (clock) --------  Teensy GPIO 19 (I2C SCK)   
4  BME280 SDA (data) ---------------Teensy GPIO 18 (I2C SDA)
5  Heltech Uart serial Tx --------- Teensy UART 2 Rx    Note, Tx to Rx 
6  Heltech Uart serial Rx --------- Teensy UART 2 Tx    Note: Rx to Tx
7  DG90 RC motor, Red ----------- +5V on Teensy  
8  DG90 RC motor, Brn ----------- GND on Teensy  
9  DG90 RC motor, Orn ----------- GPIO2 on Teensy

This project came together because I was not satisfied with the WiFi compatible temperature sensors available online.  Using several of those sensors and comparing readings, it was difficult to acheive temperature readings within 1 degree C of each other.   That's a big error considering the ultimate application.  And there was a motive for those sensors, to ultimately do smarter thermal management of a home.   I had simply not found Alexa or some website to allow integration of various sensors and smart switches the way I want.   So, rather than just throw more money at more expensive sensors and cross my fingers, I used about $50 worth of parts.   This gives a platform on which one can reliably expand to act as a digital thermostat, a control for ventilating fans, and/or heat pump.  The key advantage is a variety of calculations can be made such as to activate heating or cooling systems at a sensible time relative to combinations of local conditions.

Where from here?   
1 Figure out how to input parameters from a greatly improved webpage back through the ESP32 to the Teensy, which will then schedule the daily desired temperature profiles.
2 Hack some of those WiFi controlled switches so the ESP32 can use them to start a coffeemaker, lights, etc.
3 Delve into HTML & CSS, possibly python, to allow the ESP32 to serve up nicer webpages.   
4 Incorporate "real feel" temperature calculation into the ESP32
So the goal here really is to be less dependant on the various smartphone apps, and be able to just take a higher level of control over my WiFi system.  

