#include "Exchange.h"
Exchange::Exchange(std::string fileName) {

}

Exchange::~Exchange()
{
}

ErrorCode Exchange::registerSecurity(std::string sec_name)
{
	books.emplace_back(&orders);
	this->sec_map.insert(Secmap::value_type(sec_name, sid));
	sid++;
	return ErrorCode::OK;
}

ErrorCode Exchange::addOrder(std::string security, int qty, double price, uint32_t trader_id)
{
	uint32_t book_id = sec_map.at(security);
	return addOrder(book_id, qty, price, trader_id);
}

ErrorCode Exchange::addOrder(uint32_t book_id, int qty, double price, uint32_t trader_id)
{
	orders.emplace_back(qty > 0, uint32_t(std::abs(qty)), book_id, trader_id);
	books[book_id].processOrder(oid, &orders[oid], price);
	oid++;
	return ErrorCode::OK;
}

ErrorCode Exchange::cancelOrder( uint32_t order_id, uint32_t trader_id)
{
	Order* order = &(orders[order_id]);
	if (trader_id != order->trader_id) return ErrorCode::ID_MISMATCH;
	else {
		return books[order->book_id].cancelOrder(order_id, order);
	}
}