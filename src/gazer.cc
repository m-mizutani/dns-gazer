#include <packetmachine.hpp>
#include <fluent.hpp>

#include "optparse.hpp"
#include "utils.hpp"
#include "dns.hpp"


const std::string VERSION = "0.1.0";

int main(int argc, char* argv[]) {
  optparse::OptionParser psr = optparse::OptionParser();
  
  psr.add_option("-r").dest("pcap_file").help("read pcap format file");
  psr.add_option("-i").dest("interface").help("capture interface");
  psr.add_option("-f").dest("fluentd")
      .help("Fluentd destination, e.g. 127.0.0.1:24224");
  psr.add_option("-o").dest("output")
      .help("Output to a file as msgpack foramt, '-' is stdout");
  psr.add_option("-t").dest("text_output")
      .help("Output to a file as text foramt, '-' is stdout");
  psr.add_option("-p").dest("pid_path").help("pid file path");
  psr.add_option("-d").dest("demon_mode").action("store_true")
      .help("Enable demon mode");
  psr.add_option("-R").dest("log_record").action("store_true")
      .help("Enable logging per record");
  psr.add_option("-v").dest("version").action("store_true")
      .help("Show version");
  

  auto opt = psr.parse_args(argc, argv);
  std::vector <std::string> args = psr.args();

  // Demonize at first if needed
  if (opt.get("demon_mode")) {
    gazer::demonize();
  }

  pm::Config config;
  config.set_false("TCP.enable_session_mgmt");  
  auto machine = new pm::Machine(config);
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

  if (opt.is_set("text_output")) {
    auto& fpath = opt["text_output"];
    if (fpath == "-") {
      logger->new_textfile(1); // stdout
    } else {
      logger->new_textfile(fpath);
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

  if (opt.is_set("pid_path")) {
    gazer::create_pid_file(opt["pid_path"]);
  }
  
  // Creating DNS packet receiver.
  auto dns = new dns::Receiver(*machine, logger);

  // Enable per record logging
  if (opt.is_set("log_record")) {
    dns->set_logging_record(opt.get("log_record"));
  }

  
  machine->on("DNS", [&](const pm::Property& p) {
      dns->recv(p);
    });
  machine->loop();

  delete dns;
  delete logger;
  delete machine;
  return 0;
}
