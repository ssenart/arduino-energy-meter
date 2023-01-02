#include <unity.h>

#include "SimulatedEnergySource.h"

const double VOLTAGE_AMPLITUDE = 325.0; // 230V RMS.
const double CURRENT_AMPLITUDE = 0.6149; // 0.4348A RMS.

const double FREQUENCY = 50;
const double PHASE_ANGLE = 0; //PI / 2;

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void testSimulatedEnergySource() {

    SimulatedEnergySource simulatedEnergySource(VOLTAGE_AMPLITUDE, CURRENT_AMPLITUDE, FREQUENCY, PHASE_ANGLE);

    TEST_ASSERT_TRUE(simulatedEnergySource.voltage() != 0.0);
    TEST_ASSERT_TRUE(simulatedEnergySource.current() != 0.0);
}

int runUnityTests() {
  UNITY_BEGIN();
  RUN_TEST(testSimulatedEnergySource);
  return UNITY_END();
}
