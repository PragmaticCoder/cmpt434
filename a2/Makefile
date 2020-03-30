# Name: Alvi AKbar
# NSID: ala273
# Student No: 11118887

SRC_DIR		= src
BIN_DIR 	= bin
BUILD_DIR	= build
TESTS_DIR	= tests
INCLUDE_DIR	= include

CC 			= gcc
CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS) -I $(INCLUDE_DIR)\
			  -Wextra -Werror -Wno-unused-function -Wno-implicit-function-declaration\
              -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-address\
              -Wno-unused-variable -Wno-return-local-addr -Wno-sign-compare -Wno-pointer-sign

LDFLAGS		= $(OPTLIBS)
PREFIX		?= /usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/libans_02.a

OS=$(shell lsb_release -si)
ifeq ($(OS),Ubuntu)
	LDLIBS=-lasn_02 -lbsd -L./build -lm
endif

# The Target Build
PROGRAMS=$(BIN_DIR)/client $(BIN_DIR)/server $(BIN_DIR)/forwarder

all: build $(TARGET) $(PROGRAMS) tests


$(BIN_DIR)/client: $(BIN_DIR)/client.c $(SRC_DIR)/frame.c  $(INCLUDE_DIR)/frame.h	
	$(CC) $(CFLAGS) -o $@ $(BIN_DIR)/client.c $(SRC_DIR)/frame.c -I $(INCLUDE_DIR)	


$(BIN_DIR)/server: $(BIN_DIR)/server.c $(SRC_DIR)/frame.c  $(INCLUDE_DIR)/frame.h
	$(CC) $(CFLAGS) -o $@ $(BIN_DIR)/server.c $(SRC_DIR)/frame.c -I $(INCLUDE_DIR)	


$(BIN_DIR)/forwarder: $(BIN_DIR)/forwarder.c $(SRC_DIR)/frame.c  $(INCLUDE_DIR)/frame.h
	$(CC) $(CFLAGS) -o $@ $(BIN_DIR)/forwarder.c $(SRC_DIR)/frame.c -I $(INCLUDE_DIR)


.PHONY: rebuild
rebuild: clean all ## Clean and rebuild
	@echo "+ $@"

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	@ar rcs $@ $(OBJECTS)
	@ranlib $@

build:
	@mkdir -p build
	@mkdir -p bin

.PHONY: tests
tests: LDLIBS += $(TARGET)
tests: $(TESTS)  ## Run Unit Tests Suite
	sh ./tests/runtests.sh

.PHONY: valgrind
valgrind: ## Check Memory Leaks using Valgrind
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

.PHONY: clean
clean: clean-build clean-debug  ## Remove all file artifact 
	-$(RM) -rf $(BUILD_DIR)

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
	@tar -cvzf ala273_asn_02.tar.gz *

.PHONY: install
install: all ## library installation
	@echo "+ $@"
	@install -d $(DESTDIR)/$(PREFIX)/lib/
	@install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
.PHONY: check
check: ## Checks for dangerous function
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-22s\033[0m %s\n", $$1, $$2}'
	@echo "Contact author for help: ala273@mail.usask.ca"