output: server.o request.o response.o
	g++ server.o request.o response.o -o run

server.o: server.cpp
	g++ -c server.cpp

request.o: request.cpp
	g++ -c request.cpp

response.o: response.cpp
	g++ -c response.cpp

clean:
	rm *.o output