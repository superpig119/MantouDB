#SRC = tinystr.cpp tinyxml.cpp tinyxmlerror.cpp  tinyxmlparser.cpp 

#start_master:

#g++ -o start_master $(SRC)

.PHONY:tinyxml
CXX=g++
tinyxml:tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o

tinystr.o:tinystr.h tinystr.cpp
	$(CXX) -c tinystr.cpp
	mv tinystr.o ../
tinyxml.o:tinyxml.h tinyxml.cpp
	$(CXX) -c tinyxml.cpp
	mv tinyxml.o ../
tinyxmlerror.o:tinyxmlerror.cpp
	$(CXX) -c tinyxmlerror.cpp
	mv tinyxmlerror.o ../
tinyxmlparser.o:tinyxmlparser.cpp
	$(CXX) -c tinyxmlparser.cpp
	mv tinyxmlparser.o ../


clean:
	rm -f *.o
