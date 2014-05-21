#ifndef DMHY
#define DMHY

#include "html_source.h"
#include "database.h"
#include "rpc_remote.h"
#include <string>
#include <fstream>
#include <vector>

class DMHYResource:  private html_source
{
  public:
    enum ContentType{  TID, TITLE, MAGNET, INFO_HASH, URL, TASK_TITLE };
    DMHYResource():html_source(), d_individual_page(""), d_title(""), d_magnet(""), d_info_hash(""), d_url(""), d_task_title("") {}
    int Search( ContentType type);
    int Add();
    int ModifyContent( ContentType type, string target );
    int ModifyContent( ContentType type, int target );
    int GetTorrentLink();
    int GetInfo_hash();
  private:
    int d_tid;
    string d_title, d_magnet, d_info_hash, d_url, d_individual_page, d_task_title;
    bool setting[4];
    int GetIndividualPage();
};

class dmhy :protected html_source
{
  public:
    dmhy(int task_id, std::string title,std::string key):tid(task_id), alias(title), keywords(key){}
    int AnalizeSearchPage( );
  private:
    int tid;
    std::string keywords, alias;
};
int GetSearchingTask();
#endif
