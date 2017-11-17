#pragma once
#ifndef __TRADER__H
#define __TRADER__H
#include "Trader.h"
#endif // !__TRADER__H

#ifndef __BOOK_H
#define __BOOK_H
#include "Book.h"
#endif // !__BOOK_H

#include <unordered_map>
#include <string>
#include <vector>

typedef std::unordered_map<std::string, uint32_t> Secmap;
typedef std::vector<Book> Books;

class Exchange
{
public:
	Exchange() : orders(Orders()), oid(0), sid(0), traders(Traders()), sec_map(Secmap()), books(Books()) {};
	Exchange(std::string fileName);	
	~Exchange();
	uint32_t registerTrader();
	uint32_t registerSecurity(std::string sec_name);
	uint32_t addOrder(std::string security, int qty, double price, uint32_t trader_id);
	uint32_t addOrder(uint32_t book_id, int qty, double price, uint32_t trader_id);
	uint32_t cancelOrder(uint32_t order_id, uint32_t trader_id );
	Secmap sec_map;
	Books books; // Vector of books, each representing a security
	Orders orders;
	Traders traders;
	uint32_t sec_count;
	uint32_t oid;
	uint32_t sid;
};