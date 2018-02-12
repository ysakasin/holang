CC_SRC := main.cc lexer.cc parser.cc vm.cc
HH_SRC := holang.hh
INC_SRC := keyword.inc opcode.inc

CXXFLAGS := -std=c++11

.PHONY: all
all: ho

ho: $(CC_SRC) $(HH_SRC)
	$(CXX) -std=c++11 -o $@ $(CC_SRC)

# .cc.o:
# 	$(CXX) $(CXXFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) ho a.out *.o