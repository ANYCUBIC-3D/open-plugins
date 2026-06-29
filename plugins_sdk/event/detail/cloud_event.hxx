// Copyright (c) 2026 深圳市纵维立方科技有限公司
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

#pragma once

#include "event_helper.hxx"

enum {
  LOGIN_EVENT_SUCCESS = 1, ///< 登录成功
  LOGIN_EVENT_FAIL = -3,   ///< 登录失败
  LOGIN_EVENT_CANCEL = -1, ///< 登录取消
  LOGIN_EVENT_REJECT = -2, ///< 登录被拒绝
  LOGIN_EVENT_LOGOUT = 0   ///< 登出
};
#define NODE(x, y, z) z
#define CLOUD_EVENT_DEFAULT_LIST                                               \
  NODE(CloudEvent, EventSpaceInfo, SPACEINFO),                                 \
      NODE(CloudEvent, EventFilerename, FILERENAME),                           \
      NODE(CloudEvent, EventFileremove, FILEREMOVE),                           \
      NODE(CloudEvent, EventFileLock, FILELOCK),                               \
      NODE(CloudEvent, EventFileUnlock, FILEUNLOCK),                           \
      NODE(CloudEvent, EventFileConfirmation, FILECONFIRMATION),               \
      NODE(CloudEvent, EventSliceStatus, SLICESTATUS),                         \
      NODE(CloudEvent, EventLogout, LOGOUT),                                   \
      NODE(CloudEvent, EventCheckNick, CHECKNICK),                             \
      NODE(CloudEvent, EventCAPTCHA, CAPTCHA),                                 \
      NODE(CloudEvent, EventResetPWD, RESETPWD),                               \
      NODE(CloudEvent, EventRegister, REGISTER),                               \
      NODE(CloudEvent, EventDelPrinter, DELPRINTER),                           \
      NODE(CloudEvent, EventRenamePrinter, RENAMEPRINTER),                     \
      NODE(CloudEvent, EventRemotePrint, REMOTEPRINT),                         \
      NODE(CloudEvent, EventFeedback, FEEDBACK),                               \
      NODE(CloudEvent, EventPrintDetail, PRINTDETAIL),                         \
      NODE(CloudEvent, EventHistoryDelete, HISTORYDELETE),                     \
      NODE(CloudEvent, EventMessageSetRead, MESSAGESETREAD),                   \
      NODE(CloudEvent, EventMessageNewTest, MESSAGENEWTEST),                   \
      NODE(CloudEvent, EventMessageDelete, MESSAGEDELETE),                     \
      NODE(CloudEvent, EventClientReport, CLIENTREPORT),                       \
      NODE(CloudEvent, EventDeviceSave, DEVICESAVE),                           \
      NODE(CloudEvent, EventBatchPrint, BATCHPRINT),                           \
      NODE(CloudEvent, EventCleanPlate, CLEANPLATE),                           \
      NODE(CloudEvent, EventCreateBulkTask, CREATEBULKTASK),                   \
      NODE(CloudEvent, EventSubmitFeedback, SUBMITFEEDBACK),                   \
      NODE(CloudEvent, EventFarmMessageDelete, FARMMESSAGEDELETE),             \
      NODE(CloudEvent, EventAlarmMessageBatchSetRead,                          \
           ALARMMESSAGEBATCHSETREAD)

#define CLOUD_EVENT_SIG_LIST                                                   \
  (NODE(ProgressResponse, EventFileDownload, FILEDOWNLOAD),                    \
   CLOUD_EVENT_DEFAULT_LIST,                                                   \
   NODE(ProgressResponse, EventFileUpload, FILEUPLOAD),                        \
   NODE(LoginResponse, EventLogin, LOGIN),                                     \
   NODE(AddPrinterResponse, EventAddPrinter, ADDPRINTER),                      \
   NODE(PrinterDetailResponse, EventPrinterDetail, PRINTERDETAIL),             \
   NODE(PrintProjectDetailResponse, EventPrintProjectDetail,                   \
        PRINTPROJECTDETAIL),                                                   \
   NODE(PrintHistoryDetailResponse, EventHistoryDetail, HISTORYDETAIL),        \
   NODE(PrintHistoryFeedbackListResponse, EventGetFeedbackList,                \
        GETFEEDBACKLIST),                                                      \
   NODE(ProjectInfoResponse, EventProjectInfo, PROJECTINFO),                   \
   NODE(ModelInfoResponse, EventModelInfo, MODELINFO),                         \
   NODE(GCodeInfoResponse, EventSliceInfo, SLICEINFO),                         \
   NODE(RegionResponse, EventGetRegion, GETREGION),                            \
   NODE(UserInfoResponse, EventUpdateUser, UPDATEUSER),                        \
   NODE(GetExternalShelvesResponse, EventGetExternalShelves,                   \
        GETEXTERNALSHELVES),                                                   \
   NODE(AccessTokenResponse, EventAccessToken, ACCESSTOKEN),                   \
   NODE(EngineVersion, EventEngineVersion, ENGINEVERSION),                     \
   NODE(UpdateResponse, EventUpdateVersion, UPDATEVERSION),                    \
   NODE(SlicerFileInfoResponse, EventSlicerInfo, SLICERINFO),                  \
   NODE(SendFileToPrintersResponse, EventSendFileToPrinters,                   \
        SENDFILSTOPRINTER),                                                    \
   NODE(PrinterToolResponse, EventPrinterTool, PRINTERTOOL),                   \
   NODE(PluginUpdateResponse, EventPluginUpdate, PLUGINUPDATE),                \
   NODE(PrinterReportInfoResponse, EventPrinterReportInfo, PRINTERREPORTINFO), \
   NODE(SlicerDetailResponse, EventSlicerDetail, SLICERDETAIL),                \
   NODE(AddUserColorGroupResponse, EventAddUserColorGroup, ADDUSERCOLORGROUP), \
   NODE(EditUserColorGroupResponse, EventEditUserColorGroup,                   \
        EDITUSERCOLORGROUP),                                                   \
   NODE(DeleteUserColorGroupResponse, EventDeleteUserColorGroup,               \
        DELETEUSERCOLORGROUP),                                                 \
   NODE(TokenResponse, EventCtrlPrinter, CTRLPRINTER),                         \
   NODE(SlicerConfigResponse, EventGetSlicerConfig, GETSLICERCONFIG),          \
   NODE(MessageAggregateInfo, EventFarmMessageInfo, FARMMESSAGEINFO))

#define CLOUD_EVENT_ARRAY_LIST                                                 \
  (NODE(SlicerConfigResponse, EventSetSlicerConfig, SETSLICERCONFIG),          \
   NODE(SlicerConfigResponse, EventPreSign, PRESIGN),                          \
   NODE(PrinterResponse, EventPrinterList, PRINTERLIST),                       \
   NODE(CountryResponse, EventCountries, COUNTRIES),                           \
   NODE(MessageCountResponse, EventMessageCount, MESSAGECOUNT),                \
   NODE(ReasonResponse, EventReasons, REASONS),                                \
   NODE(PrinterStatusResponse, EventPrinterStatus, PRINTERSTATUS),             \
   NODE(MultiBoxInfoResponse, EventMultiBoxInfo, MULTIBOXINFO),                \
   NODE(MaterialResponse, EventMaterial, MATERIAL),                            \
   NODE(PrintOptionsResponse, EventPrintOption, PRINTOPTION),                  \
   NODE(ColorGroupListResponse, EventGetColorGroupList, GETCOLORGROUPLIST),    \
   NODE(VideoThumbnailResponse, EventGetTimelapseDownloadUrls,                 \
        GETTIMELAPSEDOWNLOADURLS),                                             \
   NODE(LatestErrorInfo, EventFarmMessageLatest, FARMMESSAGELATEST))

#define CLOUD_EVENT_ARRAY_LIST_DROP_COUNT 1

#define CLOUD_EVENT_PAGE_LIST                                                  \
  (NODE(MessageAggregateInfo, EventFarmMessageList, FARMMESSAGELIST),          \
   NODE(FileInfoResponse, EventFilelist, FILELIST),                            \
   NODE(PrintHistoryResponse, EventPrintHistory, PRINTHISTORY),                \
   NODE(MessageListResponse, EventMessageList, MESSAGELIST),                   \
   NODE(PrintTaskResponse, EventPrintTaskList, PRINTTASKLIST),                 \
   NODE(TaskListResponse, EventBatchTaskList, BATCHTASKLIST),                  \
   NODE(PrinterErrorInfo, EventFarmMessagePrinterErrorList,                    \
        FARMMESSAGEPRINTERERRORLIST))

EVENT_HELPER_DECLARE(impl_wxDECLARE_EVENT, EVT_CLOUD_CLIENT_,
                     CLOUD_EVENT_PAGE_LIST)
EVENT_HELPER_DECLARE(impl_wxDECLARE_EVENT, EVT_CLOUD_CLIENT_,
                     CLOUD_EVENT_ARRAY_LIST)
EVENT_HELPER_DECLARE(impl_wxDECLARE_EVENT, EVT_CLOUD_CLIENT_,
                     CLOUD_EVENT_SIG_LIST)