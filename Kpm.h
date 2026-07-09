#ifndef __KPM_H__
#define __KPM_H__

#include "types.h"      // Contains custom data type definitions

//**************************************************************
// Matrix Keypad Row Pin Definitions
//**************************************************************

// GPIO pins connected to keypad rows
#define row0 16
#define row1 17
#define row2 18
#define row3 19

//**************************************************************
// Matrix Keypad Column Pin Definitions
//**************************************************************

// GPIO pins connected to keypad columns
#define col0 20
#define col1 21
#define col2 22
#define col3 23

//**************************************************************
// Function Declarations
//**************************************************************

// Configure keypad interface pins
void init_kpm(void);

// Detect whether any key is pressed
u32 colscan(void);

// Read and return the pressed key value
s32 keyscan(void);

// Identify the selected row
u32 rowcheck(void);

// Identify the selected column
u32 colcheck(void);

// Read an integer value entered through the keypad
s32 readnum(void);

// Accept a user ID from the keypad
void id_kpm(s8 *ptr);

// Accept a password from the keypad
// The entered digits are displayed as '*' on the LCD
void password_kpm(s8 *ptr);

#endif
