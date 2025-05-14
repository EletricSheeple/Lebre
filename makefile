TARGET = lebre.bin

CC = gcc
CCFLAGS = -Wall -Wextra -O2
CCFLAGSD = -g

ifneq ($(DEBUG), 0)
	CCFLAGS += $(CCFLAGSD)
endif

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $^
	@echo "Linking complete: $(TARGET)"