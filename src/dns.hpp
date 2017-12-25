#include <fluent.hpp>
#include <packetmachine.hpp>
#include "../external/cpp-toolbox/src/cache.hpp"

namespace dns {

class Key : public tb::HashKey {
 public:
  Key(const pm::Property &p, pm::ParamKey& p_tx_id, pm::ParamKey& p_src,
      pm::ParamKey& p_dst, pm::ParamKey& p_is_query) {
    auto f = [&](pm::ParamKey& pid) mutable {
      size_t len;
      const void *ptr = p[pid].raw(&len);
      this->append(ptr, len);
    };

    f(p_tx_id);
    if (p[p_is_query].uint() == 1) {
      f(p_src);
      f(p_dst);
    } else {
      f(p_dst);
      f(p_src);
    }

    this->finalize();
  }
};


struct Target {
  const std::string name_;
  const pm::ParamKey pid_;
  Target(const std::string &name, pm::ParamKey& pid) :
      name_(name), pid_(pid) {}
};


class Receiver {
 private:

  std::vector<Target*> targets_;
  fluent::Logger *logger_;
  tb::LruHash<fluent::Message*> cache_;
  time_t prev_ts_;
  pm::ParamKey tx_id_, ipv4_src_, ipv4_dst_, is_query_;

  void build_message(fluent::Message *msg, const pm::Property &p,
                     bool without_query = false);
  
 public:
  Receiver(const pm::Machine& machine, fluent::Logger* logger);
  void recv(const pm::Property& p);

  
};

}
