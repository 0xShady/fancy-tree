CXX      = c++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror
HEADERS  = src/fancy_tree.hpp src/test.hpp
PLAIN    = env -u CLICOLOR_FORCE -u NO_COLOR
COLOUR   = env -u NO_COLOR CLICOLOR_FORCE=1

example: src/test.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) src/test.cpp -o $@

edge rb rb_bool: %: tests/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc $< -o $@

test: example edge rb rb_bool
	./example | diff -u tests/example.txt -
	./edge | diff -u tests/edge.txt -
	$(PLAIN) ./rb | diff -u tests/rb.txt -
	$(COLOUR) ./rb | diff -u tests/rb_colour.txt -
	NO_COLOR=1 CLICOLOR_FORCE=1 ./rb | diff -u tests/rb.txt -
	$(PLAIN) ./rb_bool | diff -u tests/rb_bool.txt -
	$(COLOUR) ./example | diff -u tests/example.txt -
	@echo "ok"

clean:
	rm -rf example edge rb rb_bool *.dSYM

.PHONY: test clean
