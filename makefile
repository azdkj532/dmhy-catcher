LIB = -lcurl -I/usr/include/mysql++/ -I/usr/include/mysql -lmysqlpp
CC  = g++

all:dmhy-catcher

dmhy-catcher: main.o dmhy.o rpc_remote.o html_source.o database.o sha256.o
	$(CC) -o dmhy-catcher main.o rpc_remote.o dmhy.o html_source.o database.o src/sha256.o $(LIB)

main.o: main.cpp dmhy.h rpc_remote.h
	$(CC) -c main.cpp $(LIB)

html_source.o:  html_source.cpp  html_source.h
	$(CC) -c html_source.cpp $(LIB)

dmhy.o:  dmhy.cpp  dmhy.h html_source.h database.h
	$(CC) -c dmhy.cpp $(LIB)

rpc_remote.o: rpc_remote.h rpc_remote.cpp
	$(CC) -c rpc_remote.cpp $(LIB)

database.o: database.h database.cpp src/sha256.h
	$(CC) -c database.cpp $(LIB)
sha256.o: src/sha256.h src/sha256.cpp
	$(CC) -c src/sha256.cpp -o src/sha256.o

install: dmhy-catcher
	cp dmhy-catcher /bin

.PHONY:clean
.PHONY:install

clean:
	$(RM) *.o
