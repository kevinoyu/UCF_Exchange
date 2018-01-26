#pragma once

#include "Trader.h"

#include <string>
#include <unordered_map>
#include <vector>

#include "document.h"
#include "mpsc.h"
#include "uWS.h"

typedef std::vector<Book> Books;

typedef enum MessageType
{
  REGISTER = 'R',
  ORDER = 'M',
  CANCEL = 'C'
} MessageType;


struct Message
{
  MessageType type;
  std::string security;
  uint32_t order_id;
  int qty;
  double price;
  uint32_t trader_id;
  Message() {};
  Message(MessageType type, std::string sec, uint32_t oid,
      int qty, double price, uint32_t tid)
  : type(type)
  , security(sec)
  , order_id(oid)
  , qty(qty)
  , price(price)
  , trader_id(tid) {};
};

struct ReturnMessage
{
  uint32_t trader_id;
  std::string msg;
};

typedef mpsc_queue_t<Message> MessageQueue;
typedef mpsc_queue_t<ReturnMessage> ReturnQueue;
enum class MessageQueue_id_t : uint32_t {};
typedef Pool<MessageQueue, MessageQueue_id_t, 100> MessageQueuePool;

struct Security
{
  uint32_t book_id;
  MessageQueue_id_t messages;
  Security() : book_id(-1) {};
  Security(uint32_t id);
  ~Security();
};

typedef std::unordered_map<std::string, Security> SecurityMap;
typedef std::unordered_map<const char *, uint32_t> TraderMap; 

class Exchange
{
public:
  Exchange() : orders(Orders()), oid(0), sid(0), tid(0), traders(Traders()), sec_map(SecurityMap()), books(Books()) {};
  Exchange(std::string fileName);
  ~Exchange();
  static MessageQueuePool mqp; // pool for order queues to be used by individual securities; preserves locality of queues for each thread handling a block of securities

  bool acceptingConnections;
  SecurityMap sec_map;
  TraderMap t_map; 
  Books books; // Vector of books, each representing a security
  Orders orders;
  Traders traders;
  uint32_t sec_count;
  uint32_t oid;
  uint32_t sid;
  uint32_t tid;
  uWS::Hub h;

  ErrorCode registerTrader(const char * address);
  ErrorCode registerSecurity(std::string sec_name);
  ErrorCode addOrder(const uint32_t& book_id, const Message& msg);
  ErrorCode cancelOrder(const uint32_t& book_id, const Message& msg);
  void initFromFile(std::string filename);
  void run(uint32_t port);
  void handleBooks(uint32_t num, uint32_t sequence); // handle num # of contiguous books
};
