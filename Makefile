
CONFIG=`pkg-config --cflags --libs  gtkglext-1.0`
EXECUTABLE=treemaker
SOURCES = MainWindow.cpp Point3d.h Vector3d.h Node.h ColonizationMethod.h Bmp.h ColonizationParameters.h
default:
	g++ -g -Wall $(CONFIG) $(SOURCES) -o $(EXECUTABLE)

clean :
	rm -rf $(EXECUTABLE)
	rm -rf *~

nice:
	astyle -t $(SOURCES)
