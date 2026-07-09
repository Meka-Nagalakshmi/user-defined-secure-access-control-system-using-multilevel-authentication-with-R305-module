#include "lcd.h"            // LCD interface function prototypes
#include "kpm.h"            // Matrix keypad driver functions
#include "i2c.h"            // I2C peripheral driver
#include "i2c_eeprom.h"     // EEPROM access routines through I2C
#include "uart.h"           // UART communication driver
#include "r305.h"           // Fingerprint sensor interface
#include "delay.h"          // Delay utility functions
#include "menu.h"           // Menu handling and external interrupt functions
#include "types.h"          // Custom data type definitions
#include "l293d.h"          // Motor driver control routines

// Flag indicating whether the system should switch to menu mode.
extern u8 menu_flag;

int main()
{
    // Initialize all required hardware modules.
    init_lcd();
    init_kpm();
    init_uart0();
    init_i2c();
    init_eint2();
    init_ids();
    init_l293d();

    // Display the startup message.
    cmd_lcd(0x01);
    str_lcd("SECURE ACCESS");

    cmd_lcd(0xC0);
    str_lcd("CONTROL SYSTEM");

    // Allow the welcome screen to remain visible briefly.
    delay_ms(1000);

    // Main execution loop.
    while(1)
    {
        // Open the administrator menu if the interrupt flag is set.
        if(menu_flag == 1)
        {
            menu();
        }

        // Execute the user authentication process.
        login();
    }
}
