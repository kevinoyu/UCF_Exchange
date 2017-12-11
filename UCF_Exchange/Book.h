#pragma once

#include <algorithm>
#include <deque>
#include <stdlib.h>
#include <string>
#include <vector>

typedef enum ErrorCode
{
  OK = 0,
  ID_MISMATCH = -1,
  INVALID_ORDER = -2,
  INVALD_PRICE = -3,
  INVALID_SECURITY = -4,
  EXCEEDS_LIMITS = -5
} ErrorCode;

/*
Dynamic pool allocator, used to keep levels adjacent in order to exploit cache coherency for orderbook updates
*/
enum class level_id_t : uint32_t {};
template <class T, typename ptr_t, size_t SIZE_HINT>
class Pool
{
public:
  using __ptr = ptr_t;
  using size_t__ = typename std::underlying_type<ptr_t>::type;
  std::vector<T> m_allocated;
  std::vector<ptr_t> m_free;
  Pool() { m_allocated.reserve(SIZE_HINT); }
  Pool(size_t reserve_size) { m_allocated.reserve(reserve_size); }
  T *get(ptr_t idx) { return &m_allocated[size_t__(idx)]; }
  T &operator[](ptr_t idx) { return m_allocated[size_t__(idx)]; }
  __ptr alloc(void)
  {
    if (m_free.empty()) {
      auto ret = __ptr(m_allocated.size());
      m_allocated.push_back(T());
      return ret;
    }
    else {
      auto ret = __ptr(m_free.back());
      m_free.pop_back();
      return ret;
    }
  }
  void free(__ptr idx) { m_free.push_back(idx); }
};

// Struct containing information for trade notifications
struct Trade {
  uint32_t qty;
  double price;
  uint32_t buy_id;
  uint32_t sell_id;
  Trade(uint32_t _qty, double _price, uint32_t _buy, uint32_t _sell) : qty(_qty), price(_price), buy_id(_buy), sell_id(_sell) {}
};

// Struct representing order metadata
//    ID is implied by position in the exchange order pool to save space
struct Order {
  bool isBuy;
  uint32_t qty;
  level_id_t level_idx;
  uint32_t book_id;
  uint32_t trader_id;
  Order(bool _isBuy, uint32_t _qty, uint32_t _book_id, uint32_t _trader_id)
    : isBuy(_isBuy), qty(_qty), book_id(_book_id), trader_id(_trader_id) {}
};

// struct represent Price Level metadata
struct Level{
  double price;
  uint32_t qty;
  std::deque<uint32_t> orders;
  ErrorCode cancelOrder(uint32_t order_id, uint32_t qty);
};

// struct pointing to Levels - used to exploit cache coherency
struct PriceLevel {
  double price; // store local copy of price to save need for dereference
  level_id_t level_idx; // idx of level metadata in the shared level pool
  PriceLevel(double _price, level_id_t _idx) : price(_price), level_idx(_idx) {}
};

bool operator>(Level a, Level b);
bool operator>(PriceLevel a, PriceLevel b);

typedef std::vector<PriceLevel> Levels;
typedef std::vector<Order> Orders;
typedef std::vector<Trade> Trades;
typedef Pool<Level, level_id_t, 1 << 20> LevelPool;


class Book
{
public:
  Book() : best_ask(100000), best_bid(0) {};
  Book(Orders* o) : best_ask(100000), best_bid(0), orders(o) {};
  Book(Orders* o, std::string sec_name) : sec_name(sec_name), best_ask(100000), best_bid(0), orders(o) {};
  ~Book();
  std::string sec_name;
  static LevelPool levelPool; // pointer to global pool of PriceLevels
  ErrorCode processOrder(uint32_t order_id, Order* order, double price); // execute any crossed qty in the order, then add leftover qty as a new order
  ErrorCode cancelOrder(uint32_t order_id, Order* order); // cancel an order, and remove level if that was last order in the level
private:
  Levels bids; // vector of bids, sorted from best price to worst
  Levels asks; // vector of asks, sorted from best price to worst
  Trades toNotify;
  double best_ask; // best ask price in the market
  double best_bid; // best bid price in the market
  Orders* orders; // pointer to global pool of orders in the exchange
  ErrorCode addOrder(uint32_t order_id, Order* order, double price); // add an order to the exchange, should only be called by processOrders
  uint32_t crossAsk(uint32_t order_id, Order* order, double price); // cross asks until order can no longer be filled, return unfilled qty
  uint32_t crossBid(uint32_t order_id, Order* order, double price); // cross bids until order can no longer be filled, return unfilled qty
};
