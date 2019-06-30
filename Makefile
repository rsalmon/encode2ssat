all:
	@make -s pomdp network

network: network.cpp network.hpp parser.cpp network-test.cpp
	@g++ -O3 -Wall -Wextra -Ofast -ffast-math -march=native -std=c++11 -flto -o network network-test.cpp network.cpp parser.cpp

pomdp: pomdp.cpp pomdp.hpp parser.cpp
	@g++ -O3 -Wall -Wextra -Ofast -ffast-math -march=native -std=c++11 -flto -o pomdp pomdp-test.cpp pomdp.cpp parser.cpp

clean: 
	@rm -f parser network pomdp
