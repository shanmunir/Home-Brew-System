// This module will rectify the chilling intensity of boil procedure
unsigned long PreviousMillis = 0;
const int Interval = 60000;
enum BrewState {
  IDLE,
  MASHING,
  MASHOUT,
  BOILING,
  FERMENTATION,
};
enum BrewStateTimeMinutes
{
  MASHING_TIME_MINUTES = 60,
  MASHOUT_TIME_MINUTES = 15,
  BOILING_TIME_MINUTES = 90,
  FERMENTATION_TIME_HOURS = 120 
};
BrewState CurrentState = IDLE;
unsigned long CountDownTimer = MASHING_TIME_MINUTES;
unsigned long TimerMillis = 0;
int seconds = 60;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long CurrentMillis = millis();

  // Set the process for the timer count down.
  switch (CurrentState) {
    case IDLE:
      if (CurrentMillis - PreviousMillis >= Interval) {
        CurrentState = MASHING;
      }
      break;
    case MASHING:
      if (CurrentMillis - PreviousMillis >= Interval) {
        CountDownTimer = MASHING_TIME_MINUTES;
        PreviousMillis = CurrentMillis;
        if (CurrentMillis >= MASHING_TIME_MINUTES * 60000) {
          PreviousMillis = CurrentMillis;
          CurrentState = MASHOUT;
        }
      }
      break;
    case MASHOUT:
      if (CurrentMillis - PreviousMillis >= Interval) {
        CountDownTimer = MASHOUT_TIME_MINUTES;
        PreviousMillis = CurrentMillis;
        if (CurrentMillis >= MASHOUT_TIME_MINUTES * 60000) {
          PreviousMillis = CurrentMillis;
          CurrentState = BOILING;
        }
      }
      break;
    case BOILING:
      if (CurrentMillis - PreviousMillis >= Interval) {
        CountDownTimer = BOILING_TIME_MINUTES;
        PreviousMillis = CurrentMillis;
        if (CurrentMillis >= BOILING_TIME_MINUTES * 60000) {
          PreviousMillis = CurrentMillis;
          CurrentState = FERMENTATION;
        }
      }
      break;
    case FERMENTATION:
      if (CurrentMillis - PreviousMillis >= Interval) {
        CountDownTimer = FERMENTATION_TIME_HOURS * 60;
        PreviousMillis = CurrentMillis;
        if (CurrentMillis >= FERMENTATION_TIME_HOURS * 3600000) {
          PreviousMillis = CurrentMillis;
          CurrentState = IDLE;
        }
      }
      break;
  }

  //Set the timer based on the current millis:
  if (CurrentMillis >= TimerMillis) {
    CountDownTimer--;
    TimerMillis += 60000;
  }

  // Assign the seconds value if reached 0.
  if (seconds <= 0) {
    seconds = 59;
  } else {
    seconds -= 1;
  }

  // Print the countdown of the process.
  Serial.println(CurrentMillis);
  Serial.print("Countdown Timer: ");
  Serial.print(CountDownTimer);
  Serial.print(":");
  Serial.println(seconds);
  Serial.println("");

  // Add one second delay before continue the loop.
  delay(1000);
}
