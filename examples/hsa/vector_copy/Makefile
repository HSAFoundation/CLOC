TEST_NAME=vector_copy
LFLAGS= -g  -Wl,--unresolved-symbols=ignore-in-shared-libs
INCS = -I /opt/rocm/include
C_FILES := $(wildcard *.c)
OBJ_FILES := $(addprefix obj/, $(notdir $(C_FILES:.c=.o)))

all: $(TEST_NAME) $(TEST_NAME).brig

$(TEST_NAME): $(OBJ_FILES) $(COMMON_OBJ_FILES)
	$(CC) $(LFLAGS) $(OBJ_FILES) -L/opt/rocm/lib -lhsa-runtime64 -o $(TEST_NAME)

$(TEST_NAME).brig :
	cloc.sh -brig $(TEST_NAME).cl

obj/%.o: %.c
	$(CC) -c $(CFLAGS) $(INCS) -o $@ $< -std=c99

clean:
	rm -rf obj/*o *.brig $(TEST_NAME)


test:	
	./$(TEST_NAME)


