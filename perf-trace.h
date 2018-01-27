#ifndef PERF_TRACE_H
#define PERF_TRACE_H

#include <string>

void log_start(std::string const & name, std::string const& cat/*, std::string& ph*/);
void log_end(std::string const & name, std::string const& cat/*, std::string& ph*/);
void log_function(std::string const& name, std::string const& cat);

void log_print(std::string filename);

#endif // PERF_TRACE_H
