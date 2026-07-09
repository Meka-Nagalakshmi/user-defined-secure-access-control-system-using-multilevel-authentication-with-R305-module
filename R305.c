#include "lcd.h"      // LCD display functions
#include "uart.h"     // UART communication functions
#include "delay.h"    // Delay functions

// Global transmit buffer, receive buffer and status variables
u8 buf[20], rec_buf[25], j=0, i=0, frame_ready=0;

//-----------------------------------------------------------
// Send command packet to fingerprint sensor through UART
//-----------------------------------------------------------
void send(u8 *ptr,u8 n)
{
    // Send 'n' bytes one by one
    for(j=0;j<n;j++)
        u0_Tx_byte(ptr[j]);
}
// Capture fingerprint image from sensor
s8 collect(void)
{
    u16 timeout = 0;
    i = 0;                     // Reset received byte counter
    // Clear receive buffer
    for(j=0;j<25;j++)
        rec_buf[j]=0;
    // Clear transmit buffer
    for(j=0;j<12;j++)
        buf[j]=0;
    // Build R305 command packet
    buf[0]=0xEF;               // Packet header byte 1
    buf[1]=0x01;               // Packet header byte 2
    buf[2]=0xFF;               // Default module address
    buf[3]=0xFF;
    buf[4]=0xFF;
    buf[5]=0xFF;
    buf[6]=0x01;               // Command packet identifier
    buf[7]=0x00;               // Length MSB
    buf[8]=0x03;               // Length LSB
    buf[9]=0x01;               // GenImg command
    buf[10]=0x00;              // Checksum MSB
    buf[11]=0x05;              // Checksum LSB
    send(buf,12);              // Send packet to sensor
    delay_ms(10);              // Allow sensor processing time
    // Wait for sensor response
    while(i==0)
    {
        delay_ms(1);
			  timeout++;
        if(timeout>1000) return 1; // Timeout
    }
    // Confirmation code
    if(rec_buf[9]==0x00)  return 0;   // Success
    else  return 1;              // Failure
}
s8 store1(void)
{  // Convert captured image into Character File and store in Buffer1
	u16 timeout = 0;
	i=0;
	for(j=0;j<25;j++)
		rec_buf[j]=0;
	for(j=0;j<13;j++)
	buf[j]=0;
	buf[0]= 0xEF;
	buf[1]= 0x01;  //header
	buf[2]= 0xFF;
	buf[3]= 0xFF;
	buf[4]= 0xFF;
	buf[5]= 0xFF;  //adress
	buf[6]= 0x01;  //cmd select
	buf[7]=	0x00;
	buf[8]=	0x04;  //length
	buf[9]=	0x02; //cmd for capature
	buf[10]=0x01; //store in buffer1
	buf[11]=0x00;
	buf[12]=0x08; //check sum
   i=0;
	send(buf,13);
	while(i == 0)
    {
        delay_ms(1);
        timeout++;
        if(timeout > 1000) return 1;
    }

	if(rec_buf[9]==0x00)
		return 0;
	else
		return 1;

}
s8 store2(void)
{// Convert second captured image into Character File
	u16 timeout = 0;
	i=0;
	for(j=0;j<25;j++)
		rec_buf[j]=0;
	for(j=0;j<13;j++)
	buf[j]=0;
	buf[0]= 0xEF;
	buf[1]= 0x01;  //header
	buf[2]= 0xFF;
	buf[3]= 0xFF;
	buf[4]= 0xFF;
	buf[5]= 0xFF;  //adress
	buf[6]= 0x01;  //cmd select
	buf[7]=	0x00;
	buf[8]=	0x04;  //length
	buf[9]=	0x02; //cmd for img
	buf[10]=0x02; //store in buffer2
	buf[11]=0x00;
	buf[12]=0x09; //check sum
	send(buf,13);
	while(i==0)
	{
        delay_ms(1);
        timeout++;
        if(timeout > 1000) 
			return 1;
  }
	if(rec_buf[9]==0x00)
		return 0;
	else
		return 1;

}
s8 gen_template(void)
{// Generate fingerprint template using Buffer1 and Buffer2
	u16 timeout = 0; 
	i=0;
	for(j=0;j<25;j++)
		rec_buf[j]=0;
	for(j=0;j<12;j++)
	    buf[j]=0;
	   buf[0]= 0xEF;
        buf[1]= 0x01;  //header
        buf[2]= 0xFF;
        buf[3]= 0xFF;
        buf[4]= 0xFF;
        buf[5]= 0xFF;  //adress
        buf[6]= 0x01;  //cmd select
        buf[7]= 0x00;
        buf[8]= 0x03;  //length
        buf[9]= 0x05; //cmd for regmodel
        buf[10]=0x00;
        buf[11]=0x09; //check sum
	send(buf,12);
	while(i==0)
	{
        delay_ms(1);
        timeout++;
        if(timeout > 1000) return 1;
    }
	if(rec_buf[9]==0x00)
		return 0;
	else
		return 1;
}
s8 store_temp(s8 id)
{// Store generated template into flash memory
	u16 sum,timeout = 0;
	i=0;
	for(j=0;j<25;j++)
		rec_buf[j]=0;
	for(j=0;j<15;j++)
		buf[j]=0;
		buf[0]= 0xEF;
    buf[1]= 0x01;  //header
    buf[2]= 0xFF;
    buf[3]= 0xFF;
    buf[4]= 0xFF;
    buf[5]= 0xFF;  //adress
    buf[6]= 0x01;  //cmd select
    buf[7]= 0x00;
    buf[8]= 0x06;  //length
    buf[9]= 0x06; //cmd for storing
    buf[10]=0x01;//store template from buffer1
    buf[11]=0x00;
    buf[12]=id;//template id
		sum = 0x01 + 0x00 + 0x06 + 0x06 + 0x01 + 0x00 + id;
    buf[13]= sum>>8;
    buf[14]= sum&0xff; //check sum
	
	send(buf,15);
	while(i==0)
	{
        delay_ms(1);
        timeout++;
        if(timeout > 1000) return 1;
    }

	if(rec_buf[9]==0x00)
		return 0;
	else
		return 1;

}
s8 load_temp(s8 id)
{// Load stored fingerprint template into Character Buffer2
	u16 sum,timeout = 0;
    i = 0;
    for(j=0;j<25;j++)
        rec_buf[j]=0;

    for(j=0;j<15;j++)
        buf[j]=0;
	   buf[0]=0xEF;
     buf[1]=0x01;
    buf[2]=0xFF;
    buf[3]=0xFF;
    buf[4]=0xFF;
    buf[5]=0xFF;
    buf[6]=0x01;
    buf[7]=0x00;
    buf[8]=0x06;
    buf[9]=0x07;   // LoadChar comand
    buf[10]=0x02;  // Buffer2
    buf[11]=0x00;
    buf[12]=id;    //finger id to loaad
    sum = 0x01 + 0x00 + 0x06 + 0x07 + 0x02 + 0x00 + id;
    buf[13]=(sum >> 8);
    buf[14]=(sum & 0xFF);	
	send(buf,15);
    while(i== 0)
    {
        delay_ms(1);
        timeout++;
        if(timeout > 1000) return 1;
    }
    return (rec_buf[9] == 0x00) ? 0 : 1;
}

s8 identify_fp(void)
{// Compare Buffer1 and Buffer2 templates
    u16 timeout = 0;
    i = 0;
    for(j=0;j<25;j++)
        rec_buf[j]=0;
    for(j=0;j<12;j++)
        buf[j]=0;
    buf[0]=0xEF;
    buf[1]=0x01;
    buf[2]=0xFF;
    buf[3]=0xFF;
    buf[4]=0xFF;
    buf[5]=0xFF;
    buf[6]=0x01;
    buf[7]=0x00;
    buf[8]=0x03;
    buf[9]=0x03;   // Match command
    buf[10]=0x00;
    buf[11]=0x07;
    send(buf,12);
    while(i== 0)
    {
        delay_ms(1);
        timeout++;
        if(timeout > 100) return 1;
    }
    return (rec_buf[9] == 0x00) ? 0 : 1;
}
s8 delete_fp(s8 id)
{// Delete one fingerprint template from sensor memory
  	u16 sum,timeout = 0;
    i = 0;
    for(j=0;j<25;j++)
        rec_buf[j]=0;
    for(j=0;j<15;j++)
        buf[j]=0;		
	  buf[0]=0xEF;
    buf[1]=0x01;
    buf[2]=0xFF;
    buf[3]=0xFF;
    buf[4]=0xFF;
    buf[5]=0xFF;
    buf[6]=0x01;
    buf[7]=0x00;
    buf[8]=0x07;
    buf[9]=0x0C;  // DeleteChar command
    buf[10]=0x00;  
    buf[11]=id;  // Fingerprint ID to delete
    buf[12]=0x00;
	  buf[13]= 0x01;	 //delete 1 templates
    sum = 0x01 + 0x00 + 0x07 + 0x0c + 0x00 + id + 0x00 +0x01;
    buf[14]=(sum >> 8)&0xff;
    buf[15]=(sum & 0xFF);	
 	send(buf,16);
    while(i== 0)
    {
        delay_ms(1);
        timeout++;
        if(timeout > 1000) return 1;
    }
    return (rec_buf[9] == 0x00) ? 0 : 1;
}
//------------------------------------------------------------------------------
// Enroll a new fingerprint into the sensor memory at the given ID
// Returns:
//      0 -> Enrollment successful
//      1 -> Enrollment failed
//------------------------------------------------------------------------------
s8 enroll(s8 id)
{
	s8 ret,retry=0;              // ret stores function status, retry counts finger detection attempts

    cmd_lcd(0x01);              // Clear LCD display
    str_lcd("PLACE FINGER ");   // Ask user to place finger

    // Wait until a finger is detected
	while(collect() != 0)
	{
   	 delay_ms(50);             // Small delay to avoid continuous polling
   	 retry++;                  // Increment retry counter

    	if(retry > 100)          // Timeout if finger is not detected
    	{
    	    cmd_lcd(0x01);       // Clear LCD
			str_lcd("Timeout");  // Display timeout message
		   	delay_ms(500);      // Show message for 500ms
        	return 1;           // Enrollment failed
    	}
	}

	//---------------- FIRST FINGER CAPTURE ----------------//

    ret = collect();            // Capture fingerprint image
    if(ret != 0)
    {
        cmd_lcd(0x01);
		str_lcd("Failed to collect");   // Finger image capture failed
		delay_ms(500);
        return 1;
    }

	ret = store1();              // Convert image and store into Character Buffer-1
    if(ret != 0)
    {
        cmd_lcd(0x01);
		str_lcd("Failed-conv-buf");     // Conversion failed
		delay_ms(500);
        return 1;
    }

	cmd_lcd(0x01);
    str_lcd("Remove Finger");   // Ask user to remove finger

    // Wait until finger is removed
    while(collect() == 0)
    {
        delay_ms(50);
    }

	delay_ms(1000);              // Wait for stable finger removal

	cmd_lcd(0x01);
    str_lcd("PLACE AGAIN ");    // Ask user to place the same finger again

    // Wait for second finger placement
    while(collect() != 0)
    {
        delay_ms(50);
    }

	//---------------- SECOND FINGER CAPTURE ----------------//

    ret = collect();            // Capture second fingerprint image
    if(ret != 0)
    {
        cmd_lcd(0x01);
		str_lcd("Fail-conv-buffTemp");  // Capture failed
		delay_ms(500);
        return 1;
    }

	ret = store2();              // Convert image and store into Character Buffer-2
    if(ret != 0)
    {
        cmd_lcd(0x01);
		str_lcd("Fail-con-buff2");      // Conversion failed
		delay_ms(500);
        return 1;
    }

	//---------------- TEMPLATE GENERATION ----------------//

    ret = gen_template();       // Generate template by comparing Buffer1 & Buffer2
    if(ret != 0)
    {
		cmd_lcd(0x01);
		str_lcd("Fail-generate");      // Template generation failed
		delay_ms(500);
        return 1;
    }

	//---------------- STORE TEMPLATE ----------------//

    ret = store_temp(id);       // Store generated template into flash memory with given ID
    if(ret != 0)
    {
        cmd_lcd(0x01);
		str_lcd("Fail-2-store");       // Storage failed
		delay_ms(500);
        return 1;
    }

   	cmd_lcd(0x01);              // Clear LCD
   	str_lcd("ENROLL DONE ");    // Display success message

	delay_ms(500);

    return 0;                   // Enrollment successful
}


//------------------------------------------------------------------------------
// Verify whether the placed fingerprint matches the fingerprint stored
// at the specified ID.
// Returns:
//      0 -> Fingerprint matched
//      1 -> Fingerprint not matched / Error
//------------------------------------------------------------------------------
s8 search(s8 id)
{
	s8 ret;                      // Variable to store function return status

    cmd_lcd(0x01);              // Clear LCD
    str_lcd("PLACE FINGER ");   // Ask user to place finger

	// Wait until a finger is detected
    while(collect() != 0)
        delay_ms(50);

	//---------------- CAPTURE FINGER ----------------//

    ret = collect();            // Capture fingerprint image
    if(ret != 0)
    {
        cmd_lcd(0x01);
		str_lcd("Gen Fail");      // Finger image capture failed
		delay_ms(500);
        return 1;
    }

	//---------------- STORE IN BUFFER-1 ----------------//

    ret = store1();             // Convert image and store in Character Buffer-1
    if(ret != 0)
    {
        cmd_lcd(0x01);
		str_lcd("Fail-con-temp"); // Conversion failed
		delay_ms(500);
        return 1;
    }

	//---------------- LOAD STORED TEMPLATE ----------------//

    ret = load_temp(id);        // Load stored fingerprint template into Buffer-2
    if(ret != 0)
    {
        cmd_lcd(0x01);
		str_lcd("Load Fail");     // Failed to load template
		delay_ms(500);
        return 1;
    }

	//---------------- COMPARE BOTH BUFFERS ----------------//

    ret = identify_fp();        // Compare Buffer-1 with Buffer-2

	if(ret == 0)
	{
		return 0;               // Fingerprint matched successfully
	}
	else
	{
		return 1;               // Fingerprint did not match
	}
}
