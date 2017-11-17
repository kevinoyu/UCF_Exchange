#ifndef __BOOK_H
#define __BOOK_H
#include "Book.h"
#endif 

class Trader
{
public:
	Trader();
	~Trader();
private:
	Orders order;
};

typedef std::vector<Trader> Traders;