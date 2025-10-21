#pragma once
#include "event_helper.hxx"

#define CLOUD_MQTT_EVENT_LIST                                                  \
  (MULTIBOX_INFO, PERIPHERIE_INFO, DEVICE_STATUS, LAST_WILL_MSG)

EVENT_HELPER_DECLARE(impl_wxDECLARE_EVENT, WXEVT_CLOUD_MQTT_,
                     CLOUD_MQTT_EVENT_LIST)
