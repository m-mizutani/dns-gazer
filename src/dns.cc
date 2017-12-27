#include "dns.hpp"

namespace dns {

void Receiver::build_message(fluent::Message* msg, const pm::Property& p,
                             const ParamKeySet& pks, bool without_query) {
                             
  auto is_query = (p.value(pks.is_query_).uint() == 1);

  if (is_query) {
    msg->set("tx_id",       p.value(pks.tx_id_).uint());
    msg->set("client_addr", p.value(pks.ipv4_src_).ip4());
    msg->set("server_addr", p.value(pks.ipv4_dst_).ip4());
    msg->set("client_port", p.src_port());
    msg->set("server_port", p.dst_port());
    msg->set("query_ts",    p.ts_d());
    msg->set("status",      "no_reply");
  } else if (without_query) {
    msg->set("tx_id",       p.value(pks.tx_id_).uint());
    msg->set("client_addr", p.value(pks.ipv4_dst_).ip4());
    msg->set("server_addr", p.value(pks.ipv4_src_).ip4());
    msg->set("client_port", p.dst_port());
    msg->set("server_port", p.src_port());
    msg->set("reply_ts",    p.ts_d());
    assert(msg->set("status", "no_query"));
  } else {
    msg->set("reply_ts",    p.ts_d());
    assert(msg->del("status"));
    assert(msg->set("status", "success"));
  }

  auto rec_name = (is_query) ? "query" : "reply";
  auto rec_arr = msg->retain_array(rec_name);
  for (auto& tgt : this->targets_) {
    if (p.has_value(tgt->pid_)) {
      auto& arr =
          dynamic_cast<const pm::value::Array&>(p.value(tgt->pid_));
      
      for (size_t i = 0; i < arr.size(); i++) {
        auto& rec = arr.get(i);
        auto r = rec_arr->retain_map();
        r->set("section", tgt->name_);
        r->set("name", rec.find("name").repr());
        r->set("type", rec.find("type").repr());
        const auto& rec_data = rec.find("data");
        if (!rec_data.is_null()) {
          r->set("data", rec_data.repr());
        }
      }
    }
  }
}






Receiver::Receiver(const pm::Machine& machine, fluent::Logger *logger) :
    key_set_(machine), logger_(logger), cache_(600), prev_ts_(0)
{
  auto push_record_param_key = [&](const std::string& tgt) {
    auto pid = machine.lookup_param_key(tgt);
    auto tp = new Target(tgt.substr(4), pid);
    this->targets_.push_back(tp);
  };

  push_record_param_key("DNS.question");
  push_record_param_key("DNS.answer");
  push_record_param_key("DNS.authority");
  push_record_param_key("DNS.additional");
}



void Receiver::recv(const pm::Property& p) {
  static const int TIMEOUT = 60;

  auto is_query = (p.value(this->key_set_.is_query_).uint() == 1);
  auto key = new Key(p, this->key_set_);
                         
  // search expired query
  if (this->prev_ts_ > 0 && p.ts() > this->prev_ts_) {
    this->cache_.step(p.ts() - this->prev_ts_);
    while (this->cache_.has_expired()) {
      auto expired_msg = this->cache_.pop_expired();
      this->logger_->emit(expired_msg);
    }
  }
  this->prev_ts_ = p.ts();

  fluent::Message *msg = nullptr;

  if (is_query) {
    // Query
    auto node = this->cache_.get(*key); // If exists, auto upadted
    if (node.is_null()) {
      msg = this->logger_->retain_message("dns-gazer.dns.tx");
      msg->set_ts(p.ts());

      this->build_message(msg, p, this->key_set_);
      this->cache_.put(TIMEOUT, *key, msg);
      msg = nullptr;
    }
  } else {
    // Reply
    if (this->cache_.has(*key)) {
      auto dn = this->cache_.get(*key);
      msg = dn.data();
      msg->set_ts(p.ts());
      this->cache_.remove(*key);
      this->build_message(msg, p, this->key_set_);
    } else {
      msg = this->logger_->retain_message("dns-gazer.dns.tx");
      msg->set_ts(p.ts());
      // without_query = true
      this->build_message(msg, p, this->key_set_, true);
    }
  }

  if (msg) {
    this->logger_->emit(msg);
  }

  delete key;  
}

}
