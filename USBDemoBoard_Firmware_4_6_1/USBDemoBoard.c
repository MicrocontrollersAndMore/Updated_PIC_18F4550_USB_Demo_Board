// USBDemoBoard.c

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

#define SWITCH1		PORTAbits.RA1			// input pins
#define SWITCH2		PORTAbits.RA2

#define LED1	PORTBbits.RB1			// output pins
#define LED2	PORTBbits.RB2
#define LED3	PORTBbits.RB3

#define LED1_ON		0x01			// LED command states
#define LED1_OFF	0x00

#define LED2_ON		0x01
#define LED2_OFF	0x00

#define LED3_ON		0x01
#define LED3_OFF	0x00

#define SWITCH1_ON	0x01			// switch states
#define SWITCH1_OFF	0x00

#define SWITCH2_ON	0x01
#define SWITCH2_OFF	0x00

// global variables ///////////////////////////////////////////////////////////////////////////////
extern BYTE g_USBDeviceState;
extern BYTE g_fromHostToDeviceBuffer[65];
extern BYTE g_fromDeviceToHostBuffer[65];

		// format for g_fromHostToDeviceBuffer[]
		// 
		// byte - purpose
		// -------------------------------------
		//	0 - do NOT use this for data, this is initialized to zero in USBstuff.c, leave as is (part of USB protocol)
		//	1 - LED1 state
		//	2 - LED2 state
		//	3 - LED3 state
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
		//	1 - SWITCH1 state
		//	2 - SWITCH2 state
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
	
	ADCON1bits.PCFG3 = 1;			// set all AN pins to digital I/O
	ADCON1bits.PCFG2 = 1;			//
	ADCON1bits.PCFG1 = 1;			//
	ADCON1bits.PCFG0 = 1;			//
									
	TRISAbits.RA1 = 1;				// config input pins
	TRISAbits.RA2 = 1;
	
	TRISBbits.TRISB3 = 0;			// set RB3 to output
	TRISBbits.TRISB2 = 0;			// set RB2 to output
	TRISBbits.TRISB1 = 0;			// set RB1 to output
	
	LED3 = 0;						// init output pins to off
	LED2 = 0;
	LED1 = 0;
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
		
		receiveViaUSB();									// read into input buffer
		
															// process inputs here (check g_fromHostToDeviceBuffer[x])
		if(g_fromHostToDeviceBuffer[1] == LED1_ON) {
			LED1 = 1;
		} else if(g_fromHostToDeviceBuffer[1] == LED1_OFF) {
			LED1 = 0;
		} else {
			// should never get here
		}
		
		if(g_fromHostToDeviceBuffer[2] == LED2_ON) {
			LED2 = 1;
		} else if(g_fromHostToDeviceBuffer[2] == LED2_OFF) {
			LED2 = 0;
		} else {
			// should never get here
		}
		
		if(g_fromHostToDeviceBuffer[3] == LED3_ON) {
			LED3 = 1;
		} else if(g_fromHostToDeviceBuffer[3] == LED3_OFF) {
			LED3 = 0;
		} else {
			// should never get here
		}
		
															// set outputs here (set g_fromDeviceToHostBuffer[x])
		if(SWITCH1 == 0) {									// if switch 1 is on (remember logic low)
			g_fromDeviceToHostBuffer[1] = SWITCH1_ON;
		} else if(SWITCH1 == 1) {							// else if switch 1 is off (remember logic low)
			g_fromDeviceToHostBuffer[1] = SWITCH1_OFF;
		} else {
			// should never get here
		}
		
		if(SWITCH2 == 0) {									// if switch 2 is on (remember logic low)
			g_fromDeviceToHostBuffer[2] = SWITCH2_ON;
		} else if(SWITCH2 == 1) {							// else if switch 2 is off (remember logic low)
			g_fromDeviceToHostBuffer[2] = SWITCH2_OFF;
		} else {
			// should never get here
		}
		
		sendViaUSB();
	}		

}
