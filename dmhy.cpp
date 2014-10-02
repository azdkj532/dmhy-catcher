#include "dmhy.h"
#include "database.h"
#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <unistd.h>

using std::string;

#define NOT_FOUND std::string::npos

int GetSearchingTask()
{

  Task search_task;

  search_task.GetTask();

  string keywords, title;

  int tid;
  while( search_task.ReturnTask(tid, title ,keywords) )
  {
    dmhy d( tid, title, keywords );
    std::cout << "Now processing: " << title << std::endl;
    std::cout << "keywords: " << keywords << std::endl;
    d.AnalizeSearchPage( );
    sleep(30);
  }
  return 0;
}

//==dmhy==================================================

int dmhy::AnalizeSearchPage( )
{
  std::stringstream *bodyfile = new std::stringstream;
  target_url = "http://share.dmhy.org/topics/list?keyword=" + keywords;
  if( HTTPGET( target_url , bodyfile) == 1)
  {
    delete bodyfile;
    Log log;
    log.insert( Log::WARN, "Failed to get the webpage");
    throw std::runtime_error(("Failed to get the webpage, "+target_url).c_str() );
  }
  string tmp;
  while( getline( *bodyfile, tmp) )
  {
    string title;
    //string title will store title of each magnet resource
    if( tmp.find("topics/view/") != NOT_FOUND )
    {
      int head = tmp.find("href"), length=0;
      while( tmp[head++] != '\"');
      while( tmp[head+length] != '\"' )++length;
      getline( *bodyfile, title);
      RemoveLabels( title );

      std::cout << title << std::endl;

      DMHYResource individual_resource;
      individual_resource.ModifyContent( DMHYResource::URL,("http://share.dmhy.org"+ tmp.substr(head, length)));
      individual_resource.ModifyContent( DMHYResource::TITLE, title);
      individual_resource.ModifyContent( DMHYResource::TASK_TITLE, alias);
      individual_resource.ModifyContent( DMHYResource::TID, tid);
	  
      if( !individual_resource.Search( DMHYResource::TITLE) )
      {
        std::cout << "prepare to add magnet" << std::endl;
        if( !individual_resource.GetTorrentLink() )//go to identical page to get the magnet link
	{
	  individual_resource.Add();//add to database
	}
      }
    }
  }
return 0;
}

//=======DMHYResource========
int DMHYResource::Search( ContentType type )
{
  History resource;
  switch( type )
  {
    case TITLE :
       return resource.search( History::TITLE, d_title );
    case MAGNET :
       return resource.search( History::MAGNET, d_magnet );
    default: return 0;
  }
}
int DMHYResource::Add()
{
  std::cout << "Get: " << d_title << std::endl;
//Add the torrent resource to database

  History resource;

  resource.insert( d_tid, d_title, d_magnet );

//Add the torrent resource to transmission and start to download
  rpc_remote transmission;

  string torrent_property[2];

  torrent_property[0] = d_magnet;

  torrent_property[1] = d_task_title;
  transmission.request( rpc_remote::TORRENT_ADD, torrent_property );
  string response =transmission.response();
  Log log;
  if( response.find("success") == NOT_FOUND )
  {
    log.insert(Log::WARN, ("Failed to add the mission,"+d_title+", to transmission\n Reason: "+response ) );
    return 1;
  }else
  {
	log.insert(Log::ADD, ("Successed to add mission,"+d_title+", to transmission"));
	return 0;
  }

}
int DMHYResource::ModifyContent(DMHYResource::ContentType type, string target)
{

  //modify the property of the resource such as title, magnet,etc.
  switch( type )
  {
    case TITLE:
      d_title = target;
      return 0;
    case MAGNET:
      d_magnet = target;
      return 0;
    case INFO_HASH:
      d_info_hash = target;
      return 0;
    case URL:
      d_url = target;
      return 0;

	case TASK_TITLE:

	  d_task_title = target;

	  return 0;
    default:
      return 1;
  }
}

int DMHYResource::ModifyContent(DMHYResource::ContentType type, int target)
{
  switch( type )
  {
    case TID:
      d_tid = target;
      return 0;
    default:
      return 1;
  }
}

int DMHYResource::GetIndividualPage()
{
  std::stringstream bodyfile ;
  HTTPGET( d_url , &bodyfile);//send a http get method and store respones in bodyfile
//  std::cout << d_url <<"|"<< std::endl;
//  std::cout <<"bodyfile:" <<bodyfile.str() <<std::endl;
  d_individual_page = bodyfile.str();//prepare to use string's member function
  
  return 0;
}

int DMHYResource::GetInfo_hash()
{
}

int DMHYResource::GetTorrentLink()
{
  GetIndividualPage();
  unsigned long long int length =0, head = d_individual_page.find("magnet:");
  if( head == NOT_FOUND)return 1;
  while( d_individual_page[head+length] != '\"')++length;//we ensure that magnet link will surround by '"'
  d_magnet = d_individual_page.substr(head, length);//string magnet will store the magnet link we want
  return 0;
}
