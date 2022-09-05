src=FileSys.cpp \
		Parser.cpp \
		DatabaseManagement.cpp \
		LexicalAnalyzer.cpp \
		Database.cpp \
		Table.cpp \
		main.cpp \

flags = -Wall -g -std=c++17

all:
	g++ $(flags) $(src) -o main
