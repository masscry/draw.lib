#include <draw.hpp>
#include <cstdio>

int main(int argc, char* argv[])
{
  draw::system_t& instance = draw::system_t::Instance();

  fprintf(stderr, "Before Sleep: %f\n", instance.Timestamp());
  instance.Sleep(5.3);
  fprintf(stderr, "After Sleep: %f\n", instance.Timestamp());
  return 0;
}