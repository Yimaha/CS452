# Change the name of EXEC to your executable
CXX = g++
CXXFLAGS := -Wall -MMD -g -Werror=vla
SOURCES = ${wildcard *.cc}
OBJECTS = ${SOURCES:.cc=.o}
DEPENDS = ${OBJECTS:.o=.d}

dtest: CXXFLAGS += -DPRINT_DEBUG
dtest:
	${CXX} dtest.cc ../src/routing/*.cc -o dtest.bin

buffertest:
	${CXX} buffertest.cc ../src/utils/*.cc -o buffertest.bin

utilitytest:
	${CXX} utilitytest.cc -o utilitytest.bin

randtest:
	${CXX} randtest.cc ../src/routing/*.cc -o randtest.bin

-include ${DEPENDS}

.PHONY: clean
clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC} *.bin
