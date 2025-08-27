#include "cloud_event.hxx"

#define DEFINE_EXPAND(r, prefix, elem)                                         \
  wxDEFINE_EVENT(BOOST_PP_CAT(prefix, elem),                                   \
                 Anycubic::Plugins::SDK::wxPluginEvent);

BOOST_PP_SEQ_FOR_EACH(DEFINE_EXPAND, EVT_CLOUD_CLIENT_,
                      BOOST_PP_TUPLE_TO_SEQ(CLOUD_CLIENT_EVENT_LIST))