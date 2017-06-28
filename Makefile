CC=gcc
LINK_TARGET = utf8test
OBJS = utf8test.o

$(LINK_TARGET) : $(OBJS)
	$(CC) -o $(LINK_TARGET) $(OBJS)

$(OBJS) : utf8test.c
	$(CC) -c utf8test.c

clean :
	rm $(OBJS) $(LINK_TARGET)

