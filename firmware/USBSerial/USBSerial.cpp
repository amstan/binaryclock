/*
 (Ported from Arduino to Energia)(then modified to work as a standalone library)

USBSerial.cpp (formerly NewSoftSerial.cpp) - 
 Multi-instance USB serial library for Arduino/Wiring
  
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "USBSerial.h"

#include "descriptors.h"

#include "device.h"
#include "types.h"               //Basic Type declarations
#include "usb.h"                 //USB-specific functions

#include "HAL_UCS.h"

#include "UsbCdc.h"
#include "usbConstructs.h"

volatile BYTE bCDCDataReceived_event = FALSE;   //Flag set by event handler to indicate data has been received into USB buffer

#define BUFFER_SIZE 256
char nl[2] = "\n";
char dataBuffer[BUFFER_SIZE] = "";
WORD count;                    


//
// Public methods
//

void USBSerial_open()
{
	//Initialization of clock module
	if (USB_PLL_XT == 2){
		//P5SEL |= 0x0C;                                      //enable XT2 pins for F5529
	
		XT2_Start(XT2DRIVE_0);                                          //Start the "USB crystal"
	} 
	else {
		//P5SEL |= 0x10;                                      //enable XT1 pins
	
		XT1_Start(XT1DRIVE_0);                                          //Start the "USB crystal"
	}
    __disable_interrupt();                           //Enable interrupts globally
    
    USB_init();                 //Init USB

    //Enable various USB event handling routines
    USB_setEnabledEvents(kUSB_allUsbEvents);


    // See if we're already attached physically to USB, and if so, connect to it
    // Normally applications don't invoke the event handlers, but this is an exception.
    if (USB_connectionInfo() & kUSB_vbusPresent)
      USB_handleVbusOnEvent();
    __enable_interrupt();                           //Enable interrupts globally
}

void USBSerial_close()
{
  USB_disable();
}


// Read data from buffer
int USBSerial_read()
{
  WORD count; 
  BYTE dataBuffer = 0;  

  // Read from "head"
  count = cdcReceiveDataInBuffer((BYTE*)&dataBuffer,
    1,
    CDC0_INTFNUM);                                //Count has the number of bytes received into
                                                  //dataBuffer
  // Empty buffer?
  if (count == 0){
    bCDCDataReceived_event = FALSE;
    return -1;
	}
  
  return (char)dataBuffer;
}

int USBSerial_available()
{
  if ( (USB_connectionState() == ST_ENUM_ACTIVE) && (USBCDC_bytesInUSBBuffer(CDC0_INTFNUM) > 0) ) return 1;
  return 0;
}

uint16_t USBSerial_write(char b)
{
  if (cdcSendDataInBackground((BYTE*)&b,1,CDC0_INTFNUM,0)){  	//send char to the Host
    return 0;   // could not write
  }
  return 1;
}


uint16_t USBSerial_write(const char *buffer, uint16_t size)
{

  if (cdcSendDataInBackground((BYTE*)buffer,size,CDC0_INTFNUM,0)){  	//send char to the Host
    return 0;   // could not write
  }
  return 1;
}

void USBSerial_flush()
{
  while (USBCDC_bytesInUSBBuffer(CDC0_INTFNUM) > 0);            // wait till all send
}

int USBSerial_peek()
{

  // Empty buffer?
  if (USBCDC_bytesInUSBBuffer(CDC0_INTFNUM) == 0)
    return -1;

  // Read from "head"
  return USBCDC_bytesInUSBBuffer(CDC0_INTFNUM);
}

/*  
 * ======== UNMI_ISR ========
 */
#ifndef __GNUC__
#pragma vector = UNMI_VECTOR
__interrupt
#else
__attribute__((interrupt(UNMI_VECTOR)))
#endif
//UNMI interrupt service routine
static void UNMI_ISR(void)
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG ))
    {
        case SYSUNIV_NONE:
            __no_operation();
            break;
        case SYSUNIV_NMIIFG:
            __no_operation();
            break;
        case SYSUNIV_OFIFG:
            UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG); //Clear OSC flaut Flags fault flags
            SFRIFG1 &= ~OFIFG;                          //Clear OFIFG fault flag
            break;
        case SYSUNIV_ACCVIFG:
            __no_operation();
            break;
        case SYSUNIV_BUSIFG:
            SYSBERRIV = 0;                                      //clear bus error flag
            USB_disable();                                      //Disable
    }
}
