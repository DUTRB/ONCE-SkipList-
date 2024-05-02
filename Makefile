CC=g++  
CXXFLAGS = -std=c++11 -pthread  
  
skiplist: main.o  
	$(CC) $(CXXFLAGS) -o ./bin/main main.o  
  
main.o: ./src/main.cpp  
	$(CC) $(CXXFLAGS) -c ./src/main.cpp -o main.o  
  
clean:  
	rm -f ./bin/main ./bin/stress ./*.o