// USBFunctions.c

// includes ///////////////////////////////////////////////////////////////////////////////////////
#include<p18f4550.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include"USBFunctions.h"

// global variables ///////////////////////////////////////////////////////////////////////////////
#pragma udata USB_BDT = 0x400			// see linker script, usb4: 0x400 - 0x4FF (256 byte)
volatile BDT_ENTRY g_buffDescTable[4];								// 4 bytes each, 16 bytes total
volatile CTRL_TRF_SETUP g_ctrlTrfSetupPkt;							// 				 8 bytes
volatile BYTE g_ctrlTrfData[USB_EP0_BUFF_SIZE];						// 1 byte each,    8 bytes total
#pragma udata					// end of #pragma udata USB_BDT = 0x400		      32 bytes total

#pragma udata USB_VARS
BYTE g_fromHostToDeviceBuffer[65];
BYTE g_fromDeviceToHostBuffer[65];
#pragma udata					// end of #pragma udata USB_VARS

OUT_PIPE g_outPipe;
IN_PIPE  g_inPipe;

BYTE g_USBDeviceState = DETACHED_STATE;
BYTE g_USBDeviceState;
BYTE g_controlTransferState;
BYTE g_USBActiveConfiguration;
BYTE g_shortPacketStatus;

#pragma romdata										// USB device descriptors
	// device descriptor
rom USB_DEVICE_DESCRIPTOR g_userDeviceDescriptor = {	0x12,
														USB_DESCRIPTOR_DEVICE,
														0x0200,
														0x00,
														0x00,
														0x00,
														USB_EP0_BUFF_SIZE,
														0x04D8,					// Vendor ID (VID)
														0x003F,					// Product ID (PID)
														0x0002,
														0x01,
														0x02,
														0x00,
														0x01	};
	// config 1 descriptor
rom BYTE g_configDescriptor1[] = {	0x09,					// config descriptor
									USB_DESCRIPTOR_CONFIGURATION,
									0x29,
									0x00,
									1,
									1,
									0,
									_DEFAULT | _SELF,
									50,
									
									0x09,					// interface descriptor
									USB_DESCRIPTOR_INTERFACE,
									0,
									0,
									2,
									HID_INTF,
									0,
									0,
									0,
									
									0x09,					// HID descriptor
									HID_DESCRIPTOR,
									0x11,
									0x01,
									0x00,
									HID_NUM_OF_DSC,
									REPORT_DESCRIPTOR,
									HID_RPT01_SIZE, 0x00,
									
									0x07,					// endpoint descriptor
									USB_DESCRIPTOR_ENDPOINT,
									1 | _EP_IN,
									_INT,
									0x40,
									0x00,
									0x01,
									
									0x07,					// endpoint descriptor
									USB_DESCRIPTOR_ENDPOINT,
									1 | _EP_OUT,
									_INT,
									0x40,
									0x00,
									0x01	};

	// language code string descriptor
rom struct{BYTE bLength; BYTE bDscType; WORD string[1];} g_stringDescript000 = { sizeof(g_stringDescript000), USB_DESCRIPTOR_STRING, { 0x0409 } };

	// mfg. string descriptor
rom struct{BYTE bLength; BYTE bDscType; WORD string[25];} g_stringDescript001 = {	sizeof(g_stringDescript001),
																				USB_DESCRIPTOR_STRING,
																				{'M','i','c','r','o','c','h','i','p',' ','T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'} };

	// product string descriptor
rom struct{BYTE bLength; BYTE bDscType; WORD string[22];} g_stringDescript002 = {	sizeof(g_stringDescript002),
																				USB_DESCRIPTOR_STRING,
																				{'S','i','m','p','l','e',' ','H','I','D',' ','D','e','v','i','c','e',' ','D','e','m','o'} };

rom struct{BYTE report[HID_RPT01_SIZE];} g_HIDReport01 = { {	0x06, 0x00, 0xFF,
															0x09, 0x01,
															0xA1, 0x01,
															0x19, 0x01,
															0x29, 0x40,
															0x15, 0x00,
															0x26, 0xFF, 0x00,
															0x75, 0x08,
															0x95, 0x40,
															0x81, 0x02,
															0x19, 0x01,
															0x29, 0x40,
															0x91, 0x02,
															0xC0 } };

	// array of config descriptors
rom BYTE *rom g_userConfigDescriptorPtr[] = { (rom BYTE *rom)&g_configDescriptor1 };

	// array of string descriptors
rom BYTE *rom g_USBStringDescPtr[] = {	(rom BYTE *rom)&g_stringDescript000,
										(rom BYTE *rom)&g_stringDescript001,
										(rom BYTE *rom)&g_stringDescript002		};
#pragma udata 			// end of #pragma romdata for descriptor global variables

#pragma code
///////////////////////////////////////////////////////////////////////////////////////////////////
void USBInit(void) {
	BYTE i;
	
	UEIR = 0x00;		// clear USB error interrupt status register
	UIR = 0x00;			// clear USB interrupt status register
	
						// UEIE -> USB error interrupt enable register
	UEIEbits.BTSEE   = 1;	// bit stuff error interrupt enabled
	UEIEbits.BTOEE   = 1;	// bus turnaround time-out error interrupt enabled
	UEIEbits.DFN8EE  = 1;	// data field size error interrupt enabled
	UEIEbits.CRC16EE = 1;	// CRC16 failure interrupt enabled
	UEIEbits.CRC5EE  = 1;	// CRC5 host error interrupt enabled
	UEIEbits.PIDEE   = 1;	// PID check failure interrupt enabled
	
							// UIE -> USB interrupt enable register
	UIEbits.SOFIE   = 1;	// start of frame token interrupt enabled
	UIEbits.STALLIE = 1;	// STALL interrupt enabled
	UIEbits.IDLEIE  = 1;	// idle detect interrupt enabled
	UIEbits.TRNIE   = 1;	// transaction interrupt enabled
	UIEbits.ACTVIE  = 0;	// bus activity detect interrupt disabled
	UIEbits.UERRIE  = 1;	// USB error interrupt enabled
	UIEbits.URSTIE  = 1;	// USB reset interrupt enabled
	
	UCONbits.PPBRST = 1;	// reset ping-pong buffer pointers to even buffer descriptor banks
	UCONbits.PPBRST = 0;	// ping-pong buffer pointers not being reset
	
	UADDR = 0x00;			// set USB address register
	
	for(i=0; i<(sizeof(g_buffDescTable) / sizeof(BDT_ENTRY)); i++) {		// clear all buffer descriptor table entries
		g_buffDescTable[i].STAT.STATVal = 0x00;
		g_buffDescTable[i].CNT = 0x00;
		g_buffDescTable[i].ADR = 0x0000;
	}
	
								// USB endpoint 0 configuration
	UEP0bits.EPHSHK   = 1;		// endpoint 0 handshake enabled
	UEP0bits.EPCONDIS = 0;		// enable endpoint 0 for control (SETUP) transfers, IN & OUT transfers also allowed
	UEP0bits.EPOUTEN  = 1;		// endpoint 0 output enabled
	UEP0bits.EPINEN   = 1;		// endpoint 0 input enabled
	UEP0bits.EPSTALL  = 0;		// endpoint 0 has not issued any STALL packets
	
												// flush any pending transactions
	while(UIRbits.TRNIF == 1) {					// while processing of pending transaction is complete . . .
		UIRbits.TRNIF = 0;						// set processing of pending transaction to not complete
	}
	
	g_inPipe.busy = 0;			// clear all internal pipe information
	g_outPipe.busy = 0;
	g_outPipe.wCount = 0;
	
	UCONbits.PKTDIS = 0;					// SIE token & packet processing enabled
	
	g_USBActiveConfiguration = 0;				// clear active configuration
	g_USBDeviceState = DETACHED_STATE;			// init state to detached
	
	for(i=0;i<65;i++) {								// init buffer arrays to zero
		g_fromHostToDeviceBuffer[i] = 0x00;
		g_fromDeviceToHostBuffer[i] = 0x00;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBTasks(void) {
	BYTE i;
	
	if(g_USBDeviceState == DETACHED_STATE) {	// if we are in detached state
		UCON = 0x00;							// disable & detach from bus
		UIE  = 0x00;							// disable all UIE interrupts
		while(UCONbits.USBEN == 0) {
			UCONbits.USBEN = 1;					// enable & attach to bus
		}
		g_USBDeviceState = ATTACHED_STATE;		// update state variable
		
								// USB config register
		UCFGbits.UTEYE  = 0;	// eye pattern test disabled
		UCFGbits.UOEMON = 0;	// USB output enable monitor enable bit
		UCFGbits.UPUEN  = 1;	// on chip pull-up resistors enabled
		UCFGbits.UTRDIS = 0;	// on chip transceiver active
		UCFGbits.FSEN   = 1;	// full speed clock (48 MHz)
		UCFGbits.PPB1   = 0;	// no ping pong
		UCFGbits.PPB0   = 0;	// no ping pong
	}
	
	if(g_USBDeviceState == ATTACHED_STATE) {
						/*  After enabling the USB module, it takes some time for the voltage on the D+ or D- lines to rist high enough
							to get out of the single-ended zero condition.  The USB reset interrupt should not be unmasked until the
							SE0 condition is cleared, this helps prevent the firmware from misinterpreting a single-ended zero condition
							as a USB bus reset from the USB host */
		if(UCONbits.SE0 == 0) {					// if not a single-ended zero condition . . .
			UIR = 0x00;							// clear interrupt register
			UIE = 0x00;							// disable all UIE interrupts
			UIEbits.URSTIE = 1;					// USB reset interrupt enabled
			UIEbits.IDLEIE = 1;					// idle detect interrupt enabled
			g_USBDeviceState = POWERED_STATE;	// update state
		}
	}
	
	if(UIRbits.ACTVIF &&		// if D+ / D- activity detected
	   UIEbits.ACTVIE) {		// and bus activity detect interrupt enabled . . .
		USBWakeFromSuspend();
	}
	
	if(UCONbits.SUSPND == 1) {
		return;
	}
	
	if(UIRbits.URSTIF &&						// if valid USB reset occurred
	   UIEbits.URSTIE) {						// USB reset interrupt enabled
		USBInit();								// call init again
		g_USBDeviceState = DEFAULT_STATE;		// set state variable
		g_buffDescTable[0].ADR = (BYTE*)&g_ctrlTrfSetupPkt;
		g_buffDescTable[0].CNT = USB_EP0_BUFF_SIZE;
		g_buffDescTable[0].STAT.DTS    = 0;		// data toggle synch bit
		g_buffDescTable[0].STAT.KEN    = 0;		// keep enable bit
		g_buffDescTable[0].STAT.INCDIS = 0;		// address increment disable bit
		g_buffDescTable[0].STAT.DTSEN  = 1;		// data toggle synch enable bit
		g_buffDescTable[0].STAT.BSTALL = 1;		// buffer stall enable bit
		g_buffDescTable[0].STAT.BC9    = 0;		// byte count 9 bit
		g_buffDescTable[0].STAT.BC8    = 0;		// byte count 8 bit
		g_buffDescTable[0].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)		
	}
	
	if(UIRbits.IDLEIF &&			// if idle detected
	   UIEbits.IDLEIE) {			// and idle detect interrupt enabled . . .
		USBSuspend();
	}
	
	if(UIRbits.SOFIF &&				// if start of frame token received by SIE
		UIEbits.SOFIE) {				// and start of frame token interrupt enabled . . .
		UIRbits.SOFIF = 0;			// clear start of frame token bit
	}
	
	if(UIRbits.STALLIF &&			// if stall handshake sent by SIE
		UIEbits.STALLIE) {			// and stall interrupt enabled . . .
		USBStallHandler();
	}
	
	if(UIRbits.UERRIF &&			// if unmasked error has occurred
		UIEbits.UERRIE) {			// and error interrupt enabled . . .
		UEIR = 0x00;				// clear USB error interrupt register
	}
	
	if(g_USBDeviceState < DEFAULT_STATE) return;		// bail if we have not at least reached default state
	
	if(UIEbits.TRNIE) {				// if transaction interrupts enabled
		for(i=0;i<4;i++) {				// for each entry in the USTAT FIFO . . .
			if(UIRbits.TRNIF) {			// if processing of pending transaction is complete . . .
				if(USTATbits.ENDP3 == 0 &&
				   USTATbits.ENDP2 == 0 &&
				   USTATbits.ENDP1 == 0 &&
				   USTATbits.ENDP0 == 0) {			// if last transfer was EP0 . . .
					USBEP0ControlTransfer();		// this leads to g_USBDeviceState = CONFIGURED_STATE
				}	
				UIRbits.TRNIF = 0;					// clear transaction complete interrupt bit, causes USTAT FIFO to advance
			} else {					// else jump out of for loop
				break;					// this kicks out of the for loop
			}
		}
		// break keyword takes us to here
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBWakeFromSuspend(void) {
	UCONbits.SUSPND = 0;			// clear suspend bit (wake from suspend)
	UIEbits.ACTVIE = 0;				// disable bus activity detect interrupt
	while(UIRbits.ACTVIF) {			// while activity on D+/D- lines was detected . . .
		UIRbits.ACTVIF = 0;			// clear bus activity detect interrupt flag bit
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBSuspend(void) {
	UIEbits.ACTVIE = 1;			// set bus activity detect interrupt enable bit
	UIRbits.IDLEIF = 0;			// clear idle detect interrupt bit
	UCONbits.SUSPND = 1;		// set USB suspend bit (enter suspend mode)
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBStallHandler(void) {
	if(UEP0bits.EPSTALL == 1) {				// if endpoint 0 has issued one or more stall packets . . .
		if((g_buffDescTable[0].STAT.UOWN   == 1) && (g_buffDescTable[1].STAT.UOWN   == 1)) {
			g_buffDescTable[0].STAT.DTS    = 0;		// data toggle synch bit
			g_buffDescTable[0].STAT.KEN    = 0;		// keep enable bit
			g_buffDescTable[0].STAT.INCDIS = 0;		// address increment disable bit
			g_buffDescTable[0].STAT.DTSEN  = 1;		// data toggle synch enable bit
			g_buffDescTable[0].STAT.BSTALL = 1;		// buffer stall enable bit
			g_buffDescTable[0].STAT.BC9    = 0;		// byte count 9 bit
			g_buffDescTable[0].STAT.BC8    = 0;		// byte count 8 bit
			g_buffDescTable[0].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
		}
		UEP0bits.EPSTALL = 0;				// clear endpoint 0 stall indicator bit
	}
	UIRbits.STALLIF = 0;					// clear stall handshake interrupt bit
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBEP0ControlTransfer(void) {
	if(USTATbits.DIR == 0) {				// if last trans was EP0 and OUT or SETUP
		if(g_buffDescTable[0].STAT.PID == SETUP_TOKEN) {
			USBSetupControlTransfer();		// this leads to g_USBDeviceState = CONFIGURED_STATE
		} else {
			USBOutControlTransfer();
		}
	} else if(USTATbits.DIR == 1) {		// if last trans was EP0 and IN
		USBInControlTransfer();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBSetupControlTransfer(void) {
	g_buffDescTable[1].STAT.STATVal = 0x00;
	g_shortPacketStatus = SHORT_PKT_NOT_USED;
	g_controlTransferState = CTRL_TRF_WAIT_SETUP;
	g_inPipe.wCount = 0;
	g_inPipe.busy = 0;
	USBCheckStandardRequest();				// this leads to g_USBDeviceState = CONFIGURED_STATE
	USBCheckHIDRequest();
	USBFinishControlTransferStuff();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBCheckStandardRequest(void) {
	if(g_ctrlTrfSetupPkt.RequestType != STANDARD) return;
	
	if(g_ctrlTrfSetupPkt.bRequest == SET_ADDRESS) {
		g_inPipe.busy = 1;
		g_USBDeviceState = ADR_PENDING_STATE;
	} else if(g_ctrlTrfSetupPkt.bRequest == GET_DESCRIPTOR) {
		if(g_ctrlTrfSetupPkt.bmRequestType == 0x80) {
			g_inPipe.busy = 1;
			if(g_ctrlTrfSetupPkt.bDescriptorType == USB_DESCRIPTOR_DEVICE) {
				g_inPipe.bRom = (rom BYTE*)&g_userDeviceDescriptor;
				g_inPipe.wCount = sizeof(g_userDeviceDescriptor);
			} else if(g_ctrlTrfSetupPkt.bDescriptorType == USB_DESCRIPTOR_CONFIGURATION) {
				g_inPipe.bRom = *(g_userConfigDescriptorPtr + g_ctrlTrfSetupPkt.bDscIndex);
				g_inPipe.wRom = (rom WORD*)g_inPipe.bRom;
				g_inPipe.wCount = *(g_inPipe.wRom + 1);
			} else if(g_ctrlTrfSetupPkt.bDescriptorType == USB_DESCRIPTOR_STRING) {
				g_inPipe.bRom = *(g_USBStringDescPtr + g_ctrlTrfSetupPkt.bDscIndex);
				g_inPipe.wCount = *g_inPipe.bRom;
			} else {
				g_inPipe.busy = 0;
			}
		}
	} else if(g_ctrlTrfSetupPkt.bRequest == SET_CONFIGURATION) {
		g_inPipe.busy = 1;
		g_USBActiveConfiguration = g_ctrlTrfSetupPkt.bConfigurationValue;
		if(g_ctrlTrfSetupPkt.bConfigurationValue == 0) {
			g_USBDeviceState = ADDRESS_STATE;
		} else {
			g_USBDeviceState = CONFIGURED_STATE;		// this is the only line in the program that changes g_USBDeviceState to CONFIGURED_STATE		
							// enable & configure endpoint 1
			UEP1bits.EPHSHK = 1;		// endpoint 1 handshake enabled
			UEP1bits.EPCONDIS = 1;		// disable endpoint 1 from performing control (i.e. setup) transfers
			UEP1bits.EPOUTEN = 1;		// endpoint 1 output enabled
			UEP1bits.EPINEN = 1;		// endpoint 1 input enabled
			UEP1bits.EPSTALL = 0;		// clear endpoint 1 stall indicator bit
			g_buffDescTable[2].STAT.UOWN = 0;
			g_buffDescTable[2].STAT.DTS  = 1;
			g_buffDescTable[3].STAT.UOWN = 0;
			g_buffDescTable[3].STAT.DTS  = 1;
			
			transferFromHostToDeviceViaEP1((BYTE*)&g_fromHostToDeviceBuffer[1], 64);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBCheckHIDRequest(void) {
	if(g_ctrlTrfSetupPkt.Recipient != RECIPIENT_INTERFACE) return;
	
	if(g_ctrlTrfSetupPkt.bRequest == GET_DESCRIPTOR) {
		if(g_ctrlTrfSetupPkt.bDescriptorType == HID_DESCRIPTOR) {
			if(g_USBActiveConfiguration == 1) {
				g_inPipe.bRom = (rom BYTE*)&g_configDescriptor1 + 18;
				g_inPipe.wCount = sizeof(USB_HID_DSC) + 3;
				g_inPipe.busy = 1;
			}
		} else if(g_ctrlTrfSetupPkt.bDescriptorType == REPORT_DESCRIPTOR) {
			if(g_USBActiveConfiguration == 1) {
				g_inPipe.bRom = (rom BYTE*)&g_HIDReport01;
				g_inPipe.wCount = sizeof(g_HIDReport01);
				g_inPipe.busy = 1;
			}
		} else if(g_ctrlTrfSetupPkt.bDescriptorType == PHY_DESCRIPTOR) {
			g_inPipe.busy = 1;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBFinishControlTransferStuff(void) {
	UCONbits.PKTDIS = 0;
	if(g_inPipe.busy == 0) {
		if(g_outPipe.busy == 1) {
			//
		} else {
			g_buffDescTable[0].CNT = USB_EP0_BUFF_SIZE;
			g_buffDescTable[0].ADR = (BYTE*)&g_ctrlTrfSetupPkt;
			g_buffDescTable[0].STAT.DTS    = 0;		// clear data toggle synch bit
			g_buffDescTable[0].STAT.KEN    = 0;		// clear keep enable bit
			g_buffDescTable[0].STAT.INCDIS = 0;		// clear address increment disable bit
			g_buffDescTable[0].STAT.DTSEN  = 1;		// set data toggle synch enable bit
			g_buffDescTable[0].STAT.BSTALL = 1;		// set buffer stall enable bit
			g_buffDescTable[0].STAT.BC9    = 0;		// clear byte count 9 bit
			g_buffDescTable[0].STAT.BC8    = 0;		// clear byte count 8 bit
			g_buffDescTable[0].STAT.UOWN   = 1;		// set UOWN bit (SIE owns buffer)
			
			g_buffDescTable[1].STAT.DTS    = 0;		// clear data toggle synch bit
			g_buffDescTable[1].STAT.KEN    = 0;		// clear keep enable bit
			g_buffDescTable[1].STAT.INCDIS = 0;		// clear address increment disable bit
			g_buffDescTable[1].STAT.DTSEN  = 0;		// clear data toggle synch enable bit
			g_buffDescTable[1].STAT.BSTALL = 1;		// set buffer stall enable bit
			g_buffDescTable[1].STAT.BC9    = 0;		// clear byte count 9 bit
			g_buffDescTable[1].STAT.BC8    = 0;		// clear byte count 8 bit
			g_buffDescTable[1].STAT.UOWN   = 1;		// set UOWN bit (SIE owns buffer)
		}
	} else {
		if(g_outPipe.busy == 0) {
			if(g_ctrlTrfSetupPkt.DataDir == DEV_TO_HOST) {
				if(g_ctrlTrfSetupPkt.wLength < g_inPipe.wCount) {
					g_inPipe.wCount = g_ctrlTrfSetupPkt.wLength;
				}
				USBControlTransferTransmit();
				g_controlTransferState = CTRL_TRF_TX;
				g_buffDescTable[0].CNT = USB_EP0_BUFF_SIZE;
				g_buffDescTable[0].ADR = (BYTE*)&g_ctrlTrfSetupPkt;
				g_buffDescTable[0].STAT.DTS    = 0;		// data toggle synch bit
				g_buffDescTable[0].STAT.KEN    = 0;		// keep enable bit
				g_buffDescTable[0].STAT.INCDIS = 0;		// address increment disable bit
				g_buffDescTable[0].STAT.DTSEN  = 0;		// data toggle synch enable bit
				g_buffDescTable[0].STAT.BSTALL = 0;		// buffer stall enable bit
				g_buffDescTable[0].STAT.BC9    = 0;		// byte count 9 bit
				g_buffDescTable[0].STAT.BC8    = 0;		// byte count 8 bit
				g_buffDescTable[0].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
				
				g_buffDescTable[1].ADR = (BYTE*)&g_ctrlTrfData;
				g_buffDescTable[1].STAT.DTS    = 1;		// data toggle synch bit
				g_buffDescTable[1].STAT.KEN    = 0;		// keep enable bit
				g_buffDescTable[1].STAT.INCDIS = 0;		// address increment disable bit
				g_buffDescTable[1].STAT.DTSEN  = 1;		// data toggle synch enable bit
				g_buffDescTable[1].STAT.BSTALL = 0;		// buffer stall enable bit
				g_buffDescTable[1].STAT.BC9    = 0;		// byte count 9 bit
				g_buffDescTable[1].STAT.BC8    = 0;		// byte count 8 bit
				g_buffDescTable[1].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
				
			} else {
				g_controlTransferState = CTRL_TRF_RX;
				g_buffDescTable[1].CNT = 0;
				g_buffDescTable[1].STAT.DTS    = 1;		// data toggle synch bit
				g_buffDescTable[1].STAT.KEN    = 0;		// keep enable bit
				g_buffDescTable[1].STAT.INCDIS = 0;		// address increment disable bit
				g_buffDescTable[1].STAT.DTSEN  = 1;		// data toggle synch enable bit
				g_buffDescTable[1].STAT.BSTALL = 0;		// buffer stall enable bit
				g_buffDescTable[1].STAT.BC9    = 0;		// byte count 9 bit
				g_buffDescTable[1].STAT.BC8    = 0;		// byte count 8 bit
				g_buffDescTable[1].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
				
				g_buffDescTable[0].CNT = USB_EP0_BUFF_SIZE;
				g_buffDescTable[0].ADR = (BYTE*)&g_ctrlTrfData;		
				g_buffDescTable[0].STAT.DTS    = 1;		// data toggle synch bit
				g_buffDescTable[0].STAT.KEN    = 0;		// keep enable bit
				g_buffDescTable[0].STAT.INCDIS = 0;		// address increment disable bit
				g_buffDescTable[0].STAT.DTSEN  = 1;		// data toggle synch enable bit
				g_buffDescTable[0].STAT.BSTALL = 0;		// buffer stall enable bit
				g_buffDescTable[0].STAT.BC9    = 0;		// byte count 9 bit
				g_buffDescTable[0].STAT.BC8    = 0;		// byte count 8 bit
				g_buffDescTable[0].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
				
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBOutControlTransfer(void) {
	if(g_controlTransferState == CTRL_TRF_RX) {
		USBControlTransferReceive();
	} else {
		USBPrepareForNextSetupTransfer();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBInControlTransfer(void) {
	BYTE lastDTS;
	
	lastDTS = g_buffDescTable[1].STAT.DTS;
	
	if(g_USBDeviceState == ADR_PENDING_STATE) {
		UADDR = g_ctrlTrfSetupPkt.bDevADR;
		if(UADDR > 0) {
			g_USBDeviceState = ADDRESS_STATE;
		} else {
			g_USBDeviceState = DEFAULT_STATE;
		}
	}
	
	if(g_controlTransferState == CTRL_TRF_TX) {
		g_buffDescTable[1].ADR = (BYTE*)g_ctrlTrfData;
		USBControlTransferTransmit();
		if(g_shortPacketStatus == SHORT_PKT_SENT) {
			g_buffDescTable[1].STAT.DTS    = 0;		// data toggle synch bit
			g_buffDescTable[1].STAT.KEN    = 0;		// keep enable bit
			g_buffDescTable[1].STAT.INCDIS = 0;		// address increment disable bit
			g_buffDescTable[1].STAT.DTSEN  = 0;		// data toggle synch enable bit
			g_buffDescTable[1].STAT.BSTALL = 1;		// buffer stall enable bit
			g_buffDescTable[1].STAT.BC9    = 0;		// byte count 9 bit
			g_buffDescTable[1].STAT.BC8    = 0;		// byte count 8 bit
			g_buffDescTable[1].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
		} else {
			if(lastDTS == 0) {
				g_buffDescTable[1].STAT.DTS    = 1;		// data toggle synch bit
				g_buffDescTable[1].STAT.KEN    = 0;		// keep enable bit
				g_buffDescTable[1].STAT.INCDIS = 0;		// address increment disable bit
				g_buffDescTable[1].STAT.DTSEN  = 1;		// data toggle synch enable bit
				g_buffDescTable[1].STAT.BSTALL = 0;		// buffer stall enable bit
				g_buffDescTable[1].STAT.BC9    = 0;		// byte count 9 bit
				g_buffDescTable[1].STAT.BC8    = 0;		// byte count 8 bit
				g_buffDescTable[1].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
			} else {
				g_buffDescTable[1].STAT.DTS    = 0;		// data toggle synch bit
				g_buffDescTable[1].STAT.KEN    = 0;		// keep enable bit
				g_buffDescTable[1].STAT.INCDIS = 0;		// address increment disable bit
				g_buffDescTable[1].STAT.DTSEN  = 1;		// data toggle synch enable bit
				g_buffDescTable[1].STAT.BSTALL = 0;		// buffer stall enable bit
				g_buffDescTable[1].STAT.BC9    = 0;		// byte count 9 bit
				g_buffDescTable[1].STAT.BC8    = 0;		// byte count 8 bit
				g_buffDescTable[1].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
			}
		}
	} else {
		USBPrepareForNextSetupTransfer();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBControlTransferReceive(void) {
	BYTE byteToRead;
	BYTE i;
	
	byteToRead = g_buffDescTable[0].CNT;
	if(byteToRead > g_outPipe.wCount) {
		byteToRead = g_outPipe.wCount;
	} else {
		g_outPipe.wCount = g_outPipe.wCount - byteToRead;
	}
	for(i=0;i<byteToRead;i++) {
		*g_outPipe.bRam++ = g_ctrlTrfData[i];
	}
	if(g_outPipe.wCount > 0) {
		g_buffDescTable[0].CNT = USB_EP0_BUFF_SIZE;
		g_buffDescTable[0].ADR = (BYTE*)&g_ctrlTrfData;
		if(g_buffDescTable[0].STAT.DTS == 0) {
			g_buffDescTable[0].STAT.DTS    = 1;		// data toggle synch bit
			g_buffDescTable[0].STAT.KEN    = 0;		// keep enable bit
			g_buffDescTable[0].STAT.INCDIS = 0;		// address increment disable bit
			g_buffDescTable[0].STAT.DTSEN  = 1;		// data toggle synch enable bit
			g_buffDescTable[0].STAT.BSTALL = 0;		// buffer stall enable bit
			g_buffDescTable[0].STAT.BC9    = 0;		// byte count 9 bit
			g_buffDescTable[0].STAT.BC8    = 0;		// byte count 8 bit
			g_buffDescTable[0].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
		} else {
			g_buffDescTable[0].STAT.DTS    = 0;		// data toggle synch bit
			g_buffDescTable[0].STAT.KEN    = 0;		// keep enable bit
			g_buffDescTable[0].STAT.INCDIS = 0;		// address increment disable bit
			g_buffDescTable[0].STAT.DTSEN  = 1;		// data toggle synch enable bit
			g_buffDescTable[0].STAT.BSTALL = 0;		// buffer stall enable bit
			g_buffDescTable[0].STAT.BC9    = 0;		// byte count 9 bit
			g_buffDescTable[0].STAT.BC8    = 0;		// byte count 8 bit
			g_buffDescTable[0].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
		}
	} else {
		g_buffDescTable[0].ADR = (BYTE*)&g_ctrlTrfSetupPkt;		
		g_outPipe.busy = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBControlTransferTransmit(void) {
	WORD_STRUCT byteToSend;
	BYTE *pDestination;
	
	if(g_inPipe.wCount < USB_EP0_BUFF_SIZE) {
		byteToSend.wordVal = g_inPipe.wCount;
		if(g_shortPacketStatus == SHORT_PKT_NOT_USED) {
			g_shortPacketStatus = SHORT_PKT_PENDING;
		} else if(g_shortPacketStatus == SHORT_PKT_PENDING) {
			g_shortPacketStatus = SHORT_PKT_SENT;
		}
	} else {
		byteToSend.wordVal = USB_EP0_BUFF_SIZE;
	}
	g_buffDescTable[1].STAT.BC9 = 0;							// copy value in byteToSend into COUNT
	g_buffDescTable[1].STAT.BC8 = 0;
	g_buffDescTable[1].STAT.STATVal |= byteToSend.byte.HB;
	g_buffDescTable[1].CNT = byteToSend.byte.LB;
	
	g_inPipe.wCount = g_inPipe.wCount - byteToSend.wordVal;
	
	pDestination = (BYTE*)g_ctrlTrfData;
	
	while(byteToSend.wordVal) {						// for each byte we have to send . . .
		*pDestination = *g_inPipe.bRom;				// assign contents of in pipe into g_ctrlTrfData
		*pDestination++;							// increment one pointer
		*g_inPipe.bRom++;							// increment other pointer
		byteToSend.wordVal--;						// decrement counter
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void USBPrepareForNextSetupTransfer(void) {
	if((g_controlTransferState == CTRL_TRF_RX) &&
	   (UCONbits.PKTDIS == 1) &&
	   (g_buffDescTable[0].CNT == sizeof(CTRL_TRF_SETUP)) &&
	   (g_buffDescTable[0].STAT.PID == SETUP_TOKEN) &&
	   (g_buffDescTable[0].STAT.UOWN == 0)) {
		BYTE setup_cnt;
		g_buffDescTable[0].ADR = (BYTE*)&g_ctrlTrfSetupPkt;
		for(setup_cnt = 0; setup_cnt < sizeof(CTRL_TRF_SETUP); setup_cnt++) {
			*(((BYTE*)&g_ctrlTrfSetupPkt) + setup_cnt) = *(((BYTE*)&g_ctrlTrfData) + setup_cnt);
		}
	} else {
		g_controlTransferState = CTRL_TRF_WAIT_SETUP;
		g_buffDescTable[0].CNT = USB_EP0_BUFF_SIZE;
		g_buffDescTable[0].ADR = (BYTE*)&g_ctrlTrfSetupPkt;
		g_buffDescTable[0].STAT.DTS    = 0;		// data toggle synch bit
		g_buffDescTable[0].STAT.KEN    = 0;		// keep enable bit
		g_buffDescTable[0].STAT.INCDIS = 0;		// address increment disable bit
		g_buffDescTable[0].STAT.DTSEN  = 1;		// data toggle synch enable bit
		g_buffDescTable[0].STAT.BSTALL = 1;		// buffer stall enable bit
		g_buffDescTable[0].STAT.BC9    = 0;		// byte count 9 bit
		g_buffDescTable[0].STAT.BC8    = 0;		// byte count 8 bit
		g_buffDescTable[0].STAT.UOWN   = 1;		// UOWN bit (SIE owns buffer)
		
		g_buffDescTable[1].STAT.UOWN   = 0;		// UOWN bit (CPU owns buffer)
		g_buffDescTable[1].STAT.DTS    = 0;		// data toggle synch bit
		g_buffDescTable[1].STAT.KEN    = 0;		// keep enable bit
		g_buffDescTable[1].STAT.INCDIS = 0;		// address increment disable bit
		g_buffDescTable[1].STAT.DTSEN  = 0;		// data toggle synch enable bit
		g_buffDescTable[1].STAT.BSTALL = 0;		// buffer stall enable bit
		g_buffDescTable[1].STAT.BC9    = 0;		// byte count 9 bit
		g_buffDescTable[1].STAT.BC8    = 0;		// byte count 8 bit
		
	}
	if(g_outPipe.busy == 1) {
		g_outPipe.busy = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void receiveViaUSB(void) {
	while(UIRbits.TRNIF != 1) { }						// wait here until any pending transactions are complete
		
	if(g_buffDescTable[2].STAT.UOWN == 0) {					// verify USB circuitry does not own from host USB buffer
		transferFromHostToDeviceViaEP1((BYTE*)&g_fromHostToDeviceBuffer[1], 64);		// get packet from host
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sendViaUSB(void) {
	while(UIRbits.TRNIF != 1) { }						// wait here until any pending transactions are complete
	
	if(g_buffDescTable[3].STAT.UOWN == 0) {					// verify USB circuitry does not own to host USB buffer
		transferFromDeviceToHostViaEP1((BYTE*)&g_fromDeviceToHostBuffer[1], 64);		// send packet to host
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void transferFromHostToDeviceViaEP1(BYTE* pPacketData, BYTE packetLength) {	
	g_buffDescTable[2].STAT.DTS    = !g_buffDescTable[2].STAT.DTS;	// toggle the data toggle synch bit
	g_buffDescTable[2].STAT.KEN    = 0;								// clear keep enable bit
	g_buffDescTable[2].STAT.INCDIS = 0;								// clear address increment disable bit
	g_buffDescTable[2].STAT.DTSEN  = 1;								// set data toggle synch enable bit
	g_buffDescTable[2].STAT.BSTALL = 0;								// clear buffer stall enable bit
	g_buffDescTable[2].STAT.BC9    = 0;								// clear byte count 9 bit
	g_buffDescTable[2].STAT.BC8    = 0;								// clear byte count 8 bit
	g_buffDescTable[2].STAT.UOWN   = 1;								// set UOWN bit (SIE owns buffer)
	
	g_buffDescTable[2].CNT = packetLength;
	g_buffDescTable[2].ADR = pPacketData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void transferFromDeviceToHostViaEP1(BYTE* pPacketData, BYTE packetLength) {			
	g_buffDescTable[3].STAT.DTS    = !g_buffDescTable[3].STAT.DTS;	// toggle the data toggle synch bit
	g_buffDescTable[3].STAT.KEN    = 0;								// clear keep enable bit
	g_buffDescTable[3].STAT.INCDIS = 0;								// clear address increment disable bit
	g_buffDescTable[3].STAT.DTSEN  = 1;								// set data toggle synch enable bit
	g_buffDescTable[3].STAT.BSTALL = 0;								// clear buffer stall enable bit
	g_buffDescTable[3].STAT.BC9    = 0;								// clear byte count 9 bit
	g_buffDescTable[3].STAT.BC8    = 0;								// clear byte count 8 bit
	g_buffDescTable[3].STAT.UOWN   = 1;								// set UOWN bit (SIE owns buffer)
	
	g_buffDescTable[3].CNT = packetLength;
	g_buffDescTable[3].ADR = pPacketData;
}
