#pragma once
#include "Book.h"
#include <vector>

class Trader
{
public:
  Trader();
  ~Trader();
  ErrorCode addOrder(uint32_t order_id);
private:
  Orders order;
};

typedef std::vector<Trader> Traders;
