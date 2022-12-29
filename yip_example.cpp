#include "YIP.hpp"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <fstream>

using namespace std;

int main()
{
    YIP yip(""); //your api key from https://serpapi.com/yandex-images-api
    auto links = yip.get_images("monkeys+eat+bananas"); //replace spaces with +
    short i = 0;
    for (string const &link : links)
    {
        cout << link << endl;
        ofstream ofs(to_string(i) + string(".jpg"), ostream::binary);
        yip.download_image(link, ofs); //save image to file
        i++;
    }
}