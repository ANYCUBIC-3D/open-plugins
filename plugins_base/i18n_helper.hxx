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

#include <boost/preprocessor/punctuation/comma_if.hpp>
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
  BOOST_PP_COMMA_IF(n) {                                                       \
    BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(n, lang_list)),                      \
                       BOOST_PP_TUPLE_ELEM(n, lang_list)                       \
  }

#define I18N_DEFINE_LANGUAGES(type, value, lang_list)                          \
  type value = {BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_POP_BACK(       \
                                    BOOST_PP_TUPLE_TO_SEQ(lang_list))),        \
                                I18N_DEFINE_LANG, lang_list)};
