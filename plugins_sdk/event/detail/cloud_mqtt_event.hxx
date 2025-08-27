#pragma once
#include "../plugin_event.hxx"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/tuple.hpp>

#define DECLARE_CLOUD_MQTT_EVENT_LIST                                          \
  (MULTIBOX_INFO, PERIPHERIE_INFO, DEVICE_STATUS, LAST_WILL_MSG)

#define CLOUD_MQQTT_DECLARE_EXPAND(r, prefix, elem)                            \
  wxDECLARE_EXPORTED_EVENT(EVENT_API, BOOST_PP_CAT(prefix, elem),              \
                           Anycubic::Plugins::SDK::wxPluginEvent);

BOOST_PP_SEQ_FOR_EACH(CLOUD_MQQTT_DECLARE_EXPAND, WXEVT_CLOUD_MQTT_,
                      BOOST_PP_TUPLE_TO_SEQ(DECLARE_CLOUD_MQTT_EVENT_LIST))
