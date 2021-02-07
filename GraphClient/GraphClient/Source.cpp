#include <boost/asio.hpp>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <fstream>

#include <windows.h>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;
using std::string;
using std::vector;


void test_request_response(const string msg) {

  boost::asio::io_context io_context;

  // socket creation
  tcp::socket socket(io_context);
  // connection
  socket.connect(
      tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
   
  boost::system::error_code error;
  boost::asio::write(socket, boost::asio::buffer(msg), error);
  if (!error) {
    cout << "Client sent message! " << msg << endl;
  } else {
    cout << "send failed: " << error.message() << endl;
  }

  // response from server
  boost::asio::streambuf receive_buffer;
  boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
  if (error && error != boost::asio::error::eof) {
    cout << "receive failed: " << error.message() << error.value() << endl;
  } else {
    const char* data =
        boost::asio::buffer_cast<const char*>(receive_buffer.data());
    
    string data_str(data);
    size_t bsn_pos = data_str.find('\n');

    cout << data_str.substr(0, bsn_pos + 1); // print returned result

    std::istringstream ss(msg);
    std::string command;
    std::getline(ss, command, ' ');


    if (command == "paint") {
      if (data_str.find("Ok") != string::npos) {
        std::string filename;
        std::getline(ss, filename, ' ');

        std::ofstream fs(filename + ".bmp", std::ios::out | std::ios::binary);
        fs.write(data + bsn_pos + 1, receive_buffer.size() - 1 - bsn_pos);
        fs.close();
      }
    } else if (command == "get") {
      if (data_str.find("Ok") != string::npos) {
        std::string filename;
        std::getline(ss, filename, ' ');

        std::ofstream fs(filename + ".bmp", std::ios::out | std::ios::binary);
        fs.write(data + bsn_pos + 1, receive_buffer.size() - 1 - bsn_pos);
        fs.close();
      }
    } else if (command == "list") {      
      if (data_str.find("Ok") != string::npos) {
        cout << data_str.substr(bsn_pos + 1,
                                receive_buffer.size() - 1 - bsn_pos);
      }
    } 

  }
}



int main(int argc, char** argv) {
  setlocale(LC_ALL, "Russian");
  string msg = "paint gr H=100 W=100 2 0 0 1 [-50.0, 50.0]";
  test_request_response(msg);
  msg = "list";
  test_request_response(msg);
  msg = "get gr";
  test_request_response(msg);
  msg = "rem gr";
  test_request_response(msg);
  

  // test many requests
  std::vector<std::thread> threads;
  const size_t USER_NUM = 50;

  for (int i = 0; i < USER_NUM; i++) {
    vector<string> requests;

    threads.push_back(std::thread(
        &test_request_response, "paint gr" + std::to_string(i) + " H=100 W=100 2 0 0 1 [-50.0, 50.0]"));
  }

  for (int i = 0; i < USER_NUM; i++) {
    threads.at(i).join();
  }
    
  return 0;
}

 