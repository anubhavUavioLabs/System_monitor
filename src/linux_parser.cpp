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
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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
float LinuxParser::MemoryUtilization() { 
  float percent;
  string line;
  string key;
  string value;
  unsigned long MemTotal, MemFree, Buffers, Cached, UsedMemory;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      // std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        MemTotal = std::stoi(value);
      }
      else if (key == "MemFree:") {
        MemFree = std::stoi(value);
      }
      else if (key == "Buffers:") {
        Buffers = std::stoi(value);
      }
      else if (key == "Cached:") {
        Cached = std::stoi(value);
      }
      // else if (key == "MemAvailable:") {
      //   MemAvailable = std::stoi(value);
      // }
    }
    UsedMemory = MemTotal - MemFree - Buffers - Cached;
    percent = (static_cast<float>(UsedMemory) / MemTotal);
  }
  return percent; 
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  std::string line;
    long uptime = 0;
    std::ifstream stream(kProcDirectory + kUptimeFilename);

    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> uptime; 
    }
    return uptime;
  }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return (LinuxParser::UpTime() * sysconf(_SC_CLK_TCK)); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;
  string key;
  long User = 0, Nice = 0, System = 0, Idle = 0, IOwait = 0, IRQ = 0, SoftIRQ = 0, Steal = 0, Guest = 0, GuestNice = 0;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> User >> Nice >> System >> Idle >> IOwait >> IRQ >> SoftIRQ >> Steal >> Guest >> GuestNice;
  }

  return User + Nice + System + Idle + IOwait + IRQ + SoftIRQ + Steal + Guest + GuestNice;  
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;
  string key;
  long User = 0, Nice = 0, System = 0, Idle = 0, IOwait = 0, IRQ = 0, SoftIRQ = 0, Steal = 0, Guest = 0, GuestNice = 0;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> User >> Nice >> System >> Idle >> IOwait >> IRQ >> SoftIRQ >> Steal >> Guest >> GuestNice;
  }
  return Idle + IOwait;
  }

// TODO: Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization(int pid) { 
  vector<long> cpuValues{};
  string line;
  long time = 0;
  string value;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i <= kStarttime_; i++) {
        linestream >> value;
        // read CPU time spent in user mode - utime #14
        // read CPU time spent in kernel mode - stime #15
        // read Waited for children's CPU time spent in user mode - cutime #16
        // read Waited for children's CPU time spent in kernel mode- cstime #17
        // read the time when process started - starttime #22
        if (i == kUtime_ || i == kStime_ || i == kCutime_ || i == kCstime_ || i == kStarttime_) {
          // read in clock ticks and convert to seconds
          // devide by clock ticks per second
          time = std::stol(value) / sysconf(_SC_CLK_TCK);
          // time = std::stol(value);
          cpuValues.emplace_back(time);
        }
      }
    }
  }
  return cpuValues; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, line;
  int value;
  int processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        processes = value;
        break;
      } } } 
  return processes;
  }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {  
  string key, line;
  int value;
  int procs_running;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        procs_running = value;
        break;
      } } } 
  return procs_running;}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string value = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, value);
    return value;
  }
  return value; }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
int LinuxParser::Ram(int pid) { 
  string value;
  string key;
  string line;
  int ram;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  
  while (std::getline(stream, line)){
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == "VmSize") {
      ram = (std::stol(value) * 0.001);
    }
  }
  return ram; }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return value; }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string line;
  string key;
  string value = "";
  string other;
  // find user name for this user ID in /etc/passwd
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> other >> key) {
        if (key == uid) {
          return value;
        }
      }
    }
  }
  return value; }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  long uptime;
  long value;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i <= kStarttime_; i++) {
        linestream >> value;
        // read the time when process started - starttime #22
        if (i == kStarttime_) {
          // read the starttime value in clock ticks and convert to seconds
          // devide by clock ticks per second
          uptime = value / sysconf(_SC_CLK_TCK);
          return uptime;
      }
    }
  }}
  return  uptime; }
