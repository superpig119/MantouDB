.PHONY:udt

udto = md5.o common.o window.o list.o buffer.o packet.o channel.o queue.o core.o cache.o epoll.o api.o ccc.o
CXX=g++
udt:$(udto)
	g++ -c $(udto)
md5.o:md5.cpp md5.h
	$(CXX) -c md5.cpp
	cp md5.o ../
common.o:common.h common.cpp
	$(CXX) -c common.cpp
	cp common.o ../
window.o:window.cpp
	$(CXX) -c window.cpp
	cp window.o ../
list.o:list.cpp
	$(CXX) -c list.cpp
	cp list.o ../
buffer.o:buffer.cpp buffer.h
	$(CXX) -c buffer.cpp
	cp buffer.o ../
packet.o:packet.h packet.cpp
	$(CXX) -c packet.cpp
	cp packet.o ../
channel.o:channel.cpp channel.h
	$(CXX) -c channel.cpp
	cp channel.o ../
queue.o:queue.cpp queue.h
	$(CXX) -c queue.cpp
	cp queue.o ../
core.o:core.cpp core.h
	$(CXX) -c core.cpp
	cp core.o ../
cache.o:cache.h cache.cpp
	$(CXX) -c cache.cpp
	cp cache.o ../
epoll.o:epoll.cpp epoll.h
	$(CXX) -c epoll.cpp
	cp epoll.o ../
api.o:api.cpp api.h
	$(CXX) -c api.cpp
	cp api.o ../
ccc.o:ccc.cpp ccc.h
	$(CXX) -c ccc.cpp
	cp ccc.o ../


clean:
	rm -f *.o *.so *.a

