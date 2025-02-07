CC := gcc
CFLAGS := -Wall -g -std=c17
BIN := ./bin
SRC := ./src
INC := ./include
IPATH := -I$(INC)

OBJS := $(BIN)/image_read.o $(BIN)/main.o $(BIN)/pixmap.o $(BIN)/heap.o $(BIN)/compress.o  $(BIN)/qtcmanip.o $(BIN)/variance.o
TARGET := codec

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(IPATH) -o $(TARGET) $(OBJS) -lm

$(BIN)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS)  $(IPATH) $< -o $@

clean:
	rm -f $(BIN)/*.o $(TARGET)
