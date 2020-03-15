# this is an example makefile for the first homework
# assignment this semester

all: asio-1.12.2 src/chat_server src/chat_client src/json

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

CXXFLAGS+= -DASIO_STANDALONE -Wall -O0 -g -std=c++11
CPPFLAGS+=-I./include -I./asio-1.12.2/include
LDLIBS+= -lpthread

clean:
	-rm -rf asio-1.12.2
	-rm -f src/chat_client
	-rm -f src/chat_server		
	-rm -f src/json

