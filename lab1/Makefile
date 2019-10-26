CCC = g++
FLAGS = -g -Wall -Wextra
GRBPATH = /Library/gurobi811/mac64/

exec: main.cpp
	$(CCC) $(FLAGS) main.cpp -o exec -I $(GRBPATH)/include -L $(GRBPATH)/lib -lgurobi_c++ -lgurobi81

clean:
	rm -f code *.o *~
