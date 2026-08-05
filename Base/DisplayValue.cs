#region Header Block
/// <classname>DisplayValue</classname> 
/// <description>An object to store the displayed value and actual value of an item.</description> 
/// <filename>DisplayValue.cs</filename> 
/// <authors>Tay Yu Hui</authors> 
#endregion

using System.Reflection;

namespace ste.pa.pamanager
{
	/// <summary>
	/// Object to store the displayed value and actual value of an item
	/// </summary>
	public struct DisplayValue
	{
		/// <summary>This is a constructor to create a new DisplayValue object, containing the displayed and actual value of an item.</summary>
		/// <param name="displayItem">Contains the string to be displayed.</param>
		/// <param name="valueItem">Contains the Actual Value linked to the string that is displayed.</param>
		public DisplayValue(string displayItem, string valueItem)
			: this()
		{
			string fileName = MethodBase.GetCurrentMethod().DeclaringType.Name;

			if (displayItem == null)
			{
				Program.WriteEventLog("[ERROR] displayItem is null!", fileName + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return;
			}

			if (valueItem == null)
			{
				Program.WriteEventLog("[ERROR] valueItem is null!", fileName + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return;
			}

			this.displayItem = displayItem;
			this.valueItem = valueItem;
		}

		/// <summary>
		/// Returns the display value portion of the DisplayValue object.
		/// </summary>
		public string displayItem
		{
			get;
			private set;
		}

		/// <summary>
		/// Returns the actual value portion of the DisplayValue object.
		/// </summary>
		public string valueItem
		{
			get;
			private set;
		}
	}
}
