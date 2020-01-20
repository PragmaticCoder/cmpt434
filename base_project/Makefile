CC						:= gcc
CFLAGS				:= -g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LDFLAGS				:= -ldl $(OPTLIBS)
PREFIX				?= /usr/local

BIN						:= bin
SRC						:= src
TESTS 				:= tests

SOURCES				:= $(wildcard $(SRC)/**/*.c src/*.c)
HEADERS				:= $(wildcard $(SRC)/**/*.h src/*.h)
OBJECTS				:= $(patsubst %.c,%.o,$(SOURCES))

TEST_SRC			:= $(wildcard $(TESTS)/*_tests.c)
TESTS					:= $(patsubst %.c,%,$(TEST_SRC))

PROGRAMS_SRC	:= $(wildcard bin/*.c)
PROGRAMS			:= $(patsubst %.c,%,$(PROGRAMS_SRC))

TARGET			:= build/libbase.a
SO_TARGET		:= $(patsubst %.a,%.so,$(TARGET))

all: $(TARGET) $(SO_TARGET) $(TESTS) $(PROGRAMS)

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
	@mkdir -p build
	@mkdir -p $(BIN)

install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/
	install $(SO_TARGET) $(DESTDIR)/$(PREFIX)/lib/
	install -d $(DESTDIR)/$(PREFIX)/include/baseproject/
	install $(HEADERS) $(DESTDIR)/$(PREFIX)/include/baseproject/

check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)\
		|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true

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

$(PROGRAMS): CFLAGS += $(TARGET)