#region Header Block
/// <classname>DvaVersion</classname> 
/// <description>Form to display DVA library version in all stations at OCC, Table involved 610, 1610</description> 
/// <filename>DvaVersion.cs</filename> 
/// <authors>Tay Yu Hui</authors> 
/// <version>00.03.00.00</version> 
/// <changes>Nil</changes> 
/// <todo>Nil</todo> 
/// <copyright>Copyright @ 2006-2010 Singapore Technologies Electronics Ltd</copyright>
#endregion

using System;					// Contain all other namespaces as subordinates
using System.Reflection;		// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. 
using System.Windows.Forms;		// Contain classes for creating Window-based applications
//using Oracle.DataAccess.Client;	// Provide Oracle Data Provider object which expose inherited, provider-specific, interface implementations of methods and properties
////using ste.scada.common;			// Provide commonly used classes for the creation of MFT
using ste.common.exceptions;
using ste.common.util;
using System.Data;
using System.Collections.Generic;
using System.IO;

namespace ste.pa.pamanager
{
	/// <summary>
	/// Form to display DVA library version in all stations at OCC, Table involved 610, 1610
	/// </summary>
	public partial class DvaVersion : Form
	{
		private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
		private string sysErr_ = "DVA Version";
		private string dgvStnLocation_ = "Location";
		private string dgvAdhoc1_ = "Ad Hoc 1";
		private string dgvAdhoc2_ = "Ad Hoc 2";
		private string dgvAdhoc3_ = "Ad Hoc 3";
		private string dgvAdhoc4_ = "Ad Hoc 4";
		private string dgvAdhoc5_ = "Ad Hoc 5";
		private string dgvAdhoc6_ = "Ad Hoc 6";
		private string dgvAdhocPreRec_ = "Pre-recorded";

		/// <summary>
		/// This is a constructor to initialize the DvaVersion class and its variables and components
		/// </summary>
		public DvaVersion()
		{
			InitializeComponent();
		}

		/// <summary>
		///  Update the data grid view to show the latest version information
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void DvaVersion_Load(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			int colNo = 8;
			string localSql = "";
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			string[] DvaVersion = new string[colNo];

			try
			{
				localizeForm();
				InitDataGridView_Version();

				dataGridView_Version.Rows.Clear();

				if (Program.occMode)
				{
					// OCC
					localSql = " SELECT SL.NAME AS LOCATION_NO, DVA_ADHOC_1, DVA_ADHOC_2, DVA_ADHOC_3, DVA_ADHOC_4, DVA_ADHOC_5, DVA_ADHOC_6, DVA_ADHOC_PREREC FROM PA_DVA_VERSION PV, LOCATION SL WHERE PV.LOCATION_ID = SL.PKEY ORDER BY LOCATION_NO ASC ";
                    var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                    DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                    if (ds.Tables != null || ds.Tables.Count > 0)
                    {
                        foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                        {
                            for (int i = 0; i < 8; i++)
                            {
                                DvaVersion[i] = retrievedRow[i].ToString();                              
                            }

                            dataGridView_Version.Rows.Add(DvaVersion);
                        }
                    }


					this.Height = 500;
				}
				else
				{
                    // Station
                    string strlocationId = ConfigFile.GetINIDataString("DEFAULT_MFT_SETTING", "DEF_LOCATION_ID", "", 255, Path.Combine(Program.configFolder_, "config.ini"));
                    localSql = " SELECT SL.NAME AS LOCATION_NO, DVA_ADHOC_1, DVA_ADHOC_2, DVA_ADHOC_3, DVA_ADHOC_4, DVA_ADHOC_5, DVA_ADHOC_6, DVA_ADHOC_PREREC FROM PA_DVA_VERSION PV, LOCATION SL WHERE PV.LOCATION_ID = SL.PKEY AND SL.PKEY = " + strlocationId;
                    var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                    DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                    if (ds.Tables != null || ds.Tables.Count > 0)
                    {
                        foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                        {
                            DvaVersion[0] = retrievedRow[0].ToString();

                            for (int i = 1; i < 8; i++)
                            {
                                DvaVersion[i] = retrievedRow[i].ToString();                            
                            }

                            dataGridView_Version.Rows.Add(DvaVersion);
                        }
                    }

					dataGridView_Version.Height = 128;
				}

				this.CenterToParent();

			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_NoDBConn, sysErr_);
				this.Close();
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_FormEx, sysErr_);
				this.Close();
			}

			//this.StartPosition = FormStartPosition.CenterParent;
		}

		/// <summary>
		/// This method sets the variables to its localized equivalent text.
		/// </summary>
		private void localizeForm()
		{
			System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo(Program.defaultLanguage);
			Localization.setCulture(ci);
			Localization.setApplicationName(Program.moduleName);

			if (Localization.configure())
			{
				this.Text = Localization.localize("DVAVERSION_FORM_NAME", this.Text);
				sysErr_ = Localization.localize("DVAVERSION_FORM_NAME", sysErr_);

				dgvStnLocation_ = Localization.localize("DGV_LOC", dgvStnLocation_);
				dgvAdhoc1_ = Localization.localize("DGV_ADHOC1", dgvAdhoc1_);
				dgvAdhoc2_ = Localization.localize("DGV_ADHOC2", dgvAdhoc2_);
				dgvAdhoc3_ = Localization.localize("DGV_ADHOC3", dgvAdhoc3_);
				dgvAdhoc4_ = Localization.localize("DGV_ADHOC4", dgvAdhoc4_);
				dgvAdhoc5_ = Localization.localize("DGV_ADHOC5", dgvAdhoc5_);
				dgvAdhoc6_ = Localization.localize("DGV_ADHOC6", dgvAdhoc6_);
				dgvAdhocPreRec_ = Localization.localize("DGV_ADHOCPREREC", dgvAdhocPreRec_);
			}
		}

		/// <summary>
		/// Initialize version list in data grid view
		/// </summary>
		private void InitDataGridView_Version()
		{
			dataGridView_Version.Columns[0].HeaderText = dgvStnLocation_;
			dataGridView_Version.Columns[1].HeaderText = dgvAdhoc1_;
			dataGridView_Version.Columns[2].HeaderText = dgvAdhoc2_;
			dataGridView_Version.Columns[3].HeaderText = dgvAdhoc3_;
			dataGridView_Version.Columns[4].HeaderText = dgvAdhoc4_;
			dataGridView_Version.Columns[5].HeaderText = dgvAdhoc5_;
			dataGridView_Version.Columns[6].HeaderText = dgvAdhoc6_;
			dataGridView_Version.Columns[7].HeaderText = dgvAdhocPreRec_;
		}
	}

}
