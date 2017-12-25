#include <packetmachine.hpp>
#include <fluent.hpp>

#include "optparse.hpp"
#include "dns.hpp"


const std::string VERSION = "0.1.0";

int main(int argc, char* argv[]) {
  optparse::OptionParser psr = optparse::OptionParser();
  
  psr.add_option("-r").dest("pcap_file").help("read pcap format file");
  psr.add_option("-i").dest("interface").help("capture interface");
  psr.add_option("-f").dest("fluentd")
      .help("Fluentd destination, e.g. 127.0.0.1:24224");
  psr.add_option("-o").dest("output").help("Log file path, '-' is stdout");
  psr.add_option("-v").dest("version").action("store_true")
      .help("Show version");


  auto opt = psr.parse_args(argc, argv);
  std::vector <std::string> args = psr.args();

  auto machine = new pm::Machine();
  auto logger = new fluent::Logger();

  
  if (opt.get("version")) {
    std::cout << "Version: " << VERSION << std::endl;
    return 0;
  }


  // Configuring input data stream.
  if (opt.is_set("pcap_file")) {
    machine->add_pcapfile(opt["pcap_file"]);
  }

  if (opt.is_set("interface")) {
    machine->add_pcapdev(opt["interface"]);
  }

  
  if (opt.is_set("output")) {
    auto& fpath = opt["output"];
    if (fpath == "-") {
      logger->new_dumpfile(1); // stdout
    } else {
      logger->new_dumpfile(fpath);
    }
  }

  // Configuring output data stream (logging).
  if (opt.is_set("fluentd")) {
    auto& netloc = opt["fluentd"];
    size_t pos;

    if(std::string::npos == (pos = netloc.find(":", 0))) {
      // no port number in netloc
      logger->new_forward(netloc);
    } else {
      std::string host = netloc.substr(0, pos);
      std::string str_port = netloc.substr(pos + 1);

      char *e;
      unsigned int port = strtoul(str_port.c_str(), &e, 0);
      if (*e != '\0') {
        throw std::runtime_error("Invalid port number: " + str_port);
      }

      logger->new_forward(host, port);
    }
  }

  // Creating DNS packet receiver.
  auto dns = new dns::Receiver(*machine, logger);
  
  machine->on("DNS", [&](const pm::Property& p) {
      dns->recv(p);
    });
  machine->loop();

  delete dns;
  delete logger;
  delete machine;
  return 0;
}
