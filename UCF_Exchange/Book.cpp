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
		while (qty > 0 && cur_level.orders.size() > 0) {
			uint32_t ask_id = cur_level.orders.front();
			Order* ask_order = getOrder(ask_id);
			trade_qty = ask_order->qty;
			if (trade_qty > qty) {
				ask_order->qty -= qty;
				qty = 0;
				toNotify.emplace_back(qty, cur_level.price, order_id, ask_id);
				exhausted = true;
				break;
			}
			else {
				qty -= order->qty;
				toNotify.emplace_back(order->qty, cur_level.price, order_id, ask_id);
				cur_level.cancelOrder(ask_id, trade_qty);
			}
		}

		if (exhausted || cur.price > price) {
			break;
		}
	}
	return qty;
}

uint32_t Book::crossBid(uint32_t order_id, Order * order, double price)
{
	uint32_t qty = order->qty;
	auto iter = asks.end();
	bool exhausted = false;
	while (iter-- != asks.begin()) {
		PriceLevel &cur = *iter;
		Level cur_level = levelPool[cur.level_idx];
		uint32_t trade_qty = 0;
		double trade_price = cur_level.price;
		while (qty > 0 && cur_level.orders.size() > 0) {
			uint32_t bid_id = cur_level.orders.front();
			Order* bid_order = getOrder(bid_id);
			trade_qty = bid_order->qty;
			if (trade_qty > qty) {
				order->qty -= qty;
				qty = 0;
				toNotify.emplace_back(qty, cur_level.price, order_id, bid_id);
				exhausted = true;
				break;
			}
			else {
				qty -= bid_order->qty;
				toNotify.emplace_back(order->qty, cur_level.price, order_id, bid_id);
				cur_level.cancelOrder(bid_id, trade_qty);
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
	return &((*orders)[order_id]);
}

uint32_t Book::processOrder(uint32_t order_id, Order * order, double price)
{
	int32_t qty = order->qty;
	if (order->isBuy && price >= best_ask) {
		qty = crossAsk(order_id, order, price);
	}
	else if (!order->isBuy && price <= best_bid) {
		qty = crossBid(order_id, order, price);
	}

	if (qty > 0) {
		order->qty = qty;
		addOrder(order_id, order, price);
	}
	return qty;
}

uint32_t Book::cancelOrder(uint32_t order_id, Order* order)
{
	int32_t toRet = levelPool[order->level_idx].cancelOrder(order_id, order->qty);
	if (levelPool[order->level_idx].orders.size() == 0) {
		Levels *level = order->isBuy ? &bids : &asks;
		auto iter = level->end();
		while (iter-- != level->begin()){
			if (iter->price == levelPool[order->level_idx].price) {
				level->erase(++iter);
				levelPool.free(order->level_idx);
			}
		}	
	}
	return toRet;
}

uint32_t Book::addOrder(uint32_t order_id, Order* order, double price)
{
	int32_t qty = order->qty;
	Levels* levels = order->isBuy ? &bids : &asks;
	bool found = false;
	auto iter = levels->end();
	if (order->isBuy) {
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
		iter = levels->begin();
		auto last = levels->end() - 1;
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
		iter += order->isBuy ? 1 : -1;
		if (iter == asks.begin()) best_ask = price;
		else if (iter == (bids.end() - 1)) best_bid = price; 
		levels->insert(iter, pl);
	}
	levelPool[order->level_idx].qty += order->qty;
	levelPool[order->level_idx].orders.push_back(order_id);
}

uint32_t Level::cancelOrder(uint32_t order_id, uint32_t qty)
{
	auto it = std::find(orders.begin(), orders.end(), order_id);
	if (it != orders.end()) {
		this->qty -= qty;
		orders.erase(it);
		return this->qty;
	}
	else {
		return -3;
	}
}