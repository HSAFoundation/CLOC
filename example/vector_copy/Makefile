CC=g++
CFLAGS=-g -c
LFLAGS= -g  -Wl,--unresolved-symbols=ignore-in-shared-libs

OBSEDIAN_PATH=/home/shreyas/work/foundation/HSA_Member_Projects_AMD/obsidian/release.hsa_foundation
#Temporary fix till libHSAIL on GitHub is fixed
LIBHSAIL_PATH=/home/shreyas/work/foundation/HSA_Member_Projects_AMD/obsidian/libHSAIL/
BUILD_DIR=build/lnx64a/B_rel/

INCS = -I $(OBSEDIAN_PATH)/include
#Temporary fix to libHSAIL on GitHUB is fixed
INCS += -I $(LIBHSAIL_PATH)
INCS += -I $(LIBHSAIL_PATH)/$(BUILD_DIR)
LIB_PATH=$(OBSEDIAN_PATH)/lib/x86_64


CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/, $(notdir $(CPP_FILES:.cpp=.o)))
all: vector_copy vector_copy.brig

vector_copy: $(OBJ_FILES)
	$(CC) $(LFLAGS) $(OBJ_FILES) $(LIBHSAIL_PATH)/$(BUILD_DIR)/LIBHSAIL.a -lelf -L$(LIB_PATH) -lhsa-runtime64 -o vector_copy

vector_copy.brig :
	./cloc vector_copy.cl

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(INCS) -o $@ $< -std=c++0x

clean:
	rm -rf obj/*o vector_copy vector_copy.brig vector_copy.hsail

