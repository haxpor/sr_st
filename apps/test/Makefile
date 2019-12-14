CXX = g++
CXXFLAGS = -g -std=c++11 -I../../common
CXXLDFLAGS = -lm

SOURCES = main.cpp
SOURCES += ../../common/ObjLoader.cpp ../../common/Logger.cpp ../../common/Profile.cpp
HEADERS += ../../common/Logger.h ../../common/MathUtil.h ../../common/ObjLoader.h ../../common/Platform.h ../../common/Profile.h ../../common/SR_Common.h ../../common/TGAImage.h ../../common/Types.h

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

EXE = test

.PHONY: all clean

all: $(EXE)
	@echo Build complete

%.o:../../common/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXLDFLAGS)

clean:
	rm -f $(EXE) $(OBJS)
