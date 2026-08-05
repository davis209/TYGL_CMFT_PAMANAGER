#region Header Block
/// <classname>SocketNotConnException</classname>
/// <description>Exception thrown when socket not connected</description>
/// <filename>SocketNotConnException.cs</filename>
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
	/// Exception thrown when socket not connected
	/// </summary>
	[Serializable]
	class SocketNotConnException : Exception
	{
		/// <summary>
		/// Basic Socket Not Connected Exception
		/// </summary>	
		public SocketNotConnException()
		{
		}
	}
}
