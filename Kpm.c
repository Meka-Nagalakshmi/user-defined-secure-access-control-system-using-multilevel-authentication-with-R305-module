#include "kpm.h"             // Keypad function prototypes
#include "types.h"           // Custom data type definitions
#include "defines.h"         // GPIO helper macros
#include "delay.h"           // Delay routines
#include "lcd.h"             // LCD interface functions
#include <lpc21xx.h>         // LPC21xx microcontroller header

// Flag used to switch to menu mode when an external interrupt occurs.
u8 menu_flag = 0;

// 4x4 keypad character mapping.
s32 kpmlut[4][4] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','-'},
    {'*','0','=','B'}        // '=' -> Enter, 'B' -> Backspace
};

//************************************************************
// Function : init_kpm()
// Purpose  : Initialize keypad hardware by configuring
//            row lines as outputs.
//************************************************************
void init_kpm(void)
{
    // Set row pins (P1.16-P1.19) as output pins.
    WNIBBLE(IODIR1, row0, 15);
}

//************************************************************
// Function : colscan()
// Purpose  : Verify whether any keypad key is pressed.
// Returns  : 0 -> Key detected
//            1 -> No key detected
//************************************************************
u32 colscan(void)
{
    // If any column line becomes LOW, a key press exists.
    return (RNIBBLE(IOPIN1, col0) < 15) ? 0 : 1;
}

//************************************************************
// Function : rowcheck()
// Purpose  : Identify the active row containing the key.
// Returns  : Row index (0-3)
//************************************************************
u32 rowcheck(void)
{
    u32 rno;

    // Activate one row at a time.
    for(rno = 0; rno < 4; rno++)
    {
        // Make the current row LOW while others remain HIGH.
        WNIBBLE(IOPIN1, row0, ~(1 << rno));

        // Stop scanning once a key press is detected.
        if(colscan() == 0)
        {
            break;
        }
    }

    // Reset all rows after scanning.
    WNIBBLE(IOPIN1, row0, 0x0);

    return rno;
}

//************************************************************
// Function : colcheck()
// Purpose  : Determine the column of the pressed key.
// Returns  : Column index (0-3)
//************************************************************
u32 colcheck(void)
{
    u32 cno;

    // Read each column until a LOW signal is found.
    for(cno = 0; cno < 4; cno++)
    {
        if(RBIT(IOPIN1, (col0 + cno)) == 0)
        {
            break;
        }
    }

    return cno;
}

//************************************************************
// Function : keyscan()
// Purpose  : Read a keypad key and return its ASCII value.
// Returns  : ASCII character of key
//            -1 if menu interrupt is triggered
//************************************************************
s32 keyscan(void)
{
    s32 keyv;
    s32 rno, cno;

    // Wait until a key is pressed or menu mode is requested.
    while(colscan() && menu_flag == 0);

    if(menu_flag == 1)
    {
        return -1;
    }

    // Get row and column positions.
    rno = rowcheck();
    cno = colcheck();

    // Fetch corresponding character.
    keyv = kpmlut[rno][cno];

    // Wait until the key is released.
    while(!colscan());

    // Small delay for switch debouncing.
    delay_ms(100);

    return keyv;
}

//************************************************************
// Function : readnum()
// Purpose  : Read a numeric value from the keypad.
//            '=' confirms input.
//            'B' removes the previous digit.
// Returns  : Entered integer value.
//************************************************************
s32 readnum(void)
{
    s32 key, cnt = 0;
    int num = 0;

    while(1)
    {
        key = keyscan();

        // Return immediately if menu mode is activated.
        if(key == -1)
        {
            cnt = 0;
            return -1;
        }

        // Process numeric digits.
        if((key >= '0') && (key <= '9') && (cnt < 4))
        {
            cnt++;

            // Display the typed digit.
            char_lcd(key);

            // Update integer value.
            num = (num * 10) + (key - '0');
        }

        // Remove the previously entered digit.
        else if((key == 'B') && (cnt != 0))
        {
            cnt--;

            cmd_lcd(0x10);
            char_lcd(' ');
            cmd_lcd(0x10);

            num /= 10;
        }

        // Finish input when Enter is pressed.
        else if((key == '=') && (cnt != 0))
        {
            cnt = 0;
            return num;
        }
    }
}

//************************************************************
// Function : password_kpm()
// Purpose  : Accept a 4-digit password.
//            LCD displays '*' instead of actual digits.
//************************************************************
void password_kpm(char *p)
{
    u8 a;
    u8 cnt = 0;

    while(1)
    {
        a = keyscan();

        // Accept only valid numeric keys.
        if((a >= '0') && (a <= '9') && (cnt != 4))
        {
            cnt++;

            delay_ms(300);

            // Replace entered digit with '*'.
            cmd_lcd(0x10);
            char_lcd('*');

            // Save actual character.
            *p = a;
            p++;
        }

        // Delete the previous digit.
        else if((a == 'B') && (cnt != 0))
        {
            cmd_lcd(0x10);
            char_lcd(' ');
            cmd_lcd(0x10);

            cnt--;
            p--;
        }

        // End password entry after four digits.
        else if((a == '=') && (cnt == 4))
        {
            cnt = 0;

            *p = '\0';

            return;
        }
    }
}

//************************************************************
// Function : id_kpm()
// Purpose  : Read a 4-digit user ID from the keypad.
//************************************************************
void id_kpm(s8 *p)
{
    s8 a;
    s8 cnt = 0;

    while(1)
    {
        a = keyscan();

        // Store numeric digits.
        if((a >= '0') && (a <= '9') && (cnt != 4))
        {
            char_lcd(a);

            p[cnt] = a;
            cnt++;
        }

        // Delete previously entered digit.
        else if((a == 'B') && (cnt != 0))
        {
            cnt--;

            cmd_lcd(0x10);
            char_lcd(' ');
            cmd_lcd(0x10);
        }

        // Complete ID entry after four digits.
        else if((a == '=') && (cnt == 4))
        {
            p[cnt] = '\0';
            return;
        }
    }
}
