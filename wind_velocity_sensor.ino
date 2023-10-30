#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <firefly_can.h>

//Constants
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
float pi = 3.14159;
float SPECIFIC_GAS_CONST_DRY = 287.058;
float SPECIFIC_GAS_CONST_WATERVAP = 461.495;
float wheel_dia = 0.425;
int driver_gearT = 36;
int driven_gearT = 107;


//Variables
float temp = 0;
float tempK = 0;
float patm = 0;
float humid = 0;
float Volt = 0;
float pdiff = 0;
int neg = 0;
float psat = 0;
float pvap = 0;
float pdry = 0;
float airdense = 0;
float velocity = 0;
float pitot = 0;
float yaw = 0;
float yawV = 0; 
float yawpdiff = 0;
float yawneg = 0;
float yawvelocity = 0;
float ptaps = 0;
float ptapsV = 0;
float ptapspdiff = 0;
float ptapsneg = 0;
float ptapsvelocity = 0;
int headw;
float ground_velocity = 0;
int driver_gear_rpm = 0;
int wheel_rpm = 0;
float wheel_rps = 0;
float wheel_circ = 0;

void setup() {
  Serial.begin(9600);
  FireflyCAN::init(FEATHER_CAN);

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

//getting wheel axle speed
void rpm_data_handler(FireflyCAN::axle_data *data) {
  driver_gear_rpm = data->axle_rpm;

}

void loop()
{
  //BME and derived air density
  temp = bme.readTemperature();
  tempK = temp + 273;
  Serial.print(temp);
  Serial.println("Celsius");

  patm = bme.readPressure();
  Serial.print(patm);
  Serial.println("Pa");

  humid = bme.readHumidity();
  humid /= 100;
  Serial.print(humid);
  Serial.println("humidity");

  psat = 0.61078 * exp(17.27*temp/(temp + 237.3)); //Using Teten's equation to determine saturation pressure. 
  psat *= 1000; // going from kPa to Pa
  pvap = humid * psat;
  pdry = patm - pvap;
  airdense = (pdry/(SPECIFIC_GAS_CONST_DRY*tempK)) + (pvap/(SPECIFIC_GAS_CONST_WATERVAP*tempK)); //calculating density of humid air by treating it as a mixture of ideal gases (water vapour and dry air) and using ideal gas equation
  Serial.print(airdense);
  Serial.println("air density");
  
//pitot tube calculations
  pitot = analogRead(A0) + 1.92; //correcting for offset in pressure sensor. When pdiff = 0, pitot = 40.92
  Serial.print(pitot);
  Serial.println("analogue reading");
  Volt = pitot * (3.3/1023);
  pdiff = 1000 * ((100 * Volt - 13.2)/29.7); //using line equation taken from graph on datasheet
  if (pdiff < 0) {
    neg = 0;
  }
  else {
    neg = 1; //remembering whether differential pressure is pos or neg
  }
  pdiff = abs(pdiff); // to avoid squarerooting negative numbers
  velocity = sqrt(2*pdiff/airdense); //using bernoulli's equation.
  if (neg == 1) {
    velocity *= (-1); //making velocity negative if original value was negative

  }

//yaw probe calculations
  yaw = analogRead(A1) + 1.92; //same as for pitot
  yawV = yaw * (3.3/1023);
  yawpdiff = 1000 * ((100 * yawV - 13.2)/29.7);
  if (yawpdiff < 0) {
    yawneg = 1;
  }
  else {
    yawneg = 0;
  }
  yawpdiff = abs(yawpdiff);
  yawvelocity = sqrt(2*yawpdiff/airdense);
  if (yawneg == 1) {
    yawvelocity *= (-1);
  }

//pressure taps calculations
  ptaps = analogRead(A2) + 1.92; //same as for pitot
  ptapsV = ptaps * (3.3/1023);
  ptapspdiff = 1000 * ((100 * ptapsV - 13.2)/29.7);
  if (ptapspdiff < 0) {
    ptapsneg = 1;
  }
  else {
    ptapsneg = 0;
  }
  ptapspdiff = abs(ptapspdiff);
  ptapsvelocity = sqrt(2*ptapspdiff/airdense);
  if (ptapsneg == 1) {
    ptapsvelocity *= (-1);
  }

//sending and receiving CAN, and calculating ground speed
  FireflyCAN::register_msg_handler(CANMessages::axle_data, rpm_data_handler);
  wheel_rpm = (driver_gearT * driver_gear_rpm) / driven_gearT;
  wheel_rps = wheel_rpm / 60;
  wheel_circ = pi * wheel_dia;
  ground_velocity = wheel_circ * wheel_rps;

  headw = velocity - ground_velocity;

  auto msg = FireflyCAN::pitot {
  .headwind = headw,
  .yaw_crosswind = yawvelocity,
  .ptaps_crosswind = ptapsvelocity
  };
  FireflyCAN::send_msg(msg);

// Serial printing
  Serial.print("pitot velocity: ");
  Serial.println(velocity);
  Serial.println("  ");
  Serial.print("yaw probe velocity: ");
  Serial.println(yawvelocity);
  Serial.println("  ");
  Serial.print("pressure taps velocity: ");
  Serial.println(ptapsvelocity);
  Serial.println("  ");
  delay(1000);
  
}
