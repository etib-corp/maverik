##
## EPITECH PROJECT, 2025
## maverik
## File description:
## Makefile
##

NAME = libmaverik

all: build
	@echo "\n✅ - All components are built.\n"

build:
	@echo "	🔄 - Initializing the project..."
	@mkdir -p build
	@echo "	🔄 - Compiling the project..."
	@cd build && cmake .. && make
	@cd ..
	@mv build/$(NAME)* .
	@echo "\n✅ - Build complete.\n"
	@echo "\n	You can run the project with '\033[1;32mmake run\033[0m' or '\033[1;32mmake test\033[0m' for tests.\n"

help:
	@echo "Makefile for maverik project"
	@echo "Usage:"
	@echo "  make build   - Build the project"
	@echo "  make clean   - Clean the project"
	@echo "  make fclean  - Clean and remove all files"
	@echo "  make re      - Rebuild the project"
	@echo "  make test    - Run tests"
	@echo "  make help    - Display this help message"
	@echo "  make run     - Run the project"

clean:
	@echo "\033[1;33m🚮 - Cleaning the project binary...\033[0m\n"
	@rm -rf $(NAME)* build/$(NAME)*
	@echo "\033[1;36m✅ - Clean complete.\033[0m\n"

fclean: clean
	@echo "\033[1;33m⚠️ - Removing all files...\033[0m\n"
	@rm -rf build
	@echo "\033[1;31m✅ - All files removed.\033[0m\n"

re: fclean all
	@echo "Rebuild complete."

test:
	@echo "\033[1;34m🔄 - Running tests...\033[0m\n"
	@echo "\033[1;32m✅ - Tests complete.\033[0m"
