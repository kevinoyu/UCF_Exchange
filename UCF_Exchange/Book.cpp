#include "book.h"
Book::Book()
{
}


Book::~Book()
{
}


uint32_t Book::crossAsk(uint32_t order_id, Order * order, double price)
{
	uint32_t qty = order->qty;
	auto iter = asks.begin();
	bool exhausted = false;
	while (iter++ != (asks.end() - 1)) {
		PriceLevel &cur = *iter;
		Level cur_level = levelPool[cur.level_idx];
		uint32_t trade_qty = 0;
		double trade_price = cur_level.price;
		while (qty > 0 and cur_level.orders.size() > 0) {
			uint32_t ask_id = cur_level.orders.pop_front();
			Order* order = getOrder(order_id);
			trade_qty = -order->qty;
			if (trade_qty >= qty) {
				order->qty += qty;
				toNotify.emplace(qty, cur_level.price, order_id, ask_id);
				cur_level.orders.push_front(ask_id);
				qty = 0;
				exhausted = true;
				break;
			}
			else {
				qty -= order->qty;
				toNotify.emplace(order->qty, cur_level.price, order_id, ask_id);
				cur_level.cancelOrder(ask_id);
			}
		}

		if (exhausted || cur.price > price) {
			break;
		}
	}
}

uint32_t Book::crossBid(uint32_t order_id, Order * order, double price)
{
	uint32_t qty = -order->qty;
	auto iter = asks.end();
	bool exhausted = false;
	while (iter-- != asks.begin()) {
		PriceLevel &cur = *iter;
		Level cur_level = levelPool[cur.level_idx];
		uint32_t trade_qty = 0;
		double trade_price = cur_level.price;
		while (qty > 0 and cur_level.orders.size() > 0) {
			uint32_t ask_id = cur_level.orders.pop_front();
			Order* order = getOrder(order_id);
			trade_qty = -order->qty;
			if (trade_qty >= qty) {
				order->qty += qty;
				toNotify.emplace(qty, cur_level.price, order_id, ask_id);
				cur_level.orders.push_front(ask_id);
				qty = 0;
				exhausted = true;
				break;
			}
			else {
				qty -= order->qty;
				toNotify.emplace(order->qty, cur_level.price, order_id, ask_id);
				cur_level.cancelOrder(ask_id);
			}
		}

		if (exhausted || cur.price > price) {
			break;
		}
	}
	return qty;
}


Order * Book::getOrder(uint32_t order_id)
{
	return (*orders)[order_id];
}

uint32_t Book::processOrder(uint32_t order_id, Order * order, double price)
{
	int32_t qty = order->qty;
	if (isBid && price >= best_ask) {
		qty = crossAsk(order_id, order, price);
	}
	else if (!isBid && price <= best_bid) {
		qty = crossBid(order_id, order, price);
	}

	if (qty > 0) {
		order->qty = qty;
		addOrder(order_id, order, price);
	}
	return qty;
}

uint32_t Book::cancelOrder(uint32_t order_id, uint32_t level_idx)
{
	Order* order = getOrder(order_id);
	bool isBid = order->qty > 0;
	int32_t toRet = levelPool[level_idx].cancelOrder(order_id);
	if (levelPool[level_idx].qty == 0) {
		if (isBid) {
			auto iter = bids.end() {
				if (iter->price == )
			}
		}
	}
	return toret;
}

uint32_t Book::addOrder(uint32_t order_id, Order* order, double price)
{
	qty = order->qty;
	bool isBid = qty > 0;
	Levels* levels = isBid ? &bids : &asks;
	bool found = false;
	auto iter = levels->end();
	if (isBid) {
		while (iter-- != levels->begin()) {
			PriceLevel &cur = *iter;
			if (cur.price == price) {
				order->level_idx = cur.level_idx;
				found = true;
			}
			else if (price > cur.price) {
				break;
			}
		}
	}
	else {
		qty *= -1; 
		iter = levels->begin();
		auto last = levels->end() - 1
		while (iter++ != last) {
			PriceLevel &cur = *iter;
			if (cur.price == price) {
				order->level_idx = cur.level_idx;
				found = true;
			}
			else if (price > cur.price) {
				break;
			}
		}
	}

	if (!found) {
		order->level_idx = levelPool.alloc();
		levelPool[order->level_idx].qty = 0;
		levelPool[order->level_idx].price = price;
		PriceLevel const pl(price, order->level_idx);
		iter += isBid ? 1 : -1;
		if (iter == asks.begin()) best_ask = price;
		else if (iter == (bids.last() - 1)) best_bid = price; 
		levels->insert(iter, pl);
	}
	levelPool[order->level_idx].qty += order->qty;
	levelPool[order->level_idx].orders.push_back(order_id)
}

uint32_t Level::cancelOrder(uint32_t order_id) {
	Order* order = getOrder(order_id);
	oqty = order->qty;
	oqty *= oqty > 1 ? 1 : -1;
	auto it = std::find(orders.begin(), orders.end(), order_id);
	if (it != orders.end()) {
		order->qty = 0;
		orders.erase(it);
		this->qty -= oqty;
		return 0;
	}
	else {
		return -3; 
	}
}
