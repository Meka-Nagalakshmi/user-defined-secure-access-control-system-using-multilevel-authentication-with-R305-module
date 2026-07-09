// i2c.h

#include "types.h"      // Header file containing user-defined data types

//***************************************************************
// I2C Function Declarations
//***************************************************************

// Configures and enables the I2C module
void init_i2c(void);

// Issues an I2C START signal to begin communication
void i2c_start(void);

// Sends an I2C STOP signal to end communication
void i2c_stop(void);

// Issues a Repeated START without releasing the I2C bus
void i2c_restart(void);

// Sends a single data byte to the connected I2C device
void i2c_write(u8 sdat);

// Receives a byte and acknowledges it (ACK)
u8 i2c_mack(void);

// Receives the final byte and responds with NACK
u8 i2c_nack(void);
