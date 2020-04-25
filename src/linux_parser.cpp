#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
//ys: done. Not sure when to stop the loop.
//cat /proc/meminfo
//MemTotal:       16425396 kB
//MemFree:        10944000 kB
//MemAvailable:   14579924 kB
float LinuxParser::MemoryUtilization() {
  string key, value, ignore, line;
  float total_mem, free_mem;
  std::ifstream memutil_filestream(kProcDirectory + kMeminfoFilename);
  if(memutil_filestream.is_open()) {
    while (std::getline(memutil_filestream, line)){
      	std::replace(line.begin(), line.end(), ':', ' ');
  		std::istringstream linestream(line);
    	while (linestream >> key >> value >> ignore){
        if (key == "MemTotal") {
				  total_mem = std::stof(value);
			  }
        if (key == "MemFree"){
				free_mem = std::stof(value);
			  }
    	}
    }
  }
  return (total_mem - free_mem)/total_mem;
}

// TODO: Read and return the system uptime
// cat /proc/uptime
//1117.90 3909.04
long LinuxParser::UpTime(){
  string uptime, idle_time, line;
  long final=0;
  std::ifstream uptime_stream(kProcDirectory + kUptimeFilename);
  if (uptime_stream.is_open()){
    std::getline(uptime_stream, line);
  	std::istringstream linestream(line);
    linestream >> uptime >> idle_time;
    final = std::stol(uptime);
    }
  return final;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long final = 0;
  vector<string> jlist = LinuxParser::CpuUtilization();
  /*for(auto i :jlist){
    final = final + std::stol(i);
  }*/
  for (int i = kUser_; i  <= kSteal_; i++){
    final += std::stol(jlist[i]);
  }
  return final;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  long final;
  vector<string> list;
  std::ifstream jif_stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(jif_stream.is_open()){
    std::getline(jif_stream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      list.push_back(value);
    }
  }
  final = std::stol(list[13]) + std::stol(list[14]) + std::stol(list[15]) + std::stol(list[16]);
  return final;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long final=0;
  vector<string> jlist = LinuxParser::CpuUtilization();
  final = std::stol(jlist[kIdle_]) + std::stol(jlist[kIOwait_]);
  return final;
}

// TODO: Read and return CPU utilization
/*
cpu  39232 3483 11361 456222 5536 0 564 0 0 0
cpu0 11143 1278 3371 107797 5084 0 184 0 0 0
cpu1 9323 584 2620 116345 114 0 152 0 0 0
cpu2 8662 650 2701 116964 173 0 134 0 0 0
cpu3 10103 970 2668 115115 163 0 92 0 0 0
*/
vector<string> LinuxParser::CpuUtilization() {
  string key, line, value;
  vector<string> jiffies;
  std::ifstream cpu_stream(kProcDirectory + kStatFilename);
  while(cpu_stream.is_open()){
    std::getline(cpu_stream, line);
    std::istringstream linestream(line);
    while(linestream >> key){
      if(key!="cpu"){
        jiffies.push_back(key);
      }
    }
  }
  return jiffies;
}

// TODO: Read and return the total number of processes
//processes 32285
int LinuxParser::TotalProcesses() {
  string line, key, value;
  int final=0;
  std::ifstream tot_pross_stream(kProcDirectory + kStatFilename);
  if(tot_pross_stream.is_open()){
    while(std::getline(tot_pross_stream, line)){
  		std::istringstream linestream(line);
    	while (linestream >> key >> value){
        if (key == "processes"){ 
          final = std::stoi(value);
          }
        }
      }
    }
  return final;
}

// TODO: Read and return the number of running processes
//procs_running 2
int LinuxParser::RunningProcesses() {
  string line, key, value;
  int final=0;
  std::ifstream running_pross_stream(kProcDirectory + kStatFilename);
  if(running_pross_stream.is_open()){
    while(std::getline(running_pross_stream, line)){
  		std::istringstream linestream(line);
    	while (linestream >> key >> value){
        if (key == "procs_running"){ 
          final = std::stoi(value);
          }
        }
      }
    }
  return final;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
/*
cat /proc/1008/cmdline
/usr/share/code/code --type=gpu-process --field-trial-handle=17302287630831838787,10284478238377291005,131072 --disable-features=LayoutNG,SpareRendererForSitePerProcess --disable-color-correct-rendering --no-sandbox --gpu-preferences=IAAAAAAAAAAgAAAgAAAAAAAAYAAAAAAACAAAAAAAAAAIAAAAAAAAAA== --use-gl=swiftshader-webgl --service-re
*/
string LinuxParser::Command(int pid) {
  string value, line, final;
  std::ifstream cmd_stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(cmd_stream.is_open()){
    while (std::getline(cmd_stream, line)){
  		std::istringstream linestream(line);
      std::replace(line.begin(), line.end(), ' ', '*');
    	while (linestream >> value){
			std::replace(value.begin(), value.end(), '*', ' ');
        final = value;
        }
      }
    }
  return final;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
/*
NSsid:  1
VmPeak:   455768 kB
VmSize:   438728 kB
VmLck:         0 kB
*/
string LinuxParser::Ram(int pid){
  string final;
  string line, key, value, unit;
  std::ifstream ram_stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (ram_stream.is_open()){
    while (std::getline(ram_stream, line)){
  		std::istringstream linestream(line);
    	while (linestream >> key >> value >> unit){
        	if (key == "VmSize:"){ 
            final = std::to_string(std::stoi(value)/1000); 
          }
        }
      }
    }
  return final;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
/*
Pid:    1008
PPid:   976
TracerPid:      0
Uid:    0
*/
string LinuxParser::Uid(int pid) {
  string final;
  string line, key, value;
  std::ifstream uid_stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(uid_stream.is_open()) {
    while(std::getline(uid_stream, line)){
  		std::istringstream linestream(line);
    	while(linestream >> key >> value) {
        	if(key == "Uid:"){ 
            final = value; 
          }
        }
      }
    }
  return final;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string usr, passwd, uid, line;
  string final;
  std::ifstream passwd_stream(kPasswordPath);
  if(passwd_stream.is_open()){
    while(std::getline(passwd_stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
    	while(linestream >> usr >> passwd >> uid){
            if(uid==LinuxParser::Uid(pid)){
            final = usr;
          }
        }
    	}
    }
  return final;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string value, line;
  vector<string> p_list;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
			p_list.push_back(value);
      }
    }
  long final =  LinuxParser::UpTime() - std::stol(p_list[21])/sysconf(_SC_CLK_TCK);
  return final;
}