CC=gcc

TARGET=test_atsha204a
SRCS= atsha204a_i2c.c atsha204a_comm.c atsha204a_test.c
OBJS=$(SRCS:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

%o:%c
	$(CC) -c $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(TARGET) $(OBJS)
