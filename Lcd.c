#include <lpc21xx.h>        // LPC21xx microcontroller register file
#include "defines.h"        // LCD pin mappings and control macros
#include "types.h"          // Custom data type definitions
#include "delay.h"          // Delay routines for LCD communication
#include "lcd.h"            // LCD function prototypes

//***************************************************************
// Function : write_lcd()
// Purpose  : Transfers a single byte to the LCD.
//            The byte is treated as command or data depending
//            on the current RS pin state.
// Input    : data - Byte to be sent to the LCD.
//***************************************************************
void write_lcd(u8 data)
{
    // Select write mode by clearing the RW pin.
    SCLRBIT(IOCLR0, RW);

    // Load the byte onto the LCD data bus.
    WBYTE(IOPIN0, LCD_DATA, data);

    // Generate an enable pulse.
    SSETBIT(IOSET0, EN);
    delay_us(1);

    // End the enable pulse so the LCD captures the data.
    SCLRBIT(IOCLR0, EN);

    // Allow sufficient execution time for the LCD.
    delay_ms(2);
}

//***************************************************************
// Function : cmd_lcd()
// Purpose  : Sends an instruction to the LCD controller.
// Input    : cmd - LCD command code.
//***************************************************************
void cmd_lcd(u8 cmd)
{
    // RS LOW selects the instruction register.
    SCLRBIT(IOCLR0, RS);

    // Transmit the command.
    write_lcd(cmd);
}

//***************************************************************
// Function : init_lcd()
// Purpose  : Sets up the LCD for 8-bit, 2-line operation.
//***************************************************************
void init_lcd()
{
    // Configure LCD data lines as output pins.
    WBYTE(IODIR0, LCD_DATA, 255);

    // Configure LCD control lines as outputs.
    SETBIT(IODIR0, RS);
    SETBIT(IODIR0, RW);
    SETBIT(IODIR0, EN);

    // Wait for the LCD to become stable after power-up.
    delay_ms(15);

    // Initialization sequence recommended by the LCD datasheet.
    cmd_lcd(MODE_8BIT_LINE1);
    delay_ms(5);

    cmd_lcd(MODE_8BIT_LINE1);
    delay_us(100);

    cmd_lcd(MODE_8BIT_LINE1);

    // Enable 8-bit interface with two display lines.
    cmd_lcd(MODE_8BIT_LINE2);

    // Turn on display and cursor.
    cmd_lcd(DISP_ON_CUR_ON);

    // Remove any previous contents.
    cmd_lcd(CLEAR_LCD);

    // Automatically move the cursor to the next position.
    cmd_lcd(SHIFT_CUR_RIGHT);
}

//***************************************************************
// Function : char_lcd()
// Purpose  : Displays a single character on the LCD.
// Input    : character - ASCII character to print.
//***************************************************************
void char_lcd(u8 character)
{
    // RS HIGH selects the data register.
    SETBIT(IOSET0, RS);

    // Send the character to the display.
    write_lcd(character);
}

//***************************************************************
// Function : str_lcd()
// Purpose  : Prints a string on the LCD.
// Input    : p - Pointer to a NULL-terminated string.
//***************************************************************
void str_lcd(u8 *p)
{
    // Continue printing until the string terminator is found.
    while(*p)
    {
        char_lcd(*p++);
    }
}
