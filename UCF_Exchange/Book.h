#pragma once
#include <stdlib.h>
#include <deque>
#include <vector>
#include <algorithm>

enum class level_id_t : uint32_t {};
#define MEMORY_DEFS    \
  using __ptr = ptr_t; \
  using size_t__ = typename std::underlying_type<ptr_t>::type;

template <class T, typename ptr_t, size_t SIZE_HINT>
class Pool
{
public:
	MEMORY_DEFS;
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

struct Trade {
	uint32_t qty;
	double price;
	uint32_t buy_id;
	uint32_t sell_id;
	Trade(uint32_t _qty, double _price, uint32_t _buy, uint32_t _sell) : qty(_qty), price(_price), buy_id(_buy), sell_id(_sell) {}
};

struct Order {
	bool isBuy;
	uint32_t qty;
	level_id_t level_idx;
	uint32_t book_id;
	uint32_t trader_id;
	Order(bool _isBuy, uint32_t _qty, uint32_t _book_id, uint32_t _trader_id)
		: isBuy(_isBuy), qty(_qty), book_id(_book_id), trader_id(_trader_id) {}
};

struct Level{
	double price;
	uint32_t qty;
	std::deque<uint32_t> orders;
	uint32_t cancelOrder(uint32_t order_id, uint32_t qty);
};

struct PriceLevel {
	double price;
	level_id_t level_idx;
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
	Book();
	~Book();
	static LevelPool levelPool; 
	static const Orders* orders; // reference of global pool of orders in the exchange
	Levels bids;
	Levels asks;
	uint32_t processOrder(uint32_t order_id, Order* order, double price); // execute any crossed qty in the order, then add leftover qty as a new order
	uint32_t cancelOrder(uint32_t order_id, Order* order);
private:
	Trades toNotify;
	double best_ask; // best ask price in the market
	double best_bid; // best bid price in the market
	uint32_t addOrder(uint32_t order_id, Order* order, double price); // add an order to the exchange, should only be called by processOrders
	uint32_t crossAsk(uint32_t order_id, Order* order, double price); // cross a
	uint32_t crossBid(uint32_t order_id, Order* order, double price);
};