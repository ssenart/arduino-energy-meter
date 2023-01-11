#include "SimulatedEnergySource.h"
#include "ActualEnergySource.h"
#include "EnergyMeter.h"

// Uncomment the line below to switch on simulation mode.
#define SIMULATION

// Reference documentation.
// See: https://learn.openenergymonitor.org/electricity-monitoring/ac-power-theory/advanced-maths
// See: https://solarduino.com/how-to-measure-ac-power-with-arduino/

// ---------------------------
// -- Board parameters
const float SUPPLY_VOLTAGE = 5.0; // Volts.

#ifndef ADC_RESOLUTION
const int ADC_RESOLUTION = 10; // Bits
#endif

// ---------------------------
// -- Calibration parameters


// The voltage multiplier corresponds to the voltage transformer ratio (turn ratio).
//    Example: We have an input voltage transformer from 230Vrms (650Vpp) to 1.76Vrms (5Vpp).
//             For a maximum input voltage of 230Vrms, we choose a voltage multiplier:
//                  CALIBRATION_VOLTAGE_MULTIPLIER = 230 / 1.76 = 650 / 5 = 130.
const float CALIBRATION_VOLTAGE_MULTIPLIER = 130;
const float CALIBRATION_VOLTAGE_OFFSET = 0.0;

// The current multiplier corresponds to the current sensor sensitivity (V/A) or the current transformer ratio (turn ratio).
//    Example 1: We have an input current sensor with a sensitivity of 66mV/A (example: sensor ACS712 - 30A).
//               For a maximum input current of 0.4348Arms (equivalent to 100W at 230Vrms), we choose a current multiplier:
//                  CALIBRATION_CURRENT_MULTIPLIER = 0.4348 * 0.066 = 0.0286968.
//    Example 2: We have an input current transformer with a turn ratio 15A:0.05A (example: YHDC SCT-013-015).
//               For a maximum input current of 0.4348Arms (equivalent to 100W at 230Vrms), we choose a current multiplier:
//                  CALIBRATION_CURRENT_MULTIPLIER =
const float CALIBRATION_CURRENT_MULTIPLIER[] = { 0.24596, 0.24596, 0.24596, 0.24596, 0.24596, 0.24596, 0.24596, 0.24596, 0.24596, 0.24596 }; // 0.24596 * 2.5V = 0.6149A (0.4348A RMS) => Equivalent to 100W.
const float CALIBRATION_CURRENT_OFFSET[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

// Phase correction coefficient:
// - 0 equivalent to move the waveform to the left of one sample period.
// - 1 equivalent to no change, the waveform remains the same.
// - 2 equivalent to move the waveform to the right of one sample period.
const float CALIBRATION_PHASE_SHIFTS[] = { 0.885 , 0.77, 0.665, 0.54, 0.425, 0.31, 0.195, 0.08 , 1.0, 1.0 };

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
                        CALIBRATION_VOLTAGE_OFFSET,
                        CALIBRATION_CURRENT_MULTIPLIER,
                        CALIBRATION_CURRENT_OFFSET,
                        CALIBRATION_PHASE_SHIFTS,
                        10
                       );
// ---------------------------

// ---------------------------
void setup() {
  Serial.begin(115200);
}

// ---------------------------
void loop() {

  energyMeter.update();

  Serial.println("------ Voltage ------");
  Serial.println(" Sp    Hz    Avg    Pk      Rms");
  Serial.print(energyMeter.sampleCount());
  Serial.print("  ");
  Serial.print(energyMeter.frequency());
  Serial.print("  ");
  Serial.print(energyMeter.averageVoltage());  
  Serial.print("  ");
  Serial.print(energyMeter.peakVoltage());  
  Serial.print("  ");  
  Serial.println(energyMeter.rmsVoltage());

  Serial.println("------ Currents ------");
  Serial.println(" Idx Avg    Pk    Rms   VA      W      VAR     Wh    PF    Phi");
  for (auto inputIndex = 0; inputIndex < energyMeter.currentInputCount(); ++inputIndex)
  {
      Serial.print("- ");
      Serial.print(inputIndex);
      Serial.print("  ");
      Serial.print(energyMeter.averageCurrent(inputIndex));      
      Serial.print("  ");
      Serial.print(energyMeter.peakCurrent(inputIndex));      
      Serial.print("  ");      
      Serial.print(energyMeter.rmsCurrent(inputIndex));
      Serial.print("  ");
      Serial.print(energyMeter.apparentPower(inputIndex));
      Serial.print("  ");
      Serial.print(energyMeter.activePower(inputIndex));
      Serial.print("  ");
      Serial.print(energyMeter.reactivePower(inputIndex));
      Serial.print("  ");
      Serial.print(energyMeter.energy(inputIndex));
      Serial.print("  ");
      Serial.print(energyMeter.powerFactor(inputIndex));    
      Serial.print("  ");
      Serial.println(energyMeter.phaseAngle(inputIndex));
  }

  Serial.println("------ Performance ------");
  Serial.println("TotalTime  BusyTime");
  Serial.print(energyMeter.totalElapsed());
  Serial.print(" ");
  Serial.println(energyMeter.busyElapsed());
  Serial.println("---------------------------------------");  
}

#ifndef ARDUINO
int main( int argc, char **argv)
{
  setup();
  while (true) { loop(); }
}
#endif