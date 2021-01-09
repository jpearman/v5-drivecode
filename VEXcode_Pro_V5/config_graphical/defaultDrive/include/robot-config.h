using namespace vex;

extern brain Brain;

// VEXcode devices
extern motor Motor1;
extern motor Motor2;
extern motor Motor3;
extern motor Motor4;
extern motor Motor5;
extern motor Motor6;
extern motor Motor7;
extern motor Motor8;
extern motor Motor9;
extern motor Motor10;
extern controller Controller1;
extern limit LimitSwitch3F;
extern limit LimitSwitch3R;
extern limit LimitSwitch8F;
extern limit LimitSwitch8R;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );