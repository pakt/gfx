CFLAGS = -I./glee/ -I./glm/
CXX = g++ -Wall -Wno-write-strings -O3

all: gbuffer.o mrt.o shader.o err.o objects.o camera.o
	$(CXX) -Wl,--enable-stdcall-fixup camera.o objects.o err.o shader.o gbuffer.o mrt.o glee/GLee.o -lglut32 -lglu32 -lopengl32 
mrt.o: err.o gbuffer.o mrt.cpp
	$(CXX) -c mrt.cpp $(CFLAGS)
shader.o: shader.h shader.cpp
	$(CXX) -c shader.cpp $(CFLAGS)
err.o: err.h err.cpp
	$(CXX) -c err.cpp 
gbuffer.o: gbuffer.h gbuffer.cpp
	$(CXX) -c gbuffer.cpp 
objects.o: objects.h objects.cpp
	$(CXX) -c objects.cpp 
camera.o: camera.h camera.cpp
	$(CXX) -c camera.cpp $(CFLAGS)

clean:
	rm *.a *.o a.exe
