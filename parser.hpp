#include <vector>
#include <iostream>
#include <string>

class Tokenizer {
  public:
  unsigned int pos = 0;
  std::vector<std::string> tokens;
  Tokenizer();
  Tokenizer(std::string file);
  Tokenizer(std::istream &file);
  Tokenizer(std::stringstream&);
  void read(std::istream &file);
  void read(std::stringstream&);
  bool empty();

  std::string next_peak();
  std::string next_token();
  bool next_string(std::string str);
  int next_int();
  double next_real(); 
  std::pair<int,int> next_read_token();
};

