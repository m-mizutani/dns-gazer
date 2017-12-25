#include <fluent.hpp>
#include <packetmachine.hpp>

namespace dns {

class Receiver {
 private:
  fluent::Logger *logger_;
 public:
  Receiver(fluent::Logger *logger);
  void recv(const pm::Property& p);
};

}
