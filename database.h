#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "sqlite\sqlite3.h" //banco de dados usado, documentacao disponivel em https://www.sqlite.org/docs.html


class database
{
  private:
    char* error;
    sqlite3* DB;
    sqlite3_stmt* stmt; //statement
  
  public:
    database(std::string name)
    {
      //Open new database which uses DB as handle
      sqlite3_open(name.c_str(), &DB);
      //If sqlite3_exec returns non-zero, theres a error, which is raised with vhar* err and exists program
      if(sqlite3_exec(DB, "CREATE TABLE IF NOT EXISTS algolancer(name varchar(100), idade int);", NULL, NULL, &error))
      {
        std::cout << "error: " << error << std::endl;
        exit(1);
      }

    }

    ~database()
    {
      sqlite3_close(DB);
    }
    
    int insert_data(std::string data)
    {
      std::string query = "insert into algolancer VALUES(" + data + ");";
      if(sqlite3_exec(DB, query.c_str(), NULL, NULL, &error))
      {
        std::cout << "error: " << error << std::endl;
        return -1;
      }
      else
        std::cout << "Sucess to insert" << std::endl;
      return 0;
    }
};