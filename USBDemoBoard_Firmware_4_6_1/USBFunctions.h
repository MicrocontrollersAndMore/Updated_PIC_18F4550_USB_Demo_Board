// USBFunctions.h

// **************** note to programmer !!! READ THIS!! IMPORTANT !!! ******************************
// to use the USBStuff.h and USBStuff.c files, cut and paste the following into your .c file to
// get your program started:

/*
// ********************** begin cut / paste *******************************************************

// includes ///////////////////////////////////////////////////////////////////////////////////////
#include<p18f4550.h>
#include"USBFunctions.h"

// chip config ////////////////////////////////////////////////////////////////////////////////////
					// clock options, see 18F4550 data sheet figure 2-1 "clock diagram" for explanation
#pragma config PLLDIV = 5				// 20 MHz external clock / PLL prescaler value of 5 = 4 MHz required input to PLL circuit
#pragma config CPUDIV = OSC1_PLL2		// non-PLL postscale / 1 OR PLL postscale / 2 for CPU clock speed, depending on FOSC setting below
#pragma config USBDIV = 2				// USB clock source = 96 MHz PLL source / 2, (full-speed USB mode)

						// if desired, could change this line to "FOSC = HS" & "oscillator postscaler" gate would be used 
						// (not the "PLL postscaler" gate), CPU speed would be 20MHz, USB circuitry would still receive 48Mhz clock
#pragma config FOSC = HSPLL_HS			// use high-speed external osc crystal, & use PLL postscaler gate to feed CPU (CPU speed = 48 MHz)

					// now the other less confusing options . . .
#pragma config FCMEN = OFF				// fail-safe clock monitor disabled
#pragma config IESO = OFF				// internal / external osc switchover bit disabled
#pragma config PWRT = OFF				// power-up timer disabled
#pragma config BOR = OFF				// brown-out reset disabled in hardware & software
#pragma config BORV = 3					// brown-out reset voltage bits, does not matter since brown-out is disabled 
#pragma config VREGEN = ON				// USB voltage regulator enabled (If using USB, capacitor goes on pin 18 (VUSB))
#pragma config WDT = OFF				// watchdog timer disabled
#pragma config WDTPS = 32768			// watchdog timer postscale, does not matter since watchdog timer is disabled
#pragma config CCP2MX = ON				// use RC1 (pin #16) as CCP2 MUX (this is the default pin for CCP2 MUX)
#pragma config PBADEN = OFF				// RB0, RB1, RB2, RB3, & RB4 are configured as digital I/O on reset
#pragma config LPT1OSC = OFF			// disable low-power option for timer 1 (timer 1 in regular mode)
#pragma config MCLRE = OFF				// master clear disabled, pin #1 is for VPP and / or RE3 use
#pragma config STVREN = ON				// stack full/underflow will cause reset
#pragma config LVP = OFF				// single-supply ICSP disabled
#pragma config ICPRT = OFF				// in-circuit debug/programming port (ICPORT) disabled, this feature is not available on 40 pin DIP package
#pragma config XINST = OFF				// instruction set extension and indexed addressing mode disabled (this is the default setting)
#pragma config DEBUG = OFF				// background debugger disabled, RA6 & RB7 configured as general purpose I/O pins
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF			// code protection bits off
#pragma config CPB = OFF				// boot block code protection off
#pragma config CPD = OFF				// data EEPROM code protection off
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF		// write protection bits off
#pragma config WRTC = OFF				// config registers write protection off
#pragma config WRTB = OFF				// boot block is not write protected
#pragma config WRTD = OFF				// data EEPROM is not write protected
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF	// table read protection bits off
#pragma config EBTRB = OFF				// boot block table read protection off

// #defines ///////////////////////////////////////////////////////////////////////////////////////
#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008			// these are necessary to accommodate the special linker file,
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018			// do not change these !!

// global variables ///////////////////////////////////////////////////////////////////////////////
extern BYTE g_USBDeviceState;
extern BYTE g_fromHostToDeviceBuffer[65];
extern BYTE g_fromDeviceToHostBuffer[65];

		// format for g_fromHostToDeviceBuffer[]
		// 
		// byte - purpose
		// -------------------------------------
		//	0 - do NOT use this for data, this is initialized to zero in USBstuff.c, leave as is (part of USB protocol)
		//	1 - unused
		//	2 - unused
		//	3 - unused
		//	4 - unused
		//	5 - unused
		//	6 - unused
		//	7 - unused
		//	8 - usused
		//	9 - unused
		// 10 - unused
		// 11 - unused
		// 12 - unused
		// 13 - unused
		// 14 - usused
		// 15 - unused
		// 16 - unused
		// 17 - unused
		// 18 - unused
		// 19 - unused
		// 20 - usused
		// 21 - unused
		// 22 - unused
		// 23 - unused
		// 24 - unused
		// 25 - unused
		// 26 - usused
		// 27 - unused
		// 28 - unused
		// 29 - unused
		// 30 - unused
		// 31 - usused
		// 32 - unused
		// 33 - unused
		// 34 - unused
		// 35 - unused
		// 36 - unused
		// 37 - usused
		// 38 - unused
		// 39 - unused
		// 40 - unused
		// 41 - unused
		// 42 - unused
		// 43 - usused
		// 44 - unused
		// 45 - unused
		// 46 - unused
		// 47 - unused
		// 48 - unused
		// 49 - usused
		// 50 - unused
		// 51 - unused
		// 52 - unused
		// 53 - unused
		// 54 - unused
		// 55 - usused
		// 56 - unused
		// 57 - unused
		// 58 - unused
		// 59 - unused
		// 60 - unused
		// 61 - usused
		// 62 - unused
		// 63 - unused
		// 64 - unused
		
		// format for g_fromDeviceToHostBuffer[]
		// 
		// byte - purpose
		// -------------------------------------
		//	0 - do NOT use this for data, this is initialized to zero in USBstuff.c, leave as is (part of USB protocol)
		//	1 - unused
		//	2 - unused
		//	3 - unused
		//	4 - unused
		//	5 - unused
		//	6 - unused
		//	7 - unused
		//	8 - usused
		//	9 - unused
		// 10 - unused
		// 11 - unused
		// 12 - unused
		// 13 - unused
		// 14 - usused
		// 15 - unused
		// 16 - unused
		// 17 - unused
		// 18 - unused
		// 19 - unused
		// 20 - usused
		// 21 - unused
		// 22 - unused
		// 23 - unused
		// 24 - unused
		// 25 - unused
		// 26 - usused
		// 27 - unused
		// 28 - unused
		// 29 - unused
		// 30 - unused
		// 31 - usused
		// 32 - unused
		// 33 - unused
		// 34 - unused
		// 35 - unused
		// 36 - unused
		// 37 - usused
		// 38 - unused
		// 39 - unused
		// 40 - unused
		// 41 - unused
		// 42 - unused
		// 43 - usused
		// 44 - unused
		// 45 - unused
		// 46 - unused
		// 47 - unused
		// 48 - unused
		// 49 - usused
		// 50 - unused
		// 51 - unused
		// 52 - unused
		// 53 - unused
		// 54 - unused
		// 55 - usused
		// 56 - unused
		// 57 - unused
		// 58 - unused
		// 59 - unused
		// 60 - unused
		// 61 - usused
		// 62 - unused
		// 63 - unused
		// 64 - unused
		
// function prototypes ////////////////////////////////////////////////////////////////////////////
void highISR(void);							// interrupt prototypes
void remappedHighISR(void);					//
void yourHighPriorityISRCode(void);			//
											//
void lowISR(void);							//
void remappedLowISR(void);					//
void yourLowPriorityISRCode(void);			//
											//
extern void _startup(void);					//

void yourInit(void);
void yourTasks(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code HIGH_INTERRUPT_VECTOR = 0x08
void highISR(void) {
	_asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
}
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void remappedHighISR(void) {
	_asm goto yourHighPriorityISRCode _endasm
}
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt yourHighPriorityISRCode
void yourHighPriorityISRCode(void) {
	// check which int flag is set
	// service int
	// clear flag
	// etc.
} // return will be a "retfie fast"
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code LOW_INTERRUPT_VECTOR = 0x18
void lowISR(void) {
	_asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
}
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void remappedLowISR(void) {
	_asm goto yourLowPriorityISRCode _endasm
}
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma interruptlow yourLowPriorityISRCode
void yourLowPriorityISRCode(void) {
	// check which int flag is set
	// service int
	// clear int flag
	// etc.
} // return will be a "retfie"
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code REMAPPED_RESET_VECTOR = 0x1000
void _reset(void) {
	_asm goto _startup _endasm
}
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
void main(void) {
	USBInit();				// in USBFunctions.c
	yourInit();				// in this file
	while(1) {
		USBTasks();			// in USBFunctions.c
		yourTasks();		// in this file
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void yourInit(void) {
	// initialization code specific to your board goes here
	// (TRIS registers, ADCON registers, setting up interrupt / timer registers, etc.)
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void yourTasks(void) {

// your code here
//
// NOTES:
//
// The global variable g_USBDeviceState denotes the current state of the USB connection.  There are
// 6 states for g_USBDeviceState, each with matching #defines in USBStuff.h, as follows:
// DETACHED_STATE
// ATTACHED_STATE
// POWERED_STATE
// DEFAULT_STATE
// ADDRESS_STATE
// CONFIGURED_STATE
//
// Before doing anything with the USB buffer arrary, verify that:
//
// g_USBDeviceState == CONFIGURED_STATE
//
// There are 2 USB buffer arrays, g_fromHostToDeviceBuffer[65] and g_fromDeviceToHostBuffer[65].
// Hopefully it is clear from the names which goes which direction.  Each element is one byte.
// The first elements in each, g_fromHostToDeviceBuffer[0] and g_fromDeviceToHostBuffer[0], are
// the 'Report ID' and are part of the USB protocol, do not use these.  Start you data with
// g_fromHostToDeviceBuffer[1] and g_fromDeviceToHostBuffer[1].  This leaves 64 bytes of data
// for you to use.
//
// for USB I/O, call receiveViaUSB() and sendViaUSB()

	if(g_USBDeviceState == CONFIGURED_STATE) {
		
		receiveViaUSB();											// read into input buffer
		
		// process inputs here (check g_fromHostToDeviceBuffer[x])
		
		// set outputs here (set g_fromDeviceToHostBuffer[x])
		
		sendViaUSB();
	}		

}

// **************** end cut / paste, beginning of actual USBStuff.h file **************************
*/

// #defines, typedefs, structs ////////////////////////////////////////////////////////////////////

		// USB device states - use with g_USBDeviceState (BYTE data type)
#define DETACHED_STATE		0x00
#define ATTACHED_STATE		0x01
#define POWERED_STATE		0x02
#define DEFAULT_STATE		0x04
#define ADR_PENDING_STATE	0x08
#define ADDRESS_STATE		0x10
#define CONFIGURED_STATE	0x20

		// for use with g_controlTransferState
#define CTRL_TRF_WAIT_SETUP		0
#define CTRL_TRF_TX				1
#define CTRL_TRF_RX				2

		// for use with g_shortPacketStatus
#define SHORT_PKT_NOT_USED	0
#define SHORT_PKT_PENDING	1
#define SHORT_PKT_SENT		2

		// for use with g_ctrlTrfSetupPkt.bDescriptorType
#define USB_DESCRIPTOR_DEVICE			0x01
#define USB_DESCRIPTOR_CONFIGURATION	0x02
#define USB_DESCRIPTOR_STRING			0x03
#define USB_DESCRIPTOR_INTERFACE		0x04
#define USB_DESCRIPTOR_ENDPOINT			0x05
#define USB_DESCRIPTOR_DEVICE_QUALIFIER 0x06
#define USB_DESCRIPTOR_OTHER_SPEED		0x07
#define USB_DESCRIPTOR_INTERFACE_POWER	0x08
#define USB_DESCRIPTOR_OTG				0x09

typedef enum _BOOL { FALSE = 0, TRUE = 1 } BOOL;

#define NULL 0

typedef unsigned char		BYTE;		// unsigned 8-bit int
typedef unsigned short		WORD;		// unsigned 16-bit int
typedef unsigned long		UINT32;		// unsigned 32-bit int
typedef unsigned long long	UINT64;		// unsigned 64-bit int

typedef union _WORD_VAL {		// 2 bytes
	WORD wordVal;
	struct {
		BYTE LB;
		BYTE HB;
	} byte;
} WORD_STRUCT;

typedef struct _USB_DEVICE_DESCRIPTOR {
	BYTE bLength;
	BYTE bDescriptorType;
	WORD bcdUSB;
	BYTE bDeviceClass;
	BYTE bDeviceSubClass;
	BYTE bDeviceProtocol;
	BYTE bMaxPacketSize0;
	WORD idVendor;
	WORD idProduct;
	WORD bcdDevice;
	BYTE iManufacturer;
	BYTE iProduct;
	BYTE iSerialNumber;
	BYTE bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

typedef struct _USB_HID_DSC {
	BYTE bLength;
	BYTE bDescriptorType;
	WORD bcdHID;
	BYTE bCountryCode;
	BYTE bNumDsc;
} USB_HID_DSC;

typedef struct {				// 4 bytes
	union {							// BDnSTAT imitation
		struct {					// CPU mode (UOWN == 0)
			unsigned BC8:1;
			unsigned BC9:1;
			unsigned BSTALL:1;
			unsigned DTSEN:1;
			unsigned INCDIS:1;
			unsigned KEN:1;
			unsigned DTS:1;
			unsigned UOWN:1;
		};
		struct {
			unsigned BC8:1;			// SIE mode (UOWN == 1)
			unsigned BC9:1;
			unsigned PID:4;
			unsigned :1;
			unsigned UOWN:1;
		};
		BYTE STATVal;
	} STAT;
	BYTE CNT;						// size of buffer in bytes
	BYTE *ADR;
} BDT_ENTRY;

#define USB_EP0_BUFF_SIZE		8

#define HID_INTERFACE_ID		0x00

#define HID_NUM_OF_DSC			1		// for use with config descriptor only

#define HID_RPT01_SIZE			29		// HID report 1 size in bytes, used with descriptors only

#define _EP_IN		0x80		// for use with descriptor declarations only
#define _EP_OUT		0x00		//

		// configuration attributes
#define _DEFAULT	(0x01 << 7)		// default value, bit 7 is set			used with descriptor declarations only
#define _SELF		(0x01 << 6)		// self-powered (supports if set)		why are these done as bit shifts ??!!
#define _RWU		(0x01 << 5)		// remote wakeup (supports if set)

		// endpoint transfer type, for use with config descriptor only
#define _CTRL	0x00		// control transfer
#define _ISO	0x01		// isochronous transfer
#define _BULK	0x02		// bulk transfer
#define _INT	0x03		// interrupt transfer

		// HID interface class code, for use with config descriptor only
#define HID_INTF		0x03

// every setup packet has 8 bytes, this structure allows direct access to the various members of the control transfer
typedef union _CTRL_TRF_SETUP {				// 8 bytes
	struct {
		BYTE bmRequestType;		// bit map request type
		BYTE bRequest;
		WORD wValue;
		WORD wIndex;
		WORD wLength;
	};
	struct {
		unsigned Recipient:5;
		unsigned RequestType:2;
		unsigned DataDir:1;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
	};
	struct {
		unsigned :8;
		unsigned :8;
		BYTE bDscIndex;
		BYTE bDescriptorType;
		WORD wLangID;
		unsigned :8;
		unsigned :8;
	};
	struct {
		unsigned :8;
		unsigned :8;
		BYTE bDevADR;
		BYTE bDevADRH;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
	};
	struct {
		unsigned :8;
		unsigned :8;
		BYTE bConfigurationValue;
		BYTE bCfgRSD;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
	};
} CTRL_TRF_SETUP;

		// pipe structure, used to keep track of data that is sent out of the stack automatically
typedef struct {		// 7 bytes
	rom BYTE *bRom;
	rom WORD *wRom;
	BYTE busy;
	WORD wCount;
} IN_PIPE;

typedef struct {		// 5 bytes
	BYTE *bRam;	
	BYTE busy;
	WORD wCount;
} OUT_PIPE;

			// for use with g_crtlTrfSetupPkt.bRequest, see USB 2.0 spec. table 9-3
#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR		6
#define SET_DESCRIPTOR		7
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11
#define SYNCH_FRAME			12

			// for use with g_buffDescTable.STAT.PID, note that number is 4-bit
#define SETUP_TOKEN		0b1101	// 13 base 10
#define OUT_TOKEN		0b0001	//  1 base 10
#define IN_TOKEN		0b1001	//  9 base 10

			// for use with g_ctrlTrfSetupPkt.DataDir
#define HOST_TO_DEV		0
#define DEV_TO_HOST		1

			// for use with CTRL_TRF_SETUP.RequestType
#define STANDARD	0x00
#define CLASS		0x01
#define VENDOR		0x02

			// for use with g_ctrlTrfSetupPkt.Recipient
#define RECIPIENT_DEVICE		0
#define RECIPIENT_INTERFACE		1
#define RECIPIENT_ENDPOINT		2
#define RECIPIENT_OTHER			3

			// for use with g_ctrlTrfSetupPkt.bRequest
#define GET_REPORT		0x01
#define GET_IDLE		0x02
#define GET_PROTOCOL	0x03
#define SET_REPORT		0x09
#define SET_IDLE		0x0A
#define SET_PROTOCOL	0x0B

			// for use with g_ctrlTrfSetupPkt.bDescriptorType, and also in descriptor declarations
#define HID_DESCRIPTOR		0x21
#define REPORT_DESCRIPTOR	0x22
#define PHY_DESCRIPTOR		0x23

#define CLK_FREQ 48000000

// function prototypes ////////////////////////////////////////////////////////////////////////////
void USBInit(void);
void USBTasks(void);
void USBWakeFromSuspend(void);
void USBSuspend(void);
void USBStallHandler(void);
void USBEP0ControlTransfer(void);
void USBSetupControlTransfer(void);
void USBOutControlTransfer(void);
void USBInControlTransfer(void);
void USBCheckStandardRequest(void);
void USBCheckHIDRequest(void);
void USBFinishControlTransferStuff(void);
void USBControlTransferReceive(void);
void USBControlTransferTransmit(void);
void USBPrepareForNextSetupTransfer(void);
void receiveViaUSB(void);
void sendViaUSB(void);
void transferFromHostToDeviceViaEP1(BYTE* pPacketData, BYTE packetLength);
void transferFromDeviceToHostViaEP1(BYTE* pPacketData, BYTE packetLength);
