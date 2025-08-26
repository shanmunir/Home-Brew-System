#include <PID_v1.h>
#include <EEPROM.h>
#include <HX711.h>
#define HEATER_SENSOR_PIN 11

// Variables defined:
HX711 Scale;
const int Interval = 60000;
int TimeMinutes = 0;
int TimeSeconds = 0;
float CalculateTime = 0;
double Weight = 38448;
double Volume = 36000;
float SG = Weight / Volume;
const int PRE_BOIL_TIME = 60;
double VolumeLoss = 0;
double WeightLoss = 0;
double Input, Output;
double Kp = 2.0;
double Ki = 5.0;
double Kd = 1.0;
double SetPoint = 100.0;
const int TotalVolumeLoss = 4921;
bool notRunning = true;
double CURRENT_BOIL_TIME = 0;

// PID module to maintain the temperature to reach at specific time on pre-boil:
PID BOIL_PID(&Input, &Output, &SetPoint, Kp, Ki, Kd, DIRECT);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(HEATER_SENSOR_PIN, OUTPUT);
  BOIL_PID.SetMode(AUTOMATIC);

  // Display the specific gravity:
  Serial.print("Specific Gravity = ");
  Serial.print(SG);
  Serial.println(" Kg/m³");

  // Write the Specific gravity value in the memory:
  EEPROM.write(1, SG);

  // Calculate the Volume and Weight loss:
  Weight = Scale.read();

  // Add one minute delay for calculation of volume and weight loss:
  delay(60000);

  // Again calculate the volume and weight loss:
  double tempWeight = Scale.read();
  WeightLoss = Weight - tempWeight;
  VolumeLoss = WeightLoss;
  PrintWeightLoss();

  Serial.print("Time to reach the After-boil volume: ");
  TimeRequiredForVolumeBoil();

  if (CURRENT_BOIL_TIME > PRE_BOIL_TIME) {
    // PID Request Started for Heating the Element:
    PID_Request();
  }
}

void loop() {
  // put your main code here, to run repeatedly :
  unsigned long CurrentMillis = millis();
  TimeMinutes = 0;
  TimeSeconds = 0;

  // Calculate the time of pre-boil:  
  int CurrentTime = PRE_BOIL_TIME * 60000;
  CurrentTime -= CurrentMillis;
  CalculateTime = CurrentTime;
  double CalculatedTime = 0;

  if (CalculateTime > 0) {
    // Calculate the Time to reach the pre-boil volume:
    PrintSpecificGravity();

    // Calculate the Time for the boil:
    while (CalculatedTime <= CalculateTime) {
      TimeSeconds += 1;
      CalculatedTime += 1000;
      if (TimeSeconds >= 59) {
        TimeMinutes += 1;
        TimeSeconds = 0;
      }
    }
    PrintCalculatedTime();
    Serial.println();
  } else {      
    return;
  }

  // Add a 30 second delay to process the program:
  delay(1000);
}

void AdjustHeatingElement(int output) {
  // Set the pwm value by reading the map value
  int pwmValue = map(output, 0, 255, 0, 1023);

  // Write the pwm value to heater pin to ajust heating temperature:
  analogWrite(HEATER_SENSOR_PIN, pwmValue);
}

void PID_Request() {
  // Compute the PID:
  BOIL_PID.Compute();

  // Adjust the heating element:
  AdjustHeatingElement(Output);
}

void TimeRequiredForVolumeBoil() {
  // Calculate the Time required for the boil to reach the boil-volume:
  CURRENT_BOIL_TIME = TotalVolumeLoss / WeightLoss;
  Serial.println(CURRENT_BOIL_TIME);
}

void PrintSpecificGravity() {
  // print weight with its unit:
  Serial.print("Weight = ");
  Serial.print(Weight);
  Serial.println(" grams");

  // Calculate the Specific Gravity with the unit in serail port:
  Serial.print("Specific Gravity = ");
  SG = Weight / Volume;
  Serial.print(SG);
  Serial.println(" kg/m³");
}

void PrintCalculatedTime() {
  // Display the Time to reach the boil volume:
  Serial.print("Calculated Time = ");
  Serial.print(TimeMinutes);
  Serial.print(" Min. ");
  Serial.print(TimeSeconds);
  Serial.println(" Sec.");
}

void PrintWeightLoss() {
  // The current weight loss:
  Serial.print("Weight Loss= ");
  Serial.print(WeightLoss);
  Serial.println(" grams(g)");

  // Current Volume loss:
  Serial.print("Volume Loss= ");
  Serial.print(VolumeLoss);
  Serial.println(" Litre(L)");
}