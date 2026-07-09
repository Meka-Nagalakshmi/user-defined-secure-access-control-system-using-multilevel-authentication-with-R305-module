#include "uart.h"          // UART function declarations
#include "lcd.h"           // LCD functions (used if UART messages are displayed)
#include "types.h"         // User-defined data types
#include <LPC21xx.h>       // LPC21xx register definitions
char ch, dummy;            // Variables used inside UART interrupt
// Variables declared in r305.c
extern u8 j, rec_buf[25], i;
//*************************************************************
// Function : init_uart0()
// Purpose  : Initialize UART0 for communication with the
//            R305 fingerprint sensor.
//*************************************************************
void init_uart0(void)
{
    // Configure P0.0 as TXD0 and P0.1 as RXD0
    PINSEL0 &= ~(15 << 0);
    PINSEL0 |= (TXD0_PIN | RXD0_PIN);
    // Enable access to baud-rate registers (DLL & DLM)
    U0LCR = (1 << DLAB);
    // Select word length (usually 8-bit, No parity, 1 stop bit)
    U0LCR |= WORD_LENGTH_SELECT;
    // Load baud-rate divisor registers
    U0DLM = DIVISOR >> 8;      // Higher byte
    U0DLL = DIVISOR;           // Lower byte
    // Disable DLAB so UART can begin normal communication
    U0LCR &= ~(1 << DLAB);
    //------------- UART Interrupt Configuration -------------
    VICIntSelect = 0x00000000;      // Configure as IRQ (not FIQ)
    VICVectAddr0 = (unsigned)UART0_isr; // Address of UART ISR
    VICVectCntl0 = 0x20 | 6;        // Enable vector slot 0 for UART0
    VICIntEnable = 1 << 6;          // Enable UART0 interrupt
    // Enable UART Receive and THRE interrupts
    U0IER = 0x03;
}
//*************************************************************
// Function : UART0_isr()
// Purpose  : UART Interrupt Service Routine.
//            Stores received bytes into rec_buf[].
//*************************************************************
void UART0_isr(void) __irq
{
    // Check whether receive interrupt occurred.
    if(U0IIR & 0x04)
    {
        // Read received byte.
        // Reading U0RBR automatically clears RX interrupt.
        ch = U0RBR;
       // Store received byte into receive buffer.
        rec_buf[j++] = ch;
        // Once minimum packet length is received,
        // notify the application.
        if(j >= 12)
        {
            i= 1;
        }
    }
    else
    {
        // Dummy read clears THRE interrupt.
        dummy = U0IIR;
    }
    // Inform VIC that interrupt servicing is complete.
    VICVectAddr = 0;
}
//*************************************************************
// Function : u0_Tx_byte()
// Purpose  : Transmit one byte through UART.
//*************************************************************
void u0_Tx_byte(unsigned char byte)
{
    // Load byte into UART Transmit Holding Register.
    U0THR = byte;
   // Wait until transmission is completely finished.
    while(((U0LSR >> TEMT_BIT) & 1) == 0);
}
//*************************************************************
// Function : u0_Rx_byte()
// Purpose  : Receive one byte from UART.
// Return   : Received byte.
//*************************************************************
unsigned char u0_Rx_byte(void)
{
    // Wait until data becomes available.
    while(((U0LSR >> DR_BIT) & 1) == 0);
	// Return received byte.
    return U0RBR;
}
//*************************************************************
// Function : u0_Tx_str()
// Purpose  : Transmit a NULL terminated string.
//*************************************************************
void u0_Tx_str(char *str)
{
    // Send characters one by one.
    while(*str)
    {
        u0_Tx_byte(*str++);
    }
}
//*************************************************************
// Function : u0_Rx_str()
// Purpose  : Receive a string until Enter key.
// Return   : Pointer to received string.
//*************************************************************
char* u0_Rx_str(void)
{
    // Static buffer retains its value after function returns.
    static char str[50];
   char i = 0;
    do
    {
        // Receive one character.
        str[i] = u0_Rx_byte();
        // Echo received character back to terminal.
        u0_Tx_byte(str[i]);
        // Stop receiving when Enter is pressed.
        if((str[i] == '\r') || (str[i] == '\n'))
        {
            break;
        }
        i++;
    } while(i != 49);
    // Replace Enter character with NULL terminator.
    str[i-1] = 0;
    return str;
}
