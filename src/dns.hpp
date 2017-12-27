#include <fluent.hpp>
#include <packetmachine.hpp>
#include "../external/cpp-toolbox/src/cache.hpp"

namespace dns {

struct Target {
  const std::string name_;
  const pm::ParamKey pid_;
  Target(const std::string &name, pm::ParamKey& pid) :
      name_(name), pid_(pid) {}
};

struct ParamKeySet {
  pm::ParamKey tx_id_, ipv4_src_, ipv4_dst_, is_query_;
  ParamKeySet(const pm::Machine& machine) :
      tx_id_(machine.lookup_param_key("DNS.tx_id")),
      ipv4_src_(machine.lookup_param_key("IPv4.src")),
      ipv4_dst_(machine.lookup_param_key("IPv4.dst")),
      is_query_(machine.lookup_param_key("DNS.is_query")) {
  }
};

class Key : public tb::HashKey {
 public:
  Key(const pm::Property &p, const ParamKeySet& pks) {
    
    auto f = [&](const pm::ParamKey& pid) mutable {
      size_t len;
      const void *ptr = p[pid].raw(&len);
      this->append(ptr, len);
    };

    f(pks.tx_id_);
    if (p[pks.is_query_].uint() == 1) {
      f(pks.ipv4_src_);
      f(pks.ipv4_dst_);
    } else {
      f(pks.ipv4_dst_);
      f(pks.ipv4_src_);
    }

    this->finalize();
  }
};
  


class Receiver {
 private:
  const ParamKeySet key_set_;
  std::vector<Target*> targets_;
  fluent::Logger *logger_;
  tb::LruHash<fluent::Message*> cache_;
  time_t prev_ts_;

  void build_message(fluent::Message* msg, const pm::Property& p,
                     const ParamKeySet& pks, bool without_query = false);
  
 public:
  Receiver(const pm::Machine& machine, fluent::Logger* logger);
  void recv(const pm::Property& p);  
};

}
