#include "dns.hpp"

namespace dns {

Receiver::Receiver(fluent::Logger *logger) : logger_(logger) {
}

void Receiver::recv(const pm::Property& p) {
}

}
