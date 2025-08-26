// Specific Gravity Module.
// There will be the brewer defined specific at the start.
#include <HX711.h>
HX711 Scale;
float SpecificGravity = 0;
double VolumeDensity = 36000;
double StartingWeight = 38448;
double CurrentWeight = 0.0;
bool IsUserRequestSpecificGravity = true;
int MilliCount = 10000;
double WeightLoss = 0;
double VolumeLoss = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Calculate the Specifi gravity:
  SpecificGravity = StartingWeight / VolumeDensity;

  // Print the current specific gravity:
  Serial.print("SpecificGravity = ");
  Serial.println(SpecificGravity);

  // read weight through scale:
  CurrentWeight = Scale.read();

  // Add a one minute delay to read the Weight and volume loss:
  delay(60000);

  // Again read the weight to determine the weight loss:
  double tempWeight = Scale.read();
  WeightLoss = CurrentWeight - tempWeight; // it calculates the weight loss.
  VolumeLoss = WeightLoss;
  VolumeDensity -= VolumeLoss;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long CurrentMillis = millis();

  //Calculate specific gravity after the boil:
  SpecificGravity = StartingWeight / VolumeDensity;

  // Add specifi volume and weight decrease after a minute and set millis to increase by one minute.
  if (IsUserRequestSpecificGravity == true) {
    SpecificGravity = CurrentWeight / VolumeDensity;
    if (CurrentMillis >= MilliCount) {
      MilliCount += 10000;
      CurrentWeight -= WeightLoss;
      VolumeDensity -= VolumeLoss;
      SpecificGravity = StartingWeight / VolumeDensity;

      // print current weight and specific gravity of the boil:
      Serial.print("Current Weight = ");
      CurrentWeight = Scale.read();
      Serial.println(CurrentWeight);
      Serial.print("Volume Quantity = ");
      Serial.println(VolumeDensity);r
      Serial.print("Specific Gravity = ");
      Serial.println(SpecificGravity);
      Serial.println("");
    }
  }
  delay(1000);
}
