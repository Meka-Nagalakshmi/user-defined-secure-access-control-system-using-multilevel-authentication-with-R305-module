#include "types.h"          // Contains user-defined data types like u8, u16, s8
#include "i2c.h"            // I2C driver function declarations
#include "delay.h"          // Software delay functions


//*****************************************************************
// Function Name : i2c_eeprom_write_byte()
// Description   : Writes a single byte into EEPROM.
// Parameters    :
//      SlaveAddr - 7-bit EEPROM slave address.
//      BuffAddr  - EEPROM memory location.
//      data      - Data byte to be written.
//*****************************************************************
void i2c_eeprom_write_byte(u8 SlaveAddr, u16 BuffAddr, u8 data)
{
    i2c_start();                    // Generate START condition

    i2c_write(SlaveAddr << 1);      // Send EEPROM slave address with Write bit (R/W = 0)

    i2c_write(BuffAddr >> 8);       // Send EEPROM memory address (High byte)

    i2c_write(BuffAddr);            // Send EEPROM memory address (Low byte)

    i2c_write(data);                // Send one byte of data to EEPROM

    i2c_stop();                     // Generate STOP condition to complete write

    delay_ms(10);                   // Wait for EEPROM internal write cycle to complete
}


//*****************************************************************
// Function Name : i2c_eeprom_read_byte()
// Description   : Reads one byte from EEPROM.
// Parameters    :
//      SlaveAddr  - EEPROM slave address.
//      BufferAddr - EEPROM memory location.
// Return         : Data byte read from EEPROM.
//*****************************************************************
u8 i2c_eeprom_read_byte(u8 SlaveAddr, u16 BufferAddr)
{
    u8 data;                        // Variable to store received data

    i2c_start();                    // Generate START condition

    i2c_write(SlaveAddr << 1);      // Send slave address with Write bit

    i2c_write(BufferAddr >> 8);     // Send memory address High byte

    i2c_write(BufferAddr);          // Send memory address Low byte

    i2c_restart();                  // Generate Repeated START

    i2c_write((SlaveAddr << 1) | 1);// Send slave address with Read bit (R/W = 1)

    data = i2c_nack();              // Read final byte and send NACK

    i2c_stop();                     // Generate STOP condition

    return data;                    // Return received data
}


//*****************************************************************
// Function Name : i2c_eeprom_write_page()
// Description   : Writes multiple bytes to EEPROM in one page.
// Parameters    :
//      SlaveAddr - EEPROM slave address.
//      BuffAddr  - Starting EEPROM memory location.
//      p         - Pointer to data buffer.
//      nbytes    - Number of bytes to write.
//*****************************************************************
void i2c_eeprom_write_page(u8 SlaveAddr, u16 BuffAddr, s8 *p, u8 nbytes)
{
    u8 i;                           // Loop counter

    i2c_start();                    // Generate START condition

    i2c_write(SlaveAddr << 1);      // Send slave address with Write bit

    i2c_write(BuffAddr >> 8);       // Send address High byte

    i2c_write(BuffAddr);            // Send address Low byte

    // Transmit all bytes sequentially
    for(i = 0; i < nbytes; i++)
    {
        i2c_write(p[i]);            // Write each byte from buffer
    }

    i2c_stop();                     // End communication

    delay_ms(10);                   // Wait until EEPROM finishes internal programming
}


//*****************************************************************
// Function Name : i2c_eeprom_seq_read()
// Description   : Reads multiple bytes sequentially from EEPROM.
// Parameters    :
//      SlaveAddr - EEPROM slave address.
//      BuffAddr  - Starting EEPROM address.
//      p         - Buffer to store received data.
//      nbytes    - Number of bytes to read.
//*****************************************************************
void i2c_eeprom_seq_read(u8 SlaveAddr, u16 BuffAddr, s8 *p, u8 nbytes)
{
    u8 i;                           // Loop counter

    i2c_start();                    // Generate START condition

    i2c_write(SlaveAddr << 1);      // Send slave address with Write bit

    i2c_write(BuffAddr >> 8);       // Send memory address High byte

    i2c_write(BuffAddr);            // Send memory address Low byte

    i2c_restart();                  // Generate Repeated START

    i2c_write((SlaveAddr << 1) | 1);// Send slave address with Read bit

    // Read all bytes except the last one.
    // ACK is sent after every received byte to continue reading.
    for(i = 0; i < (nbytes - 1); i++)
    {
        p[i] = i2c_mack();          // Receive byte and send ACK
    }

    // Receive final byte and send NACK to terminate reading
    p[i] = i2c_nack();

    i2c_stop();                     // Generate STOP condition

    delay_ms(10);                   // Small delay before next EEPROM operation
}
