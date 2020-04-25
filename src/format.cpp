#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    int hh, mm, ss;
    hh = seconds/3600;
    mm = (seconds%3600)/60;
    ss = (seconds%3600)%60;
    std::string final = (std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss));
    return final;
}