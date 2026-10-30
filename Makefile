CXX      = c++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror
HEADERS  = src/fancy_tree.hpp src/test.hpp

example: src/test.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) src/test.cpp -o $@

edge: tests/edge.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/edge.cpp -o $@

test: example edge
	./example | diff -u tests/example.txt -
	./edge | diff -u tests/edge.txt -
	@echo "ok"

clean:
	rm -rf example edge *.dSYM

.PHONY: test clean
