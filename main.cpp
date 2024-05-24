#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include "parser.h"
#include "utility.h"

using std::cin;
using std::cout;
int arima_kana::Parser::timestamp = 0;

int main() {
  std::ios::sync_with_stdio(false);
//  std::cin.tie(nullptr);
//  std::cout.tie(nullptr);
  std::string str;
  arima_kana::Parser parser;
  while (true) {
    getline(cin, str);
    if (str.empty()) continue;
    if (str.substr(str.size() - 4) == "exit") {
      std::cout << str.substr(0, str.size() - 4) << "bye\n";
      break;
    }
    parser.handle(str);
  }
  return 0;
}