#include "dns.hpp"

namespace dns {

Receiver::Receiver(const pm::Machine& machine, fluent::Logger *logger) :
    logger_(logger), cache_(600) {
    this->tx_id_    = machine.lookup_param_key("DNS.tx_id");
    this->ipv4_src_ = machine.lookup_param_key("IPv4.src");
    this->ipv4_dst_ = machine.lookup_param_key("IPv4.dst");
    this->is_query_ = machine.lookup_param_key("DNS.is_query");
}

void Receiver::recv(const pm::Property& p) {
  
}

}
