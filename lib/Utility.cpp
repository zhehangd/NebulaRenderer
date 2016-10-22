#include "Utility.hpp"
#include "Math.hpp"

#include <chrono>
#include <iostream>
#include <iomanip>

std::ostream& operator<<(std::ostream& ss,const Vector3 &v)
{
  auto prec = ss.precision(2);
  ss<<"("<<v[0]<<","<<v[1]<<","<<v[2]<<")";
  ss.precision(prec);
  return ss;
}

void Timer::start(void)
{
  srt=std::chrono::steady_clock::now();
}

float Timer::duration(void)
{
  return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - srt).count();
}