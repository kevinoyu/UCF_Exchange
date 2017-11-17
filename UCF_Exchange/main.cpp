#include "Exchange.h"
#include <iostream>

int main() {
	Exchange e = Exchange();
	uint32_t t = e.registerSecurity("AAPL");
	for (int i = 50; i < 100; i++) {
		e.addOrder("AAPL", 100, i, 314);
		e.addOrder("AAPL", -100, i + 100, 314);
	}
	e.cancelOrder(0, 314);
	e.cancelOrder(99, 314);
	std::cout << e.books[0].toString();
}