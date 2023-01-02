#include <Arduino.h>

#include "SimulatedEnergySource.h"
#include "ActualEnergySource.h"
#include "EnergyMeter.h"

// Uncomment the line below to switch on simulation mode.
#define SIMULATION

// Reference documentation.
// See: https://learn.openenergymonitor.org/electricity-monitoring/ac-power-theory/advanced-maths
// See: https://solarduino.com/how-to-measure-ac-power-with-arduino/

// ---------------------------
// -- Simulation mode parameters
const double VOLTAGE_AMPLITUDE = 325.0; // 230V RMS.
const double CURRENT_AMPLITUDE = 0.6149; // 0.4348A RMS.

const double FREQUENCY = 50;
const double PHASE_ANGLE = 0; //PI / 2;

#ifdef SIMULATION
SimulatedEnergySource energySource(VOLTAGE_AMPLITUDE, CURRENT_AMPLITUDE, FREQUENCY, PHASE_ANGLE);
#else
ActualEnergySource energySource(A1, VOLTAGE_AMPLITUDE, A2, CURRENT_AMPLITUDE);
#endif
// ---------------------------

// ---------------------------
// -- Sampling parameters
const double SAMPLING_FREQUENCY = 1000;
const int SAMPLE_SIZE = 1000;

EnergyMeter energyMeter(energySource, SAMPLING_FREQUENCY, SAMPLE_SIZE);
// ---------------------------

// ---------------------------
void setup() {
  Serial.begin(115200);
}

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
  Serial.print(energyMeter.energy());
  Serial.print(" ");  
  Serial.print(energyMeter.powerFactor());    
  Serial.print(" ");
  Serial.println(energyMeter.phaseAngle());    
}
