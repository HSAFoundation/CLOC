
TEST_NAME=vector_copy_codeobject
CL_FILE=vector_copy_codeobject
#LFLAGS= -g  -Wl,--unresolved-symbols=ignore-in-shared-libs
LFLAGS = 
CFLAGS =  -std=c++11

CXX_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix obj/, $(notdir $(CXX_FILES:.cpp=.o)))

all: $(TEST_NAME) $(CL_FILE).hsaco

$(TEST_NAME): $(OBJ_FILES) $(COMMON_OBJ_FILES)
	$(CXX) $(LFLAGS) $(OBJ_FILES) -L/opt/rocm/lib -lhsa-runtime64 -o $(TEST_NAME)

$(CL_FILE).hsaco :
	cloc.sh $(CL_FILE).cl

obj/%.o: %.cpp
	mkdir -p obj
	$(CXX) -I/opt/rocm/include -c $(CFLAGS) -o $@ $< 

clean:
	rm -rf obj/*o *.hsaco $(TEST_NAME)


test:	
	./$(TEST_NAME)


