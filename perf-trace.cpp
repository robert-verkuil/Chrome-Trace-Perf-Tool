#include <fstream>
#include <iostream>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>


#define DEFER(code) std::shared_ptr<void>_(nullptr, std::bind([&]{ code; }))

/*
  name:   The name of the event, as displayed in Trace Viewer
  cat:    The event categories (comma seperated) Can be used for hiding events.
  ph:     The event type. (a single character)
  ts:     The tracing clock timestamp of the event. microsecond granularity.
  tts:    Optional. The thread clock timestamp of the event. microseconds.
  pid:    The process ID for the process that output this event.
  tid:    The thread ID for the thread that output this event.
  args:   Any arguments provided for the event. Displayed in Trace Viewer.
  Optional
  cname:  A fixed color name to associate with the event. Must be valid.

  Simplest trace format is a list of these types of arrays:
  Ex: {"name": "Asub", "cat": "PERF", "ph": "B", "pid": 22630, "tid": 22630, "ts": 829}

  Duration Events     B (begin), E (end)
  Complete Events     X
  Instant Events      i, I (deprecated)
  Counter Events      C
  Async Events        b (nestable start), n (nestable instant), e (nestable end)
          Deprecated  S (start), T (step into), p (step past), F (end)
  Flow Events         s (start), t (step), f (end)
  Sample Events       P
  Object Events       N (created), O (snapshot), D (destroyed)
  Metadata Events     M
  Memory Dump Events  V (global), v (process)
  Mark Events         R
  Clock Sync Events   c
  Context Events      (, )
*/


struct TraceEvent {
  std::string name;
  std::string cat;
  std::string ph;
  pid_t pid;
  std::thread::id tid;
  std::chrono::steady_clock::time_point ts;

  TraceEvent(std::string p_name,
             std::string p_cat,
             std::string p_ph,
             pid_t p_pid,
             std::thread::id p_tid,
             std::chrono::steady_clock::time_point p_ts)
    : name(std::move(p_name)),
      cat(std::move(p_cat)),
      ph(std::move(p_ph)),
      pid(std::move(p_pid)),
      tid(std::move(p_tid)),
      ts(std::move(p_ts)) {}
};

// Global log where all info will be stored.
// Map from thread_id -> event_list
static std::map<std::thread::id, std::vector<TraceEvent> > log_ = {};


// --------------------------------------------------------------------------
// FUNCTIONS
// --------------------------------------------------------------------------

void log_start(std::string const & name, std::string const & cat/*, std::string& ph*/) {
  pid_t pid = ::getpid();
  std::thread::id tid = std::this_thread::get_id();
  // std::cerr << "start tid = " << tid << std::endl;
  std::chrono::steady_clock::time_point ts = std::chrono::steady_clock::now();

  log_[tid].emplace_back(name, cat, "B" /*ph*/, pid, tid, ts);
}


void log_end(std::string const & name, std::string const & cat/*, std::string& ph*/) {
  pid_t pid = ::getpid();
  std::thread::id tid = std::this_thread::get_id();
  // std::cerr << "end tid = " << tid << std::endl;
  std::chrono::steady_clock::time_point ts = std::chrono::steady_clock::now();

  log_[tid].emplace_back(name, cat, "E" /*ph*/, pid, tid, ts);
}


void log_function(std::string const & name, std::string const & cat) {
  log_start(name, cat);
  DEFER(log_end(name, cat));
}


std::string event_to_str(TraceEvent const &e) {
  std::ostringstream ret;
  std::hash<std::thread::id> hasher;
  ret << "{";
  ret << "\"name\": \"" << e.name  << "\", ";
  ret << "\"cat\": \""  << e.cat   << "\", ";
  ret << "\"ph\": \""   << e.ph    << "\", ";
  ret << "\"pid\": "    << e.pid   << ", ";
  ret << "\"tid\": "    << hasher(e.tid)   << ", ";
  ret << "\"ts\": "     << std::chrono::time_point_cast<std::chrono::microseconds>(e.ts).time_since_epoch().count();
  ret << "}";
  return ret.str();
}


void log_print(std::string filename) {
  std::ofstream myfile;
  myfile.open(filename);
  myfile << "[";
  for (auto const &log_itr : log_) {
    for (auto const &event_itr : log_itr.second) {
      myfile << event_to_str(event_itr);

      // Add a comma and newline on all but last entry.
      if (&event_itr != &*log_itr.second.rbegin()
            || &log_itr != &*log_.rbegin() ) {
        myfile << ",\n";
      }
    }
  }
  myfile << "]";
  myfile.close();

  std::cout << "Wrote chrome trace log to " << filename << std::endl;
}
