#include "perf-trace.h"

#include <cstdio>

#include <memory>
#include <functional>
#define DEFER(code) std::shared_ptr<void>_(nullptr, std::bind([&]{ code; }))

// int id = omp_get_thread_num( );  TODO: use this??

int main() {
  #pragma omp parallel num_threads(4)
  {
    #pragma omp for
    for (int i = 0; i < 100; ++i) {
      log_start("printf_a", "print_test");
      printf("Hello World!\n");
      log_end("printf_a", "print_test");
    }

    #pragma omp barrier

    #pragma omp for
    for (int i = 0; i < 100; ++i) {
      log_start("printf_b", "print_test");
      printf("Hello World!\n");
      log_end("printf_b", "print_test");
    }
  }

  log_print("trace_log.json");
  return 0;
}
