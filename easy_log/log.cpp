#include "log.hxx"
#include "sinks.hxx"

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/udp_sink.h>
#include <spdlog/spdlog.h>

#include <boost/nowide/convert.hpp>

#include <filesystem>
#include <sstream>
namespace fs = std::filesystem;

namespace anycubic::logger {
struct filename_calculator {
  // Create filename for the form basename.YYYY-MM-DD
  static std::string calc_filename(const std::string &filename,
                                   const tm &now_tm) {
    std::string basename, ext;
    std::tie(basename, ext) =
        spdlog::details::file_helper::split_by_extension(filename);
    std::string count;
    int nCount = 0;
    do {
      auto filename = fmt::format("{}_{:04d}{:02d}{:02d}{}{}", basename,
                                  now_tm.tm_year + 1900, now_tm.tm_mon + 1,
                                  now_tm.tm_mday, count, ext);
#ifdef _WIN32
      if (!fs::exists(boost::nowide::widen(filename))) {
#else
      if (!fs::exists(filename)) {
#endif
        return filename;
      }
      count = "_" + std::to_string(++nCount);
    } while (true);
    return std::string();
  }
};

struct BaseConfig {
  bool enable_console{false};
  fs::path log_path;
  level_enum default_level{level_enum::info};
};
static BaseConfig g_config;

bool setup_log(const char *path, level_enum default_level,
               bool enalbe_console) {
  g_config.log_path = path;
  if (!fs::exists(g_config.log_path)) {
    fs::create_directories(g_config.log_path);
  }

  g_config.default_level = default_level;
  g_config.enable_console = enalbe_console;
  return true;
}

bool enable_debug(void) {
  set_level(nullptr, level_enum::trace);
  return true;
}

bool register_logger(const char *mname, bool) {
  try {
    spdlog::drop(mname);
    auto sink = std::make_shared<dist_sink_mt>();
    if (g_config.enable_console) {
      sink->add_sink(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    sink->add_sink(
        std::make_shared<
            spdlog::sinks::daily_file_sink<std::mutex, filename_calculator>>(
            (g_config.log_path / (std::string(mname) + ".log")).string(), 0,
            0));

    auto logger = std::make_shared<spdlog::logger>(mname, sink);
    logger->set_level(
        static_cast<spdlog::level::level_enum>(g_config.default_level));
    logger->set_pattern(DEFAULT_PATTERN);
    spdlog::register_logger(logger);
    if (strcmp(mname, "MainApp") == 0) {
      spdlog::set_default_logger(logger);
    }
    return true;
  } catch (const spdlog::spdlog_ex &) {
    return false;
  }
}

bool unregister_logger(const char *mname) {
  try {
    spdlog::drop(mname);
    return true;
  } catch (const spdlog::spdlog_ex &) {
    return false;
  }
}

bool set_level(const char *mname, level_enum level) {
  try {
      if (mname == nullptr) {
      spdlog::apply_all([level](std::shared_ptr<spdlog::logger> logger) {
        logger->set_level(static_cast<spdlog::level::level_enum>(level));
        if (level <= level_enum::debug) {
          std::string strHost = "127.0.0.1";
          if (auto host = std::getenv("AC_LOG_HOST"); host != nullptr) {
            strHost = host;
          }
          spdlog::sinks::udp_sink_config sc{strHost, 55535};
          auto sink = std::make_shared<spdlog::sinks::udp_sink_mt>(sc);
          sink->set_pattern(DEFAULT_PATTERN);
          auto sink0 = logger->sinks();
          if (auto p = dynamic_cast<dist_sink_mt *>(sink0[0].get());
              p != nullptr)
            p->add_sink(sink);
        }
      });
      return true;
    }
    auto logger = spdlog::get(mname);
    if (logger) {
      logger->set_level(static_cast<spdlog::level::level_enum>(level));
      if (level <= level_enum::debug) {
        logger->flush_on(static_cast<spdlog::level::level_enum>(level));
      }
      return true;
    }
  } catch (const spdlog::spdlog_ex &) {
    // do nothing
  }
  return false;
}

bool set_pattern(const char *mname, const char *pattern) {
  try {
    auto logger = spdlog::get(mname);
    if (logger) {
      logger->set_pattern(pattern);
      return true;
    }
  } catch (const spdlog::spdlog_ex &) {
    // do nothing
  }
  return false;
}
bool should_log(const char *mname, level_enum level) {
  auto logger = spdlog::get(mname);
  if (logger) {
    return logger->level() <= static_cast<spdlog::level::level_enum>(level);
  } else {
    return false;
  }
}
bool log_(const char *mname, const source_loc &loc, level_enum level,
          const char *msg) {
  try {
    auto logger = spdlog::get(mname);
    if (logger) {
      logger->log(spdlog::source_loc{loc.filename, loc.line, loc.funcname},
                  static_cast<spdlog::level::level_enum>(level), msg);
      return true;
    }
  } catch (const spdlog::spdlog_ex &) {
    // do nothing
  }
  return false;
}

} // namespace anycubic::logger