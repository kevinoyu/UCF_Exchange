#include "Exchange.h"
Exchange::Exchange(std::string fileName) {

}

Exchange::~Exchange()
{
}

MessageQueuePool Exchange::mqp = MessageQueuePool();

ErrorCode Exchange::registerTrader()
{
	return ErrorCode::OK;
}

ErrorCode Exchange::registerSecurity(std::string sec_name)
{
	books.emplace_back(&orders);
	this->sec_map.insert(Secmap::value_type(sec_name, Security(sid)));
	sid++;
	return ErrorCode::OK;
}

ErrorCode Exchange::addOrder(const uint32_t& book_id, const Message& msg)
{
	ErrorCode trader_status = traders[msg.trader_id].addOrder(oid);
	if (trader_status == ErrorCode::OK) {
		ErrorCode book_status = books[book_id].processOrder(oid, &orders[oid], msg.price);
		if (book_status == ErrorCode::OK) {
			orders.emplace_back(msg.qty > 0, uint32_t(std::abs(msg.qty)), book_id, msg.trader_id);
			oid++;
		}
		return book_status;
	}
	return trader_status;
}

ErrorCode Exchange::cancelOrder(const uint32_t& book_id, const Message& msg)
{
	Order* order = &(orders[msg.order_id]);
	if (msg.trader_id != order->trader_id) return ErrorCode::ID_MISMATCH;
	else {
		return books[book_id].cancelOrder(msg.order_id, order);
	}
}

void Exchange::initFromFile(std::string filename)
{

}

void Exchange::run(uint32_t port) {
	h.onMessage([this](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
		rapidjson::Document d;
		d.Parse(message);
		const char* raw_type = d["message_type"].GetString();
		
		switch (raw_type[0])
		{
		case 'R': 
			registerTrader();
		case 'M':
			rapidjson::Value& orders = d["orders"];
			rapidjson::Value& cancels = d["cancels"];
			for (rapidjson::SizeType i = 0; i < orders.Size(); ++i) 
			{
				Message msg;
				msg.type = MessageType::ORDER; 
			}
			for (rapidjson::SizeType i = 0; i < cancels.Size(); ++i) 
			{

			}
		default:
			return; 
		}

	});

	h.onConnection([this](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) {
		
	});
}

void Exchange::handleBooks(uint32_t num, uint32_t sequence)
{
	while (true) {
		for (uint32_t i = sequence; i < sequence + num; i++) {

		}
	}
}

Security::Security(uint32_t id)
{
	book_id = id;
	messages = Exchange::mqp.alloc();
}

Security::~Security()
{
	Exchange::mqp.free(messages);
}
