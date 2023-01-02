#include <Arduino.h>

// Uncomment the line below 
#define SIMULATION

#include "SimulatedEnergySource.h"
#include "ActualEnergySource.h"
#include "EnergyMeter.h"

// Reference documentation.
// See: https://learn.openenergymonitor.org/electricity-monitoring/ac-power-theory/advanced-maths
// See: https://solarduino.com/how-to-measure-ac-power-with-arduino/

// ---------------------------
// -- Simulation parameters
const double VOLTAGE_AMPLITUDE = 325.0; // 230V RMS.
const double CURRENT_AMPLITUDE = 0.6149; // 0.4348A RMS.

const double FREQUENCY = 50;
const double PHASE_ANGLE = 0; //PI / 2;
// ---------------------------

// ---------------------------
// -- Sampling parameters
const double SAMPLING_FREQUENCY = 1000;
const int SAMPLE_SIZE = 1000;
// ---------------------------

// ---------------------------
void setup() {
  Serial.begin(115200);
}

#ifdef SIMULATION
SimulatedEnergySource energySource(VOLTAGE_AMPLITUDE, CURRENT_AMPLITUDE, FREQUENCY, PHASE_ANGLE);
#else
ActualEnergySource energySource(A1, VOLTAGE_AMPLITUDE, A2, CURRENT_AMPLITUDE);
#endif

EnergyMeter energyMeter(energySource, SAMPLING_FREQUENCY, SAMPLE_SIZE);

// ---------------------------
void loop() {

  energyMeter.update();

  Serial.print(energyMeter.frequency());
  Serial.print(" ");
  Serial.print(energyMeter.rmsVoltage());
  Serial.print(" ");
  Serial.print(energyMeter.rmsCurrent());
  Serial.print(" ");
  Serial.print(energyMeter.apparentPower());
  Serial.print(" ");
  Serial.print(energyMeter.activePower());
  Serial.print(" ");
  Serial.print(energyMeter.reactivePower());
  Serial.print(" ");
  Serial.println(energyMeter.phaseAngle());    
}
