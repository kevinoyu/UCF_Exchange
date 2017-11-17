#pragma once
#include <stdlib.h>
#include <deque>
#include <vector>
#include <algorithm>
#include "Pool.h"

struct Trade {
	uint32_t qty;
	double price;
	uint32_t buy_id;
	uint32_t sell_id;
	Trade(uint32_t _qty, double _price, uint32_t _buy, uint32_t _sell) : qty(_qty), price(_price), buy_id(_buy), sell_id(_sell) {}
};

struct Order {
	int32_t qty;
	uint32_t level_idx;
	uint32_t book_id;
	uint32_t trader_id;
	Order(uint32_t _qty, uint32_t _book_id, uint32_t _trader_id)
		: qty(_qty), level_idx(0), book_id(_book_id), trader_id(_trader_id) {}
};

struct Level{
	double price;
	uint32_t qty;
	std::deque<uint32_t> orders;
	uint32_t cancelOrder(uint32_t order_id);ca
};

struct PriceLevel {
	double price;
	uint32_t level_idx;
	PriceLevel(double _price, uint32_t _idx) : price(_price), level_idx(_idx) {}
};

bool operator>(Level a, Level b) {
	return a.price > b.price;
}

bool operator>(PriceLevel a, PriceLevel b) {
	return a.price > b.price;
}

typedef std::vector<PriceLevel> Levels;
typedef std::vector<Order> Orders;
typedef std::vector<Book> Books;
typedef std::vector<Trade> Trades;
typedef Pool<Level, uint32_t, 1 << 20> LevelPool;

class Book
{
public:
	Book();
	~Book();
	LevelPool levelPool; 
	Levels bids;
	Levels asks;
	Orders* orders; // pointer to global pool of orders in the exchange 
	uint32_t processOrder(uint32_t order_id, Order* order, double price); // execute any crossed qty in the order, then add leftover qty as a new order
	uint32_t cancelOrder(uint32_t order_id, uint32_t level_idx);
private:
	Trades toNotify;
	double best_ask;
	double best_bid;
	uint32_t addOrder(uint32_t order_id, Order* order, double price);
	uint32_t crossAsk(uint32_t order_id, Order* order, double price);
	uint32_t crossBid(uint32_t order_id, Order* order, double price);
	Order* getOrder(uint32_t order_id);
};