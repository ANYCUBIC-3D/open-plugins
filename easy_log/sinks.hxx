#pragma once

#include <spdlog/details/log_msg.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

template <typename Mutex>
class dist_sink : public spdlog::sinks::base_sink<Mutex> {
  using base_sink = spdlog::sinks::base_sink<Mutex>;

public:
  dist_sink() = default;
  explicit dist_sink(std::vector<spdlog::sink_ptr> sinks) : sinks_(sinks) {}

  dist_sink(const dist_sink &) = delete;
  dist_sink &operator=(const dist_sink &) = delete;

  void add_sink(spdlog::sink_ptr sub_sink) {
    std::lock_guard<Mutex> lock(base_sink::mutex_);
    sinks_.push_back(sub_sink);
  }

  void remove_sink(spdlog::sink_ptr sub_sink) {
    std::lock_guard<Mutex> lock(base_sink::mutex_);
    sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), sub_sink),
                 sinks_.end());
  }

  void set_sinks(std::vector<spdlog::sink_ptr> sinks) {
    std::lock_guard<Mutex> lock(base_sink::mutex_);
    sinks_ = std::move(sinks);
  }

  std::vector<spdlog::sink_ptr> &sinks() { return sinks_; }

protected:
  void sink_it_(const spdlog::details::log_msg &msg) override {
    if (base_sink::should_log(msg.level)) {
      for (auto &sub_sink : sinks_) {
        sub_sink->log(msg);
      }
    }
  }

  void flush_() override {
    for (auto &sub_sink : sinks_) {
      sub_sink->flush();
    }
  }

  void set_pattern_(const std::string &pattern) override {
    base_sink::set_formatter_(
        spdlog::details::make_unique<spdlog::pattern_formatter>(pattern));
  }

  void
  set_formatter_(std::unique_ptr<spdlog::formatter> sink_formatter) override {
    base_sink::formatter_ = std::move(sink_formatter);
    for (auto &sub_sink : sinks_) {
      sub_sink->set_formatter(base_sink::formatter_->clone());
    }
  }
  std::vector<spdlog::sink_ptr> sinks_;
};

using dist_sink_mt = dist_sink<std::mutex>;
using dist_sink_st = dist_sink<spdlog::details::null_mutex>;
