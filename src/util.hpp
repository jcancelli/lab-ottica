#pragma once

#include <iostream>
#include <sstream>
#include <string>

void section(const char* title);

const char* boolToString(bool b);

bool equals(double a, double b, double epsilon);

template <class T>
std::string concat(const T& arg) {
  std::stringstream ss;
  ss << arg;
  return ss.str();
}

template <class T, class... Args>
std::string concat(const T& first, const Args&... args) {
  return concat(first) + concat(args...);
}