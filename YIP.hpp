#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using std::ostream;
using std::string;
using std::stringstream;
using std::vector;
using namespace boost::property_tree;

class YIP
{
  public:
    CURL *curl = curl_easy_init();
    CURLcode resp;
    string api_key;
    YIP(string key)
    {
        api_key = key;
    }
    vector<string> get_images(string text)
    {
        string buffer;
        curl_easy_setopt(curl, CURLOPT_URL, (string("https://serpapi.com/search?engine=yandex_images&text=") + text + string("&api_key=") + api_key).c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        resp = curl_easy_perform(curl);
        curl_easy_reset(curl);
        return parse_array_json<string>(buffer, "images_results", "thumbnail");
    };
    void download_image(string url, ostream& os)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write);
        curl_easy_setopt(curl, CURLOPT_FILE, &os);
        resp = curl_easy_perform(curl);
        curl_easy_reset(curl);
    };
  private:
    template <typename T>
    vector<string> parse_array_json(string data, string key, string itemkey)
    {
        vector<string> return_data;
        stringstream sdata(data);
        ptree json_parse;
        read_json(sdata, json_parse);
        BOOST_FOREACH (const ptree::value_type &item, json_parse.get_child(key))
        {
            return_data.push_back(item.second.get<T>(itemkey));
        }
        return return_data;
    };
    static size_t data_write(void *buf, size_t size, size_t nmemb, void *userp)
    {
        if (userp)
        {
            std::ostream &os = *static_cast<std::ostream *>(userp);
            std::streamsize len = size * nmemb;
            if (os.write(static_cast<char *>(buf), len))
                return len;
        }
        return 0;
    };
    static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((string *)userp)->append((char *)contents, size * nmemb);
        return size * nmemb;
    };
};
