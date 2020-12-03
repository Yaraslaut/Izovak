#include "Extrapolator/ext.cpp"
#include <spdlog/spdlog.h>


int main(){
#ifdef USE_SPDLOG
  spdlog::set_level(spdlog::level::debug); // Set global log level to debug
  spdlog::info("Welcome to spdlog!");
#endif
  Extrapolator::test();
  return 0;
}
