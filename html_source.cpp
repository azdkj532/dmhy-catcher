#include "html_source.h"
#include <curl/curl.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <unistd.h>

#define NOT_FOUND std::string::npos

// #define _DEBUG_ 1

void html_source::RemoveLabels( std::string &target)
{
  std::string output;

  bool label_exist = false;
  for( int i=0,e=target.length(); i<e; ++i)
  {
  	if( target[i] < 32 && target[i] >= 0)continue;//clean the unvisible charactor including '\n'
    if( i < e-1 )if( target[i] == ' ' && target[i+1] == ' ' )continue;
    if( target[i] == '<')label_exist = true;
    if( !label_exist )output += target[i];
    if( target[i] == '>')label_exist = false;
  }
  target = output;
}

int html_source::HTTPGET(string& url, std::stringstream *bodyfile )
{
/*
  CURL *curl_handle;
  if( bodyfile == NULL)return 1;
#ifdef _DEBUG_
  return 1;
#endif
  sleep(10);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str() );
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, bodyfile);
  curl_easy_setopt(curl_handle, CURLOPT_LOCALPORT, 31519);
  
  std::stringstream header;
  curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, write_data);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, &header);
  
  
  CURLcode res = curl_easy_perform(curl_handle);
  curl_easy_cleanup(curl_handle);
  
  std::cout << header.str() <<std::endl;
  
  return res;
*/
  system( ("wget -q --output-document=/etc/dmhy/source/tmp \"" + url +"\"").c_str());
  std::ifstream tmpfile("/etc/dmhy/source/tmp");
  copy(std::istreambuf_iterator<char>(tmpfile), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(*bodyfile));
  return 0;
}

size_t html_source::write_data(char *ptr, size_t size, size_t nmemb, void *stream)
{
  int written = nmemb*size;
  std::stringstream* output = (std::stringstream *) stream;
  output->write( ptr, written);
  return written;
}
