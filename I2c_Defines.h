// i2c_defines.h

//=====================================================================
// I2C Pin Configuration for LPC2148
//=====================================================================
#define SCL 2                  // GPIO P0.2 configured as I2C clock pin
#define SDA 3                  // GPIO P0.3 configured as I2C data pin


//=====================================================================
// PINSEL0 Configuration Values
// Used to assign alternate I2C functionality to the GPIO pins
//=====================================================================
#define SCL_PIN_FUNC 0X00000010    // Enable SCL function on pin P0.2
#define SDA_PIN_FUNC 0X00000040    // Enable SDA function on pin P0.3


//=====================================================================
// System Clock Parameters
//=====================================================================
#define FOSC      12000000         // Oscillator frequency connected to the controller
#define CCLK      (FOSC*5)         // Core clock after PLL multiplication
#define PCLK      (CCLK/4)         // Peripheral clock driving the I2C module
#define I2C_SPEED 100000           // Desired I2C bus speed (100 kHz)

// Calculate the timing value required for SCL generation
#define LOAD_VAL  ((PCLK/I2C_SPEED)/2)


//=====================================================================
// I2CONSET Register Bit Definitions
// Used to control various I2C operations
//=====================================================================
#define AA    2      // Enable acknowledge response
#define SI    3      // Indicates completion of an I2C operation
#define STO   4      // Requests generation of a STOP condition
#define STA   5      // Requests generation of a START condition
#define I2EN  6      // Activates the I2C peripheral


//=====================================================================
// I2CONCLR Register Bit Definitions
// Used for clearing specific control/status bits
//=====================================================================
#define AAC    2     // Reset acknowledge control bit
#define SIC    3     // Reset serial interrupt flag
#define STAC   5     // Clear START request bit
#define I2ENC  6     // Turn OFF the I2C peripheral
