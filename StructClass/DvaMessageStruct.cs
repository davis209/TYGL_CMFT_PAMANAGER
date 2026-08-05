namespace ste.pa.pamanager
{
	/// <summary>
	/// Object to store DVA Message related attributes
	/// </summary>
	public class DvaMessageStruct
	{
		/// <summary>
		/// Message number
		/// </summary>
		public int msgNo = 0;

		/// <summary>
		/// Message Label
		/// </summary>
		public string msgLabel = "";

		/// <summary>
		/// Message Type (e.g. Normal, Emergency, None)
		/// </summary>
		public EnumType.MessageType msgType = EnumType.MessageType.None;

		/// <summary>
		/// Default Constructor
		/// </summary>
		public DvaMessageStruct()
		{
			msgNo = 0;
			msgLabel = "";
			msgType = EnumType.MessageType.None;
		}

		///// <summary>
		///// Override the default Dva Message Sequence ToString method
		///// </summary>
		///// <returns>The customized string to be displayed</returns>
		//
		//public override string ToString()
		//{
		//    return "Msg number: " + msgNo + 
		//            " Label: " + msgLabel + 
		//            " Type: " + msgType;
		//}
	}
}
