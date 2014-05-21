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
//#define _DEBUG_ 0

#include "database.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <mysql/mysql.h>
#include "src/sha256.h"

string  Database::d_username, Database::d_password, Database::d_host, Database::d_database_name;
Database::Database()
{
  mysql_init(&mysql_connection);
}
Database::~Database()
{
  mysql_close( &mysql_connection);
}
int Database::MYSQLconnect()
{
  MYSQL *stat;
  mysql_options( &mysql_connection, MYSQL_INIT_COMMAND, "SET NAMES utf8\0");
  stat = mysql_real_connect( &mysql_connection, d_host.c_str(), d_username.c_str(), d_password.c_str(),
		       d_database_name.c_str(), 0, NULL, 0);
  if( stat == NULL ) throw std::runtime_error("failed to connect the Database");

}
int Database::ModifyOption(Database::Option op, string target)
{
  switch( op )
  {
    case USERNAME:
      d_username = target;
      return 0;
    case PASSWORD:
      d_password = target;
      return 0;
    case HOST:
      d_host = target;
      return 0;
    case DATABASE:
      d_database_name = target;
      return 0;
    default:
      return 1;
  }
}

//=======log==========

int Log::insert(stat statement, string content)
{
  MYSQLconnect();
  std::stringstream sql_command;
  sql_command << "INSERT INTO `log` ( `type`, `content`) VALUES( ";
  switch( statement )
  {
    case ADD :
      sql_command << "'info', ";
      break;
    case WARN :
      sql_command << "'warning', ";
      break;
    default:
      throw std::runtime_error("error statement of adding log");
  }
  sql_command << "'" << content << "') ";
  int stats = mysql_query( &mysql_connection, sql_command.str().c_str() );
  if( stats )puts("Failed to add a log");
}
//=============history===============
int History::insert(int tid, string title, string magnet)
{
  string title_hash = sha256(title);
  MYSQLconnect();
  std::stringstream sql_command;
  sql_command << "INSERT INTO `resource` ( `task_id`, `title`, `title_hash`,  `magnet`) ";
  sql_command << "VALUES (";
  sql_command << "'" << tid << "',";
  sql_command << "'" << title << "',";
  sql_command << "'" << title_hash << "',";
  sql_command << "'" << magnet << "')";

  int flag = mysql_query( &mysql_connection, sql_command.str().c_str() );
  Log log;
  if( flag )
  {
    log.insert(Log::WARN, ("Failed to add the mission, \"" + title + "\", to database") );
    return 1;
  }else
  {
    log.insert(Log::ADD, ("Successed to add the mission, \"" + title + "\", to database") );
    return 0;
  }
}

int History::search(int type, string target)
{
  MYSQLconnect();
  std::stringstream sql_command;
  sql_command << "SELECT * FROM resource WHERE ";
  switch( type )
  {
    case TITLE:
      sql_command << "`title_hash`  LIKE ";
      target = sha256(target);
#ifdef _DEBUG_
std::cout << target << std::endl;
#endif
      break;
    case MAGNET:
      sql_command << "`magnet` LIKE ";
      break;
    default:
      return 2;
  }
  sql_command << "'%" << target << "%'";
  int flag = mysql_query( &mysql_connection, sql_command.str().c_str() );
  if( flag )throw std::runtime_error(("query failed at searching resource, "+target).c_str()) ;

  MYSQL_RES *res;
  res = mysql_store_result( &mysql_connection);
  if( mysql_num_rows( res ) == 0 )
  {
#ifdef _DEBUG_
std::cout << "Find!" << std::endl;
#endif
    return 0;
  }
  else return 1;

}
//=======================================
int Task::GetTask( )
{

  MYSQLconnect();
  int flag = mysql_query( &mysql_connection, " SELECT * FROM task WHERE `statement`=1");
  if( flag )throw std::runtime_error("Failed to get the task");
  task_res = mysql_use_result( &mysql_connection);
  return 0;
}

int Task::ReturnTask( int &tid, string &title, string &keywords )

{
	MYSQL_ROW row;

	if( row = mysql_fetch_row(task_res))

	{
		std::stringstream format;
		format << row[0];
		format >> tid;
		title	= row[1];
		keywords= row[2];
		return true;
	}else return false;
}

