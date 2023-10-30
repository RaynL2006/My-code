#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 4;
float swi = 0;
int plant_type = 0;
int moist = 0;
int moistper = 0;
int water_level = 0;
int light = 0;
int lightper = 0;
String refill_necessary = "Loading...";
int beeps_remaining = 0;
int seconds_of_alarm = 0;

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

AdafruitIO_Feed *moisture = io.feed("moisture");
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *luminance = io.feed("luminance");
AdafruitIO_Feed *refill = io.feed("refill");

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();
  pinMode(27, OUTPUT);
  pinMode(32, INPUT);
  pinMode(26, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(13, OUTPUT);
  ledcSetup(0, 1E5, 12);
  ledcAttachPin(25, 0);
  
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {
  // put your main code here, to run repeatedly:
  io.run();

  //Temperature
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println(" C");
  temperature->save(temperatureC); 

  //light
  light = analogRead(39);
  lightper = map(light, 0, 4095, 0, 100);
  Serial.print("light level: ");
  Serial.print(lightper);
  Serial.println("%");
  luminance->save(lightper);

  // Select plant type section (1.2)
  swi = digitalRead(32);
  if (swi > 0.5) {
    plant_type = 1;
    digitalWrite(27, LOW);
    digitalWrite(26, HIGH);
  }
  else if (swi < 0.5) {
    plant_type = 0;
    digitalWrite(27, HIGH);
    digitalWrite(26, LOW);
  }

  //moisture
  moist = analogRead(34);
  moistper = map(moist, 3650, 1300, 0, 100);
  Serial.print("moisture: ");
  Serial.print(moistper);
  Serial.println("%");
  moisture->save(moistper);

  // Water reservoir level sense section (8.5)
  water_level = analogRead(36);
  Serial.print("Refill necessary: ");
  Serial.println(refill_necessary);
  refill->save(refill_necessary);

  // Water plant with pump section (1.1)
  if (water_level < 1930) {
    digitalWrite(13, LOW);
    refill_necessary = "No";
    ledcWrite(0, 0);
    beeps_remaining = 0;
    seconds_of_alarm = 0;
    
    if (plant_type == 0) {
      
      if (moistper <= 10) {
        digitalWrite(21, HIGH);
      }
      else if (moistper >= 50) {
        digitalWrite(21, LOW);
      }
    }
  
    if (plant_type == 1) {
    
      if (moistper <= 50) {
        digitalWrite(21, HIGH);
      }
      else if (moistper >= 90) {
        digitalWrite(21, LOW);
      }
    }
    delay(10000);
  }
  // Alert user if water reservoir is too low section (8.5)
  else {
    refill_necessary = "Yes";
    digitalWrite(21, LOW);
    digitalWrite(27, LOW);
    digitalWrite(26, LOW);
    
    digitalWrite(13, HIGH);
    if (beeps_remaining > 0) {
      ledcWriteTone(0, 800);
      beeps_remaining = beeps_remaining - 1;
    }
    delay(1000);
    digitalWrite(13, LOW);
    ledcWrite(0, 0);
    delay(1000);
    digitalWrite(13, HIGH);
    if (beeps_remaining > 0) {
      ledcWriteTone(0, 800);
      beeps_remaining = beeps_remaining - 1;
    }
    delay(1000);
    digitalWrite(13, LOW);
    ledcWrite(0, 0);
    delay(1000);
    digitalWrite(13, HIGH);
    if (beeps_remaining > 0) {
      ledcWriteTone(0, 800);
      beeps_remaining = beeps_remaining - 1;
    }
    delay(1000);
    digitalWrite(13, LOW);
    ledcWrite(0, 0);
    delay(1000);
    digitalWrite(13, HIGH);
    if (beeps_remaining > 0) {
      ledcWriteTone(0, 800);
      beeps_remaining = beeps_remaining - 1;
    }
    delay(1000);
    digitalWrite(13, LOW);
    ledcWrite(0, 0);
    delay(1000);
    digitalWrite(13, HIGH);
    if (beeps_remaining > 0) {
      ledcWriteTone(0, 800);
      beeps_remaining = beeps_remaining - 1;
    }
    delay(1000);
    digitalWrite(13, LOW);
    ledcWrite(0, 0);
    delay(1000);
    
    if (seconds_of_alarm % 600 == 0) {
      beeps_remaining = 5;
    }
    seconds_of_alarm = seconds_of_alarm + 10;
    if (seconds_of_alarm == 610) {
      seconds_of_alarm = 10;
    }
  }
  Serial.print(seconds_of_alarm);
  Serial.println(" seconds");
  Serial.print(beeps_remaining);
  Serial.println(" beeps remaining");

}
