CC = gcc
CFLAGS = -Wall

SUFFIX = $(shell getconf LONG_BIT)

AGENT         = semSharedMemAgent
WATCHER       = semSharedMemWatcher
SMOKER        = semSharedMemSmoker
MAIN          = probSemSharedMemSmokers

OBJS = sharedMemory.o semaphore.o logging.o

.PHONY: all gr wt ch rt all_bin clean cleanall

all:		clean  agent        watcher      smoker       main  
ag:		    clean  agent        watcher_bin  smoker_bin   main 
wt:		    clean  agent_bin    watcher      smoker_bin   main
sm:		    clean  agent_bin    watcher_bin  smoker       main 
all_bin:	clean  agent_bin    watcher_bin  smoker_bin   main 

agent:	$(AGENT).o $(OBJS)
	$(CC) -o ../run/$@ $^ -lm

watcher:	$(WATCHER).o $(OBJS)
	$(CC) -o ../run/$@ $^

smoker:	$(SMOKER).o $(OBJS)
	$(CC) -o ../run/$@ $^ -lm

main:		$(MAIN).o $(OBJS)
	$(CC) -o ../run/$(MAIN) $^ -lm

agent_bin:
	cp ../run/agent_bin_$(SUFFIX) ../run/agent

watcher_bin:
	cp ../run/watcher_bin_$(SUFFIX) ../run/watcher

smoker_bin:
	cp ../run/smoker_bin_$(SUFFIX) ../run/smoker

clean:
	echo clean
	rm -f *.o

cleanall:	clean
	rm -f ../run/$(MAIN) ../run/agent ../run/watcher ../run/smoker 

