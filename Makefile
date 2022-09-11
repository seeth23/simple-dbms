src=FileSys.cpp \
		DatabaseManagement.cpp \
		LexicalAnalyzer.cpp \
		Database.cpp \
		Table.cpp \
		main.cpp \

flags = -Wall -g -std=c++17

all:
	clang++ $(flags) $(src) -o main
