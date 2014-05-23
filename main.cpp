#include "dmhy.h"
#include "rpc_remote.h"
#include "database.h"

#include <string>
#include <fstream>
#include <iostream>

using std::string;
using std::cout;
using std::endl;
void singleadd(string);
int main(int argc, char** argv)
{
  std::fstream authorization("/etc/dmhy/settings");
  string auth;
  getline( authorization, auth);
  Database::ModifyOption( Database::USERNAME, auth);
  getline( authorization, auth);
  Database::ModifyOption( Database::PASSWORD, auth);
  Database::ModifyOption( Database::HOST, string("localhost"));
  Database::ModifyOption( Database::DATABASE, string("dmhy_catcher"));
#ifndef _DEBUG_
  /*
  Database transmission;
  transmission.MYSQLconnect();
  string user(""), pwd("");
  TransmissionLogging( transmission, user, pwd );
  rpcRemote::SetValue( rpc_remote::USERNAME, user);
  rpcRemote::SetValue( rpc_remote::PASSWORD, pwd );
  rpcRemote::SetValue( rpc_remote::URL, "localhost:9091/transmission/rpc");
  */
#endif
  Log start;
  start.insert( Log::ADD, "Program Started");
  
  for( int i=0; i<argc; ++i)
  {
	std::cout << argv[i] << std::endl;
		if( string(argv[i]) == "--singleadd")
		{
			if( i+1 < argc )
			{
				singleadd( string(argv[i+1]));
				return 0;
			}
		}
  }
  GetSearchingTask();
}
void singleadd( string topic)
{
	DmhyResource individualResource;
	individualResource.ModifyContent( DmhyResource::URL,topic);
	individualResource.ModifyContent( DmhyResource::TITLE, topic);
	individualResource.ModifyContent( DmhyResource::TASK_TITLE, "");
	individualResource.ModifyContent( DmhyResource::TID, 0);

	std::cout << "prepare to add magnet" << std::endl;
	if( !individualResource.GetTorrentLink() )//go to identical page to get the magnet link
		individualResource.Add();//add to database
}
