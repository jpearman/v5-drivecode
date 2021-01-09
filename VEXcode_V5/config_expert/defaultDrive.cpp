/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       james                                                     */
/*    Created:      Tue Dec 29 2020                                           */
/*    Description:  V5 Default clawbot drive code                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/
//
#include "vex.h"

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

// Assign generic motor to more useful names here
// We use references
motor   &LeftDrive1  = Motor1;
motor   &RightDrive1 = Motor2;
motor   &LeftDrive2  = Motor9;
motor   &RightDrive2 = Motor10;

// Arm and claw motors will have brake mode set to hold
// Claw motor will have max torque limited
motor   &ClawMotor   = Motor3;
motor   &ArmMotor    = Motor8;

// Options for drive control, the same optiona as V5 default drive code
typedef enum _drive_mode {
    left_arcade,
    dual_stick,
    split_arcade,
    right_arcade
} drive_mode_t;

// Choose your drive mode here
const drive_mode_t drive_mode = left_arcade;

// Max motor speed (percent) for motors controlled by buttons
const int32_t maxSpeed = 50;

/*----------------------------------------------------------------------------*/
/** @brief  Drive task                                                        */
/*----------------------------------------------------------------------------*/
//
// All motors are controlled from this function which is run as a separate thread
//
void
driveTask() {
    int32_t drive_left  = 0;
    int32_t drive_right = 0;

    // setup the claw motor
    ClawMotor.setMaxTorque( 25, percent );
    ClawMotor.setStopping( hold );

    // setup the arm motor
    ArmMotor.setStopping( hold );

    // loop forever
    while( true ) {
        // buttons
        // Three values, max, 0 and -max.
        //
        int32_t drive_m_3 = (Controller1.ButtonL1.pressing() - Controller1.ButtonL2.pressing()) * maxSpeed;
        int32_t drive_m_4 = (Controller1.ButtonRight.pressing() - Controller1.ButtonLeft.pressing()) * maxSpeed;
        int32_t drive_m_5 = (Controller1.ButtonUp.pressing() - Controller1.ButtonDown.pressing()) * maxSpeed;
        int32_t drive_m_6 = (Controller1.ButtonA.pressing() - Controller1.ButtonY.pressing()) * maxSpeed;
        int32_t drive_m_7 = (Controller1.ButtonX.pressing() - Controller1.ButtonB.pressing()) * maxSpeed;
        int32_t drive_m_8 = (Controller1.ButtonR1.pressing() - Controller1.ButtonR2.pressing()) * maxSpeed;
        
        // use limit switches on motors 3 and 8
        if( ( LimitSwitch3F.pressing() && (drive_m_3 > 0)) || ( LimitSwitch3R.pressing() && (drive_m_3 < 0)) )
            drive_m_3  = 0;
        if( ( LimitSwitch8F.pressing() && (drive_m_8 > 0)) || ( LimitSwitch8R.pressing() && (drive_m_8 < 0)) )
            drive_m_8  = 0;

        // Various choices for arcade or tank drive
        switch( drive_mode ) {
            case left_arcade:
              drive_left  =  Controller1.Axis3.position() + Controller1.Axis4.position();
              drive_right =  Controller1.Axis3.position() - Controller1.Axis4.position();
              break;
            case dual_stick:
              drive_left  =  Controller1.Axis3.position();
              drive_right =  Controller1.Axis2.position();
              break;
            case split_arcade:
              drive_left  =  Controller1.Axis3.position() + Controller1.Axis1.position();
              drive_right =  Controller1.Axis3.position() - Controller1.Axis1.position();
              break;
            case right_arcade:
              drive_left  =  Controller1.Axis2.position() + Controller1.Axis1.position();
              drive_right =  Controller1.Axis2.position() - Controller1.Axis1.position();
              break;
            default:
              break;
        }

        // threshold the variable channels so the drive does not
        // move if the joystick axis does not return exactly to 0
        const int deadband = 15;
        if( abs(drive_left ) < deadband ) drive_left  = 0;
        if( abs(drive_right) < deadband ) drive_right = 0;

        // Now send all drive values to motors

        // The drivetrain
        LeftDrive1.spin( forward, drive_left, percent );
        LeftDrive2.spin( forward, drive_left, percent );
        RightDrive1.spin( forward, drive_right, percent );
        RightDrive2.spin( forward, drive_right, percent );

        // and all the auxilary motors
        Motor3.spin( forward, drive_m_3, percent );
        Motor4.spin( forward, drive_m_4, percent );
        Motor5.spin( forward, drive_m_5, percent );
        Motor6.spin( forward, drive_m_6, percent );
        Motor7.spin( forward, drive_m_7, percent );
        Motor8.spin( forward, drive_m_8, percent );

        // No need to run too fast
        this_thread::sleep_for(10);
    }
}

// define some more colors
const vex::color grey((uint32_t)0x202020);
const vex::color dgrey((uint32_t)0x2F4F4F);
const vex::color lblue((uint32_t)0x303060);
const vex::color lred((uint32_t)0x603030);

/*-----------------------------------------------------------------------------*/
/** @brief      Display data for one motor                                     */
/*-----------------------------------------------------------------------------*/
void
displayMotorData( motor &m ) {
  int ypos = 0;
  int xpos = m.index() * 48;

  // There's no C++ API to get motor command value, use C API, this returns rpm
  int v1  = vexMotorVelocityGet(m.index());

  // The actual velocity of the motor in rpm
  int v2  = m.velocity(velocityUnits::rpm);

  // The position of the motor internal encoder in revolutions
  double pos = m.position(rotationUnits::rev);

  // Motor current in Amps    
  double c1 = m.current();

  // Motor temperature
  double t1 = m.temperature(temperatureUnits::celsius);

  Brain.Screen.setFont( mono15 );

  // background color based on
  // device and whether it's left, right or other motor
  if( !m.installed() ) {
    // no motor
    Brain.Screen.setFillColor( grey );
  }
  else  
  if( m.index() == LeftDrive1.index() || m.index() == LeftDrive2.index() ) {
    Brain.Screen.setFillColor( lblue );
  }
  else  
  if( m.index() == RightDrive1.index() || m.index() == RightDrive2.index() ) {
    Brain.Screen.setFillColor( lred );
  }
  else {
    Brain.Screen.setFillColor( dgrey );
  }
  
  // Draw outline for motor info
  Brain.Screen.setPenColor( white  );
  int w = (m.index() < 9) ? 49 : 48;
  Brain.Screen.drawRectangle( xpos, ypos, w, 79 );
  
  // no motor, then return early
  if( !m.installed() ) {
    Brain.Screen.printAt( xpos+15, ypos+30, true, "NC" );
    return;
  }

  // Show commanded speed
  if( v1 != 0 )
    Brain.Screen.setPenColor( green );
  else
    Brain.Screen.setPenColor( white );
  Brain.Screen.printAt( xpos+13, ypos+13, true, "%4d", v1 );

  // Show actual speed
  Brain.Screen.setPenColor( yellow  );
  Brain.Screen.printAt( xpos+13, ypos+30, true, "%4d", v2 );

  // Show position
  Brain.Screen.printAt( xpos+5, ypos+45, true, "%5.1f", pos );

  // Show current
  Brain.Screen.printAt( xpos+5, ypos+60, true, "%4.1fA", c1 );

  // Show temperature
  Brain.Screen.printAt( xpos+5, ypos+75, true, "%4.0fC", t1 );

  Brain.Screen.setPenColor( white  );
  Brain.Screen.drawLine( xpos, ypos+14, xpos+48, ypos+14);  
}

/*----------------------------------------------------------------------------*/
/** @brief  Display task - show some useful motor data                        */
/*----------------------------------------------------------------------------*/
void
displayTask() {
    Brain.Screen.setFont( prop40 );
    Brain.Screen.setPenColor( red );
    Brain.Screen.printAt( 90, 160, "TEST DRIVE CODE");

    motor *motors[] = { &Motor1, 
                        &Motor2,
                        &Motor3,
                        &Motor4,
                        &Motor5,
                        &Motor6,
                        &Motor7,
                        &Motor8,
                        &Motor9,
                        &Motor10
                      };
                      
    while( true ) {
      for( int i=PORT1;i<=PORT10;i++) {
        displayMotorData( *motors[i] );
      }

      // display using back buffer, stops flickering
      Brain.Screen.render();

      this_thread::sleep_for(10);
    }
}

int main() {
    // Run the drive code
    thread drive( driveTask );

    // Run the display code
    thread display( displayTask );

    // main just loops here until the program is terminated
    while( true ) {
      this_thread::sleep_for(100);
    }
}
