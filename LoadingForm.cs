#region Header Block
/// <classname>LoadingForm</classname> 
/// 
/// <description>To use for showing the loading bar at startup</description> 
/// 
/// <filename>LoadingForm.cs</filename> 
/// 
/// <authors>Koh Jiefei</authors> 
/// 
/// <version>00.03.12.00</version> 
/// 
/// <changes>Not applicable</changes> 
/// 
/// <todo>Not applicable</todo> 
/// 
/// <copyright>Copyright @ 2006-2010 Singapore Technologies Electronics Ltd</copyright>
#endregion

using System;					// Contains classes that allow you to match URIs against URI Templates and groups of URI Templates
using System.Windows.Forms;		// Contains classes for creating Windows-based applications

namespace ste.pa.pamanager
{
	/// <summary> 
	/// Use for showing the loading bar at startup
	/// </summary> 
	public partial class LoadingForm : Form
	{
		#region Private Variables
		private int counterVal_ = 0;
		#endregion

		/// <summary>
		/// Default constructor
		/// </summary>
		public LoadingForm()
		{
			InitializeComponent();
			timer1.Start();
		}

		/// <summary>
		/// Timer action
		/// </summary>
		/// <param name="sender">The sender object</param>
		/// <param name="e">The event occur</param>
		private void timer1_Tick(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			if (counterVal_ > 1)
			{
				progressBar1.Value = 0;
				counterVal_ = 0;
			}

			if (progressBar1.Value != progressBar1.Maximum)
			{
				++progressBar1.Value;
			}

			if (progressBar1.Value == progressBar1.Maximum)
			{
				counterVal_++;
			}
		}

		/// <summary>
		/// Event handler that allows the release of resources used by the class during runtime from another thread.
		/// </summary>
		/// <param name="sender">The calling object.</param>
		/// <param name="e">The clean up event.</param>
		public void CleanUp(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			if (timer1 != null)
			{
				timer1.Enabled = false;
				timer1.Stop();
				timer1.Dispose();
				timer1 = null;
			}

			this.Close();
		}
	}
}