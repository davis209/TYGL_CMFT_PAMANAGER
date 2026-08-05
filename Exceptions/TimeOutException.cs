#region Header Block
/// <classname>TimeOutException</classname>
/// <description>Exception thrown when a timeout occurs</description>
/// <filename>TimeOutException.cs</filename>
/// <author>Tay Yu Hui</author>
///	<version>00.03.00.00</version>
///	<changes>Not applicable</changes>
///	<todo>Not applicable</todo>
///	<copyright>Copyright @ 2006-2010 Singapore Technologies Electronics Ltd</copyright>
#endregion

using System;	// Contains fundamental classes and base classes that define commonly used value and reference data types, events and event handlers, interfaces, attributes, and processing exceptions.

namespace ste.pa.pamanager
{
	/// <summary>
	/// Exception thrown when a timeout occurs
	/// </summary>
	[Serializable]
	public class TimeOutException : System.Exception
	{
		/// <summary>
		/// Timeout while waiting for data from PAS
		/// </summary>
		public TimeOutException(string tableNo)
			: base(tableNo)
		{
			// Nothing to validate : tableNo
			if (tableNo == "")
			{
				return;
			}
		}
	}

}