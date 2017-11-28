#pragma once
#include "Trader.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "uWS.h"
#include "rapidjson.h"

typedef std::unordered_map<std::string, uint32_t> Secmap;
typedef std::vector<Book> Books;

typedef enum MessageType 
{
	REGISTER,
	ORDER,
	CANCEL
} MessageType;

class Exchange
{
public:
	Exchange() : orders(Orders()), oid(0), sid(0), traders(Traders()), sec_map(Secmap()), books(Books()) {};
	Exchange(std::string fileName);	
	~Exchange();
	ErrorCode registerTrader();
	ErrorCode registerSecurity(std::string sec_name);
	ErrorCode addOrder(std::string security, int qty, double price, uint32_t trader_id);
	ErrorCode addOrder(uint32_t book_id, int qty, double price, uint32_t trader_id);
	ErrorCode cancelOrder(uint32_t order_id, uint32_t trader_id );
private:
	Secmap sec_map;
	Books books; // Vector of books, each representing a security
	Orders orders;
	Traders traders;
	uint32_t sec_count;
	uint32_t oid;
	uint32_t sid;
	uWS::Hub h;
	void processMessages();
	void handleMessage();
};