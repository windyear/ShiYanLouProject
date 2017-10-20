#include <iostream>
#include <string>
#include <regex>

int main(){
    std::string fnames[] = {"windyear.txt", "xuwen.txt", "wolf0.txt", "wolf", "A.txt"};
    std::regex txt_regex("[a-z]+\\.txt");
    for(const auto &fname: fnames){
        std::cout << fname <<": " << std::regex_match(fname, txt_regex) << std::endl;
    }
}
