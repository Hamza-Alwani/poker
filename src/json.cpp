#include <iostream>
#include <json.hpp>

using json = nlohmann::json;


nlohmann::json create(std::string ip, std::string c1,std::string c2,std::string c3,std::string c4,std::string c5,std::string a,std::string te,int cbet,int p)//, int b)
	{
		json k{  
			{"0,",ip},	
    		{"action", a},
    		//{"ammount", b},
			{"card1",c1},
    		{"card2",c2},
    		{"card3",c3},
    		{"card4",c4},
    		{"card5",c5},
    		{"currentbet", cbet},
    		{"toexchange", te},
    		{"pot",p}
    		
    	};	
    	return k;
}



nlohmann::json set_a(json k, std::string a)
	{
		 k["action"]=a;	
    	return k;
	}
nlohmann::json set_cbet(json k, int a)
	{
		 k["currentbet"]=a;	
    	return k;
	}
	nlohmann::json set_pot(json k, int a)
	{
		 k["pot"]=a;	
    	return k;
	}
//	
//nlohmann::json set_bet(json k,int b)
//{
//		 k["ammount"]=b;	
//    	return k;
//}

nlohmann::json set_te(json k, std::string a)
	{
		 k["toexchange"]=a;	
    	return k;
	}

nlohmann::json set_id(json k, std::string a)
	{
		 k["0"]=a;		 	
    	return k;
	}


int main()
{

return 0;
	
}





