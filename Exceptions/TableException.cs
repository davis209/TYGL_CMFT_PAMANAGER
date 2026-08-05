#region Header Block
/// <classname>TableException</classname>
/// <description>Exception thrown when there is a parsing error in the Packet</description>
/// <filename>TableException.cs</filename>
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
	/// Exception thrown when there is a parsing error in the Packet
	/// </summary>
	[Serializable]
	public class TableException : System.Exception
	{
		/// <summary>
		/// Basic Table Parsing Exception
		/// </summary>	
		public TableException(string tableNo)
		{
			// Nothing to validate : tableNo
			if (tableNo == "")
			{
				return;
			}
		}
	}
}
