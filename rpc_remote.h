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


#ifndef RPC_REMOTE_H
#define RPC_REMOTE_H

#include "database.h"
#include <string>
#include <sstream>
#include <curl/curl.h>
#include <stdexcept>

using std::string;
/*
class rpc_remte_handle
{
  static int object_number;
  rpc_remte_handle()
  {
    if( object_number == 0)
      curl_global_init( CURL_GLOBAL_ALL );
    ++object_number
  }
  ~rpc_remote_handle()
  {
    --object_number;
    if(!object_number)curl_global_cleanup();
  }
};
*/
class rpc_remote
{
  private:
    static string session_id, username, password, target_url;
    static int object_number;
    std::stringstream http_header, http_body;
    bool statue;
    int GetSessionId();
    int SendHTTPMessage(const string &post);
  public:
    enum
    {
     USERNAME=1, PASSWORD=2, URL=3
    };
    enum method
    {
      TORRENT_ADD=1
    };
    rpc_remote();
    ~rpc_remote();
    int request( const string & command);
    int request( method command_type, const string * argument);
    string response( );
    static void SetValue(int label, string value);
    static void rpc_remote_init();

};
static size_t writefunction( char *ptr, size_t size, size_t nmemb,void *stream);

#endif // RPC_REMOTE_H
