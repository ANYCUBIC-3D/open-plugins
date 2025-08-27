#include "cloud_mqtt_event.hxx"

#define DEFINE_EXPAND(r, prefix, elem)                                         \
  wxDEFINE_EVENT(BOOST_PP_CAT(prefix, elem),                                   \
                 Anycubic::Plugins::SDK::wxPluginEvent);

BOOST_PP_SEQ_FOR_EACH(DEFINE_EXPAND, WXEVT_CLOUD_MQTT_,
                      BOOST_PP_TUPLE_TO_SEQ(DECLARE_CLOUD_MQTT_EVENT_LIST))