#include "types.h"          // Contains user-defined data types like u32
//*****************************************************************
// Function Name : delay_us()
// Description   : Generates an approximate delay in microseconds.
// Parameter     : us - Delay time in microseconds.
// Return        : None.
// Note          : This is a software delay loop. The actual delay
//                 depends on the processor clock frequency.
//*****************************************************************
void delay_us(u32 us)
{
    // Multiply by 12 because approximately 12 loop iterations
    // produce a delay of 1 microsecond for the selected CPU clock.
    for(us *= 12; us > 0; us--);
}
//*****************************************************************
// Function Name : delay_ms()
// Description   : Generates an approximate delay in milliseconds.
// Parameter     : ms - Delay time in milliseconds.
// Return        : None.
// Note          : 1 ms = 1000 us.
//                 The multiplication factor is chosen based on the processor clock frequency.
//*****************************************************************
void delay_ms(u32 ms)
{
    // Convert milliseconds into loop count.
    // Approximately 12000 iterations produce 1 millisecond delay.
    for(ms *= 12000; ms > 0; ms--);
}
//*****************************************************************
// Function Name : delay_s()
// Description   : Generates an approximate delay in seconds.
// Parameter     : s - Delay time in seconds.
// Return        : None.
// Note          : 1 second = 1000 milliseconds.
//                 This routine should be used only for small delays, since software delays keep the CPU busy.
//*****************************************************************
void delay_s(u32 s)
{
    // Convert seconds into loop count.
    // Approximately 12,000,000 iterations produce 1 second delay.
    for(s *= 12000000; s > 0; s--);
}
