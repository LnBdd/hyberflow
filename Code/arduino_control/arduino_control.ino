// Includes
#include <Arduino.h>
#include <SensirionI2cSf06Lf.h>
#include <Wire.h>
#include <TimeLib.h>
#include <curveFitting.h>

// Sensor and Highdriver setup
SensirionI2cSf06Lf sensor;

// I2C Highdriver constants
#define I2C_HIGHDRIVER_ADRESS (0x7B) // Default address for Highdriver
#define I2C_DEVICEID 0x00
#define I2C_POWERMODE 0x01
#define I2C_FREQUENCY 0x02c
#define I2C_SHAPE 0x03
#define I2C_BOOST 0x04
#define I2C_PVOLTAGE 0x06
#define I2C_P1VOLTAGE 0x06
#define I2C_P2VOLTAGE 0x07
#define I2C_P3VOLTAGE 0x08
#define I2C_P4VOLTAGE 0x09
#define I2C_UPDATEVOLTAGE 0x0A
#define I2C_AUDIO 0x05

// Constants for control logic
#define NUM_SAMPLES 15 // Window size for moving average
#define SPTR_SIZE 20

// Global variables
char myString;
char newline = "\n";
char *sPtr[SPTR_SIZE];
int freq = 200; // Default frequency
int main_pump = 0;
int support_pump = 1;
static char errorMessage[32];
static int16_t error;
float sensorValues[NUM_SAMPLES]; // Array to store recent sensor values
int currentIndex = 0; // Current index in the array
int support_amp;
float input = 0.0; // Current measured value
float output;
float previous_output;
float sensorValue = 0.0;
int new_ampl;
int counter = 0;

int new_support_amp;
int old_support_amp = 30;
float slope;
float ampl_feedforward;
float tolerance_min;
float tolerance_max;
float tolerance = 0.05;
float current_target_speed = 0.0;
float elapsed_time = 0.0;
float total_time;
float diff, integral = 0.0, prev_diff = 0.0;
float derivative;

bool topUpSyringe = false;

// Relay pin for flow direction control
const int relayPin = 11;

// Timing variables
unsigned long previousMillisSerial = 0;
unsigned long previousMillisControl = 0;
const long intervalSerial = 200;
const long intervalControl = 200;

// Controller tuning parameters
double kp = 0.014; // Proportional gain
double ki = 0.008; // Integral gain
double kd = 0.0; // Derivative gain

bool pumpState = false;

// Highdriver state variables
boolean bPumpState[4] = {false, false, false, false};
uint8_t nPumpVoltageByte[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t nFrequencyByte = 0x40;

// Function declarations
void print_byte_array(uint8_t* array, uint16_t len);
bool Highdriver_check(void);
void Highdriver_init(void);
void Highdriver4_setvoltage(uint8_t _pump, uint8_t _voltage);
void Highdriver4_setvoltage(void);
void Highdriver_setfrequency(uint16_t _frequency);
float createMovingAverage(float sensorValue);
void applyOutput(float output);
float Regelung(float diff);
void vorsteuerung_setup();
float vorsteuerung(int soll_vor);
float checkSerialData();
void controlFunction(float target_speed, float dt);
void setup();
void loop();

// Function implementations

/**
 * Print a byte array in hexadecimal format.
 * 
 * @param array Pointer to the byte array.
 * @param len Length of the byte array.
 */
void print_byte_array(uint8_t* array, uint16_t len) {
  Serial.print("0x");
  for (uint16_t i = 0; i < len; i++) {
    Serial.print(array[i], HEX);
  }
}

/**
 * Check if the Highdriver is connected by verifying its device ID.
 * 
 * @return True if the Highdriver is detected, false otherwise.
 */
bool Highdriver_check(void) {
  Wire.beginTransmission(I2C_HIGHDRIVER_ADRESS);
  Wire.write(I2C_DEVICEID);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(I2C_HIGHDRIVER_ADRESS, 1);
  uint8_t data = Wire.read();
  return ((data) == 0xB2);
}

/**
 * Initialize the Highdriver with default settings.
 * Sets power mode, frequency, wave shape, and amplitudes.
 */
void Highdriver_init(void) {
  Wire.beginTransmission(I2C_HIGHDRIVER_ADRESS);
  Wire.write(I2C_POWERMODE); // Start Register 0x01
  Wire.write(0x01); // Register 0x01 = 0x01 (enable)
  Wire.write(nFrequencyByte); // Register 0x02 = 0x40 (100Hz)
  Wire.write(0x00); // Register 0x03 = 0x00 (sine wave)
  Wire.write(0x00); // Register 0x04 = 0x00 (800KHz)
  Wire.write(0x00); // Register 0x05 = 0x00 (audio off)
  Wire.write(0x00); // Register 0x06 = Amplitude1
  Wire.write(0x00); // Register 0x07 = Amplitude2
  Wire.write(0x00); // Register 0x08 = Amplitude3
  Wire.write(0x00); // Register 0x09 = Amplitude4
  Wire.write(0x01); // Register 0x0A = 0x01 (update)
  Wire.endTransmission();
  bPumpState[3] = false;
  nPumpVoltageByte[3] = 0x1F;
}

/**
 * Set the voltage for a specific pump on the Highdriver.
 * 
 * @param _pump The pump index (0-3).
 * @param _voltage The voltage value to set (scaled to 250Vpp).
 */
void Highdriver4_setvoltage(uint8_t _pump, uint8_t _voltage) {
  float temp = _voltage;
  temp *= 31.0f;
  temp /= 250.0f; // 250Vpp = 0x1F
  if (_pump >= 0 && _pump <= 3) nPumpVoltageByte[_pump] = constrain(temp, 0, 31);
  Wire.beginTransmission(I2C_HIGHDRIVER_ADRESS);
  Wire.write(I2C_PVOLTAGE);
  Wire.write((bPumpState[0] ? nPumpVoltageByte[0] : 0));
  Wire.write((bPumpState[1] ? nPumpVoltageByte[1] : 0));
  Wire.write((bPumpState[2] ? nPumpVoltageByte[2] : 0));
  Wire.write((bPumpState[3] ? nPumpVoltageByte[3] : 0));
  Wire.write(0x01); // Update new driver entries
  Wire.endTransmission();
}

/**
 * Update the voltage for all pumps on the Highdriver.
 */
void Highdriver4_setvoltage(void) {
  Wire.beginTransmission(I2C_HIGHDRIVER_ADRESS);
  Wire.write(I2C_PVOLTAGE);
  Wire.write((bPumpState[0] ? nPumpVoltageByte[0] : 0));
  Wire.write((bPumpState[1] ? nPumpVoltageByte[1] : 0));
  Wire.write((bPumpState[2] ? nPumpVoltageByte[2] : 0));
  Wire.write((bPumpState[3] ? nPumpVoltageByte[3] : 0));
  Wire.write(0x01); // Update new driver entries
  Wire.endTransmission();
}

/**
 * Set the operating frequency for the Highdriver.
 * 
 * @param _frequency The desired frequency in Hz.
 */
void Highdriver_setfrequency(uint16_t _frequency) {
  if (_frequency >= 800) {
    nFrequencyByte = 0xFF;
  } else if (_frequency >= 400) { // Range 400-800 Hz
    _frequency -= 400;
    _frequency *= 64;
    _frequency /= 400;
    nFrequencyByte = _frequency | 0xC0;
  } else if (_frequency >= 200) { // Range 200-400 Hz
    _frequency -= 200;
    _frequency *= 64;
    _frequency /= 200;
    nFrequencyByte = _frequency | 0x80;
  } else if (_frequency >= 100) { // Range 100-200 Hz
    _frequency -= 100;
    _frequency *= 64;
    _frequency /= 100;
    nFrequencyByte = _frequency | 0x40;
  } else if (_frequency >= 50) { // Range 50-100 Hz
    _frequency -= 50;
    _frequency *= 64;
    _frequency /= 50;
    nFrequencyByte = _frequency | 0x00;
  } else { // Out of frequency boundary
    nFrequencyByte = 0x00;
  }
  Wire.beginTransmission(I2C_HIGHDRIVER_ADRESS);
  Wire.write(I2C_FREQUENCY);
  Wire.write(nFrequencyByte);
  Wire.endTransmission();
}

/**
 * Create a moving average of the last NUM_SAMPLES sensor values.
 * 
 * @param sensorValue The latest sensor value to include in the average.
 * @return The calculated moving average.
 */
float createMovingAverage(float sensorValue) {
  sensorValues[currentIndex] = sensorValue;
  currentIndex = (currentIndex + 1) % NUM_SAMPLES;
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += sensorValues[i];
  }
  counter += 1;
  long divisor = fabs(min(counter, NUM_SAMPLES));
  float movingAverage = sum / (float)divisor;
  return movingAverage;
}

/**
 * Apply the calculated output voltage to the main pump.
 * Ensures the voltage is within the allowed range (0-250V).
 * 
 * @param output The calculated output voltage.
 */
void applyOutput(float output) {
  new_ampl = (int)(output + .5);
  if (new_ampl <= 0) {
    new_ampl = 0;
  } else if (new_ampl >= 250) {
    new_ampl = 250;
  }
  Highdriver4_setvoltage(main_pump, new_ampl);
}

/**
 * PI controller logic to calculate the output based on the error (difference).
 * 
 * @param diff The difference between the target and actual values.
 * @return The calculated output value.
 */
float Regelung(float diff) {
  integral += diff; // Calculate integral
  output = kp * diff + ki * integral;
  return output;
}

/**
 * Perform a setup for feedforward control.
 * Measures flow rates at different voltage levels and performs linear regression
 * to calculate the slope for feedforward control.
 */
void vorsteuerung_setup() {
  double Data_Flow[13];
  double Data_Ampl[13];
  int Data_count = 0;
  Highdriver_setfrequency(freq);
  delay(20);
  int setup_ampl = 10;
  bPumpState[main_pump] = true;
  while (setup_ampl <= 250) {
    Highdriver4_setvoltage(main_pump, setup_ampl);
    delay(500);
    float aFlow = 0.0;
    float aTemperature = 0.0;
    uint16_t aSignalingFlags = 0u;
    delay(20);
    error = sensor.readMeasurementData(10, aFlow, aTemperature, aSignalingFlags);
    if (error != NO_ERROR) {
      Serial.print("Error trying to execute readMeasurementData(): ");
      errorToString(error, errorMessage, sizeof errorMessage);
      Serial.println(errorMessage);
      return;
    }
    Data_Flow[Data_count] = aFlow;
    Data_Ampl[Data_count] = setup_ampl;
    Data_count++;
    setup_ampl += 20;
  }
  int order = 1;
  int Data_size = sizeof(Data_Flow) / sizeof(Data_Flow[0]);
  double coeffs[order + 1];
  int ret = fitCurve(order, Data_size, Data_Flow, Data_Ampl, sizeof(coeffs) / sizeof(double), coeffs);
  if (ret == 0) {
    slope = coeffs[0];
  } else {
    Serial.println("Regression setup failed");
  }
}

/**
 * Calculate the feedforward voltage based on the target flow rate.
 * 
 * @param soll_vor The target flow rate.
 * @return The calculated feedforward voltage.
 */
float vorsteuerung(int soll_vor) {
  int ampl_vor = slope * soll_vor;
  return ampl_vor;
}

/**
 * Check and process incoming serial data from the host (e.g., Python script).
 * Handles commands such as "Fillsyringe", "Stop", "Finished", and parameter updates.
 * 
 * @return The current target speed if provided in the serial data.
 */
float checkSerialData() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.equals("Fillsyringe")) {
      pumpState = true;
    }

    if (pumpState) {
      bPumpState[main_pump] = true;
      Highdriver_setfrequency(freq);
      current_target_speed = 600;
    }

    if (input.equals("Stop")) {
      pumpState = false;
      Highdriver4_setvoltage(main_pump, 0);
      bPumpState[main_pump] = false;
      Highdriver4_setvoltage(support_pump, 0);
      bPumpState[support_pump] = false;
      current_target_speed = 0;
      topUpSyringe = true;
    }

    if (input.equals("Finished")) {
      bPumpState[main_pump] = true;
      digitalWrite(relayPin, HIGH);
      Highdriver_setfrequency(freq);
      Highdriver4_setvoltage(main_pump, 150);
      delay(1000);
      digitalWrite(relayPin, LOW);
      Highdriver4_setvoltage(main_pump, 0);
      bPumpState[main_pump] = false;
      current_target_speed = 0;
    }

    if (input.startsWith("KP,")) {
      kp = input.substring(3).toFloat();
    }

    if (input.startsWith("KI,")) {
      ki = input.substring(3).toFloat();
    }

    int first_comma_index = input.indexOf(',');
    if (first_comma_index > 0) {
      elapsed_time = input.substring(0, first_comma_index).toFloat();
      int second_comma_index = input.indexOf(',', first_comma_index + 1);
      if (second_comma_index > 0) {
        current_target_speed = input.substring(first_comma_index + 1, second_comma_index).toFloat();
        total_time = input.substring(second_comma_index + 1).toFloat();
      }
    }
    return current_target_speed;
  }
}

/**
 * Main control function for the pumps.
 * Reads sensor data, calculates the control output using PI logic, and applies
 * the output to the pumps. Also manages the support pump based on flow conditions.
 * 
 * @param target_speed The desired flow rate.
 * @param dt The time step since the last control update.
 */
void controlFunction(float target_speed, float dt) {
  tolerance_min = target_speed * (1 - tolerance);
  tolerance_max = target_speed * (1 + tolerance);

  if (target_speed == 0) {
    Highdriver4_setvoltage(main_pump, 0);
    Highdriver4_setvoltage(support_pump, 0);
    bPumpState[main_pump] = false;
    bPumpState[support_pump] = false;
    return;
  }

  bPumpState[main_pump] = true;
  bPumpState[support_pump] = true;
  Highdriver_setfrequency(freq);

  float aFlow = 0.0;
  float aTemperature = 0.0;
  uint16_t aSignalingFlags = 0u;
  error = sensor.readMeasurementData(10, aFlow, aTemperature, aSignalingFlags);
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute readMeasurementData(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }
  if (elapsed_time > 5) {
    if (aFlow <= tolerance_max && aFlow >= tolerance_min) {
      input = createMovingAverage(aFlow);
      applyOutput(previous_output);
      Highdriver4_setvoltage(support_pump, support_amp);
    } else {
      input = createMovingAverage(aFlow);
      diff = target_speed - input;
      output = Regelung(diff);
      ampl_feedforward = vorsteuerung(target_speed);
      output += ampl_feedforward;
      applyOutput(output);
      previous_output = output;

      if (new_ampl == 250) {
        new_support_amp = 250;
      } else if (new_ampl > 200) {
        if (old_support_amp < 220) {
          new_support_amp = new_support_amp + 30;
        }
      } else if (aFlow < target_speed * 0.5 && new_ampl > 180) {
        new_support_amp = 180;
      } else if (aFlow < target_speed * 0.8 && new_ampl > 120) {
        new_support_amp = 120;
      } else if (aFlow < target_speed * 0.9 && new_ampl > 60) {
        new_support_amp = 60;
      } else {
        new_support_amp = 30;
      }

      if (new_support_amp > old_support_amp) {
        Highdriver4_setvoltage(support_pump, new_support_amp);
        old_support_amp = new_support_amp;
        support_amp = new_support_amp;
      }
    }
  } else {
    Highdriver4_setvoltage(support_pump, 30);
    support_amp = 30;
    input = createMovingAverage(aFlow);
    diff = target_speed - input;
    output = Regelung(diff);
    ampl_feedforward = vorsteuerung(target_speed);
    output += ampl_feedforward;
    applyOutput(output);
    previous_output = output;
  }
  String serialDataControl = "Flow," + String(aFlow);
  Serial.println(serialDataControl);
}

/**
 * Arduino setup function.
 * Initializes the serial communication, Highdriver, sensor, and relay pin.
 * Sends a "Setup complete" message to the host.
 */
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  Wire.begin();
  Highdriver_init();
  if (Highdriver_check() == 1) {
    Serial.println("Highdriver detected.");
  } else {
    Serial.println("Highdriver not detected.");
  }

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  sensor.begin(Wire, SLF3C_1300F_I2C_ADDR_08);
  sensor.stopContinuousMeasurement();
  delay(100);
  setTime(0, 0, 0, 1, 1, 1970);
  uint32_t productIdentifier = 0;
  uint8_t serialNumber[8] = {0};
  error = sensor.readProductIdentifier(productIdentifier, serialNumber, 8);
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute readProductIdentifier(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }
  Serial.print("productIdentifier: ");
  Serial.print(productIdentifier);
  Serial.print("\t");
  Serial.print("serialNumber: ");
  print_byte_array(serialNumber, 8);
  Serial.println();
  error = sensor.startH2oContinuousMeasurement();
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute startH2oContinuousMeasurement(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }

  bPumpState[main_pump] = true;
  bPumpState[support_pump] = true;
  Highdriver_setfrequency(freq);

  Serial.println("Setup complete");
}

/**
 * Arduino main loop function.
 * Periodically executes the control function and checks for serial data.
 */
void loop() {
  if (topUpSyringe) {
    vorsteuerung_setup();
    Serial.print("KP:");
    Serial.println(kp, 3);
    Serial.print("KI:");
    Serial.println(ki, 3);
    Serial.print("Control interval:");
    Serial.println(intervalControl);
    Serial.print("I2C interval:");
    Serial.println(intervalSerial);
    Serial.print("Window:");
    Serial.println(NUM_SAMPLES);
    Serial.print("Slope:");
    Serial.println(slope);

    Serial.println("Feedforward Control complete");
    topUpSyringe = false;
  }

  unsigned long currentMillisControl = millis();
  unsigned long currentMillisSerial = millis();

  if (currentMillisControl - previousMillisControl >= intervalControl) {
    float dt = (currentMillisControl - previousMillisControl) / 1000.0;
    previousMillisControl = currentMillisControl;
    controlFunction(current_target_speed, dt);
  }

  if (currentMillisSerial - previousMillisSerial >= intervalSerial) {
    previousMillisSerial = currentMillisSerial;
    checkSerialData();
  }
}
