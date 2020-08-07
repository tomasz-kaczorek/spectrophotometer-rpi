NAME=spectrophotometer

SRC_DIR=./src
OBJ_DIR=./obj
BIN_DIR=./bin

MODULES=gpio spi ads opto motor amp args tcp spectrophotometer
SOURCES=$(MODULES:%=$(SRC_DIR)/%.c)
OBJECTS=$(MODULES:%=$(OBJ_DIR)/%.o)

CC=gcc
LDFLAGS=
CFLAGS=-std=gnu99 -Wall -Wextra -pedantic

ifdef RELEASE
CFLAGS+=-O3 -ffunction-sections -fdata-sections -s
endif

all:
	@mkdir -p -v $(OBJ_DIR)
	@mkdir -p -v $(BIN_DIR)
	@$(MAKE) --no-print-directory main

main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o $(LDFLAGS) $(SRC_DIR)/main.c -o $(BIN_DIR)/$(NAME)


$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

release: export RELEASE=DEFINED
release: all

run:
	cd $(BIN_DIR) && sudo ./$(NAME)

clean:
	find $(BIN_DIR) -type f -executable -delete
	rm -f $(OBJ_DIR)/*.o
