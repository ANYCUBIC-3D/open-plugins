// copy from Slic3r::GUI::I18N::translate

#ifndef _
#define _(s) Anycubic::Plugins::I18n::translate((s))
#define _L(s) Anycubic::Plugins::I18n::translate((s))
#define _devL(s) wxString((s))
#define _omitL(s) ("")
#define _utf8(s) Anycubic::Plugins::I18n::translate_utf8((s))
#define _u8L(s) Anycubic::Plugins::I18n::translate_utf8((s))
#endif /* _ */

#ifndef _CTX
#define _CTX(s, ctx) Anycubic::Plugins::I18n::translate((s), (ctx))
#define _CTX_utf8(s, ctx) Anycubic::Plugins::I18n::translate_utf8((s), (ctx))
#endif /* _ */

#ifndef _CTX
#define _CTX(s, ctx) s
#define _CTX_utf8(s, ctx) s
#endif /* _ */

#ifndef L
// !!! If you needed to translate some wxString,
// !!! please use _L(string)
// !!! _() - is a standard wxWidgets macro to translate
// !!! L() is used only for marking localizable string
// !!! It will be used in "xgettext" to create a Locating Message Catalog.
#define L(s) s
#endif /* L */

#ifndef ANYCUBIC_I18N_HXX
#define ANYCUBIC_I18N_HXX

#include <wx/intl.h>
#include <wx/version.h>

namespace Anycubic::Plugins::I18n {
static inline wxString translate(const char *s) {
  return wxGetTranslation(wxString(s, wxConvUTF8));
}
static inline wxString translate(const wchar_t *s) {
  return wxGetTranslation(s);
}
static inline wxString translate(const std::string &s) {
  return wxGetTranslation(wxString(s.c_str(), wxConvUTF8));
}
static inline wxString translate(const std::wstring &s) {
  return wxGetTranslation(s.c_str());
}
static inline wxString translate(const wxString &s) {
  return wxGetTranslation(s);
}

static inline wxString translate(const char *s, const char *plural,
                                 unsigned int n) {
  return wxGetTranslation(wxString(s, wxConvUTF8), wxString(plural, wxConvUTF8),
                          n);
}
static inline wxString translate(const wchar_t *s, const wchar_t *plural,
                                 unsigned int n) {
  return wxGetTranslation(s, plural, n);
}
static inline wxString translate(const std::string &s,
                                 const std::string &plural, unsigned int n) {
  return wxGetTranslation(wxString(s.c_str(), wxConvUTF8),
                          wxString(plural.c_str(), wxConvUTF8), n);
}
static inline wxString translate(const std::wstring &s,
                                 const std::wstring &plural, unsigned int n) {
  return wxGetTranslation(s.c_str(), plural.c_str(), n);
}
static inline wxString translate(const wxString &s, const wxString &plural,
                                 unsigned int n) {
  return wxGetTranslation(s, plural, n);
}

static inline std::string translate_utf8(const char *s) {
  return wxGetTranslation(wxString(s, wxConvUTF8)).ToUTF8().data();
}
static inline std::string translate_utf8(const wchar_t *s) {
  return wxGetTranslation(s).ToUTF8().data();
}
static inline std::string translate_utf8(const std::string &s) {
  return wxGetTranslation(wxString(s.c_str(), wxConvUTF8)).ToUTF8().data();
}
static inline std::string translate_utf8(const std::wstring &s) {
  return wxGetTranslation(s.c_str()).ToUTF8().data();
}
static inline std::string translate_utf8(const wxString &s) {
  return wxGetTranslation(s).ToUTF8().data();
}

static inline std::string translate_utf8(const char *s, const char *plural,
                                         unsigned int n) {
  return translate(s, plural, n).ToUTF8().data();
}
static inline std::string
translate_utf8(const wchar_t *s, const wchar_t *plural, unsigned int n) {
  return translate(s, plural, n).ToUTF8().data();
}
static inline std::string translate_utf8(const std::string &s,
                                         const std::string &plural,
                                         unsigned int n) {
  return translate(s, plural, n).ToUTF8().data();
}
static inline std::string translate_utf8(const std::wstring &s,
                                         const std::wstring &plural,
                                         unsigned int n) {
  return translate(s, plural, n).ToUTF8().data();
}
static inline std::string
translate_utf8(const wxString &s, const wxString &plural, unsigned int n) {
  return translate(s, plural, n).ToUTF8().data();
}
} // namespace Anycubic::Plugins::I18n
#endif /* ANYCUBIC_I18N_HXX */
