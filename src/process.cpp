#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid(pid){
    CpuUtilization();
    Command();
    Ram();
    User();
    UpTime();
}
// TODO: Return this process's ID
int Process::Pid() { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  float cpuUsage_;
  long uptime = LinuxParser::UpTime(pid);
  vector<long> val = LinuxParser::CpuUtilization(pid);
  // only if the values could be read sucessfully
  if (val.size() == 5) {
    // add utime, stime, cutime, cstime (they are in seconds)
    float totaltime = val[kUtime_] + val[kStime_]; //+ val[kCutime_] + val[kCstime_];
    // float seconds = uptime - (val[kStarttime_] / sysconf(_SC_CLK_TCK));
    float seconds = uptime - val[kStarttime_];
    // calculate the processes CPU usage
    // cpuUsage_ =  (totaltime / sysconf(_SC_CLK_TCK)) / seconds;
    cpuUsage_ =  totaltime / seconds;
 }
  else {
    cpuUsage_ = 0;
    } 

    return cpuUsage_;}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// TODO: Return this process's memory utilization
string Process::Ram() { return std::to_string(LinuxParser::Ram(pid)); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return true; }