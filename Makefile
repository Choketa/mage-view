CC= gcc
CFLAGS= -lX11 -lm
TARGET = mage

SRCS = src/main.c

all: $(TARGET)

$(TARGET) : $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
