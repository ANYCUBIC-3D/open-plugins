#pragma once
#include "../plugin_event.hxx"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/tuple.hpp>

#define impl_wxDECLARE_EVENT(z, prefix, elem)                                  \
  wxDECLARE_EXPORTED_EVENT(EVENT_API, BOOST_PP_CAT(prefix, elem),              \
                           Anycubic::Plugins::SDK::wxPluginEvent);

#define impl_wxDEFINE_EVENT(z, prefix, elem)                                   \
  wxDEFINE_EVENT(BOOST_PP_CAT(prefix, elem),                                   \
                 Anycubic::Plugins::SDK::wxPluginEvent);

#define EVENT_HELPER_IMPL(micro, prefix, tuple)                                \
  BOOST_PP_SEQ_FOR_EACH(micro, prefix, tuple)

#define EVENT_HELPER_DECLARE(expand_macro, prefix, tuple)                      \
  EVENT_HELPER_IMPL(expand_macro, prefix, BOOST_PP_TUPLE_TO_SEQ(tuple))

#define EVENT_HELPER_DEFINE(expand_macro, prefix, tuple)                       \
  EVENT_HELPER_IMPL(expand_macro, prefix, BOOST_PP_TUPLE_TO_SEQ(tuple))
