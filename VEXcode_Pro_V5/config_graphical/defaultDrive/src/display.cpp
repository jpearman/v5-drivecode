/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       display.cpp                                               */
/*    Author:       james                                                     */
/*    Created:      Tue Dec 29 2020                                           */
/*    Description:  optional display functions                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"

using namespace vex;

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