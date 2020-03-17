#include <stdio.h>
#include <iostream>
#include "asio.hpp"
#include "json.hpp"


class player
{
public:
  void connect()
  {
  
  }
  void send(nlohmann::json hand )
  {
  }
  nlohmann::json recive()
  {
   nlohmann::json hand;
   return hand; 
  }
  nlohmann::json bet()
  {
	nlohmann::json hand;
	return hand; 
  }
  nlohmann::json check()
  {
	nlohmann::json hand;
	return hand; 
  }
  nlohmann::json exchange()
  {
	nlohmann::json hand;
	return hand; 
  }
  
  
  
  
private:
int balance;
};




