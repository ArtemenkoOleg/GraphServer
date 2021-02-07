#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <vector>

#include "Graph.h"
#include "GraphServerConfig.h"

#include <iostream>
#include <cstdio>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;


std::mutex mutex_;

size_t num_files;
size_t maxdegree;
size_t maxnfiles;
string dir;
bool VERBOSE;



class con_handler : public boost::enable_shared_from_this<con_handler> {
 public:
  using ptr_to_connection = boost::shared_ptr<con_handler>;


  explicit con_handler(boost::asio::io_context& io_context)
      : socket_(io_context) {}


  static ptr_to_connection create(boost::asio::io_context& io_context) {
    return ptr_to_connection(new con_handler(io_context));
  }


  tcp::socket& socket() { return socket_; }


  void start() {
    socket_.async_read_some(
        boost::asio::buffer(in_message, BUFFER_SIZE),
        boost::bind(&con_handler::handle_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }


  void handle_read(const boost::system::error_code& err,
                                size_t bytes_transferred) {
    if (!err) {
      if (VERBOSE) {
        cout << "Server recieved from client: " << in_message << endl;
      }
      string send_str = parse_command_str(in_message);
      send_str += "\n";
      socket_.async_write_some(
          boost::asio::buffer(send_str, send_str.size()),
          boost::bind(&con_handler::handle_write, shared_from_this(),
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
      //cout << out_message.size();
    } else {
      std::cerr << "error: " << err.message() << std::endl;
      socket_.close();
    }
  }

  void handle_write(const boost::system::error_code& err,
                                 size_t bytes_transferred) {
    if (!err) {
      if (VERBOSE) {
        cout << "Server successfully sent to the client: "
             << endl;
      }
    } else {
      std::cerr << "error: " << err.message() << endl;
      socket_.close();
    }
  }


 private:
  static const size_t BUFFER_SIZE = 1024;  /// fixed size buffer
  tcp::socket socket_;
  char in_message[BUFFER_SIZE];
  std::string out_message;


  string parse_command_str(std::string str) {
    std::istringstream ss(str);
    std::string token;

    std::getline(ss, token, ' ');
    if (VERBOSE) {
      cout << "Command is " << token << ". Parsing command..." << endl;
    }

    if (token == "paint") {
      if (num_files >= maxnfiles) {
        return "Error too much files, max is " + std::to_string(maxnfiles) +
               "\n";
      }

      
      string filename;
      std::getline(ss, filename, ' ');

      std::getline(ss, token, ' ');
      size_t height = std::stoi(token.substr(2));

      std::getline(ss, token, ' ');
      size_t width = std::stoi(token.substr(2));

      std::getline(ss, token, ' ');
      size_t degree = std::stoi(token);

      if (degree > maxdegree) {
        return "Error degree is too large, max degree is " +
               std::to_string(maxdegree) + "\n";
      }

      vector<double> coeffs(degree + 1);
      for (int i = 0; i <= degree; i++) {
        std::getline(ss, token, ' ');
        coeffs[i] = std::stod(token);
      }
      

      double from, to;

      std::getline(ss, token, ',');

      from = std::stod(token.substr(2));

      std::getline(ss, token);
      to = std::stod(token.substr(0, token.size() - 1));

      

      Graph graph(dir, filename, coeffs, from, to);
      num_files++;
      auto vec = graph.get_data_binary();
      string s(vec.begin(), vec.end());
      return "Ok\n" + s;

    } else if (token == "get") {
      string filename;
      std::getline(ss, filename, ' ');
      string full_path = dir + "\\" + filename + ".bmp\0";
      if (IsPathExist(full_path)) {
        auto vec = Graph::get_data_binary(dir, filename);
        string s(vec.begin(), vec.end());
        return "Ok\n" + s;
      }
      return "Error file " + filename + " cannot be got\n";

    } else if (token == "rem") {
      string filename;
      std::getline(ss, filename, ' ');

      string full_path = dir + "\\" + filename + ".bmp\0";

      if (remove(full_path.c_str()) != 0) {
        return "Error file " + filename + " cannot be deleted\n";
      } else {
        num_files--;
        return "Ok file " + filename + " is successfully deleted\n";
      }
      
    } else if (token == "list") {
      string result = "";
      for (const auto& entry :
           std::filesystem::directory_iterator::directory_iterator(dir)) {
        result += entry.path().filename().generic_string() + " ";
      }
      return "Ok\n" + result.substr(0, result.size() - 1);
    }
    return "";
  }
    
};


class GraphServer {

 public:

  GraphServer(boost::asio::io_context& io_context, GraphServerConfig config)
      : acceptor_(
            io_context,
            tcp::endpoint(boost::asio::ip::address::from_string(config.ip),
                          config.port)),
        io_context_(io_context)
  {
    start_accept();
    maxdegree = config.maxdegree;
    maxnfiles = config.maxnfiles;
    dir = config.dir;
    VERBOSE = config.verbose;
  }

  void handle_accept(con_handler::ptr_to_connection connection,
                     const boost::system::error_code& err) {
    if (!err) {
      connection->start();
    }
    start_accept();
  }

 private:
  tcp::acceptor acceptor_;
  io_context& io_context_;


  void start_accept() {
    // socket
    con_handler::ptr_to_connection connection =
        con_handler::create(io_context_);

    // asynchronically accept user response
    acceptor_.async_accept(
        connection->socket(),
        boost::bind(&GraphServer::handle_accept, this, connection,
                    boost::asio::placeholders::error));
  }
};