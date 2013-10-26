namespace USBDemoBoard
{
	partial class Form1
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.lblInfo = new System.Windows.Forms.Label();
			this.lblSwitchStates = new System.Windows.Forms.Label();
			this.lblSwitch1 = new System.Windows.Forms.Label();
			this.lblSwitch2 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.rdoLED1Off = new System.Windows.Forms.RadioButton();
			this.rdoLED1On = new System.Windows.Forms.RadioButton();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.rdoLED2Off = new System.Windows.Forms.RadioButton();
			this.rdoLED2On = new System.Windows.Forms.RadioButton();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.rdoLED3Off = new System.Windows.Forms.RadioButton();
			this.rdoLED3On = new System.Windows.Forms.RadioButton();
			this.tmrUSB = new System.Windows.Forms.Timer(this.components);
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.SuspendLayout();
			// 
			// lblInfo
			// 
			this.lblInfo.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblInfo.Location = new System.Drawing.Point(4, 4);
			this.lblInfo.Name = "lblInfo";
			this.lblInfo.Size = new System.Drawing.Size(428, 28);
			this.lblInfo.TabIndex = 0;
			this.lblInfo.Click += new System.EventHandler(this.lblInfo_Click);
			// 
			// lblSwitchStates
			// 
			this.lblSwitchStates.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSwitchStates.Location = new System.Drawing.Point(80, 44);
			this.lblSwitchStates.Name = "lblSwitchStates";
			this.lblSwitchStates.Size = new System.Drawing.Size(132, 28);
			this.lblSwitchStates.TabIndex = 1;
			this.lblSwitchStates.Text = "switch states:";
			// 
			// lblSwitch1
			// 
			this.lblSwitch1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblSwitch1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSwitch1.Location = new System.Drawing.Point(76, 76);
			this.lblSwitch1.Name = "lblSwitch1";
			this.lblSwitch1.Size = new System.Drawing.Size(212, 28);
			this.lblSwitch1.TabIndex = 2;
			this.lblSwitch1.Text = "switch 1 is not pressed";
			// 
			// lblSwitch2
			// 
			this.lblSwitch2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblSwitch2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSwitch2.Location = new System.Drawing.Point(76, 112);
			this.lblSwitch2.Name = "lblSwitch2";
			this.lblSwitch2.Size = new System.Drawing.Size(212, 28);
			this.lblSwitch2.TabIndex = 3;
			this.lblSwitch2.Text = "switch 2 is not pressed";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.rdoLED1Off);
			this.groupBox1.Controls.Add(this.rdoLED1On);
			this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.groupBox1.Location = new System.Drawing.Point(64, 152);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(88, 108);
			this.groupBox1.TabIndex = 4;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "LED 1:";
			// 
			// rdoLED1Off
			// 
			this.rdoLED1Off.AutoSize = true;
			this.rdoLED1Off.Checked = true;
			this.rdoLED1Off.Location = new System.Drawing.Point(16, 68);
			this.rdoLED1Off.Name = "rdoLED1Off";
			this.rdoLED1Off.Size = new System.Drawing.Size(54, 29);
			this.rdoLED1Off.TabIndex = 1;
			this.rdoLED1Off.TabStop = true;
			this.rdoLED1Off.Text = "off";
			this.rdoLED1Off.UseVisualStyleBackColor = true;
			// 
			// rdoLED1On
			// 
			this.rdoLED1On.AutoSize = true;
			this.rdoLED1On.Location = new System.Drawing.Point(16, 32);
			this.rdoLED1On.Name = "rdoLED1On";
			this.rdoLED1On.Size = new System.Drawing.Size(55, 29);
			this.rdoLED1On.TabIndex = 0;
			this.rdoLED1On.Text = "on";
			this.rdoLED1On.UseVisualStyleBackColor = true;
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.rdoLED2Off);
			this.groupBox2.Controls.Add(this.rdoLED2On);
			this.groupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.groupBox2.Location = new System.Drawing.Point(164, 152);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(88, 108);
			this.groupBox2.TabIndex = 5;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "LED 2:";
			// 
			// rdoLED2Off
			// 
			this.rdoLED2Off.AutoSize = true;
			this.rdoLED2Off.Checked = true;
			this.rdoLED2Off.Location = new System.Drawing.Point(16, 68);
			this.rdoLED2Off.Name = "rdoLED2Off";
			this.rdoLED2Off.Size = new System.Drawing.Size(54, 29);
			this.rdoLED2Off.TabIndex = 1;
			this.rdoLED2Off.TabStop = true;
			this.rdoLED2Off.Text = "off";
			this.rdoLED2Off.UseVisualStyleBackColor = true;
			// 
			// rdoLED2On
			// 
			this.rdoLED2On.AutoSize = true;
			this.rdoLED2On.Location = new System.Drawing.Point(16, 32);
			this.rdoLED2On.Name = "rdoLED2On";
			this.rdoLED2On.Size = new System.Drawing.Size(55, 29);
			this.rdoLED2On.TabIndex = 0;
			this.rdoLED2On.Text = "on";
			this.rdoLED2On.UseVisualStyleBackColor = true;
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.rdoLED3Off);
			this.groupBox3.Controls.Add(this.rdoLED3On);
			this.groupBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.groupBox3.Location = new System.Drawing.Point(264, 152);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(88, 108);
			this.groupBox3.TabIndex = 6;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "LED 3:";
			// 
			// rdoLED3Off
			// 
			this.rdoLED3Off.AutoSize = true;
			this.rdoLED3Off.Checked = true;
			this.rdoLED3Off.Location = new System.Drawing.Point(16, 68);
			this.rdoLED3Off.Name = "rdoLED3Off";
			this.rdoLED3Off.Size = new System.Drawing.Size(54, 29);
			this.rdoLED3Off.TabIndex = 1;
			this.rdoLED3Off.TabStop = true;
			this.rdoLED3Off.Text = "off";
			this.rdoLED3Off.UseVisualStyleBackColor = true;
			// 
			// rdoLED3On
			// 
			this.rdoLED3On.AutoSize = true;
			this.rdoLED3On.Location = new System.Drawing.Point(16, 32);
			this.rdoLED3On.Name = "rdoLED3On";
			this.rdoLED3On.Size = new System.Drawing.Size(55, 29);
			this.rdoLED3On.TabIndex = 0;
			this.rdoLED3On.Text = "on";
			this.rdoLED3On.UseVisualStyleBackColor = true;
			// 
			// tmrUSB
			// 
			this.tmrUSB.Interval = 50;
			this.tmrUSB.Tick += new System.EventHandler(this.tmrUSB_Tick);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(435, 264);
			this.Controls.Add(this.groupBox3);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.lblSwitch2);
			this.Controls.Add(this.lblSwitch1);
			this.Controls.Add(this.lblSwitchStates);
			this.Controls.Add(this.lblInfo);
			this.Name = "Form1";
			this.Text = "Form1";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label lblInfo;
		private System.Windows.Forms.Label lblSwitchStates;
		private System.Windows.Forms.Label lblSwitch1;
		private System.Windows.Forms.Label lblSwitch2;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.RadioButton rdoLED1Off;
		private System.Windows.Forms.RadioButton rdoLED1On;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.RadioButton rdoLED2Off;
		private System.Windows.Forms.RadioButton rdoLED2On;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.RadioButton rdoLED3Off;
		private System.Windows.Forms.RadioButton rdoLED3On;
		private System.Windows.Forms.Timer tmrUSB;
	}
}

