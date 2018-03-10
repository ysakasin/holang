CC_SRC := main.cpp lexer.cpp parser.cpp object.cpp
HH_SRC := holang.h vm.h
INC_SRC := keyword.inc opcode.inc

CXXFLAGS := -std=c++11 -Wall

.PHONY: all
all: ho

ho: $(CC_SRC) $(HH_SRC) $(INC_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(CC_SRC)

.PHONY: g
g:
	$(CXX) $(CXXFLAGS) -g -o ho $(CC_SRC)
# .cc.o:
# 	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean test
clean:
	$(RM) ho a.out *.o

test: ho
	./test.sh
