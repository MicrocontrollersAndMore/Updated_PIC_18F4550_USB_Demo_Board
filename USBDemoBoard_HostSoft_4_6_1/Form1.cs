using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace USBDemoBoard
{

	/////////////////////////////////////////////////////////////////////////////////////////////////
	public partial class Form1 : Form
	{

		// constants //////////////////////////////////////////////////////////////////////////////////
		const byte LED1_ON = 0x01;			// LED command states
		const byte LED1_OFF = 0x00;

		const byte LED2_ON = 0x01;
		const byte LED2_OFF = 0x00;

		const byte LED3_ON = 0x01;
		const byte LED3_OFF = 0x00;

		const byte SWITCH1_ON = 0x01;			// switch states
		const byte SWITCH1_OFF = 0x00;

		const byte SWITCH2_ON = 0x01;
		const byte SWITCH2_OFF = 0x00;

		// member variables ///////////////////////////////////////////////////////////////////////////
		public USBClass USBObject;

		// constructor ////////////////////////////////////////////////////////////////////////////////
		public Form1()
		{
			InitializeComponent();
			USBObject = new USBClass();
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		private void Form1_Load(object sender, EventArgs e)
		{
			attemptUSBConnectionFrontEnd();						// attempt USB connection
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		private void lblInfo_Click(object sender, EventArgs e)
		{
			if (USBObject.connectionState == USBClass.CONNECTION_NOT_SUCCESSFUL)
			{				// verify not already connected . . .
				attemptUSBConnectionFrontEnd();														// then attempt to connect again
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		void attemptUSBConnectionFrontEnd()
		{
			lblInfo.Text = "connecting . . . ";

			USBObject.connectionState = USBObject.attemptUSBConnection();												// attempt to connect to USB board

			if (USBObject.connectionState == USBClass.CONNECTION_SUCCESSFUL)
			{								// if connection was successful
				lblInfo.BackColor = System.Drawing.Color.LimeGreen;
				lblInfo.Text = "connection successful";
				tmrUSB.Enabled = true;
			}
			else if (USBObject.connectionState == USBClass.CONNECTION_NOT_SUCCESSFUL)
			{		// else if connection was not successful
				lblInfo.BackColor = System.Drawing.Color.Red;
				lblInfo.Text = "connection not successful, click here to retry";
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		private void tmrUSB_Tick(object sender, EventArgs e)
		{
			USBObject.receiveViaUSB();

			if (USBObject.fromDeviceToHostBuffer[1] == SWITCH1_ON)
			{
				lblSwitch1.Text = "switch 1 is pressed";
			}
			else if (USBObject.fromDeviceToHostBuffer[1] == SWITCH1_OFF)
			{
				lblSwitch1.Text = "switch 1 is not pressed";
			}

			if (USBObject.fromDeviceToHostBuffer[2] == SWITCH2_ON)
			{
				lblSwitch2.Text = "switch 2 is pressed";
			}
			else if (USBObject.fromDeviceToHostBuffer[2] == SWITCH2_OFF)
			{
				lblSwitch2.Text = "switch 2 is not pressed";
			}

			if (rdoLED1On.Checked == true)
			{
				USBObject.fromHostToDeviceBuffer[1] = LED1_ON;
			}
			else if (rdoLED1Off.Checked == true)
			{
				USBObject.fromHostToDeviceBuffer[1] = LED1_OFF;
			}

			if (rdoLED2On.Checked == true)
			{
				USBObject.fromHostToDeviceBuffer[2] = LED2_ON;
			}
			else if (rdoLED2Off.Checked == true)
			{
				USBObject.fromHostToDeviceBuffer[2] = LED2_OFF;
			}

			if (rdoLED3On.Checked == true)
			{
				USBObject.fromHostToDeviceBuffer[3] = LED3_ON;
			}
			else if (rdoLED3Off.Checked == true)
			{
				USBObject.fromHostToDeviceBuffer[3] = LED3_OFF;
			}

			USBObject.sendViaUSB();
		}

	}		// end class
}		// end namespace

