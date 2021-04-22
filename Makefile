sampleobjects = buffer_manager.o file_manager.o

basicfiles = constants.h errors.h file_manager.h buffer_manager.h 

join2 : $(sampleobjects) join2.o
	     g++ -std=c++11 -o join2 $(sampleobjects) join2.o

join2.o : join2.cpp
	g++ -std=c++11 -c join2.cpp

join1 : $(sampleobjects) join1.o
	     g++ -std=c++11 -o join1 $(sampleobjects) join1.o

join1.o : join1.cpp
	g++ -std=c++11 -c join1.cpp

deletion : $(sampleobjects) deletion.o
	     g++ -std=c++11 -o deletion $(sampleobjects) deletion.o

deletion.o : deletion.cpp
	g++ -std=c++11 -c deletion.cpp

binarysearch : $(sampleobjects) binarysearch.o
	     g++ -std=c++11 -o binarysearch $(sampleobjects) binarysearch.o

binarysearch.o : binarysearch.cpp
	g++ -std=c++11 -c binarysearch.cpp

debug : $(sampleobjects) debug.o
	g++ -std=c++11 -o debug $(sampleobjects) debug.o

debug.o : debug.cpp
	g++ -std=c++11 -c debug.cpp

linearsearch : $(sampleobjects) linearsearch.o
	     g++ -std=c++11 -o linearsearch $(sampleobjects) linearsearch.o

linearsearch.o : linearsearch.cpp
	g++ -std=c++11 -c linearsearch.cpp

sample_run : $(sampleobjects) sample_run.o
	     g++ -std=c++11 -o sample_run $(sampleobjects) sample_run.o

sample_run.o : sample_run.cpp
	g++ -std=c++11 -c sample_run.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp $(basicfiles)
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f sample_run linearsearch binarysearch deletion join1 join2
