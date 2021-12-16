#include <iostream>
#include <ncurses.h>
#include <string>
//#include "../backend.h"
//#include "../frontend.h"
#include <sqlite3.h> 

using std::cout; using std::cin; using std::endl; using std::cerr;

class item
{

};
int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		cout << azColName[i] << ": " << argv[i] << endl;
	}
	cout << "\n";
	return 0;
}
class sse
{
	sqlite3* db;
	int database;
	int rc;

	std::string mode;



public:


	void display()
	{

	}

	int open(const char* name)
	{
		int correct = 1;
		int opening = sqlite3_open(name, &db);


		char* error_msg;
		std::string sql = "SELECT * FROM reservations;";
		int createtable = sqlite3_exec(db, sql.c_str(), callback, 0, &error_msg);

		if (createtable != SQLITE_OK)
		{
			cout << "Error creating table\n" << endl;
			sqlite3_free(error_msg);
			correct = 0;
		}
		else
		{
			cout << "table created successfully\n" << endl;
		}
		return correct;
	}

	void switchmode()
	{

	}

	void write();


	void jump();

	item select();


};

int main()
{
	sse editor;
	int rc = editor.open("database.db");
	cout << rc << "\n";
	//editor.display();
}

