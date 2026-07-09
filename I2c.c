#include <lpc21xx.h>        // Header file containing LPC21xx register definitions
#include "types.h"          // Contains custom data type definitions (u8, u16, u32, etc.)
#include "i2c_defines.h"    // Includes I2C configuration macros and pin assignments

//***************************************************************
// Function    : init_i2c()
// Purpose     : Configures the LPC21xx I2C module to operate as
//               an I2C Master device.
//***************************************************************
void init_i2c(void)
{
    // Reset the pin function settings for SDA and SCL pins
    PINSEL0 &= ~(15 << (2 * 2));

    // Assign alternate functions to enable I2C communication
    PINSEL0 |= SCL_PIN_FUNC | SDA_PIN_FUNC;

    // Configure SCL low clock duration
    I2SCLL = LOAD_VAL;

    // Configure SCL high clock duration
    I2SCLH = LOAD_VAL;

    // Turn ON the I2C interface
    I2CONSET = 1 << I2EN;
}

//***************************************************************
// Function    : i2c_start()
// Purpose     : Sends a START condition on the I2C bus.
//***************************************************************
void i2c_start(void)
{
    // Request the I2C controller to issue a START signal
    I2CONSET = 1 << STA;

    // Wait until the START operation is completed
    while(((I2CONSET >> SI) & 1) == 0);

    // Reset the START request bit
    I2CONCLR = 1 << STAC;
}

//***************************************************************
// Function    : i2c_stop()
// Purpose     : Terminates I2C communication by sending STOP.
//***************************************************************
void i2c_stop(void)
{
    // Generate the STOP condition
    I2CONSET = 1 << STO;

    // Clear the interrupt flag to complete the operation
    I2CONCLR = 1 << SIC;
}

//***************************************************************
// Function    : i2c_restart()
// Purpose     : Generates a repeated START without releasing
//               the I2C bus.
//***************************************************************
void i2c_restart(void)
{
    // Initiate a repeated START sequence
    I2CONSET = 1 << STA;

    // Reset interrupt status flag
    I2CONCLR = 1 << SIC;

    // Wait until the repeated START has been transmitted
    while(((I2CONSET >> SI) & 1) == 0);

    // Clear the START control bit
    I2CONCLR = 1 << STAC;
}

//***************************************************************
// Function    : i2c_write()
// Parameter   : sdat - Data byte to transmit
// Purpose     : Sends a single byte to the slave device.
//***************************************************************
void i2c_write(u8 sdat)
{
    // Copy data into the transmit register
    I2DAT = sdat;

    // Clear interrupt flag to begin transmission
    I2CONCLR = 1 << SIC;

    // Wait until transmission finishes
    while(((I2CONSET >> SI) & 1) == 0);
}

//***************************************************************
// Function    : i2c_nack()
// Purpose     : Receives the final byte from the slave and
//               responds with a NACK.
// Returns     : Received data byte.
//***************************************************************
u8 i2c_nack(void)
{
    // Leave AA cleared so a NACK is transmitted
    I2CONCLR = 1 << SIC;

    // Wait until a byte is received
    while(((I2CONSET >> SI) & 1) == 0);

    // Return the received value
    return I2DAT;
}

//***************************************************************
// Function    : i2c_mack()
// Purpose     : Receives a byte from the slave and sends an ACK.
//               Used when additional bytes are expected.
// Returns     : Received data byte.
//***************************************************************
u8 i2c_mack(void)
{
    // Enable automatic ACK generation
    I2CONSET = 1 << AA;

    // Clear interrupt flag to continue reception
    I2CONCLR = 1 << SIC;

    // Wait until the incoming byte is available
    while(((I2CONSET >> SI) & 1) == 0);

    // Disable ACK for future operations if required
    I2CONCLR = 1 << AAC;

    // Return the received byte
    return I2DAT;
}
