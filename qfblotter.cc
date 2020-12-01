#include "app.h"
#include "httplib.h"

#include <quickfix/Application.h>
#include <quickfix/FileLog.h>
#include <quickfix/FileStore.h>
#include <quickfix/Session.h>
#include <quickfix/SocketAcceptor.h>

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
  try {
    FIX::SessionSettings settings(argv[1]);

    App app(argv[2], std::stoi(argv[3]));
    FIX::FileStoreFactory storeFactory(settings);
    FIX::FileLogFactory logFactory(settings);
    FIX::SocketAcceptor acceptor(app, storeFactory, settings, logFactory);
    acceptor.start();

    httplib::Server server;
    server.Get("/", [](const httplib::Request &req, httplib::Response &resp) {
      std::cout << "get" << std::endl;
      resp.set_content("{\"hello\":\"get\"}", "application/json");
    });
    server.Put("/", [](const httplib::Request &req, httplib::Response &resp) {
      std::cout << "Put" << std::endl;
      resp.set_content("{\"hello\":\"post\"}", "application/json");
      FIX::Message msg;
      msg.setField(35, "D");
      app.Send(msg)
    });
    server.listen("0.0.0.0", 8083);
  } catch (std::exception &e) {
    std::clog << e.what();
  }
  return 0;
}
