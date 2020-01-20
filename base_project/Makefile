CC					:= gcc
CFLAGS			:= -g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)

LIBS 				:= -ldl $(OPTLIBS)
PREFIX			?= /usr/local

BIN					:= bin
SRC					:= src
LIB					:= lib
TESTS 			:= tests

SOURCES			:= $(wildcard $(SRC)/**/*.c src/*.c)
OBJECTS			:= $(patsubst %.c,%.o,$(SOURCES))

TEST_SRC		:= $(wildcard $(TESTS)/*_tests.c)
TESTS				:= $(patsubst %.c,%,$(TEST_SRC))

TARGET			:= build/libYOUR_LIBRARY.a
SO_TARGET		:= $(patsubst %.a,%.so,$(TARGET))

# The Target Build
all: $(TARGET) $(SO_TARGET) tests

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

# The Target Build
all: $(TARGET) $(SO_TARGET) tests

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TESTS):	$(TARGET) $(SO_TARGET)

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)\
		|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || tru

.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS) ## Run Unit Tests Suite
	sh ./tests/runtests.sh

.PHONY: clean
clean: clean-build clean-debug ## Remove all file artifact

.PHONY: clean-build
clean-build: ## Remove build artifacts
	@echo "+ $@"
	-$(RM) -r build $(OBJECTS) $(TESTS)
	-$(RM) tests/tests.log 
	-$(RM) *.tar.gz

	@find . -name "*.gc*" -exec rm {} \;
	-$(RM) -r `find . -name "*.dSYM" -print`

.PHONY: clean-debug
clean-debug: ## Remove debug artifacts
	@echo "+ $@"
	-$(RM) output.txt
	-$(RM) .gdb_history
	-$(RM) -r vgcore*

.PHONY: release
release: clean  ## Generate release version
	@echo "+ $@"
	@tar -cvzf ala273_base_project.tar.gz .

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-22s\033[0m %s\n", $$1, $$2}'
	@echo "Contact author for help: ala273@mail.usask.ca"
