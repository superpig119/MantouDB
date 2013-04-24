.PHONY:simplesocket
CXX=g++
simplesocket: ServerSocket.o Socket.o ClientSocket.o

Socket.o:Socket.cpp Socket.h
	$(CXX) -c Socket.cpp
	mv Socket.o ../
ServerSocket.o:ServerSocket.cpp ServerSocket.h SocketException.h
	$(CXX) -c ServerSocket.cpp
	mv ServerSocket.o ../
ClientSocket.o:ClientSocket.cpp ClientSocket.h SocketException.h
	$(CXX) -c ClientSocket.cpp
	mv ClientSocket.o ../

clean:
	rm -f *.o
