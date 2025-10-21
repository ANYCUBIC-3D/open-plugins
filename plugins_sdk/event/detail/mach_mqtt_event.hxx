#pragma once
#include "event_helper.hxx"

// clang-format off
#define MACH_MQTT_EVT_LIST (\
  /*这里开始的顺序要跟 MQTT_API_TYPES 严格一致*/                 \
   PERIPHERIEREPONSE, LIGHTRESPONSE,           \
   NETWORKQUERYRESPONSE, SMARTBOXRESPONSE,     \
   OTAVERSIONRESPONSE, AXISMOVERESPONSE,       \
   TEMPATURERESPONSE, PRINTUPDATERESPONSE,     \
   PRINTREETECTRESPONSE, PRINTIGNORERESPONSE,  \
   PRINTPRINTINGRESPONSE,                                      \
   PRINTPREHEATINGRESPONSE,                                    \
   PRINTMONITORINGRESPONSE,                                    \
   PRINTDOWNLOADINGRESPONSE,                                   \
   EXTFILBOXRESPONSE, PRINTCOMMONRESPONSE,     \
   OTAUPGRADERESPONSE, FILAMENTTYPERESPONSE,   \
   RESINRESPONSE,         \
   FILTERELEMENTRESPONSE,                                      \
   MCBDRYSTATUSRESPONSE, INFORESPONSE, \
   MULTIBOXINFORESPONSE,          \
   /*MQTT_API_TYPES 结束 */ \
    /*这开始的顺序要跟 MQTT_API_ARY_TYPES 严格一致*/                             \
   AUTOOPSTATUSRESPONSE, \
   FILERESPONSE,         \
   /* MQTT_API_ARY_TYPES 结束 */        \
   CONNECTED,            /*Connection established.*/           \
   CONNECTION_LOST,      /*Connection lost.*/                  \
   DISCONNECTED,         /* Disconnected from client.*/        \
   SUBSCRIBE,            /*Subscribe to a topic.*/             \
   UNSUBSCRIBE,          /*Unsubscribe from a topic.*/         \
   PUBLISH,              /*Publish a message to a topic.*/     \
   RECONNECTED           /*Reconnected to client.*/            \
  )
/// event list end

// clang-format on

EVENT_HELPER_DECLARE(impl_wxDECLARE_EVENT, WXEVT_MACH_MQTT_, MACH_MQTT_EVT_LIST)
