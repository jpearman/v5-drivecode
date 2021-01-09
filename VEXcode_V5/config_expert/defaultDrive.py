#-----------------------------------------------------------------------------*/
#                                                                             */
#     Module:       defaultDrivePy                                            */
#     Author:       james                                                     */
#     Created:      Tue Dec 29 2020                                           */
#     Description:  Default Clawbot code - Python                             */
#                                                                             */
#-----------------------------------------------------------------------------*/
# Library imports
from vex import *

# Create motors on ports 1 through 10
motor_1 = Motor(Ports.PORT1, GearSetting.RATIO_18_1, False)
motor_2 = Motor(Ports.PORT2, GearSetting.RATIO_18_1, True)
motor_3 = Motor(Ports.PORT3, GearSetting.RATIO_18_1, False)
motor_4 = Motor(Ports.PORT4, GearSetting.RATIO_18_1, False)
motor_5 = Motor(Ports.PORT5, GearSetting.RATIO_18_1, False)
motor_6 = Motor(Ports.PORT6, GearSetting.RATIO_18_1, False)
motor_7 = Motor(Ports.PORT7, GearSetting.RATIO_18_1, False)
motor_8 = Motor(Ports.PORT8, GearSetting.RATIO_18_1, False)
motor_9 = Motor(Ports.PORT9, GearSetting.RATIO_18_1, False)
motor_10 = Motor(Ports.PORT10, GearSetting.RATIO_18_1, True)

# Limit switches for motor 3 and 8
limit_switch_3f = Limit(brain.three_wire_port.a);
limit_switch_3r = Limit(brain.three_wire_port.b);
limit_switch_8f = Limit(brain.three_wire_port.c);
limit_switch_8r = Limit(brain.three_wire_port.d);

# The controller
controller_1 = Controller(ControllerType.PRIMARY)

# Assign generic motor to more useful names here
# We use references
left_drive_1 = motor_1
right_drive_1 = motor_2
left_drive_2 = motor_9
right_drive_2 = motor_10

# Arm and claw motors will have brake mode set to hold
# Claw motor will have max torque limited
claw_motor = motor_3
arm_motor = motor_8

class DriveType:
    LEFT = 0
    DUAL = 1
    SPLIT = 2
    RIGHT = 3
    value = 0

    def __init__(self, value):
        self.value = value
    def __str__(self):
        return self.value
    def __repr__(self):
        return self.value
    def __eq__(self, value):
        return self.value == value

# pick LEFT, DUAL, SPLIT or RIGHT
drive_mode = DriveType(DriveType.LEFT)

# Max motor speed (percent) for motors controlled by buttons
MAX_SPEED = 50

#-----------------------------------------------------------------------------*/
#   @brief  Drive task                                                        */
#-----------------------------------------------------------------------------*/
#
# All motors are controlled from this function which is run as a separate thread
#
def drive_task():
    drive_left = 0
    drive_right = 0

    # setup the claw motor
    claw_motor.set_max_torque(25, PERCENT)
    claw_motor.set_stopping(HOLD)

    # setup the arm motor
    arm_motor.set_stopping(HOLD)

    # loop forever
    while True:
        # buttons
        # Three values, max, 0 and -max.
        #
        drive_m_3 = (controller_1.buttonL1.pressing() - controller_1.buttonL2.pressing()) * MAX_SPEED
        drive_m_4 = (controller_1.buttonRight.pressing() - controller_1.buttonLeft.pressing()) * MAX_SPEED
        drive_m_5 = (controller_1.buttonUp.pressing() - controller_1.buttonDown.pressing()) * MAX_SPEED
        drive_m_6 = (controller_1.buttonA.pressing() - controller_1.buttonY.pressing()) * MAX_SPEED
        drive_m_7 = (controller_1.buttonX.pressing() - controller_1.buttonB.pressing()) * MAX_SPEED
        drive_m_8 = (controller_1.buttonR1.pressing() - controller_1.buttonR2.pressing()) * MAX_SPEED

        # use limit switches on motors 3 and 8
        if (limit_switch_3f.pressing() and (drive_m_3 > 0)) or (limit_switch_3r.pressing() and (drive_m_3 < 0)):
            drive_m_3 = 0
        if (limit_switch_8f.pressing() and (drive_m_8 > 0)) or (limit_switch_8r.pressing() and (drive_m_8 < 0)):
            drive_m_8 = 0

        # Various choices for arcade or tank drive
        if drive_mode == DriveType.LEFT:
            drive_left = controller_1.axis3.position() + controller_1.axis4.position()
            drive_right = controller_1.axis3.position() - controller_1.axis4.position()
        elif drive_mode == DriveType.DUAL:
            drive_left = controller_1.axis3.position()
            drive_right = controller_1.axis2.position()
        elif drive_mode == DriveType.SPLIT:
            drive_left = controller_1.axis3.position() + controller_1.axis1.position()
            drive_right = controller_1.axis3.position() - controller_1.axis1.position()
        elif drive_mode == DriveType.RIGHT:
            drive_left = controller_1.axis2.position() + controller_1.axis1.position()
            drive_right = controller_1.axis2.position() - controller_1.axis1.position()

        # threshold the variable channels so the drive does not
        # move if the joystick axis does not return exactly to 0
        deadband = 15
        if abs(drive_left) < deadband:
            drive_left = 0
        if abs(drive_right) < deadband:
            drive_right = 0

        # Now send all drive values to motors

        # The drivetrain
        left_drive_1.spin(FORWARD, drive_left, PERCENT)
        left_drive_2.spin(FORWARD, drive_left, PERCENT)
        right_drive_1.spin(FORWARD, drive_right, PERCENT)
        right_drive_2.spin(FORWARD, drive_right, PERCENT)

        # and all the auxilary motors
        motor_3.spin(FORWARD, drive_m_3, PERCENT)
        motor_4.spin(FORWARD, drive_m_4, PERCENT)
        motor_5.spin(FORWARD, drive_m_5, PERCENT)
        motor_6.spin(FORWARD, drive_m_6, PERCENT)
        motor_7.spin(FORWARD, drive_m_7, PERCENT)
        motor_8.spin(FORWARD, drive_m_8, PERCENT)

        # No need to run too fast
        sleep(10)

# define some more colors
grey = Color(0x202020)
dgrey = Color(0x2F4F4F)
lblue = Color(0x303060)
lred = Color(0x603030)

#------------------------------------------------------------------------------*/
#   @brief      Display data for one motor                                     */
#------------------------------------------------------------------------------*/
def displayMotorData(m, index):
    ypos = 0
    xpos = index * 48

    # command value not available in Python
    v1 = 0

    # The actual velocity of the motor in rpm
    v2 = m.velocity(RPM)

    # The position of the motor internal encoder in revolutions
    pos = m.position(TURNS)

    # Motor current in Amps
    c1 = m.current()

    # Motor temperature
    t1 = m.temperature()

    brain.screen.set_font(FontType.MONO15)

    # background color based on
    # device and whether it's left, right or other motor
    if not m.installed():
        brain.screen.set_fill_color(grey)
    elif m == left_drive_1 or m == left_drive_2:
        brain.screen.set_fill_color(lblue)
    elif m == right_drive_1 or m == right_drive_2:
        brain.screen.set_fill_color(lred)
    else:
        brain.screen.set_fill_color(dgrey)

    # Draw outline for motor info
    brain.screen.set_pen_color(Color.WHITE)
    w = 49 if index < 9 else 48
    brain.screen.draw_rectangle(xpos, ypos, w, 79)

    # no motor, then return early
    if not m.installed():
        brain.screen.print_at("NC", x=xpos+15, y=ypos+30)
        return

    # we have no way to get command value in Python VM 1.0.0b20
    # so have to deviate from C++ version, just show port number
    brain.screen.print_at("%02d" % (index+1), x=xpos+13, y=ypos+13)

    # Show actual speed
    brain.screen.set_pen_color(Color.YELLOW)
    brain.screen.print_at("%4d" % v2, x=xpos+13, y=ypos+30)

    # Show position
    brain.screen.print_at("%5.1f" % pos, x=xpos+5, y=ypos+45)

    # Show current
    brain.screen.print_at("%4.1fA" % c1, x=xpos+5, y=ypos+60)

    # Show temperature
    brain.screen.print_at("%4.0fC" % t1, x=xpos+5, y=ypos+75)

    brain.screen.set_pen_color(Color.WHITE)
    brain.screen.draw_line(xpos, ypos+14, xpos+48, ypos+14)


#-----------------------------------------------------------------------------*/
#   @brief  Display task - show some useful motor data                        */
#-----------------------------------------------------------------------------*/
def display_task():
    brain.screen.set_font(FontType.PROP40)
    brain.screen.set_pen_color(Color.RED)
    brain.screen.print_at("TEST DRIVE CODE", x=90, y=160)

    motors = [motor_1,
              motor_2,
              motor_3,
              motor_4,
              motor_5,
              motor_6,
              motor_7,
              motor_8,
              motor_9,
              motor_10]

    while True:
        index = 0
        for m in motors:
            displayMotorData(m, index)
            index = index+1

        # display using back buffer, stops flickering
        brain.screen.render()

        sleep(10)

# Run the drive code
drive = Thread(drive_task)

# Run the display code
display = Thread(display_task)

# Python now drops into REPL