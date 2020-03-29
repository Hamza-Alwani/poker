# this is an example makefile for the first homework
# assignment this semester

all:  asio-1.12.2  src/player  src/dealer src/json  src/chat_client src/gtk

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

 
CXXFLAGS+= -DASIO_STANDALONE  -rdynamic -Wall -O0 -g  -std=c++11  `pkg-config --cflags gtk+-3.0` `pkg-config gtkmm-3.0 --cflags`
CPPFLAGS+=-I./include -I./asio-1.12.2/include  `pkg-config --cflags gtk+-3.0` `pkg-config gtkmm-3.0 --cflags`
LDLIBS =   -lpthread  `pkg-config --cflags gtk+-3.0`  `pkg-config gtkmm-3.0 --libs`



clean:
	-rm -rf asio-1.12.2
	-rm -f src/player
	-rm -f src/dealer		
	-rm -f src/json
	-rm -f src/global
	-rm -f src/chat_client
	-rm -f src/gtk
	
    



	
   
