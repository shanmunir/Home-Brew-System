#include "OneButton.h"
#include <HX711.h>
#include <PID_v1.h>


#define HEATER_SENSOR_PIN 11
#define SCAVENGER_WEIGHT 1.5
#define LID_WEIGHT 0.9
#define STIR_INTERVAL 20 * 60 * 100

HX711 Scale;
double CurrentWeight = 0;
double PreviousWeight = 0;
bool lidRemoved = false;
bool scavengerAdded = false;
bool grainsAdded = false;
unsigned long laststirtime = 0;
double Input, Output;
double SetPoint = 0;
double Kp, Ki, Kd;

PID Mypid(&Input, &Output, &SetPoint, Kp, Ki, Kd, DIRECT);
enum ProcessState { HEATING,
                    WAITING_FOR_GRAIN,
                    MASHING };
ProcessState state = HEATING;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Read the previous weight before tare the scale.
  double previousWeight = Scale.read();

  // Initialize scale:
  Scale.get_scale();
  Scale.tare();

  // Initialize PID:
  Mypid.SetMode(AUTOMATIC);
}

void loop() {
  // put your main code here, to run repeatedly:
  CurrentWeight = Scale.read();

  switch (state) {
    case HEATING:
      if (Mypid.Compute()) {
        AdjustHeatingElement(Output);
        if (Output > SetPoint) {
          state = WAITING_FOR_GRAIN;

          // add one minute delay for adding the scevenger grains
          delay(60000);
          grainsAdded = true;
        }
      }
      break;
    case WAITING_FOR_GRAIN:
      if (grainsAdded) {
        state = MASHING;
      }
      break;
    case MASHING:
      if (millis() - laststirtime > STIR_INTERVAL) {
        laststirtime = millis();
      }

      float weightChange = CurrentWeight - PreviousWeight;
      // Check for lid removal or replacement
      if (fabs(weightChange) >= LID_WEIGHT * 0.9 && fabs(weightChange) <= LID_WEIGHT * 1.1) {
        lidRemoved = !lidRemoved;  // Toggle lid status
      }

      // Check for The Scavenger addition
      if (!scavengerAdded && fabs(weightChange) >= SCAVENGER_WEIGHT * 0.95 && fabs(weightChange) <= SCAVENGER_WEIGHT * 1.05) {
        scavengerAdded = true;
        Scale.tare();  // Automatically tare out the weight of The Scavenger
      }

      PreviousWeight = CurrentWeight;
      break;
  }
}

void AdjustHeatingElement(double output) {
  double pwmValue = map(output, 0, 1023, 0, 255);
  analogWrite(HEATER_SENSOR_PIN, pwmValue);
}