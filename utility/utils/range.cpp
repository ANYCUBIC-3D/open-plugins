#include "range.hxx"

#ifndef NDEBUG

#include <iostream>

void TestRange() {
  for (int i : Anycubic::utility::range(10)) {
    std::cout << i << std::endl;
  }
}

#endif