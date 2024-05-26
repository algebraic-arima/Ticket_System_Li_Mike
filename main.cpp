#include <iostream>
#include <string>
#include "parser.h"

using std::cin;
using std::cout;
int arima_kana::timestamp = 0;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::string str;
  arima_kana::Parser parser;
  while (true) {
    getline(cin, str);
    if (str.empty()) continue;
    if (!parser.handle(str))
      break;
  }
  return 0;
}