#include <iostream>
#include <unistd.h>
#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    this->PrevIdle = LinuxParser::IdleJiffies();
    this->PrevNonIdle = LinuxParser::ActiveJiffies() - LinuxParser::IdleJiffies();
    this->PrevTotal = PrevIdle + PrevNonIdle;
    sleep(1);
    this->Idle = LinuxParser::IdleJiffies();
    this->NonIdle = LinuxParser::ActiveJiffies() - LinuxParser::IdleJiffies();
    this->Total = Idle + NonIdle;

    this->totald = Total - PrevTotal;
    this->idled = Idle - PrevIdle;
    
    this->CPU_Percentage = static_cast<float>(totald - idled)/totald; 
    return this->CPU_Percentage; 
    }