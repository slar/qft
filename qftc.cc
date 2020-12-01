#include <quickfix/Application.h>
#include <quickfix/FileLog.h>
#include <quickfix/FileStore.h>
#include <quickfix/Session.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/fix44/NewOrderSingle.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "app.h"

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
  try {
    FIX::SessionSettings settings(argv[1]);

    App app(argv[2], std::stoi(argv[3]));
    FIX::FileStoreFactory storeFactory(settings);
    FIX::FileLogFactory logFactory(settings);
    FIX::SocketInitiator initiator(app, storeFactory, settings, logFactory);
    initiator.start();

    std::this_thread::sleep_for(5s);
    FIX44::NewOrderSingle msg(FIX::ClOrdID("100"), FIX::Side(FIX::Side_BUY),
                              FIX::TransactTime(),
                              FIX::OrdType(FIX::OrdType_MARKET));
    app.s->send(msg);
    std::this_thread::sleep_for(1h);
  } catch (std::exception &e) {
    std::clog << e.what();
  }

  return 0;
}
