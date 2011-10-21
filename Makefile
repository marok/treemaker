
CONFIG=`pkg-config --cflags --libs  gtkglext-1.0`
EXECUTABLE=treemaker
SOURCES = MainWindow.cpp \
Point3d.h \
Vector3d.h \
Node.h \
Bmp.h \
ColonizationMethod.h \
ColonizationParameters.h \
ColonizationParametersPanel.h \
ParticleMethod.h \
Randomizer.h

default:
	g++ -g -Wall $(CONFIG) $(SOURCES) -o $(EXECUTABLE)

clean :
	rm -rf $(EXECUTABLE)
	rm -rf *~
	rm -rf *.orig

nice:
	astyle -t $(SOURCES)
