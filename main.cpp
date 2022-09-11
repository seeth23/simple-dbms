#include "DatabaseManagement.hpp"

int main()
{
	DBMS *dbms = new DBMS();

	dbms->load_state("saves/");

	dbms->run();

	dbms->save_state();

	delete dbms;

	std::cout << "END." << std::endl;
	return 0;
}
