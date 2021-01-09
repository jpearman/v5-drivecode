/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       james                                                     */
/*    Created:      Tue Dec 29 2020                                           */
/*    Description:  V5 Default clawbot drive code                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

using namespace vex;

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

int main() {
    // Initializing Robot Configuration.
    // In this demo code nothing actually happens in this function
    vexcodeInit();
    
    // Run the drive code
    thread drive( driveTask );

    // Run the display code
    thread display( displayTask );

    // main just loops here until the program is terminated
    while( true ) {
      this_thread::sleep_for(100);
    }
}
