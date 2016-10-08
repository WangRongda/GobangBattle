all: Gobang_serv
.PHONY: all
Gobang_serv: Gobang_serv.cpp libGobang.so
	g++ Gobang_serv.cpp -ggdb -pthread -L/home/socketer/GobangBattle -lGobang -o Gobang_serv
clean:
	rm *.o
