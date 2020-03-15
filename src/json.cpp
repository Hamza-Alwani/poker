#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

	nlohmann::json create(json j, std::string n, int i, std::string t, std::string h[])
	{
		json k{
    
   		 	{"name",n},
    		{"id", i},
    		{"chrono",t},
			{"card1",h[0]},
    		{"card2",h[1]},
    		{"card3",h[2]},
    		{"card4",h[3]},
    		{"card5",h[4]},
    		
    	};	
    	return k;
}


int main()
{
   return 0;
}
