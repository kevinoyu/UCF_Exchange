#include "Exchange.h"
#include <string>

Exchange::Exchange(std::string fileName) {

}

Exchange::~Exchange()
{
}

MessageQueuePool Exchange::mqp = MessageQueuePool();

ErrorCode Exchange::registerTrader()
{
  return ErrorCode::OK;
}

ErrorCode Exchange::registerSecurity(std::string sec_name)
{
  books.emplace_back(&orders, sec_name);
  this->sec_map.insert(Secmap::value_type(sec_name, Security(sid)));
  sid++;
  return ErrorCode::OK;
}

ErrorCode Exchange::addOrder(const uint32_t& book_id, const Message& msg)
{
  ErrorCode trader_status = traders[msg.trader_id].addOrder(oid);
  if (trader_status == ErrorCode::OK) {
    ErrorCode book_status = books[book_id].processOrder(oid, &orders[oid], msg.price);
    if (book_status == ErrorCode::OK) {
      orders.emplace_back(msg.qty > 0, uint32_t(std::abs(msg.qty)), book_id, msg.trader_id);
      oid++;
    }
    return book_status;
  }
  return trader_status;
}

ErrorCode Exchange::cancelOrder(const uint32_t& book_id, const Message& msg)
{
  Order* order = &(orders[msg.order_id]);
  if (msg.trader_id != order->trader_id) return ErrorCode::ID_MISMATCH;
  else {
    return books[book_id].cancelOrder(msg.order_id, order);
  }
}

void Exchange::initFromFile(std::string filename)
{

}

void Exchange::run(uint32_t port) {
  h.onMessage([this](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
    rapidjson::Document d;
    d.Parse(message);
    const char* raw_type = d["message_type"].GetString();

    switch (raw_type[0])
    {
    case 'R':
      registerTrader();
    case 'M':
    {
      rapidjson::Value& orders = d["orders"];
      rapidjson::Value& cancels = d["cancels"];

      uint32_t trader_id = d["token"].GetInt();

      if(!orders.IsArray()) { return; }
      for(auto& ord : orders.GetArray())
      {
        std::string sec_name = ord["ticker"].GetString();
        Message msg(
            MessageType::ORDER,
            sec_name,
            0,
            ord["quantity"].GetInt(),
            ord["price"].GetDouble(),
            trader_id);
        MessageQueue* mqueue = Exchange::mqp.get(sec_map[sec_name].messages);
        mqueue->enqueue(msg);
      }

      if(!cancels.IsArray()) { return; }
      for (auto& ord : cancels.getArray())
      {
        std::string sec_name = ord["ticker"].GetString();
        Message msg(
            MessageType::CANCEL,
            sec_name,
            ord["order_id"].GetString(),
            -1,
            -1,
            trader_id);
        MessageQueue* mqueue = Exchange::mqp.get(sec_map[sec_name].messages);
        mqueue->enqueue(msg);
      }
    }
    default:
      return;
    }

  });

  h.onConnection([this](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) {

  });
}

void Exchange::handleBooks(uint32_t num, uint32_t sequence) {
  while (true) {
    for (uint32_t i = sequence; i < sequence + num; i++) {
      Book book = books[i];
      MessageQueue_id_t mqid = sec_map[books.sec_name].messages;
      MessageQueue* mqueue = Exchange::mqp.get(messages);
      if(mqueue == nullptr) { continue; }
      // Handle orders in the given book
      Message msg;
      while(mqueue->dequeue(msg)) {
        switch(msg.type) {
        case MessageType::REGISTER:
          ErrorCode exchange_status = registerTrader();
          break;
        case MessageType::ORDER:
          ErrorCode exchange_status = addOrder(i, msg);
          break;
        case MessageType::CANCEL:
          ErrorCode exchange_status = cancelOrder(i, msg);
          break;
        default:
          break;
        }
      }
    }
  }
}

Security::Security(uint32_t id)
{
  book_id = id;
  messages = Exchange::mqp.alloc();
}

Security::~Security()
{
  Exchange::mqp.free(messages);
}
