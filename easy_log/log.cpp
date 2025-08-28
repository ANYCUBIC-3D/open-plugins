#include "log.hxx"
#include "sinks.hxx"

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <sstream>
namespace fs = std::filesystem;

namespace anycubic::logger {

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
  spdlog::apply_all(
      [](auto logger) { logger->set_level(spdlog::level::trace); });
  return true;
}

bool register_logger(const char *mname, bool) {
  try {
    spdlog::drop(mname);
    auto sink = std::make_shared<dist_sink_mt>();
    if (g_config.enable_console) {
      sink->add_sink(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    sink->add_sink(std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        (g_config.log_path / (std::string(mname) + ".log")).string(), 0, 0));

    auto logger = std::make_shared<spdlog::logger>(mname, sink);
    logger->set_level(
        static_cast<spdlog::level::level_enum>(g_config.default_level));
    logger->set_pattern(DEFAULT_PATTERN);
    spdlog::register_logger(logger);
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
    auto logger = spdlog::get(mname);
    if (logger) {
      logger->set_level(static_cast<spdlog::level::level_enum>(level));
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