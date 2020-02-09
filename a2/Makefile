
SRC_DIR		= src
BUILD_DIR 	= build
INCLUDES	= include

CC 			= gcc
CFLAGS 		= -g -O2 -Wall -Wextra -Werror -Wno-unused-function\
              -Wno-unused-but-set-parameter -Wno-unused-parameter -Wno-address\
              -Wno-unused-variable -Wno-return-local-addr -Wno-sign-compare -Wno-pointer-sign -rdynamic\
              -DNDEBUG

all: build_dir $(BUILD_DIR)/client $(BUILD_DIR)/server $(BUILD_DIR)/forwarder
	
$(BUILD_DIR)/client: $(SRC_DIR)/client.c $(SRC_DIR)/frame.c  $(INCLUDES)/frame.h
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/client.c $(SRC_DIR)/frame.c -I $(INCLUDES)

$(BUILD_DIR)/server: $(SRC_DIR)/server.c $(SRC_DIR)/frame.c  $(INCLUDES)/frame.h
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/server.c $(SRC_DIR)/frame.c -I $(INCLUDES)

$(BUILD_DIR)/forwarder: $(SRC_DIR)/forwarder.c $(SRC_DIR)/frame.c  $(INCLUDES)/frame.h
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/forwarder.c $(SRC_DIR)/frame.c -I $(INCLUDES)

build_dir:
	mkdir -p $(BUILD_DIR)
	
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
	-$(RM) -fr $(BUILD_DIR)

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
	@tar -cvzf base_project.tar.gz *

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-22s\033[0m %s\n", $$1, $$2}'
	@echo "Contact author for help: ala273@mail.usask.ca"

$(PROGRAMS): CFLAGS += $(TARGET)
		
