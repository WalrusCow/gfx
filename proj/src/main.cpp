#include <iostream>

#include <map>
#include <set>
#include <string>

#include "scene_lua.hpp"

struct Argument {
  Argument() {}
  Argument(bool hasValue_) : hasValue(hasValue_) {}
  bool hasValue = false;
};

int main(int argc, char** argv) {
  auto printUsage = [&] () {
    std::cerr
      << "Usage: " << argv[0] << " file [-t threads] [-p] [-g]"
      << std::endl
      << "\t-p:  Use phong interpolation" << std::endl
      << "\t-g:  Use a uniform grid structure" << std::endl
      << "\t-u:  SeSet uniform grid size factor. Default 8." << std::endl;
  };

  if (argc < 2) {
    std::cerr << "Scene file required." << std::endl;
    printUsage();
    return 1;
  }

  std::map<char, Argument> argMap = {
    {'t', {true}},
    {'p', {false}},
    {'g', {false}},
    {'u', {true}},
  };

  std::set<char> flags;
  std::map<char, std::string> args;

  std::string filename;

  // Pull out options
  for (int i = 1; i < argc; ++i) {
    // If we spot a flag, get the next arg as the value
    std::string argStr = argv[i];
    if (argStr.size() != 2 || argStr[0] != '-') {
      if (filename.empty() && argStr[0] != '-') {
        // We didn't yet find the filename, so treat this unmatched argument
        // as the filename (but only if it doesn't start with a dash)
        filename = argStr;
        continue;
      }
      else {
        // We have already seen the filename and this isn't a recognized
        // argument: Error out
        std::cerr << "Unknown option: " << argStr << std::endl;
        printUsage();
        return 1;
      }
    }

    char option = argStr[1];
    if (argMap.find(option) == argMap.end()) {
      // Not in our map: Error out
      std::cerr << "Unknown option: " << argStr << std::endl;
      printUsage();
      return 1;
    }

    const auto& arg = argMap[option];
    std::string argVal;
    if (arg.hasValue) {
      if (i + 1 >= argc) {
        // Not enough arguments
        std::cerr << "Missing value for option " << argStr << std::endl;
        printUsage();
        return 1;
      }
      argVal = argv[i + 1];
      i += 1;
      args[option] = argVal;
    }
    else {
      flags.insert(option);
    }
  }

  // Now we have a set and a map of arguments
  for (const char c : flags) {
    switch (c) {
    case 'p':
      rayTracerOptions.phongInterpolation = true;
      break;
    case 'g':
      rayTracerOptions.uniformGrid = true;
      break;
    }
  }

  for (const auto& arg : args) {
    switch (arg.first) {
    case 't':
      {
      int numThreads = std::stoi(arg.second);
      // Enforce reasonable limits
      if (numThreads <= 0 || numThreads >= 32) {
        std::cerr << "Invalid number of threads: " << numThreads << std::endl;
        printUsage();
      }
      rayTracerOptions.threadCount = numThreads;
      break;
      }
    case 'u':
      {
      int constant = std::stoi(arg.second);
      // Enforce reasonable limits
      if (constant <= 0) {
        std::cerr << "Invalid grid constant: " << constant << std::endl;
        printUsage();
      }
      rayTracerOptions.uniformGridSizeFactor = constant;
      break;
      }
    }
  }

  if (!run_lua(filename)) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }
}
