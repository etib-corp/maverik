##
## EPITECH PROJECT, 2025
## maverik
## File description:
## Makefile
##

NAME = libmaverik

all: build
	@echo -e "\n✅ - All components are built.\n"

build:
	@echo -e "	🔄 - Initializing the project..."
	@mkdir -p build
	@echo -e "	🔄 - Compiling the project..."
	@cd build && cmake .. && make
	@cd ..
	@mv build/$(NAME)* .
	@echo -e "\n✅ - Build complete.\n"
	@echo -e "\n	You can run the project with '\033[1;32mmake run\033[0m' or '\033[1;32mmake test\033[0m' for tests.\n"

help:
	@echo -e "Makefile for maverik project"
	@echo -e "Usage:"
	@echo -e "  make build   - Build the project"
	@echo -e "  make clean   - Clean the project"
	@echo -e "  make fclean  - Clean and remove all files"
	@echo -e "  make re      - Rebuild the project"
	@echo -e "  make test    - Run tests"
	@echo -e "  make help    - Display this help message"
	@echo -e "  make run     - Run the project"

clean:
	@echo -e "\033[1;33m🚮 - Cleaning the project binary...\033[0m\n"
	@rm -rf $(NAME)* build/$(NAME)*
	@echo -e "\033[1;36m✅ - Clean complete.\033[0m\n"

fclean: clean
	@echo -e "\033[1;33m⚠️ - Removing all files...\033[0m\n"
	@rm -rf build
	@echo -e "\033[1;31m✅ - All files removed.\033[0m\n"

re: fclean all
	@echo -e "Rebuild complete."

test:
	@echo -e "\033[1;34m🔄 - Running tests...\033[0m\n"
	@echo -e "\033[1;32m✅ - Tests complete.\033[0m"
