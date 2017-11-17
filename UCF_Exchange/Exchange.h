#pragma once
#include "Trader.h"
#include "Book.h"
#include <unordered_map>
#include <string>
#include <vector>

typedef std::unordered_map<std::string, uint32_t> Secmap;
typedef std::vector<Book> Books;

class Exchange
{
public:
	Exchange(std::string fileName);
	~Exchange();
	uint32_t registerTrader();
	uint32_t registerSecurity(std::string sec_name);
	uint32_t addOrder(std::string security, int qty, double price, uint32_t trader_id);
	uint32_t cancelOrder(uint32_t order_id, uint32_t trader_id );
private:
	Secmap sec_map;
	Books books;
	Orders orders;
	Traders traders;
	uint32_t sec_count;
	static uint32_t oid;
	static uint32_t sid;
};