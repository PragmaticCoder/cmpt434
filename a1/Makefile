SRC_DIR=./src
BUILD_DIR=./build
TESTS_DIR=./tests
BIN_DIR	=./bin
PART_1 = $(BIN_DIR)
PART_2 = $(BIN_DIR)
PART_3 = $(BIN_DIR)
INCLUDES=-I$(SRC_DIR)

CC=gcc
CFLAGS=-g -std=c99 -O2 	-Wall -Wextra -Werror -Wno-unused-function\
			-Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-address\
			-Wno-unused-variable -Wno-return-local-addr -Wno-sign-compare -Wno-pointer-sign\
			-rdynamic -DNDEBUG -Du_char="unsigned char" -DNDEBUG $(INCLUDES) $(OPTFLAGS)

LDFLAGS=-ldl -lm $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard $(SRC_DIR)/**/*.c src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

TEST_SOURCES=$(wildcard $(TESTS_DIR)/*_tests.c)
TESTS=$(patsubst %.c, %, $(TEST_SOURCES))


PROGRAMS=$(PART_1)/server_v1 $(PART_1)/client_v1\
	  $(PART_2)/server_v2 $(PART_2)/tcp_tcp_proxy $(PART_2)/client_v2\
	  $(PART_3)/server_v3 $(PART_3)/udp_tcp_proxy $(PART_3)/client_v3

TARGET=$(BUILD_DIR)/libasn_01.a
SO_TARGET=$(patsubst %.a, %.so,$(TARGET))

all: target tests

target: build $(TARGET) $(SO_TARGET) $(PROGRAMS)

$(PART_1)/server_v1:$(BIN_DIR)/server_v2.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(INCLUDES) -lasn_01 -L $(BUILD_DIR)

$(PART_1)/client_v1:$(BIN_DIR)/client.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(INCLUDES) -lasn_01 -L $(BUILD_DIR)

$(PART_2)/server_v2:$(BIN_DIR)/server_v2.c 
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDES) -lasn_01 -L $(BUILD_DIR)

$(PART_2)/tcp_tcp_proxy:$(BIN_DIR)/tcp_tcp_proxy_v2.c 
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDES) -lasn_01 -L $(BUILD_DIR)

$(PART_2)/client_v2:$(BIN_DIR)/client.c
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDES) -lasn_01 -L $(BUILD_DIR)

$(PART_3)/server_v3:$(BIN_DIR)/udp_server_v3.c 
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDES)  -lasn_01 -L $(BUILD_DIR)

$(PART_3)/udp_tcp_proxy:$(BIN_DIR)/udp_tcp_proxy_v3.c 
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDES)  -lasn_01 -L $(BUILD_DIR)

$(PART_3)/client_v3:$(BIN_DIR)/client.c 
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDES) -lasn_01 -L $(BUILD_DIR)

$(TESTS_DIR)/%:	$(TESTS_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@ $(TARGET) $(LDFLAGS)

$(TARGET): CFLAGS += -fPIC
$(TARGET): $(OBJECTS)
	@ar rcs $@ $(OBJECTS)
	@ranlib $@

$(SO_TARGET): $(OBJECTS)
	$(CC) -shared $(LDFLAGS) -o $@ $(OBJECTS) 

$(TESTS): $(TEST_SOURCES)
	$(CC) $(CFLAGS) $< -o $@ $(TARGET) $(LDFLAGS)

build:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(TESTS_DIR)
	@touch storage

.PHONY: rebuild
rebuild: clean all ## Clean and rebuild
	@echo "+ $@"

.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS) ## Run Unit Tests Suite
	sh $(TESTS_DIR)/runtests.sh

.PHONY: valgrind
valgrind: ## Check Memory Leaks using Valgrind
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

.PHONY: clean
clean: clean-build clean-debug  ## Remove all file artifact 

.PHONY: clean-build
clean-build: ## Remove build artifacts
	@echo "+ $@"
	@find . -name "*.gc*" -exec rm {} \;
	-$(RM) -r $(OBJECTS) $(TESTS) $(PROGRAMS) $(BUILD_DIR)
	-$(RM) *.tar.gz
	-$(RM) -r `find . -name "*.dSYM" -print`

.PHONY: clean-debug
clean-debug: ## Remove debug artifacts
	@echo "+ $@"
	-$(RM) tests/tests.log
	-$(RM) output.txt storage
	-$(RM) -r *.log
	-$(RM) .gdb_history
	-$(RM) -r vgcore*

.PHONY: release
release: clean  ## Generate release version
	@echo "+ $@"
	@tar -cvzf ala273_asn_01.tar.gz *

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-22s\033[0m %s\n", $$1, $$2}'
	@echo "Contact author for help @ala273@mail.usask.ca"

$(PROGRAMS): CFLAGS += $(TARGET)
