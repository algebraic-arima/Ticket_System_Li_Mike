#include <iostream>
#include <string>
#include "parser.h"

using std::cin;
using std::cout;
int arima_kana::timestamp = 0;

int main() {
  std::ios::sync_with_stdio(false);
//  std::cin.tie(nullptr);
//  std::cout.tie(nullptr);
  std::string str;
  arima_kana::Parser parser;
  while (true) {
    try{
      getline(cin, str);
      if (str.empty()) continue;
      if (!parser.handle(str))
        break;
    }catch(const ErrorException &e){
      cout << e.getMessage() << '\n';
    }
    catch(const std::exception &e){
      cout << e.what() << '\n';
    }
    catch(...){
      cout << "Unknown error\n";
    }
  }
  return 0;
}