#define _CRT_SECURE_NO_WARNINGS  // avoid some unsafe functions from getopt.h
                                 // error

#define WIN32_LEAN_AND_MEAN  // boost on windows

#include <stdio.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "GraphServer.h"
#include "GraphServerConfig.h"
#include "getopt.h" /* getopt_long */

bool help_opt = false;

void parse_input_data(int argc, char **argv, GraphServerConfig &config);

int main(int argc, char **argv) {
  GraphServerConfig config;
  config.dir = "images";
  config.ip = "127.0.0.1";
  config.port = 1234;
  config.workers = 1;
  config.maxdegree = 100;
  config.maxnfiles = 10000;
  config.verbose = true;
  parse_input_data(argc, argv, config);


  if (!help_opt) {
    try {
      boost::thread_group threads_;  // thread_pool
      boost::asio::io_context io_context;
      boost::asio::io_context::work work_(io_context);

      // setting thread_pool tasks
      for (std::size_t i = 0; i < config.workers; ++i) {
        threads_.create_thread(
            boost::bind(&boost::asio::io_context::run, &io_context));
      }
      GraphServer server(io_context, config);  // create server and run it
      io_context.run();
    } catch (std::exception &e) {
      std::cerr << e.what() << endl;
    }
  }
  return 0;
}

void parse_input_data(int argc, char **argv, GraphServerConfig &config) {
  static struct option long_options[] = {
      {"dir", required_argument, 0, 'd'},
      {"ip", required_argument, 0, 'i'},
      {"port", required_argument, 0, 'p'},
      {"maxdegree", required_argument, 0, 'm'},
      {"maxnfiles", required_argument, 0, 'n'},
      {"workers", required_argument, 0, 'w'},
      {"verbose", no_argument, 0, 'v'},  // 0
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int c, option_index = 0;
  while (-1 != (c = getopt_long(argc, argv, "d:i:p:m:n:w:v:h", long_options,
                                &option_index))) {
    switch (c) {
      case 0:
        printf("option %s", long_options[option_index].name);
        if (optarg) printf(" with arg %s", optarg);
        printf("\n");
        switch (option_index) {
          case 0:
            config.dir = optarg;
            break;
          case 1:
            config.ip = optarg;
            break;
          case 2:
            config.port = std::stoi(optarg);
            break;
          case 3:
            config.maxdegree = std::stoi(optarg);
            break;
          case 4:
            config.maxnfiles = std::stoi(optarg);
            break;
          case 5:
            config.workers = std::stoi(optarg);
            break;
          case 6:
            config.verbose = true;  // optarg;
            break;
          case 7:
            help_opt = true;
            printf(
                "using:\n\t./exe -d|--dir <dir> -i|--ip <ip> -p|--port <port> "
                "-m|--maxdegree <uint> -n|--maxnfiles <uint> -w|--workers <num> "
                "[-v|--verbose ] [-h|--help <uint>]\n\n");
            break;
          case 8:
            printf(
                "using:\n\t./exe -d|--dir <dir> -i|--ip <ip> -p|--port <port> "
                "-m|--maxdegree <uint> -n|--maxnfiles <uint> -w|--workers "
                "<num> "
                "[-v|--verbose ] [-h|--help <uint>]\n\n");
            break;
        }
        break;

      case 'd':
        config.dir = optarg;
        break;
      case 'i':
        config.ip = optarg;
        break;
      case 'p':
        config.port = std::stoi(optarg);
        break;
      case 'm':
        config.maxdegree = std::stoi(optarg);
        break;
      case 'n':
        config.maxnfiles = std::stoi(optarg);
        break;
      case 'w':
        config.workers = std::stoi(optarg);
        break;
      case 'v':
        config.verbose = std::stoi(optarg);
        break;
      case 'h':
        help_opt = true;
        printf(
            "using:\n\t./exe -d|--dir <dir> -i|--ip <ip> -p|--port <port> "
            "-m|--maxdegree <uint> -n|--maxnfiles <uint> -w|--workers "
            "<num> "
            "[-v|--verbose ] [-h|--help <uint>]\n\n");
        break;

      case '?': /* getopt_long already printed an error message. */
        printf(
            "using:\n\t./exe -d|--dir <dir> -i|--ip <ip> -p|--port <port> "
            "-m|--maxdegree <uint> -n|--maxnfiles <uint> -w|--workers "
            "<num> "
            "[-v|--verbose ] [-h|--help <uint>]\n\n");
        break;

      default:
        printf("?? getopt returned character code 0%o ??\n", c);
        break;
    }
  }
  if (optind < argc) {
    printf("non-option argv elements: ");
    while (optind < argc) printf("%s ", argv[optind++]);
    printf("\n");
  }
}