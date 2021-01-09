#include "vex.h"

using namespace vex;

// A global instance of brain used for printing to the V5 brain screen
brain Brain;

// Create motors on ports 1 through 10
motor Motor1 = motor(PORT1, ratio18_1, false);
motor Motor2 = motor(PORT2, ratio18_1, true);
motor Motor3 = motor(PORT3, ratio18_1, false);
motor Motor4 = motor(PORT4, ratio18_1, false);
motor Motor5 = motor(PORT5, ratio18_1, false);
motor Motor6 = motor(PORT6, ratio18_1, false);
motor Motor7 = motor(PORT7, ratio18_1, false);
motor Motor8 = motor(PORT8, ratio18_1, false);
motor Motor9 = motor(PORT9, ratio18_1, false);
motor Motor10 = motor(PORT10, ratio18_1, true);

// Limit switches for motor 3 and 8
limit LimitSwitch3F = limit(Brain.ThreeWirePort.A);
limit LimitSwitch3R = limit(Brain.ThreeWirePort.B);
limit LimitSwitch8F = limit(Brain.ThreeWirePort.C);
limit LimitSwitch8R = limit(Brain.ThreeWirePort.D);

// The controller
controller Controller1 = controller(primary);

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void) {
  // Nothing to initialize
}