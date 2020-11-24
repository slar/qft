#include <quickfix/Application.h>
#include <quickfix/FileLog.h>
#include <quickfix/FileStore.h>
#include <quickfix/Session.h>
#include <quickfix/SocketAcceptor.h>

#include <chrono>
#include <thread>

#include "app.h"

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
  try {
    FIX::SessionSettings settings(argv[1]);

    App app("/usr/local/share/quickfix/FIX44.xml", std::stoi(argv[2]));
    FIX::FileStoreFactory storeFactory(settings);
    FIX::FileLogFactory logFactory(settings);
    FIX::SocketAcceptor acceptor(app, storeFactory, settings, logFactory);
    acceptor.start();

    std::this_thread::sleep_for(1h);
  } catch (std::exception &e) {
    std::clog << e.what();
  }
  return 0;
}
