// uart.h

//============================================================
// UART System Clock Parameters

#define FOSC 12000000        // Frequency of the external oscillator
#define PLL 5                // PLL multiplication factor
#define CCLK (FOSC*PLL)      // Processor clock frequency
#define PCLK (CCLK/4)        // Peripheral clock frequency
#define BAUD 9600            // Desired UART communication speed

// UART Baud Rate Divider
#define DIVISOR (PCLK/(16*BAUD))

//============================================================
// UART Pin Assignments

#define TXD0_PIN 0x00000001  // Function selection value for TXD0
#define RXD0_PIN 0x00000004  // Function selection value for RXD0

//============================================================
// UART Line Control Register Settings

#define DLAB 7               // Enables access to divisor registers
#define _8BIT 3              // Selects 8-bit data format
#define WORD_LENGTH_SELECT _8BIT

//============================================================
// UART Line Status Register Flags

#define TEMT_BIT 6           // Indicates transmit register is empty
#define DR_BIT   0           // Indicates received data is available

//============================================================
// UART Driver Function Declarations

// Configures UART0 for serial communication.
void init_uart0(void);

// Sends a single byte through UART0.
void u0_Tx_byte(unsigned char byte);

// Receives one byte from UART0.
unsigned char u0_Rx_byte(void);

// Sends a null-terminated string.
void u0_Tx_str(char *str);

// Receives characters until Enter is pressed.
char *u0_Rx_str(void);

// Interrupt handler for UART0 receive/transmit events.
void UART0_isr(void) __irq;
