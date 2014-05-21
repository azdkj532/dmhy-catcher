/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2014  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <mysql/mysql.h>

using std::string;

class Database
{
  public:
    Database();
    ~Database();
    enum Option
    {
      USERNAME, PASSWORD, HOST, DATABASE
    };
    static int ModifyOption( Option op, string target );
//  protected:
    int MYSQLconnect();
    MYSQL mysql_connection;
  private:
    static string d_username, d_password, d_host, d_database_name;
};
class Log : private Database
{
  public:
  enum stat
  {
    ADD, WARN
  };
  int insert( stat statement, string content);
};
class History : private Database
{
  public:
  int insert( int tid, string title, string magnet);
  enum{ TID, TITLE, MAGNET };
  int search( int type, string target);
};
class Task : private Database
{
	public:
	int ReturnTask( int &tid, string &title, string &keyword);
	int GetTask();
	private:
		MYSQL_RES *task_res;
};
#endif // DATABASE_H
