#include <packetmachine.hpp>
#include <fluent.hpp>

#include "dns.hpp"

int main(int argc, char* argv[]) {
  auto machine = new pm::Machine();
  auto logger = new fluent::Logger();
  logger->new_forward("localhost", 24224);

  auto dns = new dns::Receiver(logger);
  
  machine->on("DNS", [&](const pm::Property& p) {
      dns->recv(p);
    });
  machine->loop();

  delete dns;
  delete logger;
  delete machine;
  return 0;
}
