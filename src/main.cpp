#include "SimulatedEnergySource.h"
#include "ActualEnergySource.h"
#include "EnergyMeter.h"

// Uncomment the line below to switch on simulation mode.
#define SIMULATION

// Reference documentation.
// See: https://learn.openenergymonitor.org/electricity-monitoring/ac-power-theory/advanced-maths
// See: https://solarduino.com/how-to-measure-ac-power-with-arduino/

// ---------------------------
// -- Calibration parameters
const float SUPPLY_VOLTAGE = 5.0; // Volts.
const float ADC_RESOLUTION = 10; // Bits
const float CALIBRATION_VOLTAGE_MULTIPLIER = 130.0; // 130 * 2.5V = 325V (230V RMS)
const float CALIBRATION_CURRENT_MULTIPLIER = 0.24596; // 0.24596 * 2.5V = 0.6149A (0.4348A RMS) => Equivalent to 100W.
const float CALIBRATION_PHASE_SHIFT = 1.5; // Coefficient.

// ---------------------------
// -- Simulation mode parameters
const float FREQUENCY = 50;
const float PHASE_ANGLE = 0; //PI / 2;

#ifdef SIMULATION
SimulatedEnergySource energySource(ADC_RESOLUTION, FREQUENCY, PHASE_ANGLE, 10);
#else
int currentInputPins[MAX_CURRENT_INPUT] = {
   A2
};
ActualEnergySource energySource(
  ADC_RESOLUTION,
  A1, (int[]) { A2 }, 1);
#endif
// ---------------------------

// ---------------------------
// -- Sampling parameters
const float SAMPLING_FREQUENCY = 1000;
const unsigned long SAMPLING_DURATION_MS = 1000;

EnergyMeter energyMeter(energySource,
                        SUPPLY_VOLTAGE,
                        SAMPLING_FREQUENCY,
                        SAMPLING_DURATION_MS,
                        CALIBRATION_VOLTAGE_MULTIPLIER,
                        CALIBRATION_CURRENT_MULTIPLIER,
                        CALIBRATION_PHASE_SHIFT
                       );
// ---------------------------

// ---------------------------
void setup() {
  Serial.begin(115200);
}

// ---------------------------
void loop() {

  energyMeter.update();

  Serial.print(energyMeter.sampleCount());
  Serial.print(" ");
  Serial.print(energyMeter.frequency());
  Serial.print(" ");
  Serial.println(energyMeter.rmsVoltage());

  for (auto inputIndex = 0; inputIndex < energyMeter.currentInputCount(); ++inputIndex)
  {
      Serial.print("- ");
      Serial.print(inputIndex);
      Serial.print(" ");
      Serial.print(energyMeter.rmsCurrent(inputIndex));
      Serial.print(" ");
      Serial.print(energyMeter.apparentPower(inputIndex));
      Serial.print(" ");
      Serial.print(energyMeter.activePower(inputIndex));
      Serial.print(" ");
      Serial.print(energyMeter.reactivePower(inputIndex));
      Serial.print(" ");
      Serial.print(energyMeter.energy(inputIndex));
      Serial.print(" ");
      Serial.print(energyMeter.powerFactor(inputIndex));    
      Serial.print(" ");
      Serial.println(energyMeter.phaseAngle(inputIndex));
  }

  Serial.print(energyMeter.totalElapsed());
  Serial.print(" ");
  Serial.println(energyMeter.busyElapsed());
  Serial.println("-------------");  
}

#ifndef ARDUINO
int main( int argc, char **argv)
{
  setup();
  while (true) { loop(); }
}
#endif