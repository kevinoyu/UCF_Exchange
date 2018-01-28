#include "Exchange.h"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <thread>

// args - case file, port num
int main(int argc, char* argv[]) {
	Exchange e;
	if (argc > 2) {
		e.initFromFile(argv[1]);
	  // spawn book handlers
    // spawn trader handlers
    e.run(atoi(argv[2]));
  }
	

	/*
	uint32_t ta = e.registerSecurity("AAPL");
	clock_t t;
	t = clock();
	for (int i = 50; i < 100; i++) {
		for (int j = -100; j < 100; ++j) 
		{
			e.addOrder("AAPL", j, i, 314);
		}
	}
	t = clock() - t; 
	printf("It took %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);*/
}