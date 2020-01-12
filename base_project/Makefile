CFLAGS = -std=gnu99 -Wall -pedantic -g

.DEFAULT_GOAL := all

.PHONY: all
all: clean base_project ## Clean and compile
	@echo "+ $@"

.PHONY: release
release: clean  ## Generate release version
	@echo "+ $@"
	@tar -cvzf base_project.tar.gz .

.PHONY: clean
clean: clean-build clean-debug ## Remove all file artifacts

.PHONY: clean-build
clean-build: ## Remove all build artifacts
	@echo "+ $@"
	@rm -f base_project
	@rm -rf *.tar.gz

.PHONY: clean-debug
clean-debug: ## Remove all debug artifacts
	@echo "+ $@"
	@rm -f output.txt
	@rm -f .gdb_history
	@rm -rf vgcore*
	
.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-19s\033[0m %s\n", $$1, $$2}'
	@echo "Contact author for more help: ala273@mail.usask.ca"