all: main.c
	gcc main.c QueueMessage.c ListMessage.c Message.c SharedMemory.c -o server -lm -lpthread
clean:
	$(RM) server