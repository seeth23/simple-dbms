src=FileSys.cpp \
		Parser.cpp \
		DatabaseManagement.cpp \
		LexicalAnalyzer.cpp \
		Database.cpp \
		Table.cpp \
		main.cpp \

flags = -Wall -g

all:
	g++ $(flags) $(src) -o main
