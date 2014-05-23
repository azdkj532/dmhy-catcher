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


#include "database.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <mysql++.h>
#include "src/sha256.h"

#define _DENUG_ 0

string  Database::d_username, Database::d_password, Database::d_host, Database::d_database_name;
Database::Database()
{
}
Database::~Database()
{
	if( d_connectionSettings.connected() )d_connectionSettings.disconnect();
}
mysqlpp::Query Database::MysqlConnect()
{
  d_connectionSettings.connect( d_database_name.c_str(), "127.0.0.1", d_username.c_str(), d_password.c_str() );
  if( !d_connectionSettings.connected() ) throw std::runtime_error("failed to connect the Database");
  mysqlpp::Query mysql_connection = d_connectionSettings.query();
  mysql_connection <<  "SET NAMES utf8";
  mysql_connection.execute();
  return mysql_connection;
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
  mysqlpp::Query sql_query = MysqlConnect();
  sql_query << "INSERT INTO `log` ( `type`, `content`) VALUES( ";
  switch( statement )
  {
    case ADD :
      sql_query << "'info', ";
      break;
    case WARN :
      sql_query << "'warning', ";
      break;
    default:
      throw std::runtime_error("error statement of adding log");
  }
  sql_query << "'" << content << "') ";
  mysqlpp::StoreQueryResult stats = sql_query.store();
  if( stats )puts("Warning: Failed to add a log");
}
//=============history===============
int History::insert(int tid, string title, string magnet)
{
  string title_hash = sha256(title);
  mysqlpp::Query sql_query = MysqlConnect();
  sql_query << "INSERT INTO `resource` ( `task_id`, `title`, `title_hash`,  `magnet`) ";
  sql_query << "VALUES (";
  sql_query << "'" << tid << "',";
  sql_query << "'" << title << "',";
  sql_query << "'" << title_hash << "',";
  sql_query << "'" << magnet << "')";

  mysqlpp::StoreQueryResult flag = sql_query.store();
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
  sql_query.reset();
}

int History::search(int type, string target)
{
  mysqlpp::Query sql_query = MysqlConnect();
  sql_query << "SELECT * FROM resource WHERE ";
  switch( type )
  {
    case TITLE:
      sql_query << "`title_hash`  LIKE ";
      target = sha256(target);
      break;
    case MAGNET:
      sql_query << "`magnet` LIKE ";
      break;
    default:
      return 2;
  }
  sql_query << "'%" << target << "%'";
  mysqlpp::StoreQueryResult res = sql_query.store();
  if(!res )throw std::runtime_error(("query failed at searching resource, "+target).c_str()) ;
  sql_query.reset();

  if( res.num_rows() != 0 )
  {
	#ifdef _DEBUG_
	std::cout << "Find!" << std::endl;
	#endif
   return 1;
  }
  else return 0;
}
//=======================================
int Task::GetTask( )
{

  mysqlpp::Query sql_query = MysqlConnect();
  sql_query <<" SELECT * FROM task WHERE `statement`=1";
  mysqlpp::StoreQueryResult resultOfTask = sql_query.store();
  sql_query.reset();
  if( resultOfTask )
  {
	  d_task_res = resultOfTask;
	  d_nowRead = 0;
  }else throw std::runtime_error("Failed to get the task" + string(sql_query.error()) );

  return 0;
}

int Task::ReturnTask( int &tid, string &title, string &keywords )

{
	mysqlpp::Row row;

	if(  d_nowRead < d_task_res.num_rows()  )
	{
		row = d_task_res.at( d_nowRead++ );
		std::stringstream format;
		format << row.at(0).c_str();
		format >> tid;
		title	= row.at(1).c_str();
		keywords= row.at(2).c_str();
		return true;
	}else return false;
}


//=====transmission logging==========
int TransmissionLogging(Database& connection, string &user, string &pwd)
{
	Database account;
	mysqlpp::Query sqlQuery =account.MysqlConnect();

	sqlQuery << "SELECT * FROM user\0";
	mysqlpp::StoreQueryResult result= sqlQuery.store();

	if( result ){
		mysqlpp::Row row = result.at(0);
		user = string( row[0] );
		pwd  = string( row[1] );
	}else
		throw std::runtime_error( "Query failed(transmission account). " + string( sqlQuery.error() ) );

	return 0;
}
