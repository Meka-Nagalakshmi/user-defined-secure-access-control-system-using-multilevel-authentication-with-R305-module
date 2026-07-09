// lcd.h

#include <string.h>     // Standard C string library
#include "types.h"      // User-defined integer data types

//---------------------------------------------------------------------
// LCD Pin Configuration
//---------------------------------------------------------------------

// LCD data lines are connected to P0.8 - P0.15
#define LCD_DATA 8

// LCD control signal connections
#define RS       16      // Register Select control pin
#define EN       17      // Enable control pin
#define RW       18      // Read/Write control pin

//---------------------------------------------------------------------
// Basic LCD Instructions
//---------------------------------------------------------------------

#define CLEAR_LCD             0x01    // Erase display and reset cursor
#define RET_CUR_HOME          0x02    // Place cursor at the home position

//---------------------------------------------------------------------
// Display Control Instructions
//---------------------------------------------------------------------

#define DISP_OFF              0x08    // Disable LCD display
#define DISP_ON               0x0C    // Enable display without cursor
#define DISP_ON_CUR_ON        0x0E    // Enable display with visible cursor
#define DISP_ON_CUR_ON_BLK_ON 0x0F    // Enable display with blinking cursor

//---------------------------------------------------------------------
// Entry Mode Settings
//---------------------------------------------------------------------

#define DISP_SHIFT_OFF_INC    0x06    // Cursor advances after each character
#define DISP_SHIFT_INC        0x07    // Cursor advances and display shifts

//---------------------------------------------------------------------
// LCD Interface Configuration
//---------------------------------------------------------------------

#define MODE_8BIT_LINE1       0x30    // 8-bit mode with one display line
#define MODE_8BIT_LINE2       0x38    // 8-bit mode with two display lines
#define MODE_4BIT_LINE1       0x20    // 4-bit mode with one display line
#define MODE_4BIT_LINE2       0x28    // 4-bit mode with two display lines

//---------------------------------------------------------------------
// Cursor Address Locations
//---------------------------------------------------------------------

#define GOTO_LINE1_POS_0      0x80    // Beginning of first row
#define GOTO_LINE2_POS_0      0xC0    // Beginning of second row
#define GOTO_LINE3_POS_0      0x94    // Beginning of third row (20x4 LCD)
#define GOTO_LINE4_POS_0      0xD4    // Beginning of fourth row (20x4 LCD)

//---------------------------------------------------------------------
// Cursor Movement Commands
//---------------------------------------------------------------------

#define SHIFT_CUR_LEFT        0x10    // Shift cursor one position left
#define SHIFT_CUR_RIGHT       0x14    // Shift cursor one position right

//---------------------------------------------------------------------
// LCD Driver Function Declarations
//---------------------------------------------------------------------

// Writes a byte to the LCD interface.
void write_lcd(u8);

// Issues a control command to the LCD.
void cmd_lcd(u8);

// Performs LCD initialization.
void init_lcd(void);

// Displays one character on the LCD.
void char_lcd(u8);

// Displays a complete string on the LCD.
void str_lcd(u8 *);
