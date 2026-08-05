using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ste.common.util;

namespace ste.pa.pamanager
{
    public static class LocalizeStringsDefintions
    {
        /// <summary>
        /// Message shown when the database cannot be connected
        /// </summary>
        public static string errDBConnection_ = "The program cannot establish a connect to the database, please check your connection!";

        /// <summary>
        ///  Message shown when a generic error occured
        /// </summary>
        public static string errGeneralExcep_ = "An Error has occur, please contact Administrator.";

        /// <summary>
        /// Message when user input blank value
        /// </summary>
        public static string errCannotBlank_ = "cannot be blank!";

        public static string mbPaAlreadyRunning_ = "PA Manager is already running.";

        public static string mbUserNotLogin_ = "You are required to login and launched the application from the MFT.";

        public static string mbLaunchRight_ = "This profile does not have the right to launch SCADA Configurator Manager.";

        public static string mbNoUser_ = "No user specify!";

        public static string mbWrongUser_ = "Wrong user specify!";

        public static string dbErrNoStnZOneDefnFound_ = "Error!! Unable to find any station and zone defintions from PA_STATION_ZONE_LIST_V";

        public static string ERROR_PASERVICE_CONNECTION = "Unable to contact PA service, please contact the system administrator.";
        public static string ERROR_CONFIG_MIX_FILE_AUDIO_FORMAT = "PA Hardware only support broadcasting of a single type of audio type. You cannot mix mp3 format with G.722!";
        public static string ERROR_CONFIG_INCORRECT_AUDIO_FORMAT = "The audio file you have specified is in the wrong language!!";
        public static string ERROR_CONFIG_INCORRECT_FILE_FORMAT = "The type of audio file you have specified is not supported!!";
        public static string ERROR_CONFIG_NO_FILE_SELECTED = "Unable to activate/deactivate this audio message. You have not selected any file !!";
        public static string ERROR_SCHAGENT_CONNECTION = "Unable to connect to Scheduling agent!";
        public static string ERROR_SCHAGENT_ERROR = "Scheduling agent is unable to process request. Please contact your system administrator";
        public static string ERROR_DATABASE_ERROR = "Database errors occurred!";

        public static string ACTION_BROADCAST_FAILED = "無法啓動廣播";
        public static string ACTION_VOLUME_IN_PROGRESS = "音量調整進行中";
        public static string ACTION_VOLUME_CHANGE_SUCCESS = "音量調整成功";
        public static string ACTION_VOLUME_CHANGE_FAILURE = "音量調整失敗";

        public static string PROGRESS_MSG_PLEASE_WAIT = "請稍候，系統正在處理中";

        // ZoneDataGridView definitions
        public static string BROADCAST_STATUS_INVALID = "無法讀取";
        public static string BROADCAST_STATUS_PLAYING = "廣播中";      //ZoneBroadcastInProgress
        public static string BROADCAST_STATUS_INUSE = "使用中";
        public static string BROADCAST_STATUS_DISCONNECTED = "離線狀態";
        public static string BROADCAST_STATUS_IDLE = "閒置區域";
        public static string BROADCAST_STATUS_FAILED = "廣播失敗";      // ZoneBroadcastCompleted
        public static string BROADCAST_STATUS_COMPLETED = "廣播成功";   // ZoneBroadcastFailed
        public static string BROADCAST_STATUS_STARTED = "等待中";       //ZoneBroadcastStarted
        public static string BROADCAST_STATUS_WAITING = "未發送";       //ZoneBroadcastNoBroadcast

        public static string MSG_BROADCAST_STATUS_UNSTARTED = "未播放";
        public static string MSG_BROADCAST_STATUS_PLAYING = "播放中";
        public static string MSG_BROADCAST_STATUS_COMPLETED = "播放完畢";

        public static string PA_ADD_MESSAGE_CONFIRM = "請確認是否要保存這個新紀錄？";
        public static string PA_INSERT_MESSAGE_SUCCESS = "新增成功!";
        public static string PA_INSERT_MESSAGE_FAIL = "新增失敗!";
        public static string PA_EDIT_SUCCESS = "編輯成功！";
        public static string PA_EDIT_FAIL = "編輯成功！";
        public static string PA_EDIT_MESSAGE_CONFIRM = "請確認是否要編輯語音?";
        public static string PA_DELETE_SUCCESS = "成功刪除語音！";
        public static string PA_DELETE_FAIL = "刪除失敗！";
        public static string PA_DELETE_CONFIRM = "請確認是否要刪除語音?";

        public static string PA_SCHEDULE_MODE_EMERGENCY="緊急";
        public static string PA_SCHEDULE_MODE_NORMAL = "一般";

        public static string PA_SCHEDULE_NAME_BLANK = "Schedule Name cannot be blank!";
        public static string PA_DIFFERENT_SCHEDULE_TIME_REASON = "Time selected should be later than the current time";
        public static string PA_SCHEDULE_ENABLE_BLANK = "請確認是否要啓用此排程？";
        public static string PA_SCHEDULE_CONFIRM_ENABLE = "請確認是否要啓用此排程？";
        public static string PA_SCHEDULE_CONFIRM_DISABLE="請確認是否要停用此排程？";
        public static string PA_SCHEDULE_ADD_SCHEDULE_CONFIRM="請確認是否要保存此設定爲排程表？";
        public static string PA_SCHEDULE_DELETE_SCHEDULE_CONFIRM ="請確認是否要刪除排程表?";
        public static string PA_SCHEDULE_DELETE_ENABLED_SCHEDULE_CONFIRM = "排程表已啟用。請確認是否要刪除排程表?";
        public static string PA_SCHEDULE_ENABLE_SUCCESS="成功啓用排程。";
        public static string PA_SCHEDULE_ENABLE_FAILED="無法啓用排程";
        public static string PA_SCHEDULE_DISABLE_SUCCESS = "成功停用排程。";
        public static string PA_SCHEDULE_DISABLE_FAILED="無法停用排程。";
        public static string PA_SCHEDULE_DELETE_SUCCESS="成功刪除排程。";
        public static string PA_SCHEDULE_DELETE_FAILED = "無法刪除排程。";
        public static string PA_SCHEDULE_ADD_IN_PROGRESS = "新增排程處理中，請稍候......";
        public static string PA_SCHEDULE_EDIT_IN_PROGRESS = "編輯排程處理中，請稍候......";
        public static string PA_SCHEDULE_DELETE_IN_PROGRESS = "刪除排程處理中，請稍候......";

        public static string PA_SCHEDULE_ADD_SCHEDULE_DISABLE_CONFIRM = "此排程表尚未被啓用。請確認是否保存此設定爲新排程表？";
        public static string PA_SCHEDULE_ADD_SCHEDULE_NO_ZONES_SELECTED="在儲存排程預錄廣播前，請選擇至少一個區域。";
        public static string PA_SCHEDULE_ADD_SCHEDULE_NO_MESSAGE_SELECTED="在儲存排程預錄廣播前，請選擇至少一個預錄語音。?";

        public static string PA_SCHEDULE_VOLUME_CONFIRM_ENABLE="Are you sure you want to save these settings?";
        public static string PA_SCHEDULE_VOLUME_SUCCESS="Successfully save volume adjustment schedule.";
        public static string PA_SCHEDULE_VOLUME_ERROR = "Errors occurred while saving volume adjustment schedule.";
        public static string PA_SCHEDULE_VOLUME_FAILED="Unable to save volume adjustment schedule.";
        public static string PA_PREVIEW_AUDIO_ERROR = "Unable to play audio. Please contact your system administrator.";

        public static string VALIDATE_MSG_COUNT_LIMIT_REACHED = "Unable to add selected message. Maximum number of messages allowed per broadcast is";
        public static string VALIDATE_NO_ZONES_SELECTED = "Please select at least one zone before starting a broadcast";
        public static string VALIDATE_NO_MIX_MESSAGE_BROADCAST_MODE = "Unable to mix message mode in a single broadcast.";
        public static string VALIDATE_NO_MIX_MESSAGE_AUDIO_MODE = "PA Hardware only support broadcasting of a single type of audio type. You cannot mix mp3 format with G.722 in a single broadcast.";
        public static string VALIDATE_NO_MESSAGE_SELECTED = "Please select at least one message before start a broadcast";
        public static string VALIDATE_PREVIEW_NO_MESSAGE_SELECTED = "Please select at least one message before you can preview the audio message.";
        public static string VALIDATE_RETRY_NO_FAILED_ZONES = "No zones have been detected to have a failed broadcast.";
        public static string VALIDATE_CONFIG_NO_MSG_NAME = "Unable to save message. Please provide a message name before saving the message.";
        public static string VALIDATE_CONFIG_NO_MSG_CONTENT = "Unable to save message. Please provide a message content before saving the message.";
        public static string VALIDATE_CONFIG_NO_ENABLED_AUDIO = "Unable to save message. Please enable at least one language audio file.";
        public static string VALIDATE_CONFIG_NO_CAT = "Unable to save message. Please upload at least one langauge audio file before saving.";

        public static string CAPTION_BROADCAST_PREDEFINE_ERROR = "Predefined Broadcast Error!";
        public static string CAPTION_BROADCAST_LIVE_ERROR = "Live Broadcast Error!";
        public static string CAPTION_BROADCAST_PREDEFINE = "Predefined Broadcast";
        public static string CAPTION_BROADCAST_LIVE = "Live Broadcast";
        public static string CAPTION_CONFIGURE_MSG = "Configuring Message";
        public static string CAPTION_CONFIGURE_MSG_ERROR = "Configuring Message Error!";
        public static string CAPTION_SCHEDULE_BROADCAST = "Schedule Broadcast";
        public static string CAPTION_SCHEDULE_VOLUME="Adjust Volume Schedule";
        public static string PA_TITLE = "Pa Manager";
        public static string CAPTION_PREVIEW = "Preview Audio";
        public static string CAPTION_SUCCEEDED = "Succeeded";
        public static string CAPTION_FAILED = "Failed";

        public static string PA_ERR_GENERAL = "General Errors";
        public static string PA_ERR_SRV_NOT_AVAILABLE = "Unable to connect PA local service";
        public static string PA_ERR_DB_CONNECTION = "Unable to connect to database";
        public static string PA_ERR_DB_ERROR = "Sql statement errors";
        public static string PA_ERR_INVALID_PARAMS = "Invalid parameters specified";
        public static string PA_ERR_ANOTHER_BROADCAST_IN_PROGRESS = "Another broadcast is going on";
        public static string PA_ERR_UNABLE_TO_START_BROADCAST = "Unable to start a broadcast";
        public static string PA_ERR_UNABLE_TO_STOP_BROADCAST = "Unable to stop a broadcast";
        public static string PA_ERR_CHG_VOLUME_FAILED = "Unable to change volume";
        public static string PA_ERR_SCH_AGENT_CONNECTION = "Unable to connect to scheduling agent";
        public static string PA_ERR_SCH_AGENT_OPS_FAILURE = "Scheduling agent throws back an error";
        public static string PA_ERROR_NO_RIGHTS = "No rights to execute this action";
        public static string PA_ERR_MESSAGE_USED_BY_SCHEDULE = "Predefined messages is currently being used by a schedule";

        public static string PA_EVENT_STATUS_SUCCESS= "success";
        public static string PA_EVENT_STATUS_FAILURE = " failed";
        public static string PA_EVENT_STATUS_ENABLED = "enabled";
        public static string PA_EVENT_STATUS_DISABLED = "disabled";

        public static string PA_VALIDATE_PATH_ACCESS = "You have no access to folder";
        public static string PA_VALIDATE_PATH_PREDEFINED_DISABLED = "Predefined Message broadcast will be disabled. Please contact your administrator.";

        public static string PA_CONNECT_DMD_SERVER_FAILED = "Could not connect to DMD server.";
        public static string PA_CONNECT_DMS_SERVER = "Connect to DMD server";

        public static void localize()
        {
            errDBConnection_ = Localization.localize("ERROR_DB_CONNECTION", errDBConnection_);
            errGeneralExcep_ = Localization.localize("ERROR_GENERAL_EX", errGeneralExcep_);
            errCannotBlank_ = Localization.localize("ERROR_BLANK", errCannotBlank_);
            mbPaAlreadyRunning_ = Localization.localize("ERROR_APP_ALREADY_RUNNING", mbPaAlreadyRunning_);
            mbUserNotLogin_ = Localization.localize("ERROR_USER_NOT_LOGIN_FROM_MFT", mbUserNotLogin_);
            mbLaunchRight_ = Localization.localize("MB_LAUNCH_RIGHT", mbLaunchRight_);
            mbNoUser_ = Localization.localize("MB_NO_USER", mbNoUser_);
            mbWrongUser_ = Localization.localize("MB_WRONG_USER", mbWrongUser_);

            ERROR_PASERVICE_CONNECTION = Localization.localize("ERROR_PASERVICE_CONNECTION", ERROR_PASERVICE_CONNECTION);
            ERROR_CONFIG_MIX_FILE_AUDIO_FORMAT = Localization.localize("PA_ERROR_CONFIG_MIX_FILE_AUDIO_FORMAT", ERROR_CONFIG_MIX_FILE_AUDIO_FORMAT);
            ERROR_CONFIG_INCORRECT_AUDIO_FORMAT = Localization.localize("PA_ERROR_CONFIG_INCORRECT_AUDIO_FORMAT", ERROR_CONFIG_INCORRECT_AUDIO_FORMAT);
            ERROR_CONFIG_INCORRECT_FILE_FORMAT = Localization.localize("PA_ERROR_CONFIG_INCORRECT_FILE_FORMAT", ERROR_CONFIG_INCORRECT_FILE_FORMAT);
            ERROR_CONFIG_NO_FILE_SELECTED = Localization.localize("PA_ERROR_CONFIG_NO_FILE_SELECTED", ERROR_CONFIG_NO_FILE_SELECTED);
            ERROR_SCHAGENT_CONNECTION = Localization.localize("PA_ERROR_SCHAGENT_CONNECTION", ERROR_SCHAGENT_CONNECTION);
            ERROR_SCHAGENT_ERROR = Localization.localize("PA_ERROR_SCHAGENT_ERROR", ERROR_SCHAGENT_ERROR);
            ERROR_DATABASE_ERROR = Localization.localize("PA_ERROR_DATABASE_ERROR", ERROR_DATABASE_ERROR);

            ACTION_BROADCAST_FAILED = Localization.localize("PA_BROADCAST_FAILED", ACTION_BROADCAST_FAILED);
            ACTION_VOLUME_IN_PROGRESS = Localization.localize("PA_STATE_VOLUME_IN_PROGRESS", ACTION_VOLUME_IN_PROGRESS);
            ACTION_VOLUME_CHANGE_SUCCESS = Localization.localize("PA_STATE_VOLUME_CHANGE_SUCCESS", ACTION_VOLUME_CHANGE_SUCCESS);
            ACTION_VOLUME_CHANGE_FAILURE = Localization.localize("PA_STATE_VOLUME_CHANGE_FAILURE", ACTION_VOLUME_CHANGE_FAILURE);

            PROGRESS_MSG_PLEASE_WAIT = Localization.localize("PA_PROGRESS_MSG_PLEASE_WAIT", PROGRESS_MSG_PLEASE_WAIT);

            BROADCAST_STATUS_INVALID = Localization.localize("PA_BROADCAST_STATUS_INVALID", BROADCAST_STATUS_INVALID);
            BROADCAST_STATUS_PLAYING = Localization.localize("PA_BROADCAST_STATUS_PLAYING", BROADCAST_STATUS_PLAYING);
            BROADCAST_STATUS_INUSE = Localization.localize("PA_BROADCAST_STATUS_INUSE", BROADCAST_STATUS_INUSE);
            BROADCAST_STATUS_DISCONNECTED = Localization.localize("PA_BROADCAST_STATUS_DISCONNECTED", BROADCAST_STATUS_DISCONNECTED);
            BROADCAST_STATUS_IDLE = Localization.localize("PA_BROADCAST_STATUS_IDLE", BROADCAST_STATUS_IDLE);
            BROADCAST_STATUS_FAILED = Localization.localize("PA_BROADCAST_STATUS_FAILED", BROADCAST_STATUS_FAILED);
            BROADCAST_STATUS_COMPLETED = Localization.localize("PA_BROADCAST_STATUS_COMPLETED", BROADCAST_STATUS_COMPLETED);

            BROADCAST_STATUS_STARTED = Localization.localize("PA_BROADCAST_STATUS_STARTED", BROADCAST_STATUS_STARTED);
            BROADCAST_STATUS_WAITING = Localization.localize("PA_BROADCAST_STATUS_WAITING", BROADCAST_STATUS_WAITING);


            MSG_BROADCAST_STATUS_UNSTARTED = Localization.localize("PA_MSG_BROADCAST_STATUS_UNSTARTED", MSG_BROADCAST_STATUS_UNSTARTED);
            MSG_BROADCAST_STATUS_PLAYING = Localization.localize("PA_MSG_BROADCAST_STATUS_PLAYING", MSG_BROADCAST_STATUS_PLAYING);
            MSG_BROADCAST_STATUS_COMPLETED = Localization.localize("PA_MSG_BROADCAST_STATUS_COMPLETED", MSG_BROADCAST_STATUS_COMPLETED);
            
            PA_ADD_MESSAGE_CONFIRM = Localization.localize("PA_ADD_MESSAGE_CONFIRM", PA_ADD_MESSAGE_CONFIRM);
            PA_INSERT_MESSAGE_SUCCESS = Localization.localize("PA_INSERT_MESSAGE_SUCCESS", PA_INSERT_MESSAGE_SUCCESS);
            PA_INSERT_MESSAGE_FAIL = Localization.localize("PA_INSERT_MESSAGE_FAIL", PA_INSERT_MESSAGE_FAIL);
            PA_EDIT_SUCCESS = Localization.localize("PA_EDIT_SUCCESS", PA_EDIT_SUCCESS);
            PA_EDIT_FAIL = Localization.localize("PA_EDIT_FAIL", PA_EDIT_FAIL);
            PA_EDIT_MESSAGE_CONFIRM = Localization.localize("PA_EDIT_MESSAGE_CONFIRM", PA_EDIT_MESSAGE_CONFIRM);
            PA_DELETE_SUCCESS = Localization.localize("PA_DELETE_SUCCESS", PA_DELETE_SUCCESS);
            PA_DELETE_FAIL = Localization.localize("PA_DELETE_FAIL", PA_DELETE_FAIL);
            PA_DELETE_CONFIRM = Localization.localize("PA_DELETE_CONFIRM", PA_DELETE_CONFIRM);
            PA_INSERT_MESSAGE_SUCCESS = Localization.localize("PA_INSERT_MESSAGE_SUCCESS", PA_INSERT_MESSAGE_SUCCESS);

            PA_SCHEDULE_MODE_EMERGENCY = Localization.localize("PA_SCHEDULE_MODE_EMERGENCY", PA_SCHEDULE_MODE_EMERGENCY);
            PA_SCHEDULE_MODE_NORMAL = Localization.localize("PA_SCHEDULE_MODE_NORMAL", PA_SCHEDULE_MODE_NORMAL);

            PA_SCHEDULE_NAME_BLANK = Localization.localize("PA_SCHEDULE_NAME_BLANK", PA_SCHEDULE_NAME_BLANK);
            PA_DIFFERENT_SCHEDULE_TIME_REASON = Localization.localize("PA_DIFFERENT_SCHEDULE_TIME_REASON", PA_DIFFERENT_SCHEDULE_TIME_REASON);
            PA_SCHEDULE_ENABLE_BLANK = Localization.localize("PA_SCHEDULE_ENABLE_BLANK", PA_SCHEDULE_ENABLE_BLANK);
            PA_SCHEDULE_CONFIRM_ENABLE = Localization.localize("PA_SCHEDULE_CONFIRM_ENABLE", PA_SCHEDULE_CONFIRM_ENABLE);
            PA_SCHEDULE_CONFIRM_DISABLE = Localization.localize("PA_SCHEDULE_CONFIRM_DISABLE", PA_SCHEDULE_CONFIRM_DISABLE);
            PA_SCHEDULE_ADD_SCHEDULE_CONFIRM = Localization.localize("PA_SCHEDULE_ADD_SCHEDULE_CONFIRM", PA_SCHEDULE_ADD_SCHEDULE_CONFIRM);
            PA_SCHEDULE_DELETE_SCHEDULE_CONFIRM = Localization.localize("PA_SCHEDULE_DELETE_SCHEDULE_CONFIRM", PA_SCHEDULE_DELETE_SCHEDULE_CONFIRM);
            PA_SCHEDULE_DELETE_ENABLED_SCHEDULE_CONFIRM = Localization.localize("PA_SCHEDULE_DELETE_ENABLED_SCHEDULE_CONFIRM", PA_SCHEDULE_DELETE_ENABLED_SCHEDULE_CONFIRM);
            PA_SCHEDULE_ENABLE_SUCCESS = Localization.localize("PA_SCHEDULE_ENABLE_SUCCESS", PA_SCHEDULE_ENABLE_SUCCESS);
            PA_SCHEDULE_ENABLE_FAILED = Localization.localize("PA_SCHEDULE_ENABLE_FAILED", PA_SCHEDULE_ENABLE_FAILED);
            PA_SCHEDULE_DISABLE_SUCCESS = Localization.localize("PA_SCHEDULE_DISABLE_SUCCESS", PA_SCHEDULE_DISABLE_SUCCESS);
            PA_SCHEDULE_DISABLE_FAILED = Localization.localize("PA_SCHEDULE_DISABLE_FAILED", PA_SCHEDULE_DISABLE_FAILED);
            PA_SCHEDULE_DELETE_SUCCESS = Localization.localize("PA_SCHEDULE_DELETE_SUCCESS", PA_SCHEDULE_DELETE_SUCCESS);
            PA_SCHEDULE_DELETE_FAILED = Localization.localize("PA_SCHEDULE_DELETE_FAILED", PA_SCHEDULE_DELETE_FAILED);
            PA_SCHEDULE_ADD_SCHEDULE_DISABLE_CONFIRM = Localization.localize("PA_SCHEDULE_ADD_SCHEDULE_DISABLE_CONFIRM", PA_SCHEDULE_ADD_SCHEDULE_DISABLE_CONFIRM);
            PA_SCHEDULE_ADD_SCHEDULE_NO_ZONES_SELECTED = Localization.localize("PA_SCHEDULE_ADD_SCHEDULE_NO_ZONES_SELECTED", PA_SCHEDULE_ADD_SCHEDULE_NO_ZONES_SELECTED);
            PA_SCHEDULE_ADD_SCHEDULE_NO_MESSAGE_SELECTED = Localization.localize("PA_SCHEDULE_ADD_SCHEDULE_NO_MESSAGE_SELECTED", PA_SCHEDULE_ADD_SCHEDULE_NO_MESSAGE_SELECTED);

            PA_SCHEDULE_ADD_IN_PROGRESS = Localization.localize("PA_SCHEDULE_ADD_IN_PROGRESS", PA_SCHEDULE_ADD_IN_PROGRESS);
            PA_SCHEDULE_EDIT_IN_PROGRESS = Localization.localize("PA_SCHEDULE_EDIT_IN_PROGRESS", PA_SCHEDULE_EDIT_IN_PROGRESS);
            PA_SCHEDULE_DELETE_IN_PROGRESS = Localization.localize("PA_SCHEDULE_DELETE_IN_PROGRESS", PA_SCHEDULE_DELETE_IN_PROGRESS);

            PA_SCHEDULE_VOLUME_CONFIRM_ENABLE = Localization.localize("PA_SCHEDULE_VOLUME_CONFIRM_ENABLE", PA_SCHEDULE_VOLUME_CONFIRM_ENABLE);
            PA_SCHEDULE_VOLUME_SUCCESS = Localization.localize("PA_SCHEDULE_VOLUME_SUCCESS", PA_SCHEDULE_VOLUME_SUCCESS);
            PA_SCHEDULE_VOLUME_ERROR = Localization.localize("PA_SCHEDULE_VOLUME_ERROR", PA_SCHEDULE_VOLUME_ERROR);
            PA_SCHEDULE_VOLUME_FAILED = Localization.localize("PA_SCHEDULE_VOLUME_FAILED", PA_SCHEDULE_VOLUME_FAILED);
            PA_PREVIEW_AUDIO_ERROR = Localization.localize("PA_PREVIEW_AUDIO_ERROR", PA_PREVIEW_AUDIO_ERROR);

            VALIDATE_MSG_COUNT_LIMIT_REACHED = Localization.localize("PA_VALIDATE_MSG_COUNT_LIMIT_REACHED", VALIDATE_MSG_COUNT_LIMIT_REACHED);
            VALIDATE_NO_ZONES_SELECTED = Localization.localize("PA_VALIDATE_NO_ZONES_SELECTED", VALIDATE_NO_ZONES_SELECTED);
            VALIDATE_NO_MIX_MESSAGE_BROADCAST_MODE = Localization.localize("PA_VALIDATE_NO_MIX_MESSAGE_BROADCAST_MODE", VALIDATE_NO_MIX_MESSAGE_BROADCAST_MODE);
            VALIDATE_NO_MIX_MESSAGE_AUDIO_MODE = Localization.localize("PA_VALIDATE_NO_MIX_MESSAGE_AUDIO_MODE", VALIDATE_NO_MIX_MESSAGE_AUDIO_MODE);
            VALIDATE_NO_MESSAGE_SELECTED = Localization.localize("PA_VALIDATE_NO_MESSAGE_SELECTED", VALIDATE_NO_MESSAGE_SELECTED);
            VALIDATE_PREVIEW_NO_MESSAGE_SELECTED = Localization.localize("PA_VALIDATE_PREVIEW_NO_MESSAGE_SELECTED", VALIDATE_PREVIEW_NO_MESSAGE_SELECTED);
            VALIDATE_RETRY_NO_FAILED_ZONES = Localization.localize("PA_VALIDATE_RETRY_NO_FAILED_ZONES", VALIDATE_RETRY_NO_FAILED_ZONES);

            VALIDATE_CONFIG_NO_MSG_NAME = Localization.localize("PA_VALIDATE_CONFIG_NO_MSG_NAME", VALIDATE_CONFIG_NO_MSG_NAME);
            VALIDATE_CONFIG_NO_MSG_CONTENT = Localization.localize("PA_VALIDATE_CONFIG_NO_MSG_CONTENT", VALIDATE_CONFIG_NO_MSG_CONTENT);
            VALIDATE_CONFIG_NO_ENABLED_AUDIO = Localization.localize("PA_VALIDATE_CONFIG_NO_ENABLED_AUDIO", VALIDATE_CONFIG_NO_ENABLED_AUDIO);
            VALIDATE_CONFIG_NO_CAT = Localization.localize("PA_VALIDATE_CONFIG_NO_CAT", VALIDATE_CONFIG_NO_CAT);

            CAPTION_BROADCAST_PREDEFINE_ERROR = Localization.localize("PA_CAPTION_BROADCAST_PREDEFINE_ERROR", CAPTION_BROADCAST_PREDEFINE_ERROR);
            CAPTION_BROADCAST_LIVE_ERROR = Localization.localize("PA_CAPTION_BROADCAST_LIVE_ERROR", CAPTION_BROADCAST_LIVE_ERROR);
            CAPTION_BROADCAST_PREDEFINE = Localization.localize("PA_CAPTION_BROADCAST_PREDEFINE", CAPTION_BROADCAST_PREDEFINE);
            CAPTION_BROADCAST_LIVE = Localization.localize("PA_CAPTION_BROADCAST_LIVE", CAPTION_BROADCAST_LIVE);
            CAPTION_CONFIGURE_MSG = Localization.localize("PA_CAPTION_CONFIGURE_MSG", CAPTION_CONFIGURE_MSG);
            CAPTION_CONFIGURE_MSG_ERROR = Localization.localize("PA_CAPTION_CONFIGURE_MSG_ERROR", CAPTION_CONFIGURE_MSG_ERROR);
            CAPTION_SCHEDULE_BROADCAST = Localization.localize("PA_CAPTION_SCHEDULE_BROADCAST", CAPTION_SCHEDULE_BROADCAST);
            CAPTION_SCHEDULE_VOLUME = Localization.localize("PA_CAPTION_SCHEDULE_VOLUME", CAPTION_SCHEDULE_VOLUME);
            PA_TITLE = Localization.localize("PA_TITLE", PA_TITLE);
            CAPTION_PREVIEW = Localization.localize("PA_CAPTION_PREVIEW", CAPTION_PREVIEW);
            CAPTION_SUCCEEDED = Localization.localize("PA_CAPTION_SUCCEEDED", CAPTION_SUCCEEDED);
            CAPTION_FAILED = Localization.localize("PA_CAPTION_FAILED", CAPTION_FAILED);
                        
            PA_ERR_GENERAL = Localization.localize("PA_ERR_GENERAL", PA_ERR_GENERAL);
            PA_ERR_SRV_NOT_AVAILABLE = Localization.localize("PA_ERR_SRV_NOT_AVAILABLE", PA_ERR_SRV_NOT_AVAILABLE);
            PA_ERR_DB_CONNECTION = errDBConnection_;
            PA_ERR_DB_ERROR = ERROR_DATABASE_ERROR;
            PA_ERR_INVALID_PARAMS = Localization.localize("PA_ERR_INVALID_PARAMS", PA_ERR_INVALID_PARAMS);
            PA_ERR_ANOTHER_BROADCAST_IN_PROGRESS = Localization.localize("PA_ERR_ANOTHER_BROADCAST_IN_PROGRESS", PA_ERR_ANOTHER_BROADCAST_IN_PROGRESS);
            PA_ERR_UNABLE_TO_START_BROADCAST = Localization.localize("PA_ERR_UNABLE_TO_START_BROADCAST", PA_ERR_UNABLE_TO_START_BROADCAST);
            PA_ERR_UNABLE_TO_STOP_BROADCAST = Localization.localize("PA_ERR_UNABLE_TO_STOP_BROADCAST", PA_ERR_UNABLE_TO_STOP_BROADCAST);
            PA_ERR_CHG_VOLUME_FAILED = Localization.localize("PA_ERR_CHG_VOLUME_FAILED", PA_ERR_CHG_VOLUME_FAILED);
            PA_ERR_SCH_AGENT_CONNECTION = Localization.localize("PA_ERR_SCH_AGENT_CONNECTION", PA_ERR_SCH_AGENT_CONNECTION);
            PA_ERR_SCH_AGENT_OPS_FAILURE = Localization.localize("PA_ERR_SCH_AGENT_OPS_FAILURE", PA_ERR_SCH_AGENT_OPS_FAILURE);
            PA_ERROR_NO_RIGHTS = Localization.localize("PA_ERROR_NO_RIGHTS", PA_ERROR_NO_RIGHTS);
            PA_ERR_MESSAGE_USED_BY_SCHEDULE = Localization.localize("PA_ERR_MESSAGE_USED_BY_SCHEDULE", PA_ERR_MESSAGE_USED_BY_SCHEDULE);

            PA_EVENT_STATUS_SUCCESS = Localization.localize("PA_EVENT_STATUS_SUCCESS", PA_EVENT_STATUS_SUCCESS);
            PA_EVENT_STATUS_FAILURE = Localization.localize("PA_EVENT_STATUS_FAILURE", PA_EVENT_STATUS_FAILURE);
            PA_EVENT_STATUS_ENABLED = Localization.localize("PA_EVENT_STATUS_ENABLED", PA_EVENT_STATUS_ENABLED);
            PA_EVENT_STATUS_DISABLED = Localization.localize("PA_EVENT_STATUS_DISABLED", PA_EVENT_STATUS_DISABLED);

            PA_VALIDATE_PATH_ACCESS = Localization.localize("PA_VALIDATE_PATH_ACCESS", PA_VALIDATE_PATH_ACCESS);
            PA_VALIDATE_PATH_PREDEFINED_DISABLED = Localization.localize("PA_VALIDATE_PATH_PREDEFINED_DISABLED", PA_VALIDATE_PATH_PREDEFINED_DISABLED);

            PA_CONNECT_DMD_SERVER_FAILED = Localization.localize("PA_CONNECT_DMD_SERVER_FAILED", PA_CONNECT_DMD_SERVER_FAILED);
            PA_CONNECT_DMS_SERVER = Localization.localize("PA_CONNECT_DMS_SERVER", PA_CONNECT_DMS_SERVER);
            
            #region new added strings
            dbErrNoStnZOneDefnFound_ = Localization.localize("ERROR_NO_STN_ZN_DEF", dbErrNoStnZOneDefnFound_);
            #endregion
        }
    }
}
