CC := clang
SRC := src
BUILD := build
TARGET := bin/server bin/client

SOURCES := $(shell find $(SRC) -type f -name *.c)
OBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(SOURCES:.c=.o))
CFLAGS := -g -Wall
LIB :=
INC := -I include

all: server client

server: $(OBJECTS)
	$(CC) $(BUILD)/server.o $(BUILD)/message.o -o bin/server $(LIB)

client: $(OBJECTS)
	$(CC) $(BUILD)/client.o $(BUILD)/message.o -o bin/client $(LIB)

$(BUILD)/%.o: $(SRC)/%.c
	@mkdir -p $(BUILD)
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "$(RM) -r $(BUILD) $(TARGET)"; $(RM) -r $(BUILD) $(TARGET)

.PHONY: all clean server client