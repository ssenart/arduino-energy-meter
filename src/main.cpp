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
const float VOLTAGE_AMPLITUDE = 325.0; // 230V RMS.
const float CURRENT_AMPLITUDE = 0.6149; // 0.4348A RMS => Equivalent to 100W.

const float FREQUENCY = 50;
const float PHASE_ANGLE = 0; //PI / 2;

#ifdef SIMULATION
SimulatedEnergySource energySource(VOLTAGE_AMPLITUDE, CURRENT_AMPLITUDE, FREQUENCY, PHASE_ANGLE, 2);
#else
ActualEnergySource energySource(A1, VOLTAGE_AMPLITUDE, A2, CURRENT_AMPLITUDE);
#endif
// ---------------------------

// ---------------------------
// -- Sampling parameters
const float SAMPLING_FREQUENCY = 1000;
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