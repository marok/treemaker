
CONFIG=`pkg-config --cflags --libs  gtkglext-1.0`
EXECUTABLE=treemaker
SOURCES = MainWindow.cpp

default:
	g++ -g -Wall $(CONFIG) $(SOURCES) -lglut -o $(EXECUTABLE)
clean :
	rm -rf $(EXECUTABLE)
	rm -rf *~
	rm -rf *.orig

nice:
	astyle -t *.h
