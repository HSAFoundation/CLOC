ifndef HSA_RUNTIME_PATH
	HSA_RUNTIME_PATH=/opt/hsa
endif

TEST_NAME=vector_copy_codeobject
#LFLAGS= -g  -Wl,--unresolved-symbols=ignore-in-shared-libs
LFLAGS = 
CFLAGS =  -std=c++11

INCS = -I $(HSA_RUNTIME_PATH)/include
CXX_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix obj/, $(notdir $(CXX_FILES:.cpp=.o)))

all: $(TEST_NAME) $(TEST_NAME).hsaco

$(TEST_NAME): $(OBJ_FILES) $(COMMON_OBJ_FILES)
	$(CXX) $(LFLAGS) $(OBJ_FILES) -L$(HSA_RUNTIME_PATH)/lib -lhsa-runtime64 -o $(TEST_NAME)

$(TEST_NAME).hsaco :
	cloc.sh $(TEST_NAME).cl

obj/%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(INCS) -o $@ $< 

clean:
	rm -rf obj/*o *.hsaco $(TEST_NAME)


test:	
	echo "export LD_LIBRARY_PATH=$(HSA_RUNTIME_PATH)/lib" > test.sh
	echo "./$(TEST_NAME)" >> test.sh
	bash test.sh
	rm test.sh


