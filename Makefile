CFLAGS=-std=c++11 -stdlib=libc++ -c -g
BIN=bin
BLD=build
EXCH=UCF_Exchange
UWS=uWS/src

SRCS=$(wildcard $(EXCH)/*.cpp) $(wildcard $(UWS)/*.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

INCS=-I rapidjson -I uWS/src -I /usr/local/opt/openssl/include
LIBS=-L uWS -l uWS -L /usr/local/opt/openssl/lib -l ssl
EXES=Exchange

.SECONDARY:



all : $(BIN)/$(EXES)

$(BIN)/% : $(OBJS)
	$(CXX) $(LIBS) $^ -o $(BIN)/$(EXES)

%.o : %.cpp
	$(CXX) $(CFLAGS) $(INCS) $< -o $@

test :
	echo $(SRCS)

clean :
	rm -rf $(EXCH)/*.o
	rm -rf $(UWS)/*.o
	rm -rf $(BIN)/*

