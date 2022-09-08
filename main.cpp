#include "DatabaseManagement.hpp"
int main()
{
	DBMS *dbms = new DBMS();
	dbms->run();

	std::cout << "END." << std::endl;

	delete dbms;
	return 0;
}
