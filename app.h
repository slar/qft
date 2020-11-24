#include <quickfix/DataDictionary.h>
#include <quickfix/Message.h>
#include <quickfix/fix44/ExecutionReport.h>

#include <sstream>

class Readable {
public:
  Readable(const std::string &xmldef) : dict(xmldef) {}
  std::string operator()(const FIX::Message &msg) {
    std::stringstream ss;
    for (auto &m : {static_cast<FIX::FieldMap>(msg.getHeader()),
                    static_cast<FIX::FieldMap>(msg),
                    static_cast<FIX::FieldMap>(msg.getTrailer())}) {
      for (auto &field : m) {
        std::string name;
        std::string value;
        dict.getFieldName(field.getTag(), name);
        dict.getValueName(field.getTag(), field.getString(), value);

        ss << name << " (" << field.getTag() << ") = " << field.getString()
           << " (" << value << ")" << std::endl;
      }
    }
    return ss.str();
  }

private:
  FIX::DataDictionary dict;
};

class App : public FIX::NullApplication {
public:
  App(std::string xmldef, int seqModifier = 0)
      : readable(xmldef), m_seqModifier(seqModifier) {}
  FIX::Session *s;

  void onCreate(const FIX::SessionID &session) override {
    s = FIX::Session::lookupSession(session);
  }

  void onLogon(const FIX::SessionID &session) override {}

  void fromAdmin(const FIX::Message &msg, const FIX::SessionID &session) throw(
      FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue,
      FIX::RejectLogon) override {
    std::cout << "<: " << session << "\n" << readable(msg) << std::endl;
  }

  void toApp(FIX::Message &msg,
             const FIX::SessionID &session) throw(FIX::DoNotSend) override {
    std::cout << ">: " << session << "\n" << readable(msg) << std::endl;
  }

  void toAdmin(FIX::Message &msg, const FIX::SessionID &session) override {
    std::cout << ">: " << session << "\n" << readable(msg) << std::endl;
    if (msg.getHeader().getField(35) == "A") {
      if (m_seqModifier > 0) {
        s->setNextTargetMsgSeqNum(m_seqModifier);
      }
    }
  }

  void fromApp(const FIX::Message &msg, const FIX::SessionID &session) throw(
      FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue,
      FIX::UnsupportedMessageType) override {
    std::cout << "<: " << session << "\n" << readable(msg) << std::endl;
    auto msgType = msg.getHeader().getField(35);
    if (msgType == "D") {
      FIX44::ExecutionReport msgout(FIX::OrderID("456"), FIX::ExecID("123"),
                                    FIX::ExecType_NEW, FIX::OrdStatus_NEW,
                                    FIX::Side_BUY, FIX::LeavesQty(1),
                                    FIX::CumQty(1), FIX::AvgPx(1));
      s->send(msgout);
    }
  }

private:
  Readable readable;
  int m_seqModifier;
};
