CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wno-register
LEX = flex
YACC = bison

TARGET = mini-scheme
OBJS = main.o interpreter.o parser.tab.o lex.yy.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

parser.tab.cpp parser.tab.hpp: parser.y
	$(YACC) -d -o parser.tab.cpp parser.y

lex.yy.cpp: lexer.l parser.tab.cpp parser.tab.hpp
	$(LEX) -o lex.yy.cpp lexer.l

main.o: main.cpp interpreter.h parser.tab.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

interpreter.o: interpreter.cpp interpreter.h
	$(CXX) $(CXXFLAGS) -c interpreter.cpp

parser.tab.o: parser.tab.cpp interpreter.h
	$(CXX) $(CXXFLAGS) -c parser.tab.cpp

lex.yy.o: lex.yy.cpp parser.tab.cpp parser.tab.hpp
	$(CXX) $(CXXFLAGS) -c lex.yy.cpp

clean:
	rm -f $(TARGET) $(OBJS) parser.tab.cpp parser.tab.hpp lex.yy.cpp

.PHONY: all clean
