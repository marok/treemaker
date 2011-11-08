
CONFIG=`pkg-config --cflags --libs  gtkglext-1.0`
EXECUTABLE=treemaker
SOURCES = MainWindow.cpp \
Point3d.h \
Vector3d.h \
Node.h \
Bmp.h \
ColonizationMethod.h \
ParticleMethod.h \
MethodParameters.h \
MethodParametersPanel.h \
Randomizer.h\
TrunkParameters.h\
TrunkParametersPanel.h\
Texture.h

default:
	g++ -g -Wall $(CONFIG) $(SOURCES) -lglut -o $(EXECUTABLE)

clean :
	rm -rf $(EXECUTABLE)
	rm -rf *~
	rm -rf *.orig

nice:
	astyle -t *.h
