#ifndef TICKET_SYSTEM_TIMER_H
#define TICKET_SYSTEM_TIMER_H
#pragma once

#include <chrono>
#include <cstdio>
#include <fstream>  // Add this line

class Timer {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  long totduration;
  const char *name;
  std::ofstream logFile;  // Add this line
public:
  explicit Timer(const char *s) : name(s), totduration(0) {
    logFile.open("timer_log.txt", std::ios::app);
  }

  void start() {
    start_time = std::chrono::system_clock::now();
  }

  void stop() {
    auto end_time = std::chrono::system_clock::now();
    totduration += std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
  }

  double duration() const {
    return (double) totduration * std::chrono::nanoseconds::period::num / std::chrono::nanoseconds::period::den;
  }

  ~Timer() {
    logFile << name << " took " << duration() << " seconds\n";  // Change this line
    logFile.close();  // Close the log file
//        fprintf(stderr, "%s took %.6lf seconds\n", name, duration());
  }
};

#endif //TICKET_SYSTEM_TIMER_H
