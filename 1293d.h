#ifndef __l293d
#define __l293d

//---------------------------------------------------------------------
// GPIO pin assignments for the L293D motor driver
//---------------------------------------------------------------------

#define M1 5          // Input pin IN1 of L293D connected to P0.5
#define M2 6          // Input pin IN2 of L293D connected to P0.6

// Configures the motor driver control pins as output pins.
void init_l293d(void);

// Drives the motor in the opening direction.
void door_open(void);

// Drives the motor in the closing direction.
void door_close(void);

// Disables motor movement by making both control lines LOW.
void motor_stop(void);

#endif
