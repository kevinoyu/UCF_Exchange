#include "Exchange.h"


Exchange::Exchange(std::string fileName)
{
}

Exchange::~Exchange()
{
}

uint32_t Exchange::registerSecurity(std::string sec_name)
{
	this->sec_map.insert(Secmap::value_type(sec_name, sid));
	books[sid] = Book();
	sid++;
	return 0;
}

uint32_t Exchange::addOrder(std::string security, int qty, double price, uint32_t trader_id)
{
	uint32_t book_id = sec_map.at(security);
	Book book = books[book_id];
	orders.emplace_back(qty > 0, uint32_t(std::abs(qty)), book_id, trader_id);
	book.processOrder(oid, &orders[oid], price);
	oid++;
	return 0;
}

uint32_t Exchange::cancelOrder( uint32_t order_id, uint32_t trader_id)
{
	Order *order = &(orders[order_id]);
	if (trader_id != order->trader_id) return -1;
	if (order->qty == 0) return -2;
	else {
		return books[order->book_id].cancelOrder(order_id, order);
	}
}