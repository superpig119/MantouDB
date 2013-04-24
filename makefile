.PHONY:start_master
CXX=g++
start_master:conf.o master.o start_master.o tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o user.o tcptransport.o ServerSocket.o Socket.o ClientSocket.o	
	g++ -o start_master conf.o master.o start_master.o tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o user.o  tcptransport.o ServerSocket.o Socket.o -lpthread 
	g++ -o start_slave start_slave.cpp slave.cpp tcptransport.cpp conf.o tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o ClientSocket.o Socket.o -lpthread 
	mv *.o ./obj
	g++ -o stop_all stop_all.cpp
#SRC =  conf.cpp master.cpp start_master.cpp
tinyxml.o:
	cd ./tinyxml && make
tinyxmlerror.o:
	cd ./tinyxml && make
tinyxmlparser.o:
	cd ./tinyxml && make
tinystr.o:
	cd ./tinyxml && make

Socket.o:
	cd ./simplesocket && make
ServerSocket.o:
	cd ./simplesocket && make
ClientSocket.o:
	cd ./simplesocket && make

conf.o:conf.cpp conf.h
	$(CXX) -c conf.cpp
master.o:master.cpp master.h
	$(CXX) -c master.cpp
user.o:user.h user.cpp
	$(CXX) -c user.cpp
tcptransport.o:transport.h tcptransport.h tcptransport.cpp
	$(CXX) -c tcptransport.cpp
start_master.o:start_master.cpp
	$(CXX) -c start_master.cpp
slave.o:slave.cpp slave.h
	$(CXX) -c slave.cpp
slave_start.o:start_slave.cpp
	$(CXX) -c slave_slave.cpp
#start_master:
#	g++ -o start_master $(SRC)
#	cd ./tinyxml && $(MAKE)

clean:
	rm -f ./obj/*.o start_master start_slave
