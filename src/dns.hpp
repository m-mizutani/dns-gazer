#include <fluent.hpp>
#include <packetmachine.hpp>
#include "../external/cpp-toolbox/src/cache.hpp"

namespace dns {

class Receiver {
 private:
  fluent::Logger *logger_;
  tb::LruHash<fluent::Message*> cache_;
  time_t prev_ts_;
  pm::ParamKey tx_id_, ipv4_src_, ipv4_dst_, is_query_;
  
 public:
  Receiver(const pm::Machine& machine, fluent::Logger* logger);
  void recv(const pm::Property& p);
};

}
