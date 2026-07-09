// motor.c

#include <lpc21xx.h>      // LPC21xx microcontroller register header
#include "l293d.h"        // L293D driver declarations

//***************************************************************
// Function : init_l293d()
// Purpose  : Configures the GPIO pins used for motor control
//            as output pins.
//***************************************************************
void init_l293d(void)
{
    // Enable output mode for both motor control pins.
    IODIR0 |= ((1 << M1) | (1 << M2));
}

//***************************************************************
// Function : door_open()
// Purpose  : Drives the motor in the direction required
//            to unlock/open the door.
//***************************************************************
void door_open(void)
{
    // Drive IN1 HIGH.
    IOSET0 = 1 << M1;

    // Drive IN2 LOW.
    IOCLR0 = 1 << M2;

    // The motor now rotates in the opening direction.
}

//***************************************************************
// Function : door_close()
// Purpose  : Drives the motor in the opposite direction
//            to lock/close the door.
//***************************************************************
void door_close(void)
{
    // Drive IN1 LOW.
    IOCLR0 = 1 << M1;

    // Drive IN2 HIGH.
    IOSET0 = 1 << M2;

    // The motor rotates in the closing direction.
}

//***************************************************************
// Function : motor_stop()
// Purpose  : Turns off motor movement by clearing both
//            control outputs.
//***************************************************************
void motor_stop(void)
{
    // Disable both motor input signals.
    IOCLR0 = 1 << M1;
    IOCLR0 = 1 << M2;

    // No voltage is applied to the motor, so it stops.
}
