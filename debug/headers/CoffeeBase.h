#ifndef COFFEE_BASE
#define COFFEE_BASE

#include <iostream>
#include <string>
#include <SQLite3/sqlite3.h>

class CoffeeBase{
public:
	CoffeeBase(const char* name){
		int rc = sqlite3_open(name, &database);
		if(rc){
			std::cerr << "Error: Can't open " << name << "! " << sqlite3_errmsg(database) << std::endl; 
			sqlite3_close(database);
		}
	}
	~CoffeeBase(){
		sqlite3_close(database);
	}
	void sql_stmt(std::string command){
		int rc = sqlite3_prepare_v2(database, command.c_str(), command.length(), &stmt, NULL);
		int rcstmt = sqlite3_step(stmt);
		std::cout << rcstmt << std::endl;
		switch(rcstmt){
			case 0:
				std::cerr << "SQLITE_OK: " << sqlite3_errmsg(database) << std::endl;
				break;
			case 1:
				std::cerr << sqlite3_errmsg(database) << std::endl;
				break;
			case 19:
				std::cerr << "SQLITE_CONSTRAINTS: " << sqlite3_errmsg(database) << std::endl;
				break;
			case 21:
				std::cerr << "SQLITE_MISUSE: " << sqlite3_errmsg(database) << std::endl;
				break;
			case 100:
				std::cerr << "SQLITE_ROW: " << sqlite3_errmsg(database) << std::endl;
				break;
			case 101:
				std::cerr << "SQLITE_DONE: " << sqlite3_errmsg(database) << std::endl;
				break;
			default:
				break;
		}
	}
private:
	sqlite3* database;
	sqlite3_stmt* stmt;
};

#endif