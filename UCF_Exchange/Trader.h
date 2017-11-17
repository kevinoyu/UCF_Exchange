#ifndef __BOOK_H
#define __BOOK_H
#include "Book.h"
#endif 


typedef std::vector<Trader> Traders;

class Trader
{
public:
	Trader();
	~Trader();
private:
	Orders order;
};