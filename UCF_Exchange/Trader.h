#pragma once
#include "Book.h"

class Trader
{
public:
	Trader();
	~Trader();
private:
	Orders order;
};

typedef std::vector<Trader> Traders;