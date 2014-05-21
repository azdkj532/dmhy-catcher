#ifndef HTML_RESOURCE
#define HTML_RESOURCE

#include <string>
#include <fstream>
#include <curl/curl.h>
#include <unistd.h>
#include <vector>

using std::string;

class html_source
{
//	public:
	
  protected:
    string target_url, name;
    int magnet_number;
    void RemoveLabels( string &target);
    static size_t write_data(char *ptr, size_t size, size_t nmemb, void *stream);
    int HTTPGET(std::string &url, std::stringstream *bodyfile);
};


#endif
