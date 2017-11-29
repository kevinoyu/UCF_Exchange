#pragma once
#include "Trader.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "uWS.h"
#include "document.h"
#include "mpsc.h"

typedef std::vector<Book> Books;

typedef enum MessageType 
{
	REGISTER = 'R',
	ORDER = 'M',
	CANCEL = 'C'
} MessageType;


struct Message 
{
	MessageType type;
	std::string security;
	uint32_t order_id;
	int qty;
	double price;
	uint32_t trader_id;
};

struct ReturnMessage
{
	uint32_t trader_id;
	std::string msg;
};

typedef mpsc_queue_t<Message> MessageQueue;
typedef mpsc_queue_t<ReturnMessage> ReturnQueue;
enum class MessageQueue_id_t : uint32_t {};
typedef Pool<MessageQueue, MessageQueue_id_t, 100> MessageQueuePool;

struct Security
{
	uint32_t book_id;
	MessageQueue_id_t messages;
	Security(uint32_t id);
	~Security();
};

typedef std::unordered_map<std::string, Security> Secmap;

class Exchange
{
public:
	Exchange() : orders(Orders()), oid(0), sid(0), traders(Traders()), sec_map(Secmap()), books(Books()) {};
	Exchange(std::string fileName);	
	~Exchange();
	static MessageQueuePool mqp; // pool for order queues to be used by individual securities; preserves locality of queues for each thread handling a block of securities

	bool acceptingConnections;
	Secmap sec_map;
	Books books; // Vector of books, each representing a security
	Orders orders;
	Traders traders;
	uint32_t sec_count;
	uint32_t oid;
	uint32_t sid;
	uWS::Hub h;

	ErrorCode registerTrader();
	ErrorCode registerSecurity(std::string sec_name);
	ErrorCode addOrder(const uint32_t& book_id, const Message& msg);
	ErrorCode cancelOrder(const uint32_t& book_id, const Message& msg);
	void initFromFile(std::string filename);
	void run(uint32_t port);
	void handleBooks(uint32_t num, uint32_t sequence); // handle num # of contiguous books
};