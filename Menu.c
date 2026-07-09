#include<lpc21xx.h>
#include "lcd.h"
#include "types.h"
#include "menu.h"
#include "kpm.h"
#include "lcd.h"
#include "string.h"
#include"delay.h"
#include "i2c_eeprom.h"
#include "r305.h"
#include "l293d.h"

//#define NEW_ROM      // Enable this when EEPROM is blank for first-time programming
#define OLD_ROM        // Enable this to read previously stored IDs from EEPROM

extern u8 menu_flag;   // Global flag set by external interrupt to enter admin menu

static u8 flag=0;      // Reserved flag (currently not used)

u8 admin_id,ids=0;     // admin_id stores admin ID, ids stores total enrolled users

s8 pswd1[5],pswd2[5]="435";   // pswd1 stores entered password, pswd2 stores default password

u32 id;                // Variable to store entered ID


//---------------------------------------------------------------------------
// Configure External Interrupt-2 (EINT2)
// Used to enter administrator menu whenever external switch is pressed
//---------------------------------------------------------------------------
void init_eint2(void)
{
	// Select P0.7 as GPIO before configuring as EINT2
	PINSEL0 &= ~((u32)3<<14);

	// Configure P0.7 function as External Interrupt-2
	PINSEL0 |= EINT2_INPUT_PIN;

	// Enable EINT2 interrupt channel in VIC
	VICIntEnable |= 1<<EINT2_VIC_CHNO;

	// Assign ISR slot-1 for EINT2
	VICVectCntl1 = (1<<5)|EINT2_VIC_CHNO;

	// Load ISR address into VIC
	VICVectAddr1 =(u32)eint2_isr;

	// Configure EINT2 as Edge Triggered Interrupt
	EXTMODE =(1<<2);

	// Falling edge trigger is selected by default
}


//---------------------------------------------------------------------------
// Initialize number of enrolled IDs
// NEW_ROM  -> Initialize EEPROM with zero IDs
// OLD_ROM  -> Read previously stored ID count from EEPROM
//---------------------------------------------------------------------------
void init_ids(void)
{
#ifdef NEW_ROM

	ids = 0;                                    // No users stored initially

	i2c_eeprom_write_byte(0X50,0X0000,ids);     // Save ID count into EEPROM

#endif

#ifdef OLD_ROM

	ids = i2c_eeprom_read_byte(0x50,0x0000);    // Read total IDs from EEPROM

#endif
}


//---------------------------------------------------------------------------
// External Interrupt Service Routine
// Executed whenever EINT2 switch is pressed
//---------------------------------------------------------------------------
void eint2_isr(void) __irq
{
	menu_flag=1;          // Notify main program to open admin menu

	EXTINT =1<<2;         // Clear EINT2 interrupt flag

	VICVectAddr =0;       // Inform VIC that ISR execution is complete
}


//---------------------------------------------------------------------------
// Search whether a given fingerprint ID exists in EEPROM database
// Returns:
//      EEPROM Address -> if ID found
//      0              -> if ID not found
//---------------------------------------------------------------------------
u16 is_id_in_db(char id)
{
	char i=0,id2;

	// Search through every stored user ID
	for(i=0;i<ids;i++)
	{
		// Read stored ID from EEPROM
		id2 = i2c_eeprom_read_byte(0x50,(0x0001+(i*6)));
		// Compare stored ID with requested ID
		if(id2==id)
		{
			// Return EEPROM address of matching record
			return 0x0001+(i*6);
		}
	}
	return 0;   // ID not found
}
//---------------------------------------------------------------------------
// Verify Administrator Credentials Checks both Admin ID and Password
// Returns:     0 -> Access Granted   1 -> Access Denied
//---------------------------------------------------------------------------
s8 check_admin(void)
{
	u8 wrong=0;          // Counts invalid login attempts
//===================== ADMIN ID VERIFICATION =====================//
ID:
	cmd_lcd(CLEAR_LCD);               // Clear LCD
	cmd_lcd(GOTO_LINE1_POS_0);        // Move cursor to first line
	str_lcd("enter admin ID");        // Display prompt
	cmd_lcd(GOTO_LINE2_POS_0);        // Move cursor to second line
	id = readnum();                   // Read numeric ID from keypad
	// Valid Admin ID is assumed to be 0
	if(id != 0)
	{
		// Deny access after 3 wrong attempts
		if(wrong==3)
		{
			cmd_lcd(0x01);

			str_lcd("ACESS DENIED");

			delay_ms(1000);

			return 1;
		}

		// Display wrong ID message
		cmd_lcd(CLEAR_LCD);

		cmd_lcd(GOTO_LINE1_POS_0);

		str_lcd("wrong id");

		cmd_lcd(GOTO_LINE2_POS_0);

		str_lcd("1 more chance");

		delay_ms(500);

		// Allow another attempt
		if(wrong!=3)
		{
			wrong++;

			goto ID;
		}
	}

	// Reset wrong counter after successful ID verification
	wrong = 0;


//===================== PASSWORD VERIFICATION =====================//

PSWD:

	cmd_lcd(CLEAR_LCD);

	cmd_lcd(GOTO_LINE1_POS_0);

	str_lcd("enter PASSWORD ");

	cmd_lcd(GOTO_LINE2_POS_0);

	password_kpm(pswd1);          // Read password from keypad

	// Compare entered password with stored password
	if(strcmp((const char*)pswd1,"435")!=0)
	{
		cmd_lcd(CLEAR_LCD);

		cmd_lcd(GOTO_LINE1_POS_0);

		str_lcd("wrong pass");

		cmd_lcd(GOTO_LINE2_POS_0);

		str_lcd(" more chance");

		delay_ms(500);

		// Retry password entry
		if(wrong!=3)
		{
			wrong++;

			goto PSWD;
		}
		return 1;                  // Password verification failed
	}
	// Login successful
	cmd_lcd(CLEAR_LCD);

	cmd_lcd(GOTO_LINE1_POS_0);

	str_lcd("ACCESS GRANTED");

	delay_ms(500);

	return 0;
}
//---------------------------------------------------------------------------
// Enroll a new user by storing ID, password and fingerprint template
//---------------------------------------------------------------------------
void enroll_id(void)
{
ID:
	// Ask user to enter a new User ID
	cmd_lcd(0x01);
	str_lcd("ENTER E-ID");
	cmd_lcd(0xc0);

	// Read ID from keypad
	id = readnum();

	// Allow only IDs from 1 to 10
	if(!((id>=1)&&(id<=10)))
	{
		cmd_lcd(0x01);
		char_lcd(0);                 // Display custom warning symbol
		str_lcd("INVALID ID");
		char_lcd(0);
		cmd_lcd(0xc0);
		str_lcd("try again");
		delay_ms(500);
		goto ID;                     // Ask for ID again
	}

	// Check whether ID already exists in EEPROM
	if(is_id_in_db(id))
	{
		cmd_lcd(0x01);
		str_lcd("already There");
		cmd_lcd(0xc0);
		str_lcd("TRY Another ID");
		delay_s(1);
		goto ID;
	}

PSWD:
	// Ask user to create a password
	cmd_lcd(0x01);
	str_lcd("ENTER PASSWORD:");
	cmd_lcd(0xc0);
	password_kpm(pswd1);

	// Ask user to confirm password
	cmd_lcd(0x01);
	str_lcd("ENTER AGAIN:");
	cmd_lcd(0xc0);
	password_kpm(pswd2);

	// Verify both passwords are identical
	if(strcmp((const char*)pswd1,(const char*)pswd2)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("wrong pass");
		cmd_lcd(0xc0);
		str_lcd("try again");
		delay_ms(500);
		goto PSWD;
	}
	else
	{
		cmd_lcd(0x01);
		char_lcd(2);                 // Display fingerprint icon
		str_lcd("goto finger");
		delay_ms(500);
	}

FP1:
	// Start fingerprint enrollment process
	if(enroll(id)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("FINGER not matched");
		cmd_lcd(0xc0);
		str_lcd("TRY AGAIN");
		delay_ms(500);
		goto FP1;
	}

	// Save User ID into EEPROM
	i2c_eeprom_write_byte(0x50,(0x0001+(ids*6)),id);

	// Save password into EEPROM
	i2c_eeprom_write_page(0x50,(0x0002+(ids*6)),(s8*)pswd1,5);

	// Increase total enrolled user count
	ids++;

	// Update user count in EEPROM
	i2c_eeprom_write_byte(0x50,0x0000,ids);

	// Notify enrollment success
	cmd_lcd(0x01);
	str_lcd("ENROLL DONE ");
	char_lcd(1);                   // Display success symbol
	delay_s(1);
}
//---------------------------------------------------------------------------
// User Login
// Authentication Order:
//      1. User ID
//      2. Password
//      3. Fingerprint
//---------------------------------------------------------------------------
void login(void)
{
	int l_id;
	s8 buf1[5],buf2[5],l,retry=0;
	u16 found = 0;

	// Check whether any users are enrolled
	if(ids==0)
	{
		if(flag==0)
		{
			flag=1;
			cmd_lcd(0x01);
			str_lcd("NO DATA FOUND");
			cmd_lcd(0xc0);
			str_lcd("ENROLL");
		}
		return;
	}

	flag=0;

	// Ask user ID
	cmd_lcd(0x01);
	cmd_lcd(0x80);
	str_lcd("ENTER ID:");

	l_id=readnum();

	// Return immediately if admin interrupt occurs
	if(l_id==-1)
		return;

	// Verify ID exists
	if((found=is_id_in_db(l_id))==0)
	{
		cmd_lcd(0x01);
		str_lcd("NOT FOUND");
		delay_ms(1000);
		return;
	}

	cmd_lcd(0x01);
	str_lcd("ID IS FOUND");
	delay_ms(600);

	// Read stored password from EEPROM
	i2c_eeprom_seq_read(0x50,found+1,(s8 *)buf1,5);

PSWD:

	// Ask password
	cmd_lcd(0x01);
	str_lcd("ENTER PASSWORD:");
	cmd_lcd(0xc0);
	password_kpm((s8 *)buf2);

	// Compare entered password with stored password
	if(strcmp((const char*)buf1,(const char *)buf2)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("WRONG PASSWORD ");
		char_lcd(0);

		retry++;

		if(retry==3)
		{
			cmd_lcd(0x01);
			str_lcd("LOGIN FAILS");
			delay_ms(1000);
			return;
		}

		cmd_lcd(0xc0);
		str_lcd("1 more chances");
		delay_ms(1000);
		goto PSWD;
	}

	cmd_lcd(0x01);
	str_lcd("PASSWORD matched");
	delay_ms(500);

	retry=0;

FP:

	// Verify fingerprint
	cmd_lcd(0x01);

	if(search(l_id)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("FINGER NOT FOUND");
		cmd_lcd(0xc0);
		str_lcd("try again");
		delay_ms(600);
		retry++;
		if(retry==1)
		{
			cmd_lcd(0x01);
			str_lcd("1 more chance");
			delay_ms(800);
			goto FP;
		}
		else if(retry==2)
		{
			cmd_lcd(0x01);
			str_lcd("last chance");
			delay_ms(800);
			goto FP;
		}
		else
		{
			cmd_lcd(0x01);
			str_lcd("ACCESS DENIED");
			delay_ms(1000);
			return;
		}
	}
	// Authentication successful - Open door
	cmd_lcd(0x01);
	str_lcd("DOOR OPENING ");
	door_open();
	delay_ms(300);
	motor_stop();
	cmd_lcd(0x01);
	str_lcd("DOOR OPENED ");
	delay_ms(3000);
	// Countdown before closing door
	for(l=10;l>0;l--)
	{
		cmd_lcd(0x80);
		str_lcd("DOOR CLOSE IN ");
		cmd_lcd(0xc0);
		str_lcd("  ");
		char_lcd((l/10)+48);
		char_lcd((l%10)+48);
		str_lcd(" sec ");
	}
	// Close the door
	cmd_lcd(0x01);
	str_lcd("DOOR IS CLOSEING");
	door_close();
	delay_ms(2000);
	motor_stop();
	cmd_lcd(0x01);
	str_lcd("DOOR IS CLOSED ");

	delay_ms(1000);
}
//---------------------------------------------------------------------------
// Disable Admin External Interrupt
//---------------------------------------------------------------------------
void disable(void)
{
	VICIntEnClr = 1<<EINT2_VIC_CHNO;
}
//---------------------------------------------------------------------------
// Enable Admin External Interrupt
//---------------------------------------------------------------------------
void enable(void)
{
	VICIntEnable |= 1<<EINT2_VIC_CHNO;
}
//---------------------------------------------------------------------------
// Change User Password
// Authentication:
//      ID -> Old Password -> Fingerprint -> New Password
//---------------------------------------------------------------------------
void password_edit(void)
{// This function first verifies the user's identity using
	// stored ID, old password and fingerprint.
	// Only after successful verification,
	// the new password is written into EEPROM.
	s8 p_id,pw1[5],pw2[5],retry=0;
	u16 found=0;
	cmd_lcd(0x01);
	str_lcd("ENTER UR ID:");
	p_id = readnum();
	if((found=is_id_in_db(p_id))==0)
	{
	  cmd_lcd(0x01);
	  str_lcd("ID NOT FOUND");
	  delay_ms(1000);
	  return;	
	}
	cmd_lcd(0x01);
	str_lcd("ID IS FOUND");
	delay_ms(700);
	i2c_eeprom_seq_read(0x50,found+1,pw1,5);
PSWD:
	cmd_lcd(0x01);
	str_lcd("ENTER OLD PASSWORD:");
	cmd_lcd(0xc0);
	password_kpm(pw2);
	if(strcmp((const char*)pw2,(const char *)pw1)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("WRONG PASSWORD");
		retry++;
		if(retry==3)
		{
		  cmd_lcd(0x01);
		  str_lcd("NOT MATCHED ");
		  
		  cmd_lcd(0xc0);
		  str_lcd("unable change ");
		  delay_ms(1000);
		  return;
		}
		cmd_lcd(0xc0);
		str_lcd("1 more chances");
		delay_ms(1000);
		goto PSWD;
	}
	cmd_lcd(0x01);
	str_lcd("password matched");
	delay_ms(500);
	retry = 0;

FP:
	cmd_lcd(0x01);
	if(search(p_id)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("FINGER NOT FOUND");
		cmd_lcd(0xc0);
		str_lcd("try again");
		delay_ms(600);
		retry++;
		 if(retry==1)
		 {
			cmd_lcd(0x01);
			str_lcd("1 more chance");
			delay_ms(500);
			goto FP;
		 }
		 else if(retry==2)
	 	{	 
			cmd_lcd(0x01);
			str_lcd("last chance");
			delay_ms(500);
			goto FP;
		 }
	 	else
	 	{
		  cmd_lcd(0x01);
		   str_lcd("NOT MATCHED ");
		   cmd_lcd(0xc0);
		  str_lcd("CAN'T CHANGED ");
		  delay_ms(1000);
		  return;
	    }
	}
	else
	{
PSWD1:
		cmd_lcd(0x01);	
		str_lcd("ENTER NEW PASSWORD:");
		cmd_lcd(0xc0);
		password_kpm(pw1);
		cmd_lcd(0x01);
		str_lcd("ENTER AGAIN:");
		cmd_lcd(0xc0);
		password_kpm(pw2);
		if(strcmp((const char*)pw1,(const char*)pw2)!=0)
		{
			cmd_lcd(0x01);
			str_lcd("NOT MATCHED");
			cmd_lcd(0xc0);
			str_lcd("try again");
			delay_ms(500);
			goto PSWD1;	
		}
		else
		{
			i2c_eeprom_write_page(0x50,found+1,pw1,5);	

				cmd_lcd(0x01);
				str_lcd(" NEW PASSWORD ");
				cmd_lcd(0xc0);
				str_lcd("UPDATED ");
				delay_ms(500);
		}		
	}
}
//---------------------------------------------------------------------------
// Update Registered Fingerprint
// Authentication:
//      ID -> Password -> Enroll New Fingerprint
//---------------------------------------------------------------------------
void  finger_edit(void)
{// Verify user ID.
	// Read stored password from EEPROM.
	// Compare entered password with stored password.
	// Delete old fingerprint template from R305 sensor.
	// Enroll the new fingerprint for the same ID.
	// Display update successful message.
	s8 f_id,pw1[5],pw2[5],retry=0;
	u16 found;
                //ID
//==========================================//
	
	cmd_lcd(0x01);
	str_lcd("ENTER ID:");
	f_id = readnum();
	if((found=is_id_in_db(f_id))==0)
	{
		cmd_lcd(0x01);
		str_lcd("ID NOT FOUND");
		delay_ms(1000);
		return;
	}
	cmd_lcd(0x01);
	str_lcd("ID FOUND");
	delay_ms(900);
//		PASSWORD		                   //
//=========================================//
	i2c_eeprom_seq_read(0x50,found+1,pw2,5);
PSWD:	
	cmd_lcd(0x01);
	str_lcd("ENTER PASSWORD:");
	cmd_lcd(0xc0);
	password_kpm(pw1);
	if(strcmp((const char*)pw2,(const char *)pw1)!=0)
	{
		cmd_lcd(0x01);
                str_lcd("WRONG PASSWORD");
		delay_ms(1000);
                retry++;
                if(retry==3)
                {
                  cmd_lcd(0x01);
                  str_lcd("   NOT MATCHED ");
                  cmd_lcd(0xc0);
                  str_lcd("CAN'T CHANGED ");
                  delay_ms(1000);
                  return;
                }
                cmd_lcd(0xc0);
                str_lcd("1 more chances");
                delay_ms(1000);
                goto PSWD;		
	}
//      	 FINGER UPDATATION	             	 
//===============================================//
FP1:
		 delete_fp(f_id);           
		 if(enroll(f_id)!=0)
           {
		   	  cmd_lcd(0x01);
              str_lcd("FAIL FINGER");
              str_lcd("please try again");
		      delay_ms(500);
		      goto FP1;
		  }            
    	cmd_lcd(0x01);
		str_lcd(" SUCCESSFULLY ");	
		cmd_lcd(0xc0);	
		str_lcd("UPDATED ");
 		
}
//===================== FORGOT PASSWORD FUNCTION =====================//
// Allows a user to reset the password using the default password.
void forget_password(void)
{
	s8 fo_id,pw1[5],pw2[5],retry=0;   // fo_id = entered ID, pw1 & pw2 store passwords, retry counts invalid attempts.
	u16 found;                        // Stores EEPROM address if ID exists.

	//--------------- Read User ID ----------------//
	cmd_lcd(0x01);                    // Clear LCD display.
	str_lcd("ENTER THE ID:");         // Ask user to enter ID.

	fo_id = readnum();                // Read numeric ID from keypad.

	// Check whether entered ID exists in EEPROM database.
	if((found=is_id_in_db(fo_id))==0)
	{
		cmd_lcd(0x01);                // Clear LCD.
		str_lcd("ID IS NOT FOUND");   // Display error message.
		delay_ms(1000);               // Allow user to read message.
		return;                       // Exit function.
	}

	cmd_lcd(0x01);                    // Clear LCD.
	str_lcd("ID IS FOUND");           // Inform user that ID exists.
	delay_ms(1000);

	//--------------- Verify Default Password ----------------//
PSWD:
	cmd_lcd(0x01);                    // Clear LCD.
	str_lcd("ENTER THE DEFAULT ");    // Display first line.
	cmd_lcd(0xc0);                    // Move cursor to second line.
	str_lcd("PASSWORD:");
	password_kpm(pw1);                // Read default password.

	// Compare entered password with factory default password.
	if(strcmp((const char*)"0000",(const char *)pw1)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("WRONG PASSWORD");    // Wrong default password.
		delay_ms(1000);

		retry++;                      // Increment retry count.

		if(retry==2)                  // Allow only two attempts.
		{
			cmd_lcd(0x01);
			str_lcd(" NOT MATCHED ");
			cmd_lcd(0xc0);
			str_lcd(" CAN'T CHANGED ");
			delay_ms(1000);
			return;                  // Exit if retries exceeded.
		}

		cmd_lcd(0xc0);
		str_lcd("try one more");      // Ask user to retry.
		delay_ms(1000);
		goto PSWD;
	}

	//--------------- Enter New Password ----------------//
PSWD1:
	cmd_lcd(0x01);
	str_lcd("ENTER NEW PASSWORD:");
	cmd_lcd(0xc0);
	password_kpm(pw1);               // Read new password.

	cmd_lcd(0x01);
	str_lcd("ENTER AGAIN:");
	cmd_lcd(0xc0);
	password_kpm(pw2);               // Confirm password.

	// Verify both passwords match.
	if(strcmp((const char*)pw1,(const char*)pw2)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("NOT MATCHED");
		cmd_lcd(0xc0);
		str_lcd("try again");
		delay_ms(500);
		goto PSWD1;                  // Ask again if mismatch.
	}
	else
	{
		// Store new password into EEPROM.
		i2c_eeprom_write_page(0x50,found+1,pw1,5);

		cmd_lcd(0x01);
		str_lcd(" NEW PASSWORD ");
		cmd_lcd(0xc0);
		str_lcd("UPDATED ");
		delay_ms(500);
	}
}


//===================== EDIT MENU =====================//
// Allows user to edit password, fingerprint or recover password.
void edit(void)
{
	u8 choice;                      // Stores selected menu option.

	// Check whether any users are enrolled.
	if(ids==0)
	{
		if(flag==0)
		{
			flag =1;                // Prevent repeated LCD message.
			cmd_lcd(0x01);
			str_lcd("NO DATA FOUND");
			cmd_lcd(0xc0);
			str_lcd("PRESS SW TO ENROLL");
		}
		return;                    // Exit if database is empty.
	}

	flag =0;                      // Reset display flag.

	while(1)
	{
		cmd_lcd(0x01);            // Clear LCD.
		str_lcd("1.PASS 2.FINGER");
		cmd_lcd(0xc0);
		str_lcd("3.FORGOT 4.EXIT");

		choice = keyscan();        // Read keypad option.

		switch(choice)
		{
			case '1':
				password_edit();   // Change password.
				break;

			case '2':
				finger_edit();     // Update fingerprint.
				break;

			case '3':
				forget_password(); // Recover forgotten password.
				break;

			case '4':
				break;             // Exit menu.
		}

		if(choice=='4')
			break;
	}
}
//===================== DELETE USER =====================//
// Deletes fingerprint and password corresponding to an ID.
void delete_id(void)
{
	char d_id,retry=0;
	char null_buff[5]={'\0','\0','\0','\0','\0'}; // Empty password buffer.
	u16 found;                                    // EEPROM location.

ID:
	cmd_lcd(0x01);
	str_lcd("ENTER THE ID:");

	d_id = readnum();               // Read user ID.

	// Verify ID exists.
	if((found=is_id_in_db(d_id))==0)
	{
		cmd_lcd(0x01);
		str_lcd("ID NOT FOUND");
		delay_ms(1000);

		retry++;

		if(retry==2)
		{
			cmd_lcd(0xc0);
			str_lcd("CANOT BE DELETED");
			delay_ms(1000);
			retry=0;
			return;
		}
		else
		{
			cmd_lcd(0x01);
			str_lcd("TRY AGAIN ");
			cmd_lcd(0xc0);
			str_lcd("ONE LAST TIME ");
			delay_ms(1000);
			goto ID;
		}
	}

	// Delete fingerprint template from R305 sensor.
	if(delete_fp(d_id)!=0)
	{
		cmd_lcd(0x01);
		str_lcd("fails to delete");
		delay_s(1);
		return;
	}

	// Clear password from EEPROM.
	i2c_eeprom_write_page(0x50,found+1,null_buff,5);

	// Mark ID as deleted by writing 0xFF.
	i2c_eeprom_write_byte(0x50,found,0xff);

	cmd_lcd(0x01);
	str_lcd(" DELETED");
	cmd_lcd(0xc0);
	str_lcd("SUCESSFULLY ");
	delay_ms(1000);
}


//===================== ADMIN MENU =====================//
// Displays administrator menu after external interrupt.
void menu(void)
{
	u8 choice;                     // Stores selected menu option.

	menu_flag=0;                   // Clear menu request flag.
	disable();                     // Disable external interrupt while inside menu.
	flag =0;                       // Reset LCD display flag.

	while(1)
	{
		cmd_lcd(CLEAR_LCD);        // Clear LCD.
		cmd_lcd(GOTO_LINE1_POS_0); // Move cursor to line 1.
		str_lcd("1.enroll 2.edit");

		cmd_lcd(GOTO_LINE2_POS_0); // Move cursor to line 2.
		str_lcd("3.delete 4.exit");

		choice = keyscan();        // Read keypad selection.

		switch(choice)
		{
			case '1':
				enroll_id();      // Register new user.
				break;

			case '2':
				edit();           // Edit user information.
				break;

			case '3':
				delete_id();      // Delete user.
				break;

			case '4':
				break;            // Exit menu.
		}

		if(choice=='4')
		{
			cmd_lcd(0x01);       // Clear LCD before exit.
			menu_flag=0;         // Reset menu flag.
			enable();            // Re-enable external interrupt.
			char_lcd(ids+48);    // Display current number of users (ASCII conversion).
			break;
		}
	}
}
