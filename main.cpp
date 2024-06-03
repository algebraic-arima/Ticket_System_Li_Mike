#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "parser.h"

using std::cin;
using std::cout;
using json = nlohmann::json;
int arima_kana::timestamp = 0;
using arima_kana::handle;

void handle_post(const httplib::Request &req, httplib::Response &res) {
  json body = json::parse(req.body);
  if (body.contains("string")) {
    std::string str = body["string"];
    std::ostringstream oss;
    std::streambuf *cout_buf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    try {
      if (str.empty()) return;
      if (!handle(str))
        return;
    } catch (const ErrorException &e) {
      cout << e.getMessage() << '\n';
    } catch (const std::exception &e) {
      cout << e.what() << '\n';
    } catch (...) {
      cout << "Unknown error\n";
    }
    std::string output = oss.str();
    json response_json;
    response_json["string"] = output;
    res.set_content(response_json.dump(), "application/json");
    std::cout.rdbuf(cout_buf);
    std::cout << body.dump() << std::endl;
    std::cout << response_json.dump() << std::endl;
  } else {
    res.status = 400;
    res.set_content("Missing num1 or num2 in request body", "text/plain");
  }
}

//int main() {
//  httplib::Server svr;
//
//  svr.new_task_queue = [] { return new httplib::ThreadPool(1); };
//  svr.set_default_headers({
//                                  {"Access-Control-Allow-Origin",  "*"},
//                                  {"Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE"},
//                                  {"Access-Control-Max-Age",       "3600"},
//                                  {"Access-Control-Allow-Headers", "*"},
//                                  {"Content-Type",                 "application/json;charset=utf-8"}
//                          });
//  svr.Options("/about", [](const httplib::Request &req, httplib::Response &res) {
//    std::cout << "Options request received" << std::endl;
//    res.status = 200;
//  });
//
//  svr.Post("/about", handle_post);
//  std::cout << "Server started on port 8090" << std::endl;
//  svr.listen("localhost", 8090);
//
//  return 0;
//}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::string str;
  while (true) {
    try {
      getline(cin, str);
      if (str.empty()) continue;
      if (!handle(str))
        break;
    } catch (const ErrorException &e) {
      cout << e.getMessage() << '\n';
    }
    catch (const std::exception &e) {
      cout << e.what() << '\n';
    }
    catch (...) {
      cout << "Unknown error\n";
    }
  }
  return 0;
}