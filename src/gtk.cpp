
#include <stdio.h>

#include <gtkmm.h>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <assert.h>
#include "asio.hpp"
#include "chat_message.hpp"
#include "json.hpp"
#include <gtk/gtk.h>

	GtkWidget   *g_card1;
	GtkWidget   *g_card2;
	GtkWidget   *g_card3;
	GtkWidget   *g_card4;
	GtkWidget   *g_card5;
	GtkWidget   *g_bank;
	GtkWidget   *g_cpot;
	GtkWidget   *g_cbet;
	GtkWidget	*g_bet_hldr;
	GtkWidget	*g_exchange_button;
	GtkWidget	*g_err_lbl;
	bool 		c1=FALSE;
	bool 		c2=FALSE;
	bool 		c3=FALSE;
	bool 		c4=FALSE;
	bool 		c5=FALSE;
	nlohmann::json j;
	
	
using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }



  void write(const chat_message& msg)
  {
    asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void close()
  {
    asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type& endpoints)
  {
    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, tcp::endpoint)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

void setup(std::string s)
{
	int pos;
	std::string card1;
	std::string card2;
	std::string card3;
	std::string card4;
	std::string card5;
	std::string action;
	std::string cbet;
	std::string pot;
	
	pos=s.find(",\"card1\"");
	pos=pos+10;
	card1=s.substr(pos,3);
	pos=s.find(",\"card2\"");
	pos=pos+10;
	card2=s.substr(pos,3);
	pos=s.find(",\"card3\"");
	pos=pos+10;
	card3=s.substr(pos,3);
	pos=s.find(",\"card4\"");
	pos=pos+10;
	card4=s.substr(pos,3);
	pos=s.find(",\"card5\"");
	pos=pos+10;
	card5=s.substr(80,3);	
	action=s.substr(12,1);	
	pos=s.find(",\"pot\"");
	pos=pos-98;
	cbet=s.substr(98,1);
	pos=s.length();
	pos=pos-106;
	pot=s.substr(106,1);	
	char temp[30];	
	
	int cb;//=stoi(cbet);
	int p;//=stoi(pot);
	
	std::istringstream(cbet)>>cb;
	std::istringstream(pot)>>p;
	
	j=j.create(card1,card2,card3,card4,card5,action," ",cb,p);	
//	std::cerr<<j.dump();
	std::cerr<<j.dupm()<<endl;
	strcpy(temp,card1.c_str());
	gtk_button_set_label(GTK_BUTTON(g_card1),  temp);
	
	strcpy(temp,card2.c_str());
	gtk_button_set_label(GTK_BUTTON(g_card2),  temp);
	
	strcpy(temp,card3.c_str());
	gtk_button_set_label(GTK_BUTTON(g_card3),  temp);
	
	strcpy(temp,card4.c_str());
	gtk_button_set_label(GTK_BUTTON(g_card4),  temp);
	
	strcpy(temp,card5.c_str());
	gtk_button_set_label(GTK_BUTTON(g_card5),  temp);
	
	strcpy(temp,cbet.c_str());
	gtk_label_set_text(GTK_LABEL(g_cbet), temp);
	
	strcpy(temp,pot.c_str());
	gtk_label_set_text(GTK_LABEL(g_cpot), temp);  
}
 
 
  void do_read_body()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            char outline[read_msg_.body_length() + 2];
            // '\n' + '\0' is 2 more chars
            outline[0] = '\n';
            outline[read_msg_.body_length() + 1] = '\0';
            std::memcpy ( &outline[1], read_msg_.body(), read_msg_.body_length() );
            std::string str(outline);
	        setup(str);     
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write()
  {
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](std::error_code ec, std::size_t /*length*/)
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
            socket_.close();
          }
        });
  }

private:
  asio::io_context& io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

// global symbols
chat_client *c;

void send (nlohmann::json a)
{
	std::string temp;
	temp=j.dump();
	char text[514];
	strcpy(text,temp.c_str());
	chat_message msg;
    msg.body_length ( strlen(text) );
    std::memcpy(msg.body(), text, msg.body_length());
    msg.encode_header();
    assert ( c );  // this is a global class
    c->write(msg);
}

int main(int argc, char* argv[])
{
	
	  if (argc != 3)
   {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
   }
	GtkBuilder  *builder; 
	GtkWidget   *window ;
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "src/client.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_builder_connect_signals(builder, NULL);   
    g_card1 = GTK_WIDGET(gtk_builder_get_object(builder, "card1"));
    g_card2 = GTK_WIDGET(gtk_builder_get_object(builder, "card2"));
    g_card3 = GTK_WIDGET(gtk_builder_get_object(builder, "card3"));
    g_card4 = GTK_WIDGET(gtk_builder_get_object(builder, "card4"));
    g_card5 = GTK_WIDGET(gtk_builder_get_object(builder, "card5"));
    g_bank=GTK_WIDGET(gtk_builder_get_object(builder, "bank_int_lbl"));
    g_cbet=GTK_WIDGET(gtk_builder_get_object(builder, "cbet_lbl_int"));
    g_cpot=GTK_WIDGET(gtk_builder_get_object(builder, "pot_lbl_int"));
    g_cbet=GTK_WIDGET(gtk_builder_get_object(builder, "cbet_lbl_int"));
    g_bet_hldr=GTK_WIDGET(gtk_builder_get_object(builder,"bet_hldr"));
    g_err_lbl=GTK_WIDGET(gtk_builder_get_object(builder,"err_lbl"));
    g_exchange_button=GTK_WIDGET(gtk_builder_get_object(builder,"exchange_button"));

    g_object_unref(builder);
    gtk_widget_show(window); 
	gtk_label_set_text(GTK_LABEL(g_bank), "100");// set lbls
   	asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    c = new chat_client(io_context, endpoints);
    assert(c);
    std::thread t([&io_context](){ io_context.run(); });
    gtk_main();
    c->close();
    t.join();

return 0;
}







extern "C" void check_clicked_cb()
{
	int bet;
	int cbet;
	int bank;
	const gchar *entry_text;
	bet=0;
	entry_text=gtk_label_get_text(GTK_LABEL (g_cbet));
	cbet=atoi(entry_text);
	entry_text=gtk_label_get_text(GTK_LABEL (g_bank));
	bank=atoi(entry_text);
	if(bet<cbet)
	{
		
		gtk_label_set_text(GTK_LABEL(g_err_lbl), "bet must be bigger than current bet" );
	}
	else
	{
		j=j.set_a(j,"B");
		send(j);
	}
	
}


extern "C" void bet_clicked_cb()
{
	int bet;
	int cbet;
	int bank;
	const gchar *entry_text;
	entry_text=gtk_entry_get_text(GTK_ENTRY (g_bet_hldr));
	bet=atoi(entry_text);
	
	entry_text=gtk_label_get_text(GTK_LABEL (g_cbet));
	cbet=atoi(entry_text);
	
	entry_text=gtk_label_get_text(GTK_LABEL (g_bank));
	bank=atoi(entry_text);
	
	if(bet<bank)
	{
		if(bet<cbet)
		{
		gtk_label_set_text(GTK_LABEL(g_err_lbl), "bet must be bigger than current bet" );
		}
		else
		{
			bank=bank-bet;
			entry_text=g_strdup_printf("%i", bank);
			gtk_label_set_text(GTK_LABEL(g_bank),  entry_text);// set lbls
			j=j.set_a(j,"B");	
			send(j);
		}
	}
	else
	{
		gtk_label_set_text(GTK_LABEL(g_err_lbl), "bet is bigger than bank" );
	
	}
}

extern "C" void call_clicked_cb()
{
	
	int cbet;
	int bank;
	const gchar *entry_text;
	
	
	entry_text=gtk_label_get_text(GTK_LABEL (g_cbet));
	cbet=atoi(entry_text);
	
	entry_text=gtk_label_get_text(GTK_LABEL (g_bank));
	bank=atoi(entry_text);
	
	if(cbet<bank)
	{	
			bank=bank-cbet;
			entry_text=g_strdup_printf("%i", bank);
			gtk_label_set_text(GTK_LABEL(g_bank),  entry_text);// set lbls
			j=j.set_a(j,"B");		
			std::cerr<<j<<endl;
			send(j);
			//send();	
	}
	else
	{
		gtk_label_set_text(GTK_LABEL(g_err_lbl), "bet is bigger than bank" );
	}
}


extern "C" void fold_clicked_cb()
{
		j=j.set_a(j,"F");	
		std::cerr<<j<<endl;
		send(j);	//send
}

extern "C" void exchange_clicked_cb()
{
	
	std::string te;
	
	//if exhcnage possible
	if((c1==TRUE)&&(c2==TRUE)&&(c3==TRUE)&&(c4==TRUE)&&(c5==TRUE))
	{
	gtk_label_set_text(GTK_LABEL(g_err_lbl), "can only exchange 0-4 cards" );
	}
	else
	{
		if(c1==TRUE)
		{
			te= te+ "" +"1 ";
		}
		if(c2==TRUE)
		{
			te= te+ "" +"2 ";
		}
		if(c3==TRUE)
		{
			te= te+ "" +"3 ";
		}
		if(c4==TRUE)
		{
			te= te+ "" +"4 ";
		}
		if(c5==TRUE)
		{
			te= te+ "" +"5 ";
		}
	}
		j=j.set_a(j,"E");
		j=j.set_te(j,te);
			
		std::cerr<<j<<endl;
		send(j);
	//std::cerr<<te;
}



extern "C" void on_card1_toggled()
{
	c1=!c1;
}
extern "C" void on_card2_toggled()
{
	c2=!c2;
}
extern "C" void on_card3_toggled()
{
	c3=!c3;
}
extern "C" void on_card4_toggled()
{
	c4=!c4;
}
extern "C" void on_card5_toggled()
{
	c5=!c5;	
}

extern "C" void on_main_window_destroy()
{
gtk_main_quit();
}

