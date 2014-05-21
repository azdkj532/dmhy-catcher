
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

#include "rpc_remote.h"
#include <string>
#include <curl/curl.h>
#include <iostream>

#define NOT_FOUND std::string::npos

using std::string;
int rpc_remote::object_number;
string rpc_remote::session_id, rpc_remote::username, rpc_remote::password, rpc_remote::target_url;

static size_t writefunction( char *ptr, size_t size, size_t nmemb,void *stream)
{
  size_t total = size*nmemb;
  std::stringstream* tmp = (std::stringstream *) stream;
  tmp->write(ptr, total);
  return total;
}

void rpc_remote::rpc_remote_init()
{
  object_number = 0;
  curl_global_init( CURL_GLOBAL_ALL );
}

void rpc_remote::SetValue(int label, string value)
{
  switch( label )
  {
    case USERNAME:
      username = value;
      break;
    case PASSWORD:
      password = value;
      break;
    case URL:
      target_url = value;
  }
  return;
}
int rpc_remote::GetSessionId()
{
  string key, value;
  http_header >> key >>value; //HTTP1.1 409 Conflict or other statement
  if( key == "HTTP/1.1" && value =="200" )return 0;
  if( !(key == "HTTP/1.1" && value == "409"))return 1;
  for( int i=0; i<4; ++i)
    http_header >> key;
  http_header >> session_id;
//   std::cout << "session_id: "<< session_id << std::endl;
  return 0;
}

int rpc_remote::SendHTTPMessage(const string& post)
{
  //throw away their contents
  http_body.str("");
  http_header.str("");

  CURL *rpc_request;
  rpc_request = curl_easy_init();
  if( !rpc_request )return 2;

  //set the header of the http request which includes the X-Transmission-Session-Id
  curl_slist *header_setting = NULL;
  header_setting = curl_slist_append( header_setting, ("X-Transmission-Session-Id: " + session_id ).c_str() );
  header_setting = curl_slist_append( header_setting, "Content-Type: application/rpc" );

  //the option name will tell what they means
  curl_easy_setopt( rpc_request, CURLOPT_URL, target_url.c_str());
  curl_easy_setopt( rpc_request, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
  curl_easy_setopt( rpc_request, CURLOPT_USERNAME, username.c_str());
  curl_easy_setopt( rpc_request, CURLOPT_PASSWORD, password.c_str());
  curl_easy_setopt( rpc_request, CURLOPT_HTTPHEADER, header_setting);
  //write the header of the http request to stringstream http_header
  curl_easy_setopt( rpc_request, CURLOPT_HEADERFUNCTION, writefunction);
  curl_easy_setopt( rpc_request, CURLOPT_WRITEHEADER, &http_header);
  //similar to option CURLOPT_WRITEHEADER
  curl_easy_setopt( rpc_request, CURLOPT_WRITEFUNCTION, writefunction);
  curl_easy_setopt( rpc_request, CURLOPT_WRITEDATA, &http_body);
  
  curl_easy_setopt( rpc_request, CURLOPT_NOPROGRESS, 1);// I don't know :P
  curl_easy_setopt( rpc_request, CURLOPT_POST, 1);//tell libcurl that we will send some data by post method
  //set the contents what we will send
  curl_easy_setopt( rpc_request, CURLOPT_POSTFIELDS, post.c_str());
  curl_easy_setopt( rpc_request, CURLOPT_POSTFIELDSIZE, post.length());
//   curl_easy_setopt( rpc_request, CURLOPT_VERBOSE, 1);
//   std::cout << post << std::endl;

  CURLcode res = curl_easy_perform( rpc_request );
  curl_slist_free_all( header_setting );
  curl_easy_cleanup( rpc_request );

  return res;
}

int rpc_remote::request(const string& command)
{
  //At first, we needs the X-Transmission-Session-Id in order to send our command correctly.
  //And we expect that it will return error 409 and correct Session-Id in the header of http request
  SendHTTPMessage(string(""));
  //Here will analize what we got, and store the Session-Id
  if (GetSessionId())return 1;
  //Now we can send our command
  int res = SendHTTPMessage( command );
  return res;
}

int rpc_remote::request( method label, const string* arguments)
{
  //At first, we needs the X-Transmission-Session-Id in order to send our command correctly.
  //And we expect that it will return error 409 and correct Session-Id in the header of http request
  request("");
  //Here will analize what we got, and store the Session-Id
  switch( label )
  {
    case TORRENT_ADD:
      return SendHTTPMessage( ( "{\"method\": \"torrent-add\",\"arguments\":{\"filename\": \""+ arguments[0] +"\", \"download-dir\":\"/home/pydio/fileserver/BT/"+arguments[1]+"\" } }") );
    default:
      return 1;
  }
}

string rpc_remote::response()
{
  return http_body.str();
}

rpc_remote::rpc_remote()
{
  ++object_number;
}

rpc_remote::~rpc_remote()
{
  //clean up the data.I don't know clearly how it work, it should be clean up when all object are destroied.
  --object_number;
  if( object_number == 0)
  {
    curl_global_cleanup();
  }
}

//=====transmission logging==========
int TransmissionLogging(Database& connection, string &user, string &pwd)
{
  int stat = mysql_query( &connection.mysql_connection, "SELECT * FROM user\0");
  if( stat )throw std::runtime_error( "Query failed(transmission account)." );
  MYSQL_RES *res = mysql_use_result( &connection.mysql_connection );
  MYSQL_ROW row;
  if( row = mysql_fetch_row( res ) )
  {
    user = string( row[0] );
    pwd  = string( row[1] );
  }else throw std::runtime_error( "Failed to get the transmission account" );
  return 0;

}
