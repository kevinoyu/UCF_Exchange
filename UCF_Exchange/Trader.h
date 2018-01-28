#pragma once
#include "Book.h"
#include <vector>

class Trader
{
public:
  Trader();
  ~Trader();
  std::vector<uint32_t> orders;
  ErrorCode addOrder(uint32_t oid); // adds an order to the trader's open order book
  ErrorCode cancelOrder(uint32_t oid); 
  double fees;
  double fines;
  double pnl;
  double rebates;
};

typedef std::vector<Trader> Traders;