using System;					// Contain all other namespaces as subordinates
using System.Reflection;		// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. 
//using Oracle.DataAccess.Client; // Provide Oracle Data Provider object which expose inherited, provider-specific, interface implementations of methods and properties
////using ste.scada.common;			// Provide commonly used classes for the creation of MFT  
using ste.common.exceptions;
using ste.common.util;
using System.Collections.Generic;
using System.Data;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Class to store all common user functions used in PA Manager
    /// </summary>
    public static class UserFunctions
    {
        private static string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;

        /// <summary>
        /// Static constructor 
        /// </summary>
        static UserFunctions()
        {
        }

        /// <summary>
        /// Check Table 590, for the status of the PTT button
        /// </summary>
        /// <param name="pttSourceCodeNo">Value of the source code of the paging console (LIST 50)</param>
        /// <returns>bool: true if ppt button is pressed, otherwise false</returns>
        public static bool PTT_Button_Pressed(int pttSourceCodeNo)
        {
            // Nothing to validate : pttSourceCodeNo
            if (pttSourceCodeNo <= 0 || pttSourceCodeNo > 16)
            {
                Program.WriteEventLog("[ERROR] PTT Source code error!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                return false;
            }

            int tableNo = 590;
            int tableSize = 2;

            byte[] pttPressStatus = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);
            bool[] pttBoolArray = DataManipulation.ByteToBoolArray(pttPressStatus);

            return pttBoolArray[pttSourceCodeNo - 1];

        }

        #region AdHoc_recording
        /// <summary>
        /// 604 W - Update PAS the new type of the selected adhoc msg index
        /// </summary>
        /// <param name="newMsgType">Message Type</param>
        /// <param name="adhocMsgNo">range:1 to 6, number of adhoc message to be updated</param>
        /// <param name="reqStatus">Status of the request</param>
        /// <param name="reqStatusLocal">Localized status of the request</param>
        /// <returns>bool: true if the Adhoc message is successfully updated, else false.</returns>
        public static bool Adhoc_Update_MsgType(EnumType.MessageType newMsgType, int adhocMsgNo, out string reqStatus, out string reqStatusLocal)
        {
            // Nothing to validate : newMsgType, adhocMsgNo, reqStatus

            // Will read Table 604 first, and update Table 604 only when the message type of the 
            // adhoc message is different.
            int currMsgType_ = 0;
            int tableNo = 604;
            int tableSize = Program.NBADHOC;
            int packetSize = tableSize + Program.WRITE_HEADER_SIZE;
            bool updateFlag = false;
            byte[] receivedData = new byte[tableSize];

            reqStatus = "";
            reqStatusLocal = "";

            receivedData = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);
            currMsgType_ = Convert.ToInt16(receivedData[adhocMsgNo - 1]);

            Program.WriteDebugEventLog("[DEBUG] adhocMsgNo: [" + adhocMsgNo + "] Current msg type in database: [" + currMsgType_ + "], new msg type: [" + (int)newMsgType + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            // Update selected adhoc to new message type if they doesn't match
            if ((int)newMsgType != currMsgType_)
            {
                receivedData[adhocMsgNo - 1] = Convert.ToByte((int)newMsgType);

                byte[] receivedPacket = PaFunctions.Adhoc_Update_DVA_MsgType(receivedData);

                reqStatus = Program.returnPacketError(receivedPacket[3], "en-US");
                reqStatusLocal = Program.returnPacketError(receivedPacket[3], "zh-TW");

                if (receivedPacket[3] == (int)EnumType.PacketError.PAS_ERR_OK)
                {
                    updateFlag = true;
                }
            }
            else
            {
                reqStatus = "[INFO] No update to PAS required, as there is no change to Adhoc messages.";
                Program.WriteEventLog(reqStatus, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                updateFlag = true;
            }

            return updateFlag;
        }

        #endregion

        #region General Functions
        /// <summary>
        /// Method to get interested Table from PAS. (exclude header)
        /// </summary>
        /// <param name="tableNo">Requested table number</param>
        /// <param name="tableSize">Size of requested table</param>
        /// <returns>receivedData (exclude header)</returns>
        /// <exception cref="TimeOutException">Throws Timeout exception if there is no reply from PAS</exception>
        /// <exception cref="TableException">Throws Table Exception if received table is empty</exception>
        public static byte[] TableRequest_ExcludeHeader(int tableNo, int tableSize)
        {
            // Nothing to validate : tableNo, tableSize
            int packetSize = tableSize + Program.WRITE_HEADER_SIZE;
            byte[] receivedDataNoHeader = new byte[tableSize];

            try
            {
                byte[] receivedData = PaFunctions.TableRequest(tableNo, packetSize);

                int msbValue = Convert.ToInt16(receivedData[3]);
                int lsbValue = Convert.ToInt16(receivedData[4]);
                int recTableSize = Util.Byte_To_Int(msbValue, lsbValue);

                if (recTableSize == 0)
                {
                    Program.WriteEventLog("[ERROR] Read request failure on Table [" + tableNo + "]!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    throw new TableException(tableNo.ToString());
                }
                else
                {
                    if (recTableSize + Program.WRITE_HEADER_SIZE == receivedData.Length)
                    {
                        try
                        {
                            Program.WriteDebugEventLog("[DEBUG] Returning packet.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                            receivedDataNoHeader = DataManipulation.byteTransferExcludeHeader(tableSize, receivedData);
                        }
                        catch (Exception ex)
                        {
                            Program.WriteEventLog("[ERROR] Cannot parse received bytes! Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                            throw new TableException(tableNo.ToString());
                        }
                    }
                    else
                    {
                        Program.WriteEventLog("[ERROR] Packet size mismatch!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                        throw new TableException(tableNo.ToString());
                    }
                }

            }
            catch (TimeOutException ex)
            {
                Program.WriteEventLog("[ERROR] [TimeOutException] Table " + tableNo + " from PAS. Exception: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw;
            }
            catch (Exception ex)
            {
                // Will also catch the TableException and rethrow it
                Program.WriteEventLog("[ERROR] Error in parsing Table " + tableNo + " from PAS. Exception: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw new TableException(tableNo.ToString());
            }

            return receivedDataNoHeader;
        }

        /// <summary>
        /// 200 / 1200 R - Check the Broadcast Status of the selected Zones
        /// </summary>
        /// <param name="definedSc">Defined source code to be check</param>
        /// <param name="selZoneBits">Selected zones to be checked</param>
        /// <returns>broadcast state of all zones</returns>
        public static EnumType.EZoneBcState[] Check_ZoneBcStatus(int definedSc, bool[] selZoneBits)
        {
            // Nothing to validate : definedSc, selZoneBits, bcInProgress
            EnumType.EZoneBcState[] zoneBcState = null;

            try
            {
                if (Program.occMode)
                {
                    // Read Table 1200
                    int tableNo = 1200;
                    int elementSize = 2;
                    int tableSize = Program.MAXSTNID * Program.MAXZONEID * elementSize;
                    byte[] zoneStatus = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);

                    zoneBcState = new EnumType.EZoneBcState[Program.MAXZONEID * Program.MAXSTNID];

                    for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
                    {
                        for (int z = 0; z < Program.MAXZONEID; z++)
                        {
                            int occZoneIndex = (stnIndex * Program.MAXZONEID) + z;

                            // Check only selected zone
                            if (selZoneBits[occZoneIndex])
                            {
                                int zoneOffset = occZoneIndex * 2;

                                // Zone's SourceCode
                                int zoneSc = Convert.ToInt32(zoneStatus[zoneOffset]);

                                // Zone's Amplifier Status
                                int zoneAmpState = Convert.ToInt32(zoneStatus[zoneOffset + 1]);

                                if (zoneAmpState > 127)
                                {
                                    zoneBcState[occZoneIndex] = EnumType.EZoneBcState.Failed;
                                }
                                else if (zoneSc == 0)
                                {
                                    zoneBcState[occZoneIndex] = EnumType.EZoneBcState.Aborted;
                                }
                                else if (zoneSc != definedSc)
                                {
                                    zoneBcState[occZoneIndex] = EnumType.EZoneBcState.Overridden;
                                }
                                else
                                {
                                    zoneBcState[occZoneIndex] = EnumType.EZoneBcState.Normal;
                                }
                            }
                            else
                            {
                                // Unselected zones will have status normal
                                zoneBcState[occZoneIndex] = EnumType.EZoneBcState.Normal;
                            }

                        }

                    }
                }
                else
                {
                    // Read Table 200
                    int tableNo = 200;
                    int elementSize = 2;
                    int tableSize = Program.MAXZONEID * elementSize;
                    byte[] zoneStatus = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);

                    zoneBcState = new EnumType.EZoneBcState[Program.MAXZONEID];

                    for (int zoneIndex = 0; zoneIndex < Program.MAXZONEID; zoneIndex++)
                    {
                        // Check only selected zone
                        if (selZoneBits[zoneIndex])
                        {
                            int zoneOffset = zoneIndex * 2;

                            // Zone's SourceCode
                            int zoneSc = Convert.ToInt32(zoneStatus[zoneOffset]);

                            // Zone's Amplifier Status
                            int zoneAmpState = Convert.ToInt32(zoneStatus[zoneOffset + 1]);

                            if (zoneAmpState > 127)
                            {
                                zoneBcState[zoneIndex] = EnumType.EZoneBcState.Failed;
                            }
                            else if (zoneSc == 0)
                            {
                                zoneBcState[zoneIndex] = EnumType.EZoneBcState.Aborted;
                            }
                            else if (zoneSc != definedSc)
                            {
                                zoneBcState[zoneIndex] = EnumType.EZoneBcState.Overridden;
                            }
                            else
                            {
                                zoneBcState[zoneIndex] = EnumType.EZoneBcState.Normal;
                            }
                        }
                        else
                        {
                            // Unselected zones will have status normal
                            zoneBcState[zoneIndex] = EnumType.EZoneBcState.Normal;
                        }

                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return zoneBcState;
        }
        #endregion

        #region Flag Table
        /// <summary>
        /// Set the selected flag index to false and update PAS
        /// </summary>
        /// <param name="flagNumber">Flag number to be cleared</param>
        public static void clearSelectedFlag(EnumType.FlagIndex flagNumber)
        {
            // Nothing to validate : flagNumber
            try
            {
                int flagIndex = (int)flagNumber * 2 - 2;
                bool[] flagData = DataObject.Read_Flag_Data();

                flagData[flagIndex] = false;

                byte[] receivedPacket = PaFunctions.ModificationFlags(flagData);

                #region Problems might happen when 2 or more table 560 writes occurs, as the returned packet will overwrite each other in the dictionary
                if (receivedPacket != null)
                {
                    if (receivedPacket[3] == 0)
                    {
                        // Successful
                        PaFunctions.UpdateFlagTable();

                        Program.WriteEventLog("[INFO] [FLAG] Flag " + flagNumber + " cleared!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    }
                    else
                    {
                        string errorInfo = Program.returnPacketError(receivedPacket[3], "en-US");
                        Program.WriteEventLog("[ERROR] [FLAG] Packet returns error: " + errorInfo, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    }
                }
                else
                {
                    Program.WriteEventLog("[ERROR] [FLAG] Packet is empty!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                }
                #endregion
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [FLAG] Fail to clear [" + flagNumber + "] Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }
        #endregion

        #region DVA Broadcast
        /// <summary>
        /// Read Table 303/1303 for DVA Message Sequence Description
        /// </summary>
        /// <returns>Byte Array containing details of DVA Broadcast</returns>
        public static byte[] Read_DVA_Message_Sequence_Description()
        {
            int tableNo = 0;
            int tableSize = 0;
            int packetSize = 0;
            byte[] receivedData = null;

            if (Program.occMode)
            {
                tableNo = 1303;
                tableSize = (22 + Program.MAXSTNID * Program.MAXZONEID / Program.BITS_PER_BYTE) * Program.MAXMSGSEQ;
                packetSize = tableSize + Program.WRITE_HEADER_SIZE;
            }
            else
            {
                tableNo = 303;
                tableSize = (22 + Program.MAXZONEID / Program.BITS_PER_BYTE) * Program.MAXMSGSEQ;
                packetSize = tableSize + Program.WRITE_HEADER_SIZE;
            }

            receivedData = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);

            return receivedData;
        }

        /// <summary>
        /// DVA - 350, 301/1301 - Send a request for DVA Cyclic Broadcast to PAS
        /// </summary>
        /// <param name="dvaBcSettings">Contains the DVA broadcast information</param>
        /// <returns>bool: true if the Cyclic broadcast request is successful, otherwise false</returns>
        public static bool Send_PAS_DVA_Cyclic_Bc(DvaMessageSequenceStruct dvaBcSettings)
        {
            // Nothing to validate : dvaBcSettings
            bool requestStatus = false;

            try
            {
                // Zones operator selected for broadcast use
                bool[] selCycleZones = null;
                byte[] Reply_350W = null;

                Program.WriteEventLog("[INFO] Cyclic Broadcast ->" + dvaBcSettings.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

                if (!Program.occMode)
                {
                    selCycleZones = (bool[])DataObject.Selected_Station_Zone.Clone();
                }
                else
                {
                    selCycleZones = (bool[])DataObject.Selected_OCC_Zone.Clone();
                }

                dvaBcSettings.zoneCoverage = DataManipulation.BoolToByteArray(selCycleZones);

                // 350 W - Define sequence message to be broadcast
                Reply_350W = PaFunctions.Message_Sequence_Specification(dvaBcSettings.msgSeqNo, dvaBcSettings.chimeFlag, dvaBcSettings.dvaMessage, dvaBcSettings.dwellTime);

                // 301 W -  Start request to broadcast mesasge, and define the time and coverage
                if (Util.PAS_Timeout(Reply_350W))
                {
                    // Error in submitting request for dva cyclic broadcast
                    string[] eventParam = new string[] { "", "", Localization.GetEventMessage("EVT_PAS_ERROR", "en-US") };
                    string[] eventParamLocal = new string[] { "", "", Localization.GetEventMessage("EVT_PAS_ERROR", "zh-TW") };
                    Program.logEvent(EnumType.PaEvent.PaDvaCyclicBroadcastFail, eventParam, eventParamLocal);
                }
                else
                {
                    if (Reply_350W[3] == 0)
                    {
                        // No error in sending request to PAS
                        byte[] Reply_301W = PaFunctions.Message_Sequence_Broadcast_Request(dvaBcSettings);

                        if (Util.PAS_Timeout(Reply_301W))
                        {
                            string[] eventParam = new string[] { "", "", Localization.GetEventMessage("EVT_PAS_ERROR", "en-US") };
                            string[] eventParamLocal = new string[] { "", "", Localization.GetEventMessage("EVT_PAS_ERROR", "zh-TW") };
                            Program.logEvent(EnumType.PaEvent.PaDvaCyclicBroadcastFail, eventParam, eventParamLocal);
                        }
                        else
                        {
                            if (Reply_301W[3] == 0)
                            {
                                string[] eventParam = null;
                                string[] eventParamLocal = null;
                                requestStatus = true;

                                string chimeStatus = "";
                                string chimeStatusLocal = "";
                                string msgIdStr = "";

                                if (dvaBcSettings.chimeFlag == 1)
                                {
                                    chimeStatus = Localization.GetEventMessage("EVT_LB_HAVE", "en-US");
                                    chimeStatusLocal = Localization.GetEventMessage("EVT_LB_HAVE", "zh-TW");
                                }
                                else
                                {
                                    chimeStatus = Localization.GetEventMessage("EVT_LB_NO_HAVE", "en-US");
                                    chimeStatusLocal = Localization.GetEventMessage("EVT_LB_NO_HAVE", "zh-TW");
                                }

                                msgIdStr = DataManipulation.getMsgIdStr(dvaBcSettings);

                                // DVA Cyclic Broadcast Request succesfully submitted
                                if (Program.occMode)
                                {
                                    //{{OPERATOR}} @ {{MACHINE}} submitted a DVA Broadcast request. Station {{STNNO}}, Zone: {{ZONEID}}, From {{STARTTIME}} to {{ENDTIME}}, Chime: {{CHIME}}, MSG: {{MSGID}}, PERIOD: {{PERIOD}}, DWELL: {{DWELL}}
                                    for (int i = 0; i < Program.MAXSTNID; i++)
                                    {
                                        if (Program.stnNoToStnNameDic_.ContainsKey(i + 1))
                                        {
                                            string stnIdStr = Program.stnNoToStnNameDic_[i + 1];
                                            string zoneIdString = DataManipulation.Zone_BoolToString(selCycleZones, i);

                                            if (zoneIdString != "")
                                            {
                                                eventParam = new string[] { "", "", stnIdStr, zoneIdString, dvaBcSettings.startTime.ToString(), dvaBcSettings.stopTime.ToString(), chimeStatus, msgIdStr, dvaBcSettings.periodSecs.ToString(), dvaBcSettings.dwellTime.ToString() };
                                                eventParamLocal = new string[] { "", "", stnIdStr, zoneIdString, dvaBcSettings.startTime.ToString(), dvaBcSettings.stopTime.ToString(), chimeStatusLocal, msgIdStr, dvaBcSettings.periodSecs.ToString(), dvaBcSettings.dwellTime.ToString() };

                                                Program.logEvent(EnumType.PaEvent.PaDvaCyclicBroadcast, eventParam, eventParamLocal);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    string stnIdStr = Translater.locId_to_locationNo(Program.profileLocIndex);

                                    eventParam = new string[] { "", "", stnIdStr, DataManipulation.Zone_BoolToString(selCycleZones, 0), dvaBcSettings.startTime.ToString(), dvaBcSettings.stopTime.ToString(), chimeStatus, msgIdStr, dvaBcSettings.periodSecs.ToString(), dvaBcSettings.dwellTime.ToString() };
                                    eventParamLocal = new string[] { "", "", stnIdStr, DataManipulation.Zone_BoolToString(selCycleZones, 0), dvaBcSettings.startTime.ToString(), dvaBcSettings.stopTime.ToString(), chimeStatusLocal, msgIdStr, dvaBcSettings.periodSecs.ToString(), dvaBcSettings.dwellTime.ToString() };

                                    Program.logEvent(EnumType.PaEvent.PaDvaCyclicBroadcast, eventParam, eventParamLocal);
                                }
                            }
                            else
                            {
                                string[] eventParam = new string[] { "", "", Program.returnPacketError(Reply_301W[3], "en-US") };
                                string[] eventParamLocal = new string[] { "", "", Program.returnPacketError(Reply_301W[3], "zh-TW") };
                                Program.logEvent(EnumType.PaEvent.PaDvaCyclicBroadcastFail, eventParam, eventParamLocal);
                            }
                        }
                    }
                    else
                    {
                        string[] eventParam = new string[] { "", "", Program.returnPacketError(Reply_350W[3], "en-US") };
                        string[] eventParamLocal = new string[] { "", "", Program.returnPacketError(Reply_350W[3], "zh-TW") };
                        Program.logEvent(EnumType.PaEvent.PaDvaCyclicBroadcastFail, eventParam, eventParamLocal);
                    }

                }
            }
            catch (NoOracleDBException ex)
            {
                Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return requestStatus;
        }

        /// <summary>
        /// DVA - 301/1301 - Abort Selected Message Sequence Slot
        /// </summary>
        /// <param name="msgSeqNo">Number of message sequence to be aborted</param>
        /// <returns>bool: true if msg sequence successfully aborted, otherwise false</returns>
        public static bool DVA_Abort_MsgSeq(int msgSeqNo)
        {
            // Nothing to validate : msgSeqNo
            try
            {
                byte[] receivedData = null;
                DvaMessageSequenceStruct dvaMsgSeq = new DvaMessageSequenceStruct();

                dvaMsgSeq.msgSeqNo = msgSeqNo;
                dvaMsgSeq.announceId = 0;
                dvaMsgSeq.periodSecs = 0;
                dvaMsgSeq.eventTrigger = 0;

                receivedData = PaFunctions.Message_Sequence_Broadcast_Request(dvaMsgSeq);

                if (Convert.ToInt16(receivedData[3]) == 0)
                {
                    Program.WriteEventLog("[INFO] DVA slot " + msgSeqNo + " aborted.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return true;
                }
                else
                {
                    Program.WriteEventLog("[INFO] DVA slot " + msgSeqNo + " NOT aborted.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return false;
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                return false;
            }
        }

        /// <summary>
        /// DVA - 301/1301 - Method to auto abort all message sequences with status of 8 or 9.  
        /// </summary>
        public static void DVA_autoClear_MsgSeq()
        {
            //Read Table 303/1303
            //If Stop Time of Message sequence is over, auto clear it. - WRONG as stop time over doesn't mean broadcast ended

            Program.WriteDebugEventLog("[DEBUG] Trying to auto clear message sequence.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            // Read table 302
            int[] msgSeqStatus = getMsgSlotStatus();

            //if status is 8 or 9, means broadcast has ended. Auto clear the slot
            if (msgSeqStatus != null)
            {
                for (int msgSeqIndex = 0; msgSeqIndex < Program.MAXMSGSEQ; msgSeqIndex++)
                {
                    if (msgSeqStatus[msgSeqIndex] == 8 || msgSeqStatus[msgSeqIndex] == 9)
                    {
                        if (DVA_Abort_MsgSeq(msgSeqIndex + 1))
                        {
                            Program.WriteEventLog("[INFO] DVA - Message Sequence Slot No " + (msgSeqIndex + 1) + " auto cleared successful.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                        }
                        else
                        {
                            Program.WriteEventLog("[ERROR] DVA - Message Sequence Slot No " + (msgSeqIndex + 1) + " auto cleared FAILED!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                        }

                        //UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag302);
                    }
                }
            }
            else
            {
                Program.WriteEventLog("[ERROR] DVA - Message Sequence Slots auto cleared FAILED!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Check which of the "Reserved" slots for Immediate Broadcast is free, and return the slot number.
        /// </summary>
        /// <param name="freeSlotNo">Returns the free message slot number</param>
        /// <returns>bool: true if there is a free slot, otherwise false</returns>
        public static bool DVA_Immediate_MsgSeq_Free(out int freeSlotNo)
        {
            // Nothing to validate : freeSlotNo
            // Check slot 1,2
            byte[] receivedData = Read_DVA_Message_Sequence_Description();

            DvaMessageSequenceStruct[] msgSeqSettingsArray = Parse_DVA_Msg_Seq_Desc(receivedData);

            if (msgSeqSettingsArray[0].dvaMessage[0] == 0)
            {
                freeSlotNo = 1;
                return true;
            }
            else if (msgSeqSettingsArray[1].dvaMessage[0] == 0)
            {
                freeSlotNo = 2;
                return true;
            }
            else
            {
                // Both slots not free
                freeSlotNo = 0;
                return false;
            }
        }

        /// <summary>
        /// Check which of the "Reserved" slots for Cyclical Broadcast is free, and return the slot number.
        /// </summary>
        /// <param name="freeSlotNo">returns Free Slot Number</param>
        /// <returns>bool: true if there is a free slot, otherwise false</returns>
        public static bool DVA_Cyclical_MsgSeq_Free(out int freeSlotNo)
        {
            // Nothing to validate : freeSlotNo
            // Check slot 3 - 6
            byte[] receivedData = Read_DVA_Message_Sequence_Description();
            DvaMessageSequenceStruct[] msgSeqSettingsArray = Parse_DVA_Msg_Seq_Desc(receivedData);

            for (int i = 2; i < Program.MAXMSGSEQ; i++)
            {
                if (msgSeqSettingsArray[i].dvaMessage[0] == 0)
                {
                    freeSlotNo = i + 1;
                    return true;
                }
            }

            freeSlotNo = 0;
            return false;
        }

        /// <summary>
        /// DVA - Table 302 - Get the status of all Message Sequence Slot
        /// </summary>
        /// <returns>Status of all  Message Sequence Slot</returns>
        public static int[] getMsgSlotStatus()
        {
            int tableNo = 302;
            int tableSize = Program.MAXMSGSEQ * 3;
            int packetSize = tableSize + Program.WRITE_HEADER_SIZE;
            //byte[] receivedPacket = new byte[packetSize];
            byte[] receivedData = new byte[tableSize];
            int[] msgSeqStatus = null;

            try
            {
                Program.WriteEventLog("[INFO] 302 R - Get the status of all Message Sequence Slot.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                receivedData = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);

                msgSeqStatus = new int[Program.MAXMSGSEQ];

                for (int i = 0; i < Program.MAXMSGSEQ; i++)
                {
                    int statusOffset = (i * 3) + 2;
                    msgSeqStatus[i] = receivedData[statusOffset];
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return msgSeqStatus;
        }

        /// <summary>
        /// Table 302 - Check status of given message sequence
        /// </summary>
        /// <param name="msgSeqNo">Range 1 to 6</param>
        /// <returns>Status of the Broadcast</returns>
        /// <exception cref="Exception">Throws generic exception</exception>
        public static EnumType.DvaBcStatus DVA_Check_Broadcast_Status(int msgSeqNo)
        {
            // Nothing to validate : msgSeqNo
            int msgBcStatusInt = 0;

            try
            {
                int tableNo = 302;
                int tableSize = Program.MAXMSGSEQ * 3;
                int elementSize = 3;
                int packetSize = tableSize + Program.WRITE_HEADER_SIZE;
                EnumType.DvaBcStatus msgBcStatus = 0;

                byte[] receivedData = new byte[tableSize];

                Program.WriteEventLog("[INFO] Checking Broadcast status on msg slot " + msgSeqNo, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                receivedData = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);

                //To view the broadcast status of a selected msg seq, return the corresponding status
                msgBcStatusInt = Convert.ToInt32(receivedData[msgSeqNo * elementSize - 1]);
                msgBcStatus = (EnumType.DvaBcStatus)msgBcStatusInt;

                return msgBcStatus;
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] BC Status: " + msgBcStatusInt + " - " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw;
            }
        }

        /// <summary>
        /// Update dvaMsgSeq details into database
        /// </summary>
        /// <param name="dvaMsgSeq">Details of DVA Message Sequence</param>
        public static void Update_PAS_DVA_MSG_SEQ(DvaMessageSequenceStruct dvaMsgSeq)
        {
            //// Nothing to validate : dvaMsgSeq
            //OracleTransaction oraTran = null;
            //dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            //try
            //{
            //	//Insert into Database
            //	String[] queryList = null;

            //	if (Program.occMode)
            //	{
            //		queryList = new String[3 + DataObject.occCoverageSize];
            //	}
            //	else
            //	{
            //		queryList = new String[3 + DataObject.stnCoverageSize];
            //	}

            //	queryList[0] = " DELETE FROM PA_DVA_MSG_SEQ_ZONES WHERE MSG_SEQ_ID = " + dvaMsgSeq.msgSeqNo;
            //	queryList[1] = " DELETE FROM PA_DVA_MSG_SEQ WHERE MSG_SEQ_ID = " + dvaMsgSeq.msgSeqNo;
            //	queryList[2] = " INSERT INTO PA_DVA_MSG_SEQ ( MSG_SEQ_ID, CHIME, DVA_MSG1, DVA_MSG2, DVA_MSG3, DVA_MSG4, " +
            //						" DWELL_TIME, PERIOD, START_TIME, STOP_TIME, EVENT_TRIGGERED )" +
            //						" VALUES (" + dvaMsgSeq.msgSeqNo + ", " +
            //						dvaMsgSeq.chimeFlag + ", " +
            //						dvaMsgSeq.dvaMessage[0] + ", " +
            //						dvaMsgSeq.dvaMessage[1] + ", " +
            //						dvaMsgSeq.dvaMessage[2] + ", " +
            //						dvaMsgSeq.dvaMessage[3] + ", " +
            //						dvaMsgSeq.dwellTime + ", " +
            //						dvaMsgSeq.periodSecs + ", " +
            //						" to_date('" + dvaMsgSeq.startTime.ToString("yyyyMMddHHmmss") + "','YYYYMMDDHH24MISS') , " +
            //						" to_date('" + dvaMsgSeq.stopTime.ToString("yyyyMMddHHmmss") + "','YYYYMMDDHH24MISS') , " +
            //						dvaMsgSeq.eventTrigger +
            //						" )";

            //	for (int i = 3; i < queryList.Length; i++)
            //	{
            //		queryList[i] = " INSERT INTO PA_DVA_MSG_SEQ_ZONES ( MSG_SEQ_ID, BYTE_INDEX, BYTE_VALUE ) " +
            //						" VALUES ( " + dvaMsgSeq.msgSeqNo + ", " +
            //						(i - 3) + ", " +
            //						dvaMsgSeq.zoneCoverage[i - 3] +
            //						" )";
            //	}

            //	oraTran = Program.dbLock.getActiveConnection(ref dbConn).BeginTransaction();

            //	for (int i = 0; i < queryList.Length; i++)
            //	{
            //		int retVal = -1;
            //		Program.dbLock.oracleQuery2(ref retVal, queryList[i], ref dbConn);
            //	}

            //	oraTran.Commit();
            //}
            //catch (NoOracleDBException ex)
            //{
            //	Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            //}
            //catch (Exception ex)
            //{
            //	Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            //	if (oraTran != null)
            //	{
            //		oraTran.Rollback();
            //	}
            //}
            //finally
            //{
            //	if (oraTran != null)
            //	{
            //		oraTran.Dispose();
            //	}
            //}
        }

        /// <summary>
        /// Parse received data from PAS into DvaMessageSequenceStruct array
        /// </summary>
        /// <param name="receivedData">byte[] containing data received from PAS</param>
        /// <returns>The DVA Message Sequence Object</returns>
        public static DvaMessageSequenceStruct[] Parse_DVA_Msg_Seq_Desc(byte[] receivedData)
        {
            // Nothing to validate : receivedData
            DvaMessageSequenceStruct[] dvaMsgSegArray = new DvaMessageSequenceStruct[Program.MAXMSGSEQ];

            // Process slot by slot
            for (int msgSeqIndex = 0; msgSeqIndex < Program.MAXMSGSEQ; msgSeqIndex++)
            {
                int byteOffSet = 0;
                int selBcZoneSize = 0;
                byte[] dtByte = new byte[4];
                DvaMessageSequenceStruct dvaMsgSeq = new DvaMessageSequenceStruct();

                if (Program.occMode)
                {
                    byteOffSet = msgSeqIndex * (22 + DataObject.occCoverageSize);
                    selBcZoneSize = DataObject.occCoverageSize;
                }
                else
                {
                    byteOffSet = msgSeqIndex * (22 + DataObject.stnCoverageSize);
                    selBcZoneSize = DataObject.stnCoverageSize;
                }

                dvaMsgSeq.msgSeqNo = (msgSeqIndex + 1);
                dvaMsgSeq.chimeFlag = receivedData[byteOffSet + 0];

                for (int i = 0; i < Program.MAXDVAPERSEQ; i++)
                {
                    int dvaMsgMsb = receivedData[(byteOffSet) + (i * 2) + 1];
                    int dvaMsgLsb = receivedData[(byteOffSet) + (i * 2) + 2];

                    dvaMsgSeq.dvaMessage[i] = Util.Byte_To_Int(dvaMsgMsb, dvaMsgLsb);
                }

                dvaMsgSeq.dwellTime = Util.Byte_To_Int(receivedData[byteOffSet + 9], receivedData[byteOffSet + 10]);
                dvaMsgSeq.periodSecs = Util.Byte_To_Int(receivedData[byteOffSet + 11], receivedData[byteOffSet + 12]);

                dtByte = DataManipulation.Variable_Byte_Transfer((byteOffSet + 13), 4, receivedData);
                dvaMsgSeq.startTime = Util.Byte_To_DateTime(dtByte);

                dtByte = DataManipulation.Variable_Byte_Transfer((byteOffSet + 17), 4, receivedData);
                dvaMsgSeq.stopTime = Util.Byte_To_DateTime(dtByte);

                dvaMsgSeq.eventTrigger = receivedData[byteOffSet + 21];

                dvaMsgSeq.zoneCoverage = DataManipulation.Variable_Byte_Transfer((byteOffSet + 22), selBcZoneSize, receivedData);

                dvaMsgSegArray[msgSeqIndex] = dvaMsgSeq;
            }

            return dvaMsgSegArray;
        }
        #endregion

        #region Query Database
        /// <summary>
        /// Query Database to get the next Announce ID
        /// </summary>
        /// <returns>Next Announce ID</returns>
        public static int getNextAnnounceId()
        {
            return 0;
            //int retVal = -1;
            //int announceIdCurr = 0;
            //int announceIdNext = 0;
            //OracleDataReader oracleDr = null;

            //try
            //{
            //	dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            //	string localSql = " SELECT ANNOUNCE_ID_1 FROM PA_ANNOUNCE ";
            //	Program.dbLock.oracleQuery2(ref oracleDr, localSql, ref dbConn);

            //	if (oracleDr.Read())
            //	{
            //		if (!oracleDr.IsDBNull(0))
            //		{
            //			announceIdCurr = oracleDr.GetOracleDecimal(0).ToInt32();
            //		}
            //	}

            //	// Increment announceId, loop back if value more than 255
            //	announceIdNext = announceIdCurr + 1;

            //	if (announceIdNext <= Program.MAXSRCID || announceIdNext > 255)
            //	{
            //		announceIdNext = Program.MAXSRCID + 1;
            //	}

            //	Program.WriteEventLog("[INFO] Announce ID, [Current: " + announceIdCurr + "] [Next: " + announceIdNext + "].", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            //	// Delete ANNOUNCE_ID_1
            //	localSql = " DELETE FROM PA_ANNOUNCE ";
            //	Program.dbLock.oracleQuery2(ref retVal, localSql, ref dbConn);

            //	// Insert new ANNOUNCE_ID_1
            //	localSql = " INSERT INTO PA_ANNOUNCE ( ANNOUNCE_ID_1, ANNOUNCE_ID_2, ANNOUNCE_ID_3, ANNOUNCE_ID_4, ANNOUNCE_ID_5, ANNOUNCE_ID_6 ) " +
            //					" VALUES (" + announceIdNext + ", 0 " + ", 0 " + ", 0 " + ", 0 " + ", 0 )";
            //	Program.dbLock.oracleQuery2(ref retVal, localSql, ref dbConn);
            //}
            //catch (NoOracleDBException ex)
            //{
            //	announceIdNext = Program.MAXSRCID + 1;
            //	Program.WriteEventLog("[ERROR] [NoOracleDBException] " + Program.MB_NoDBConn + "Announce ID: " + announceIdNext + " (Next) - " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            //}
            //catch (Exception ex)
            //{
            //	announceIdNext = Program.MAXSRCID + 1;
            //	Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString() + "Announce ID: " + announceIdNext + " (Next)", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            //}

            //return announceIdNext;
        }

        /// <summary>
        /// Query Database to get the Message Label given Message No
        /// </summary>
        /// <param name="dvaMsgNoArray">Integer array containing message numbers</param>
        /// <returns>String array containing all message labels </returns>
        public static string[] Query_MsgNo_To_MsgLabel(int[] dvaMsgNoArray)
        {
            // Nothing to validate : dvaMsgNoArray
            string[] selDvaMsg = new string[Program.MAXDVAPERSEQ];

            for (int i = 0; i < dvaMsgNoArray.Length; i++)
            {
                selDvaMsg[i] = "--";
                dbConnEnum dbConn = dbConnEnum.ErrNoConn;
                string localSql = " SELECT MSG_LABEL " +
                                    " FROM PA_MSG_LABEL WHERE MSG_ID = " + dvaMsgNoArray[i];

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        selDvaMsg[i] = retrievedRow[0].ToString();
                        break;
                    }
                }

            }
            return selDvaMsg;
        }

        public static DvaMessageStatusStruct QueryLatestDvaMessageStatus(int locationId)
        {
            DvaMessageStatusStruct result = null;
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            try
            {
                string localSql = $"select * from  pa_dva_msg_status where location_id={locationId} order by SENT_TIME desc limit 1";

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        result = new DvaMessageStatusStruct();
                        result.PKey = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_PKEY].ToString());
                        result.LocationId = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_LOCAITON_ID].ToString());
                        result.ConsoleId = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_CONSOLE_ID].ToString());
                        result.MsgId = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_MSG_ID].ToString());
                        result.RecycleTime = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_RECYCLE_TIME].ToString());
                        result.IntervalSec = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_INTERVAL_SEC].ToString());
                        result.Zones = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_ZONES].ToString());
                        result.Status = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_STATUS].ToString());
                        result.SeatId = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_SEAT_ID].ToString());
                        result.Language = int.Parse(retrievedRow[DvaMessageStatusStruct.COLUMN_LANGUAGE].ToString());
                        result.Stations = retrievedRow[DvaMessageStatusStruct.COLUMN_STATIONS].ToString();
                        result.MsgVersion = retrievedRow[DvaMessageStatusStruct.COLUMN_MSG_VERSION].ToString();
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return result;
        }

        public static void UpdateDvaMessageStatus(DvaMessageStatusStruct msgStatus)
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            try
            {
                string localSql = $"UPDATE pa_dva_msg_status SET STATUS={msgStatus.Status} WHERE PKEY={msgStatus.PKey};";

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                {
                    Program.WriteEventLog("[ERROR] update pa_dva_msg_status", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return;
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        public static void InsertDvaMessageStatus(DvaMessageStatusStruct msgStatus)
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            try
            {
                string localSql = "INSERT INTO pa_dva_msg_status " +
                               "(LOCATION_ID, CONSOLE_ID, MSG_ID, MSG_VERSION, RECYCLE_TIME, INTERVAL_SEC, ZONES, STATUS, STATIONS, SEAT_ID, LANGUAGE) " +
                               $"VALUES({msgStatus.LocationId}, {msgStatus.ConsoleId}, {msgStatus.MsgId}, {msgStatus.MsgVersion}, {msgStatus.RecycleTime}, {msgStatus.IntervalSec}," +
                               $"{msgStatus.Zones}, {msgStatus.Status}, '{msgStatus.Stations}', {msgStatus.SeatId}, {msgStatus.Language})";
  
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                {
                    Program.WriteEventLog("[ERROR] INSERT INTO pa_dva_msg_status", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return;
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        public static string QueryPreRecordedMessageContent(int msgId, string msgVersion)
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            try
            {
                string localSql = $"SELECT MSG_CONTENT FROM PA_PRERECORDED_MSG WHERE MSG_ID={msgId}  AND VERSION='{msgVersion}'";
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        return retrievedRow[0].ToString();
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return string.Empty;
        }

        public static string QueryPreRecordedMessageLabel(int msgId, string msgVersion)
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            try
            {
                string localSql = $"SELECT MSG_LABEL FROM PA_PRERECORDED_MSG WHERE MSG_ID={msgId} AND VERSION='{msgVersion}'";
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        return retrievedRow[0].ToString();
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return string.Empty;
        }


        #endregion

        /// <summary>
        /// Check the selected Flag Status 
        /// </summary>
        /// <param name="flagNumber">Number of the Flag in the Table 560</param>
        /// <returns>bool: true if the flag is set, otherwise false</returns>
        public static bool checkFlagStatus(EnumType.FlagIndex flagNumber)
		{
			// Nothing to validate : flagNumber
			int flagIndex = (int)flagNumber * 2 - 2;

			return DataObject.Read_Flag_Data()[flagIndex];
		}

		/// <summary>
		/// Table 202 - Check AnnounceId is in the interested SourceCode
		/// </summary>
		/// <param name="sourceCode">List 50 - SourceCode number</param>
		/// <param name="announceId">Announce Id to check</param>
		/// <param name="pasScAnnounceId">returns the announce ID set in the interested sourcecode</param>
		/// <returns>bool: true if the announce Id in that sourcecode matches, otherwise false</returns>
		public static bool Check_Source_AnnouncementID(int sourceCode, int announceId, out int pasScAnnounceId)
		{
			// Nothing to validate : sourceCode, announceId, pasScAnnounceId
			bool announceIdMatch = false;

			int tableNo = 202;
			int elementSize = 1;
			int tableSize = Program.MAXSRCID * elementSize;

			byte[] receivedData = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);
			pasScAnnounceId = Convert.ToInt32(receivedData[sourceCode - 1]);

			if (announceId == pasScAnnounceId)
			{
				//Program.WriteEventLog("[INFO] Source Code matches announce ID.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				announceIdMatch = true;
			}

			return announceIdMatch;
		}

		/// <summary>
		/// Table 202 - Check AnnounceId is in the interested SourceCode
		/// </summary>
		/// <param name="sourceCode">List 50 - SourceCode number</param>
		/// <returns>bool: true if the announce Id in that sourcecode matches, otherwise false</returns>
		public static int Get_Source_AnnouncementID(int sourceCode)
		{
			// Nothing to validate : sourceCode
			int tableNo = 202;
			int elementSize = 1;
			int tableSize = Program.MAXSRCID * elementSize;
			int pasScAnnounceId = -1;

			if (sourceCode < 1 || sourceCode > Program.MAXSRCID)
			{
				Program.WriteEventLog("[ERROR] Invalid sourceCode given: " + sourceCode, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return pasScAnnounceId;
			}

			byte[] receivedData = UserFunctions.TableRequest_ExcludeHeader(tableNo, tableSize);
			pasScAnnounceId = Convert.ToInt32(receivedData[sourceCode - 1]);

			return pasScAnnounceId;
		}

		/// <summary>
		/// Write to Table 630 to abort Adhoc
		/// </summary>
		/// <returns>bool: true if adhoc recording can be stopped, otherwise false</returns>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static bool Adhoc_Stop()
		{
			byte[] abortRecording = new byte[Program.NBADHOC];

			for (int i = 0; i < Program.NBADHOC; i++)
			{
				abortRecording[i] = 0x0;
			}

			if (PaFunctions.TableWrite(630, Program.NBADHOC, abortRecording, MethodInfo.GetCurrentMethod().Name)[3] == 0)
			{
				return true;
			}

			return false;
		}

		/// <summary>
		/// 630 W -Start Adhoc message recording
		/// </summary>
		/// <param name="msgNo">Adhoc message number to record</param>
		/// <returns>byte[] containing the request status</returns>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static byte[] Adhoc_Start(int msgNo)
		{
			// Nothing to validate : msgNo
			int tableSize = 6;
			byte[] recStatus = new byte[tableSize];

			// Update selected adhoc index with recording source (LIST 50)
			recStatus[msgNo - 1] = Convert.ToByte(DataObject.Read_PTT_Source_Code());

			return PaFunctions.TableWrite(630, tableSize, recStatus, MethodInfo.GetCurrentMethod().Name);
		}

	}
}
