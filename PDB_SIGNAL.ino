#include <Wire.h>
#include <Adafruit_SI5351.h>
#include <HardwareSerial.h>

#define UART2_TX_PIN 17
#define UART2_RX_PIN 16

HardwareSerial uart2(2);
Adafruit_SI5351 si5351 = Adafruit_SI5351();

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize UART communication for display
  uart2.begin(9600, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN);

  // Initialize I2C communication
  Wire.begin();  // Use default SDA/SCL pins managed by your microcontroller

  // Initialize SI5351
  if (!si5351.begin()) {
    Serial.println("Couldn't find the SI5351 clock generator!");
    while (1);
  } 
  Serial.println("SI5351 initialized!");

  // Configure PLL A with a multiplier of 36 (e.g., for a 900 MHz PLL output)
  // Here num = 0, denom = 1, so it's a whole number multiplier
  si5351.setupPLL(SI5351_PLL_A, 36, 0, 1);

  // Configure output 0 to divide PLL A to 25 MHz
  si5351.setupMultisynth(0, SI5351_PLL_A, 36, 0, 1);  // 900 MHz / 36 = 25 MHz

  // Enable output
  si5351.enableOutputs(true);

  Serial.println("Clock signal configured.");

  // Send PDB signal
  SendPDBSignal();

  // add 200ms delay for processing.
  delay(200);
}

void loop() {
  // Put the main code here. Running in the loop.
}