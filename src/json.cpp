#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

	nlohmann::json create(json j, std::string n, int i, std::string t)
	{
		json k{
    
   		 	{"name",n},
    		{"id", i},
    		{"chrono",t},
    	};	
    	return k;
}


int main()
{
   return 0;
}
