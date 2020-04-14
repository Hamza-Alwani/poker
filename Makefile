# this is an example makefile for the first homework
# assignment this semester

all:  asio-1.12.2    src/gtk src/dealer src/json 
#src/chat_client 

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

 
GTKLINKFLAGS=$(shell pkg-config --libs gtkmm-3.0)
GTKCOMPILEFLAGS=$(shell pkg-config --cflags gtkmm-3.0)
#GLINK=$(shell pkg-config --cflags gtk+-3.0)
#GCF=$(shell pkg-config --cflags gtk+-3.0)
# 
#CXXFLAGS+= -DASIO_STANDALONE  -rdynamic -Wall -O0 -g  -std=c++11  `pkg-config --cflags gtk+-3.0` `pkg-config gtkmm-3.0 --cflags`
#CPPFLAGS+=-I./include -I./asio-1.12.2/include  `pkg-config --cflags gtk+-3.0` `pkg-config gtkmm-3.0 --cflags`
#LDLIBS =   -lpthread  `pkg-config --cflags gtk+-3.0`  `pkg-config gtkmm-3.0 --libs`

CXXFLAGS+= -DASIO_STANDALONE -Wall -O0 -g -std=c++11 -rdynamic 
CPPFLAGS+= ${GTKCOMPILEFLAGS} -I./include -I./asio-1.12.2/include
LDLIBS+=-lpthread ${GTKLINKFLAGS}


clean:
	-rm -rf asio-1.12.2
	-rm -f src/dealer		
	-rm -f src/json
	-rm -f src/chat_client
	-rm -f src/gtk

	
	
    



	
   
