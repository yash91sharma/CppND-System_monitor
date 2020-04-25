#include "processor.h"
#include "linux_parser.h"
#include <unistd.h>
#include <string>

// TODO: Return the aggregate CPU utilization
//value!="cpu" had a loop issue, so trying hardcoded loop below.
float Processor::Utilization() {
  std::string line, t1, t2, t3, t4, t5, t6, t7, t8, t9 ,t10, t11;
  float cpu_util, total_diff, idle_diff;
  long total, idle, total_old=0, idle_old=0;
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if(filestream.is_open()){
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >>t1>>t2 >>t3>>t4>>t5>>t6>>t7>>t8>>t9>>t10>>t11;
      total = std::stol(t2) + std::stol(t3) + std::stol(t4) + std::stol(t5) +
         std::stol(t6) + std::stol(t7) + std::stol(t8) + std::stol(t9) +
         std::stol(t10) + std::stol(t11);
      idle = std::stol(t5) + std::stol(t6);
      total_diff = (float)(total - total_old);
      idle_diff = (float)(idle - idle_old);
      total_old = total;
      idle_old = idle;
      cpu_util = (total_diff - idle_diff) / total_diff;
   }
   return cpu_util;
}