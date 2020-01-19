CC		:= gcc
CFLAGS	:= -std=gnu99 -g -O2 -Wall -Wextra -pedantic $(OPTFLAGS)

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:= -ldl $(OPTLIBS)

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= main.exe
else
EXECUTABLE	:= main
endif

SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)

CINCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
CLIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

SOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
OBJECTS		:= $(SOURCES:.c=.o)

.DEFAULT_GOAL := run

all: clean $(BIN)/$(EXECUTABLE)

.PHONY: clean
clean: clean-build clean-debug ## Remove all file artifact

.PHONY: clean-build
clean-build: ## Remove build artifacts
	@echo "+ $@"
	-$(RM) $(BIN)/$(EXECUTABLE)
	-$(RM) $(OBJECTS)
	-$(RM) *.tar.gz

.PHONY: clean-debug
clean-debug: ## Remove debug artifacts
	@echo "+ $@"
	@rm -f output.txt
	@rm -f .gdb_history
	@rm -rf vgcore*

.PHONY: release
release: clean  ## Generate release version
	@echo "+ $@"
	@tar -cvzf ala273_asn_01.tar.gz .

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-22s\033[0m %s\n", $$1, $$2}'
	@echo "Contact author for help: ala273@mail.usask.ca

.PHONY: run
run: all ## Build and Run
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(CINCLUDES) $(CLIBS) $^ -o $@ $(LIBRARIES)