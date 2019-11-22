CCC = g++
FLAGS = -g -Wall -Wextra
GRBPATH = /Library/gurobi811/mac64/

build: main.cpp
	$(CCC) $(FLAGS) main.cpp -o lab -I $(GRBPATH)/include -L $(GRBPATH)/lib -lgurobi_c++ -lgurobi81

clean:
	rm -f lab; rm -rf lab.dSYM
