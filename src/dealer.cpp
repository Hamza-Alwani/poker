//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"
#include "chat_message.hpp"
#include "json.hpp"
#include <ctime>


using asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------
int pot=0;
bool inplay=0;
int tplayer=-1;
int inplayer=-1;
int hand[52];
int active_players;
int cardnumber=0;
int ce=0;


std::string deck[52]=
{"AC","KC","QC","JC","2C", "3C","4C","5C","6C","7C","8C","9C", "TC",
 "AD","KD","QD","JD","2D", "3D","4D","5D","6D","7D","8D","9D", "TD",
 "AH","KH","QH","JH","2H", "3H","4H","5H","6H","7H","8H","9H", "TH",
 "AS","KS","QS","JS","2S", "3S","4S","5S","6S","7S","8S","9S", "TS",
};
bool cur_state_r1=0;
bool cur_state_r2=0;
bool cur_state_e=0;
int turn=0;
int count=0;
void shuffle()
{
srand(time(0));
int i=0;
int temp;
int found=0;
	while(i!=51)
	{
		temp=rand()%52;
		found=0;
		for(int k=0;k<52;k++)
		{
			if(hand[k]==temp)
				{
					found=1;
				}
		}
		if(found==0)
		{
			hand[i]=temp;
			i++;
		}
	}


};

class player
{
public:
	std::string id;
	std::string phand[5];
	bool active;
	int bet;
	void set_id(std::string s)
	{
		id=s;
	}
	 std::string get_id()
	{
		return id;
	}
	void set_active(bool a)
	{
		active=a;
	}
	bool get_active()
	{
		return active;
	}
	void set_bet(int b )
	{
		bet=b;
	}
	int get_bet()
	{
		return bet;
	}
	
	void set_phand(std::string a, std::string b, std::string c, std::string d, std::string e)
	{
		phand[0]=a;
		phand[1]=b;
		phand[2]=c;
		phand[3]=d;
		phand[4]=e;
	}
	std::string get_phand(int i)
	{
		return phand[i];
	}

	
};

player pl[5];

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
  	if(tplayer<4)
  	{
  		tplayer++;
    	participants_.insert(participant);
  	}
  	
  }	

  void leave(chat_participant_ptr participant)
  {
  	tplayer--;
    participants_.erase(participant);
  }

  void deliver(const chat_message& msg)
  {
  	
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_) //cse3310 messages are sent to all connected clients
      participant->deliver(msg);
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 }; //cse3310  maximum number of messages are stored
  chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------









class chat_session
  : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(tcp::socket socket, chat_room& room)
    : socket_(std::move(socket)),
      room_(room)
  {
  }

  void start()
  {
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void do_read_header()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
           do_read_body();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }


  void do_read_body()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
          	std::cout.write(read_msg_.body(), read_msg_.body_length()); //cse3310 message body is received from the server
            std::cout <<"\n";
          	
           char outline[read_msg_.body_length() + 2];
            // '\n' + '\0' is 2 more chars
            outline[0] = '\n';
            outline[read_msg_.body_length() + 1] = '\0';
            std::memcpy ( &outline[1], read_msg_.body(), read_msg_.body_length() );
            std::string str(outline);
            
            
            
             
       
          	if(str.find("join")!=-1)
           	{
        	   
        	   	int pos= str.find(",");
        	   	pos=pos+2;
        	   	int len;
            	len= str.find("\"]")-pos;
            	std::string temp;
            	temp=str.substr(pos,len);
            	std::cerr<<temp;
            	pl[tplayer].set_id(temp);
            	if(inplay==0)	
          		{
          			inplayer++;
            		pl[tplayer].set_active(1);
            	}
            	else
            	{
            		pl[tplayer].set_active(0);
            	}	
            }
          
         
            
            if( inplay==1 && cur_state_r1==1)
		    {
		    	std::cerr<<"round1\n";
		    	round(str);
		    }
            else if( inplay==1 && cur_state_e==1)
		    {
		    	std::cerr<<"exchange round\n";
		    	exchange(str);
		    }
		    
		  	else if( inplay==1 && cur_state_r2==1)
		    {
				std::cerr<<"round2\n";
		    	round2(str);
		    }
            else if(tplayer>=1 && inplay==0 && tplayer==inplayer)
           	{
           		std::cerr<<"game start\n";
           		setup();      
           		cur_state_r1=1; 	
		    }    
            do_read_header();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }


  void do_write()
  {
    auto self(shared_from_this());
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }
  void send(nlohmann::json j)
  {
  	
    chat_message msg;
    char text[514];
    std::string temp;
	temp=j.dump();
	strcpy(text,temp.c_str());
    msg.body_length ( strlen(text) );
    std::memcpy(msg.body(), text, msg.body_length());
    msg.encode_header();
    room_.deliver(msg);
  
  }
  void sendreset()
  {
  	std::cerr<<"in reset";
  	for(int i=0; i<=tplayer;i++)
  	{
  		std::cerr<<"in reset loop";
  		nlohmann::json j{  
			{"0,",pl[i].get_id()},	
    		{"action", "R"}
    		};
		std::cerr<<j.dump();
		chat_message msg;
		char text[514];
		std::string temp;
		temp=j.dump();
		strcpy(text,temp.c_str());
		msg.body_length ( strlen(text) );
		std::memcpy(msg.body(), text, msg.body_length());
		msg.encode_header();
		room_.deliver(msg);
	
  	}
  
  }
  
  void sendall(int n)
  {
  	for(int i=0; i<=inplayer;i++)
  	{
  		if(pl[i].get_active()==1)
  		{
  			nlohmann::json j{  
				{"0,",pl[i].get_id()},	
    			{"action", "U"},
    			{"currentbet",n},
    			{"pot",pot}
    		};
  			//std::cerr<<j.dump();
//  			std::cerr<<"in send json is"<<j.dump()<<std::endl;
			chat_message msg;
			char text[514];
			std::string temp;
			temp=j.dump();
			strcpy(text,temp.c_str());
			msg.body_length ( strlen(text) );
			std::memcpy(msg.body(), text, msg.body_length());
			msg.encode_header();
			room_.deliver(msg);
	  	}
  	}
  	
  
  }
  
	void setup()
	{
	inplay=1;
    std::string temp;
    shuffle();
    nlohmann::json j;
    std::string pcard[5];
    active_players=inplayer;
		for(int i=0; i<=inplayer;i++)
		{
			for(int k=0;k<5;k++)
			{
				pcard[k]=deck[hand[cardnumber]];
				cardnumber++;
				
			}
			pl[i].set_phand(pcard[0],pcard[1],pcard[2],pcard[3],pcard[4]);
			j=j.create(pl[i].get_id(),pcard[0],pcard[1],pcard[2],pcard[3],pcard[4],"S",temp,1,pot);
			send(j);
		}
	
	}
	void round(std::string s)
	{		
		int pos;
		int len;
		std::string tid;            
		pos=s.find(",\"0\"");
		pos= pos+9;
		len=s.find("\",\"action")-pos;
		tid=s.substr(pos,len);	
		if(turn>inplayer)
		{
			turn=0;
		}
		if(pl[turn].get_active()==0 && pl[turn].get_id()==tid)
		{
			turn++;
		}	
		else if(pl[turn].get_active()==1 && pl[turn].get_id()==tid)
			{	
				pos=s.find(",\"action\"");
				pos=pos+11;
				std::string action;
				action=s.substr(pos,1);
				
				
				if(action=="B")
				{
					bet(s);
					turn++;
					count++;
				}
				if(action=="F")
				{
					fold();
					turn++;
					count++;
				}
				check_round();		
			}
					
	}
	

	void check_round()
	{
		bool c=0;
		if(active_players==0)
		{
			end();
		}
		else if(count>inplayer)
		{
			
			for(int i=0;i<=inplayer;i++)
			{
			
				if(pl[i].get_active()==1)
				{
				
					for(int k=i;k<=inplayer;k++)
					{
					
						if(pl[k].get_active()==1)
						{
						
							if(pl[k].get_bet()!= pl[i].get_bet())
								{
									
									c=1;
									i=8;
									k=8;
								}		
						}
					}
				}
			}
			if(c==0)
			{
				cur_state_r1=0;
				cur_state_e=1;
				turn=0;
				
			}	
		}	
	}


	void exchange(std::string s)
	{
		int pos;
		int len;
		std::string tid;            
		pos=s.find(",\"0\"");
		pos= pos+9;
		len=s.find("\",\"action")-pos;
		tid=s.substr(pos,len);	
		
		if(pl[turn].get_active()==0)// && pl[turn].get_id()==tid)
		{
			turn++;
		}	
		else if(pl[turn].get_active()==0 && pl[turn].get_id()==tid)// && pl[turn].get_id()==tid)
		{
			turn++;
		}
		else if(pl[turn].get_active()==1 && pl[turn].get_id()==tid)
		{	    
				pos=s.find("\"toexchange\"");
				pos=pos+13;
				len=pos-s.find("}")-1;
				std::string te= s.substr(pos,len);
				nlohmann::json j;
				std::string cards[5];
				
				
				if(te.find("1")!=-1)
				{
					cardnumber++;
					cards[0]=deck[hand[cardnumber]];
				}
				else
				{
					pos=s.find(",\"card1\"");
					pos=pos+10;
					cards[0]=s.substr(pos,2);
				}
				
				
				if(te.find("2")!=-1)
				{
					cardnumber++;
					cards[1]=deck[hand[cardnumber]];
				}
				else
				{
					pos=s.find(",\"card2\"");
					pos=pos+10;
					cards[1]=s.substr(pos,2);
				}
				
				
				if(te.find("3")!=-1)
				{
					cardnumber++;
					cards[2]=deck[hand[cardnumber]];
				}
				else
				{
					pos=s.find(",\"card3\"");
					pos=pos+10;
					cards[2]=s.substr(pos,2);
				}
				
				
				if(te.find("4")!=-1)
				{
					cardnumber++;
					cards[3]=deck[hand[cardnumber]];
				}
				else
				{
					pos=s.find(",\"card4\"");
					pos=pos+10;
					cards[3]=s.substr(pos,2);
				}
				
				
				if(te.find("5")!=-1)
				{
					cardnumber++;
					cards[4]=deck[hand[cardnumber]];
				}
				else
				{
					pos=s.find(",\"card5\"");
					pos=pos+10;
					cards[4]=s.substr(pos,2);
				}
			pl[turn].set_phand(cards[0],cards[1],cards[2],cards[3],cards[4]);
			j=j.create(pl[turn].get_id(),cards[0],cards[1],cards[2],cards[3],cards[4],"S","",0,pot);
			send(j);	
			turn++;	
		}
		if(turn>inplayer)
		{
			turn=0;
			count=0;
			cur_state_r2=1;
			cur_state_e=0;
		}
	}
	
	
	

	void round2(std::string s)
	{		
		int pos;
		int len;
		std::string tid;            
		pos=s.find(",\"0\"");
		pos= pos+9;
		len=s.find("\",\"action")-pos;
		tid=s.substr(pos,len);	
		
		
		if(turn>inplayer)
		{
			turn=0;
		}
		if(pl[turn].get_active()==0 && pl[turn].get_id()==tid)
		{
			turn++;
		}	
		else if(pl[turn].get_active()==1 && pl[turn].get_id()==tid)
			{	
				pos=s.find(",\"action\"");
				pos=pos+11;
				std::string action;
				action=s.substr(pos,1);
				if(action=="B")
				{
					bet(s);
					turn++;
					count++;
				}
				if(action=="F")
				{
					fold();
					turn++;
					count++;
				}
				check_round2();		
			}
					
	}
	

	void check_round2()
	{
		
		bool c=0;
		if(active_players==0)
		{
			end();
		}
		else if(count>inplayer)
		{
			
			for(int i=0;i<=inplayer;i++)
			{
			
				if(pl[i].get_active()==1)
				{
				
					for(int k=i;k<=inplayer;k++)
					{
					
						if(pl[k].get_active()==1)
						{
						
							if(pl[k].get_bet()!= pl[i].get_bet())
								{
									
									c=1;
									i=8;
									k=8;
								}		
						}
					}
				}
			}
			if(c==0)
			{
				end();
			}
		}

		
	}

	
	
	
	void end()
	{
		unsigned long int score=0;
		unsigned long int highscore=0;
		int winnerid=0;
		
		std::cerr<<"REACHED END";
		for(int i=0;i<=inplayer;i++)
		{
			if(pl[i].get_active()==1)
			{
				score=get_value(i);
				std::cerr<<"score of "<<i<<"="<<score<<std::endl;
				if(score>highscore)
				{
					highscore=score;
					winnerid=i;
				}
			}
		}
		nlohmann::json j{  
				{"0,",pl[winnerid].get_id()},	
    			{"action", "W"},
    			{"pot",pot}
		};
		send(j);
		reset();
		
	}
	

  	void bet(std::string s)
	{
				int pos;
				int cbet;
				pos=0;
				int len=0;
				pos=s.find(",\"currentbet\"") +14;
				len=s.find(",\"pot\"")-pos;
				std::string temp=s.substr(pos,len);
				cbet=atoi(temp.c_str());
				pot+=cbet;
				pl[turn].set_bet(cbet);
				sendall(pl[turn].get_bet());
	}
	void fold()
	{
		
		active_players--;
		pl[turn].set_active(0);
	}
  
 	unsigned long int get_value(int i)
	{
		
		std::string tphand[5];
		tphand[0]=pl[i].get_phand(0);
		tphand[1]=pl[i].get_phand(1);
		tphand[2]=pl[i].get_phand(2);
		tphand[3]=pl[i].get_phand(3);
		tphand[4]=pl[i].get_phand(4);
		int val[13];
		int suite[4];
		std::string key;
		int  j;
		
		for (int k = 1; k < 5; k++) 
		{  
			key = tphand[k];  
		    j = k - 1;  
		    while (j >= 0 && tphand[j]>key) 
		    {  
		        tphand[j + 1] = tphand[j];  
		        j = j - 1;  
		    }  
		    tphand[j + 1] = key;  
		}  
		
		pl[i].set_phand(tphand[0],tphand[1],tphand[2],tphand[3],tphand[4]);
		for(int a=0;a<5;a++)
		{
			std::cerr<<pl[i].get_phand(a);
		}
		std::cerr<<std::endl;
	
	
		std::string temp;
		for(int a=0;a<13;a++)
		{
			val[a]=0;
				
		}
		
		
	
		for(int k=0;k<5;k++)
		{
			temp=pl[i].get_phand(k);
			temp=temp.substr(0,1);
			
			if(temp=="2")
				val[0]= val[0]+1;
			if(temp=="3")
				val[1]= val[1]+1;
			if(temp=="4")
				val[2]= val[2]+1;
			if(temp=="5")
				val[3]= val[3]+1;
			if(temp=="6")
				val[4]= val[4]+1;
			if(temp=="7")
				val[5]= val[5]+1;
			if(temp=="8")
				val[6]= val[6]+1;
			if(temp=="9")
				val[7]= val[7]+1;
			if(temp=="T")
				val[8]= val[8]+1;
			if(temp=="J")
				val[9]= val[9]+1;
			if(temp=="Q")
				val[10]= val[10]+1;
			if(temp=="K")
				val[11]= val[11]+1;
			if(temp=="A")
				val[12]= val[12]+1;
		}
		for(int k=0;k<5;k++)
		{
			temp=pl[i].get_phand(k);
			temp=temp.substr(1,1);
			if(temp=="C")
				suite[0]= suite[0]+1;
			if(temp=="D")
				suite[1]= suite[1]+1;
			if(temp=="H")
				suite[2]= suite[2]+1;
			if(temp=="S")
				suite[3]= suite[3]+1;
		}
		
		int rank[5];
		
		for(int k=0;k<5;k++)
		{
			temp=pl[i].get_phand(k);
			temp=temp.substr(0,1);
			if(temp=="2")
				rank[k]= 2;
			if(temp=="3")
				rank[k]= 3;
			if(temp=="4")
				rank[k]= 4;
			if(temp=="5")
				rank[k]= 5;
			if(temp=="6")
				rank[k]= 6;
			if(temp=="7")
				rank[k]= 7;
			if(temp=="8")
				rank[k]= 8;
			if(temp=="9")
				rank[k]= 9;
			if(temp=="T")
				rank[k]= 10;
			if(temp=="J")
				rank[k]= 11;
			if(temp=="Q")
				rank[k]= 12;
			if(temp=="K")
				rank[k]= 13;
			if(temp=="A")
				rank[k]= 14;
	}
	int key1;
	for (int k = 1; k < 5; k++) 
		{  
			key1 = rank[k];  
		    j = k - 1;  
		    while (j >= 0 && rank[j]>key1) 
		    {  
		        rank[j + 1] = rank[j];  
		        j = j - 1;  
		    }  
		    rank[j + 1] = key1;  
		}  

	if(stright(val) && flush(suite))
	{
		std::cerr<<"sf";
		return 8000000+ value_high(val);
	}
	else if(four_of_a_kind(val))
	{
		std::cerr<<"fok";
		return 7000000+ four_of_a_kind(val);
	}
	else if(full_house(val))
	{
		std::cerr<<"f-ho";
		return	 6000000 + full_house(val);
	}
	else if(flush(suite))
	{
		std::cerr<<"f";
		return	 5000000 + value_high(val);
	}
	else if(stright(val))
	{
		std::cerr<<"s";
		return  4000000 + value_high(val);
	}
	else if(three_of_a_kind(val))
	{
		std::cerr<<"tok";
		return	 3000000 + three_of_a_kind(val);
	}	
	else if(two_pair(rank))
	{
		std::cerr<<"tp";
		return	 2000000 + two_pair(rank);
	}
	else if(pair(rank))
	{
		std::cerr<<"p";
		return	 1000000 + pair(rank);
	}
	else
		return value_high(rank);

}

	int value_high(int val[])
	{
		int temp=0;
		int count=1;
		for(int a=5;a>=0;a--) //two piar
		{
				temp=temp*10;
				temp+=val[a];
		}
		return temp;
	}
	
	
	int pair(int val[])
	{
		if(val[0]==val[1])
			return val[1];
		else if(val[1]==val[2])
			return val[2];
		else if(val[2]==val[3])
			return val[3];
		else if(val[3]==val[4])
			return val[4];
		else
			return 0;
	}

	int two_pair(int val[])
	{
		if(val[0]== val[1] && val[2]==val[3])
		{
			std::cerr<<"tp";
			return (pow(14,4)*val[2])+(pow(14,2)*val[0])+val[4];
		}
		else if(val[0]==val[1] && val[3]==val[4])
		{
		std::cerr<<"tp";
			return (pow(14,4)*val[3])+(pow(14,2)*val[1])+val[2];
		}
		else if(val[1]==val[2] && val[3]==val[4])
		{
			std::cerr<<"tp";
			return (pow(14,4)*val[2])+(pow(14,2)*val[3])+val[5];
		}	
		else
			return 0;
	}
	
	int three_of_a_kind(int val[])
	{
		for(int a=0;a<13;a++) //check three of a kind
		{
			if(val[a]==3)
				{
				std::cerr<<"tok";
					return (a+1);
				}
		}
		return 0;
	}
	
	int four_of_a_kind(int val[])
	{
		for(int a=0;a<13;a++)
		{
			if(val[a]==4)
				{
				std::cerr<<"fok";
					return (a+1);
				}
		}
		return 0;
	}
	
	int stright(int val[])
	{
		for(int a=0;a<9;a++) //straight
		{
			
			if(val[a]==1 && val[a+1]==1 && val[a+2]==1 && val[a+3]==1 && val[a+4]==1)
			{
			std::cerr<<"s";
				return (a+4);
			}
		}
		return 0;	
	}
	
	int flush (int suite[])
	{
		for(int a=0;a<4;a++) //flush
		{
			
			if(suite[a]==5)
			{
				std::cerr<<"F";
				return (a+1);
			}
		}
		return 0;		
	}
	
	int full_house(int val[])
	{
		for(int a=0;a<13;a++) //two piar
		{
			for(int b=0;a<13;a++)
			{
				if(val[a]==2 && val[b]==3)
				{			
					std::cerr<<"F-house";
					return b+1;	
				}
			}
		}
		return 0;
	}
	
	
		
	
	void reset()
	{
		for(int zz=0;zz<52;zz++)
    	{
   	 		hand[zz]=1;
    	}
    	cur_state_r1=0;
		cur_state_r2=0;	
		cur_state_e=0;
		turn=0;
		count=0;
		inplay=0;
		pot=0;
		sendreset();
		inplayer=-1;

	}
  
  
  tcp::socket socket_;
  chat_room& room_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(asio::io_context& io_context,
      const tcp::endpoint& endpoint)
    : acceptor_(io_context, endpoint)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<chat_session>(std::move(socket), room_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  chat_room room_;
};

//----------------------------------------------------------------------



int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    asio::io_context io_context;

    std::list<chat_server> servers;
    for (int i = 1; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_context, endpoint);
    }
    //std::cout<<"gets here"<<std::endl;
//    while(true)
//    {
//    	run();
//    }
   
    io_context.run();
   // std::cout<<"gets here1"<<std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
