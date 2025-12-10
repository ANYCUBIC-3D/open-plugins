#pragma once

#include <boost/preprocessor/if_comma.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple.hpp>

#define I18N_DECLARE_LANG(r, nil, lang) extern result_type lang();

#define I18N_DECLARE_LANGUAGES(lang_list)                                      \
  BOOST_PP_SEQ_FOR_EACH(                                                       \
      I18N_DECLARE_LANG, _,                                                    \
      BOOST_PP_SEQ_POP_BACK(BOOST_PP_TUPLE_TO_SEQ(lang_list)))

#define I18N_DEFINE_LANG(r, n, lang_list)                                      \
  BOOST_PP_IF_COMMA(n) {                                                       \
    BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(n, lang_list)),                      \
                       BOOST_PP_TUPLE_ELEM(n, lang_list)                       \
  }

#define I18N_DEFINE_LANGUAGES(type, value, lang_list)                          \
  type value = {BOOST_PP_REPEAT(BOOST_PP_TUPLE_SIZE(lang_list) - 1,            \
                                I18N_DEFINE_LANG, lang_list)};
