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

// 事件列表
#define CLOUD_CLIENT_EVENT_LIST                                                \
  (FORCE_LOGOUT,       /* 强制登出--这个事件由sdk产生*/                        \
   SPACEINFO,          /* 获取空间信息*/                                       \
   FILERENAME,         /* 文件重命名*/                                         \
   FILEREMOVE,         /* 文件删除*/                                           \
   FILELIST,           /* 文件列表数据*/                                       \
   FILELOCK,           /* 文件空间锁定*/                                       \
   FILEUNLOCK,         /* 文件空间解锁*/                                       \
   FILECONFIRMATION,   /* 文件上传确认*/                                       \
   FILEUPLOAD,         /* 文件上传*/                                           \
   FILEDOWNLOAD,       /* 文件下载*/                                           \
   SLICEINFO,          /* 切片文件信息*/                                       \
   SLICESTATUS,        /* 切片文件状态(切片文件状态更准确)*/                   \
   MODELINFO,          /* 模型文件信息*/                                       \
   LOGIN,              /* 登录*/                                               \
   LOGOUT,             /* 登出*/                                               \
   CHECKNICK,          /* 检查昵称*/                                           \
   CAPTCHA,            /* 获取验证码*/                                         \
   RESETPWD,           /* 重置密码*/                                           \
   REGISTER,           /* 用户注册*/                                           \
   PRINTERLIST,        /* 打印机列表*/                                         \
   MULTIBOXINFO,       /* 多功能盒子信息*/                                     \
   ADDPRINTER,         /* 添加打印机*/                                         \
   DELPRINTER,         /* 删除打印机*/                                         \
   RENAMEPRINTER,      /* 重命名打印机*/                                       \
   REMOTEPRINT,        /* 远程打印*/                                           \
   FEEDBACK,           /* 反馈*/                                               \
   COUNTRIES,          /* 国家列表*/                                           \
   CTRLPRINTER,        /* 远程打印控制*/                                       \
   PRINTTASKLIST,      /* 打印任务列表*/                                       \
   PRINTHISTORY,       /* 打印历史列表*/                                       \
   PRINTDETAIL,        /* 打印历史详细*/                                       \
   HISTORYDELETE,      /* 打印历史删除*/                                       \
   MESSAGELIST,        /* 消息列表*/                                           \
   MESSAGECOUNT,       /* 数据数量*/                                           \
   MESSAGESETREAD,     /* 标记已读*/                                           \
   MESSAGENEWTEST,     /* 新消息标记获取*/                                     \
   MESSAGEDELETE,      /* 消息删除*/                                           \
   REASONS,            /* 错误码信息*/                                         \
   PRINTERSTATUS,      /* 打印机状态*/                                         \
   PRINTERDETAIL,      /* 打印机详细信息*/                                     \
   PROJECTINFO,        /* 项目详细信息*/                                       \
   CLIENTREPORT,       /* 客户端上报*/                                         \
   GETREGION,          /* 获取区域信息*/                                       \
   UPDATEUSER,         /* 更新用户信息*/                                       \
   GETEXTERNALSHELVES, /* 获取外置料架信息*/                                   \
   ACCESSTOKEN,        /* 获取accesstoken*/                                    \
   MATERIAL,           /* 获取材料数据*/                                       \
   PRINTOPTION,        /* 获取打印选项*/                                       \
   DEVICESAVE,         /* 设备信息保存*/                                       \
   ENGINEVERSION,      /* 获取引擎版本*/                                       \
   UPDATEVERSION,      /*获取更新版本信息*/                                    \
   PRESIGN,            /*预签名*/                                              \
   GETSLICERCONFIG,    /*获取slicer配置信息*/                                  \
   SETSLICERCONFIG,    /*设置slicer配置信息*/                                  \
   BATCHPRINT,         /*批量打印*/                                            \
   BATCHTASKLIST,      /* 获取批量任务列表*/                                   \
   SLICERINFO,         /* 获取slicer信息*/                                     \
   SENDFILETOPRINTERS, /* 发送文件到打印机*/                                   \
   PRINTERTOOL,        /* 打印机详情工具*/                                     \
   PLUGINUPDATE        /* 插件更新*/                                           \
  )

EVENT_HELPER_DECLARE(impl_wxDECLARE_EVENT, EVT_CLOUD_CLIENT_,
                     CLOUD_CLIENT_EVENT_LIST)