#include <iostream>
#include <json.hpp>

using json = nlohmann::json;


nlohmann::json create(std::string c1,std::string c2,std::string c3,std::string c4,std::string c5,std::string a,std::string te,int cbet,int p)
	{
		json k{  		
    		{"action", a},
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

nlohmann::json set_te(json k, std::string a)
	{
		 k["toexchange"]=a;
    
   		 	
    	return k;
	}

int main()
{

return 0;
	
}





