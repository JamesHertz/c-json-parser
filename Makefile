# dirs
BIN_DIR := bin

# exec files c:
EXE      := $(BIN_DIR)/main
TEST_EXE := $(BIN_DIR)/test

# other files c:
SRC      := $(wildcard src/*.c)
HS       := $(wildcard src/*.h)
ALL_OBJ  := $(SRC:src/%.c=$(BIN_DIR)/%.o)
EXE_OBJ  := $(filter-out $(TEST_EXE).o , $(ALL_OBJ)) 
TEST_OBJ := $(filter-out $(EXE).o , $(ALL_OBJ)) 
FLAGS    := -DJSON_MEM_FAIL_STOP

all: $(EXE)

$(EXE): $(EXE_OBJ) | $(BIN_DIR)
	$(CC) $(FLAGS) -o $@ $^

$(TEST_EXE): $(TEST_OBJ) | $(BIN_DIR)
	$(CC) $(FLAGS) -o $(TEST_EXE) $^

$(BIN_DIR)/%.o: src/%.c $(HS) | $(BIN_DIR)
	$(CC) $(FLAGS) -o $@ -c $<
 
$(BIN_DIR):
	mkdir -p $@

test: $(TEST_EXE)
run-test: $(TEST_EXE)
	@$<

.PHONNY: clean run-test
clean:
	@rm -rfv $(BIN_DIR)
