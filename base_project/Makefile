CC						:= gcc
CFLAGS				:= -g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LDFLAGS				:= -ldl $(OPTLIBS)
PREFIX				?= /usr/local

SRC_DIR				:= ./src
BUILD_DIR			:= ./build
TESTS_DIR			:= ./tests

SOURCES				:= $(wildcard $(SRC_DIR)/**/*.c src/*.c)
OBJECTS				:= $(patsubst %.c, %.o, $(SOURCES))

TEST_SOURCES	:= $(wildcard $(TESTS)/*_tests.c)
TESTS					:= $(patsubst %.c, %, $(TEST_SOURCES))

PROGRAMS_SRC	:= $(wildcard bin/*.c)
PROGRAMS			:= $(patsubst %.c, %, $(PROGRAMS_SRC))

TARGET				:= build/libbase.a
SO_TARGET			:= $(patsubst %.a, %.so,$(TARGET))

all: $(TARGET) $(SO_TARGET) tests $(PROGRAMS)

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TESTS):	$(TARGET) $(SO_TARGET)

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) $(LDFLAGS) -shared -o $@ $(OBJECTS)

build:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(TESTS_DIR)

.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS) ## Run Unit Tests Suite
	sh ./tests/runtests.sh

.PHONY: valgrind
valgrind: ## Check Memory Leaks using Valgrind
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

.PHONY: clean
clean: clean-build clean-debug ## Remove all file artifact

.PHONY: clean-build
clean-build: ## Remove build artifacts
	@echo "+ $@"
	@find . -name "*.gc*" -exec rm {} \;
	-$(RM) -rf $(OBJECTS) $(BUILD_DIR) || true
	-$(RM) $(OBJECTS) || true
	-$(RM) $(TESTS) || true
	-$(RM) *.tar.gz
	-$(RM) -r `find . -name "*.dSYM" -print`

.PHONY: clean-debug
clean-debug: ## Remove debug artifacts
	@echo "+ $@"
	-$(RM) tests/tests.log || true
	-$(RM) output.txt || true
	-$(RM) .gdb_history || true
	-$(RM) -r vgcore* || true

.PHONY: release
release: clean  ## Generate release version
	@echo "+ $@"
	@tar -cvzf ala273_base_project.tar.gz .

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-22s\033[0m %s\n", $$1, $$2}'
	@echo "Contact author for help: ala273@mail.usask.ca"

$(PROGRAMS): CFLAGS += $(TARGET)