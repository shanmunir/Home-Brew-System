#include <PID_v1.h>
#include <HX711.h>
#include <Adafruit_MAX31865.h>

#define HEATER_PIN 11
#define PUMP_PIN 24
#define VALVE_PIN 9

#define MASHING_TIME_MINUTES 60
#define MASHOUT_TIME_MINUTES 10
#define BOILING_TIME_MINUTES 90
#define FERMENTATION_TIME_HOURS 120
#define PUMP_TURNED_OFF_TIME_MINUTES 50

HX711 Scale;
double WortVolume = 34196;
const int Interval = 60000;
unsigned long PreviousMillis;
double Weight = 0;
int Temperature;
double Input, Output;
double Kp = 2.0;
double Ki = 5.0;
double Kd = 1.0;
double SetPoint = 68.0;
double MashOutSetPoint = 75.0;
double BoilingSetPoint = 100.0;
double FermentationSetPoint = 20.0;


enum BrewState {
  IDLE,
  MASHING,
  MASHOUT,
  BOILING,
  FERMENTATION,
};
BrewState CurrentState = IDLE;

PID Boiling_PID(&Input, &Output, &BoilingSetPoint, Kp, Ki, Kd, DIRECT);
PID MashOut_PID(&Input, &Output, &MashOutSetPoint, Kp, Ki, Kd, DIRECT);
PID Mashing_Pid(&Input, &Output, &SetPoint, Kp, Ki, Kd, DIRECT);
PID Fermentation_PID(&Input, &Output, &FermentationSetPoint, Kp, Ki, Kd, DIRECT);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //RTD.begin();
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);
  Mashing_Pid.SetMode(AUTOMATIC);
  MashOut_PID.SetMode(AUTOMATIC);
  Boiling_PID.SetMode(AUTOMATIC);
  Fermentation_PID.SetMode(AUTOMATIC);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long CurrentMillis = millis();
  Weight = Scale.read();
  Temperature = ReadTemperature();

  Serial.print("WEIGHT = ");
  Serial.println(Weight);
  Serial.print("TEMPERATURE = ");
  Serial.println(Temperature);

  Input = Temperature;

  switch (CurrentState) {
    case IDLE:
      if (HomeBrewProcessStarted()) {
        CurrentState = MASHING;
      }
      break;
    case MASHING:
      if (CurrentMillis - PreviousMillis >= Interval) {
        Mashing_Pid.Compute();
        AdjustHeatingElement(Output);
        ControlPumpSpeed(Output);
        PreviousMillis = CurrentMillis;
        if (Temperature >= SetPoint) {
          digitalWrite(VALVE_PIN, LOW);
        }
        if (CurrentMillis >= PUMP_TURNED_OFF_TIME_MINUTES * 60000) {
          digitalWrite(PUMP_PIN, LOW);
          digitalWrite(VALVE_PIN, HIGH);
        }
        if (CurrentMillis >= MASHING_TIME_MINUTES * 60000) {
          PreviousMillis = CurrentMillis;
          CurrentState = MASHOUT;
        }
      }
      break;
    case MASHOUT:
      if (CurrentMillis - PreviousMillis >= Interval) {
        MashOut_PID.Compute();
        AdjustHeatingElement(Output);
        PreviousMillis = CurrentMillis;
        if (CurrentMillis >= MASHOUT_TIME_MINUTES * 60000) {
          PreviousMillis = CurrentMillis;
          CurrentState = BOILING;
        }
      }
      break;
    case BOILING:
      if (CurrentMillis - PreviousMillis >= Interval) {
        Boiling_PID.Compute();
        AdjustHeatingElement(Output);
        PreviousMillis = CurrentMillis;
        if (CurrentMillis >= BOILING_TIME_MINUTES * 60000) { 
          PreviousMillis = CurrentMillis;
          CurrentState = FERMENTATION;
        }
      }
      break;
    case FERMENTATION:
      if (CurrentMillis - PreviousMillis >= Interval) {
        PreviousMillis = CurrentMillis;
        Fermentation_PID.Compute();
        AdjustHeatingElement(Output);
        if (CurrentMillis >= FERMENTATION_TIME_HOURS * 3600000) {
          PreviousMillis = CurrentMillis;
          CurrentState = IDLE;
        }
      }
      break;
  }
}

int ReadTemperature() {
  int rawValue = analogRead(HEATER_PIN);
  float Temperature_C = rawValue * (5.0 / 1023.0) * 100.0;
  return int(Temperature_C);
}

void AdjustHeatingElement(double output) {
  int pwmValue = map(output, 0, 255, 0, 1023);
  analogWrite(HEATER_PIN, pwmValue);
}

void ControlPumpSpeed(double output) {
  int pwmValue = map(output, 0, 255, 0, 255);
  analogWrite(PUMP_PIN, pwmValue);
}

bool HomeBrewProcessStarted() {
  if (Weight >= WortVolume) {
    return true;
  }
}