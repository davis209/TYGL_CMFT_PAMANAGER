#region Header Block
/// <classname>LoadingTask</classname>
/// <description>Worker threads that run the Loading Form when application is launched.
/// </description>
/// <filename>LoadingTask.cs</filename>
/// <authors>
/// Nah Guo Yuan
/// </authors>
/// <version>00.01.00.00 29-JUL-2010</version>
/// <changes>Not applicable</changes>
/// <todo>Not applicable</todo>
/// <copyright>Copyright @ 2006-2010 Singapore Technologies Electronics Ltd</copyright>
#endregion

using System;						// Contains classes that allow you to match URIs against URI Templates and groups of URI Templates
using System.Threading;				// Provides classes and interfaces that enable multithreaded programming
using System.Windows.Forms;			// Contains classes for creating Windows-based applications
////using ste.scada.common;				// Contains classes for creating Windows-based applications
using ste.common.exceptions;
using ste.common.util;
using ste.common.Forms;

namespace ste.pa.pamanager
{
	/// <summary>
	/// Worker threads that run the Loading Form when application is launched.
	/// </summary>
	class LoadingTask
	{
		private int screenXCoordinate_ = 0;
		private int screenYCoordinate_ = 0;
		private Thread progressThread_ = null;
		private LoadingForm progressDisplay_ = null;

		/// <summary>
		/// The LoadingTask constructor initializes a new thread and runs it.
		/// </summary>
		public LoadingTask(int screenXCoordinate, int screenYCoordinate)
		{
			screenXCoordinate_ = screenXCoordinate;
			screenYCoordinate_ = screenYCoordinate;

			progressThread_ = new Thread(new ThreadStart(Worker));
			progressThread_.IsBackground = true;
			progressThread_.Name = "startUpProgressBar_";
			progressThread_.SetApartmentState(ApartmentState.STA);
			progressThread_.Start();
		}

		/// <summary>
		/// This method assigns a thread to the application and runs it.
		/// </summary>
		private void Worker()
		{
			try
			{
				progressDisplay_ = new LoadingForm();
				FormPosition.setFormLocation(progressDisplay_, screenXCoordinate_, screenYCoordinate_);
				Application.Run(progressDisplay_);
			}
			catch (ThreadAbortException)
			{
			}
		}

		/// <summary>
		/// The cleanUp method is for freeing up resource that was held by the class during runtime.
		/// </summary>
		public void CleanUp()
		{
			if (progressDisplay_ != null)
			{
				try
				{
					progressDisplay_.Invoke(new EventHandler(progressDisplay_.CleanUp));
					progressDisplay_.Dispose();
					progressDisplay_ = null;
				}
				catch (Exception)
				{
				}
			}

			if (progressThread_ != null && progressThread_.ThreadState == ThreadState.Background)
			{
				progressThread_.Abort();
				progressThread_ = null;
			}
		}
	}
}
