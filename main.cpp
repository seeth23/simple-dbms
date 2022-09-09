#include "DatabaseManagement.hpp"

int main()
{
	DBMS *dbms = new DBMS();
	dbms->load_state("saves/");
#if 1
	dbms->run();

	dbms->save_state();
#endif
	delete dbms;
	std::cout << "END." << std::endl;
	
	
	return 0;
}
