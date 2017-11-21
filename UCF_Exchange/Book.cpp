#include "book.h"

Book::~Book()
{
}

LevelPool Book::levelPool = LevelPool();

uint32_t Book::crossAsk(uint32_t order_id, Order * order, double price)
{
	uint32_t qty = order->qty;
	bool exhausted = false;
	for (auto iter = asks.begin(); iter != asks.end(); ++iter) {
		PriceLevel &cur = *iter;
		if (exhausted || cur.price > price) break;
		Level cur_level = levelPool[cur.level_idx];
		uint32_t trade_qty = 0;
		double trade_price = cur_level.price;
		while (cur_level.orders.size() > 0) {
			uint32_t ask_id = cur_level.orders.front();
			Order ask_order = (*orders)[ask_id];
			trade_qty = ask_order.qty;
			best_ask = cur.price;
			if (trade_qty > qty) {
				ask_order.qty -= qty;
				qty = 0;
				toNotify.emplace_back(qty, cur_level.price, order_id, ask_id);
				exhausted = true;
				break;
			}
			else {
				qty -= order->qty;
				toNotify.emplace_back(ask_order.qty, cur_level.price, order_id, ask_id);
				cur_level.cancelOrder(ask_id, trade_qty);
			}
		}

	}
	return qty;
}

uint32_t Book::crossBid(uint32_t order_id, Order * order, double price)
{
	uint32_t qty = order->qty;
	bool exhausted = false;
	for (auto iter = asks.end(); iter != asks.begin();) {
		PriceLevel &cur = *--iter;
		if (cur.price < price || exhausted) break;
		Level cur_level = levelPool[cur.level_idx];
		uint32_t trade_qty = 0;
		double trade_price = cur_level.price;
		while (qty > 0 && cur_level.orders.size() > 0) {
			uint32_t bid_id = cur_level.orders.front();
			Order bid_order = (*orders)[bid_id];
			trade_qty = bid_order.qty;
			best_bid = cur.price;
			if (trade_qty > qty) {
				bid_order.qty -= qty;
				qty = 0;
				toNotify.emplace_back(qty, cur_level.price, order_id, bid_id);
				exhausted = true;
				break;
			}
			else {
				qty -= bid_order.qty;
				toNotify.emplace_back(bid_order.qty, cur_level.price, order_id, bid_id);
				cur_level.cancelOrder(bid_id, trade_qty);
			}
		}
	}
	return qty;
}

ErrorCode Book::processOrder(uint32_t order_id, Order * order, double price)
{
	int32_t qty = order->qty;
	if (order->isBuy && price >= best_ask) {
		qty = crossAsk(order_id, order, price);
		if (qty > 0) best_bid = price;
	}
	else if (!order->isBuy && price <= best_bid) {
		qty = crossBid(order_id, order, price);
		if (qty > 0) best_ask = price;
	}
	if (qty > 0) {
		order->qty = qty;
		addOrder(order_id, order, price);
	}
	return ErrorCode::OK;
}

ErrorCode Book::cancelOrder(uint32_t order_id, Order* order)
{
	ErrorCode toRet = levelPool[order->level_idx].cancelOrder(order_id, order->qty);
	if (levelPool[order->level_idx].orders.size() == 0) {
		Levels *levels = order->isBuy ? &bids : &asks;
		for (auto iter = levels->end(); iter-- != levels->begin();) {
			if (iter->price == levelPool[order->level_idx].price) {
				levels->erase(iter);
				levelPool.free(order->level_idx);
				break;
			}
		}	
	}
	return toRet;
}

ErrorCode Book::addOrder(uint32_t order_id, Order* order, double price)
{
	bool found = false;
	bool better = false;

	if (order->isBuy)
	{
		auto cur = bids.begin();
		for (; cur != bids.end(); ++cur) {
			if (cur->price == price) {
				order->level_idx = cur->level_idx;
				found = true;
				break;
			}
			else if (price > cur->price) break;
			better = true;
		}
		if (!found) {
			order->level_idx = levelPool.alloc();
			levelPool[order->level_idx].qty = 0;
			levelPool[order->level_idx].price = price;
			bids.emplace(cur, price, order->level_idx);
			if (!better) best_bid = price;
		}
		levelPool[order->level_idx].qty += order->qty;
		levelPool[order->level_idx].orders.push_back(order_id);
	}
	else 
	{
		auto cur = asks.begin();
		for (; cur != asks.end(); ++cur) {
			if (cur->price == price) {
				order->level_idx = cur->level_idx;
				found = true;
				break;
			}
			else if (price < cur->price) break;
			better = true;
		}
		if (!found) {
			order->level_idx = levelPool.alloc();
			levelPool[order->level_idx].qty = 0;
			levelPool[order->level_idx].price = price;
			asks.emplace(cur, price, order->level_idx);
			if (!better) best_ask = price;
		}
		levelPool[order->level_idx].qty += order->qty;
		levelPool[order->level_idx].orders.push_back(order_id);
	}

	return ErrorCode::OK;
}

ErrorCode Level::cancelOrder(uint32_t order_id, uint32_t qty)
{
	auto it = std::find(orders.begin(), orders.end(), order_id);
	if (it == orders.end()) return ErrorCode::INVALID_ORDER;
	this->qty -= qty;
	orders.erase(it);
	return ErrorCode::OK;
}

bool operator>(Level a, Level b)
{
	return a.price > b.price;
}

bool operator>(PriceLevel a, PriceLevel b)
{
	return a.price > b.price;
}