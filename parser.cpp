#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "parser.hpp"

Tokenizer::Tokenizer() {
  read(std::cin);
}

Tokenizer::Tokenizer(std::istream &file) {
  read(file);
}

Tokenizer::Tokenizer(std::stringstream &stream) {
  read(stream);
}

Tokenizer::Tokenizer(std::string file) {
  std::fstream fs(file);
  if (fs.is_open()) {
    read(fs);
    fs.close();
  } else {
    std::cout << "Unable to open file" << std::endl;
  }
}

void Tokenizer::read(std::stringstream &stream) {
  tokens.reserve(100);
  pos = 0;
  std::string line;
  while (getline(stream, line)) {
    std::stringstream ss(line);
    std::string substr;
    while(getline(ss, substr, ' ')) {
      if (substr.find(";") == 0)
        break;
      if (substr.size() > 0) {
        tokens.push_back(substr);
      }
    }
  }
}

void Tokenizer::read(std::istream &file) {
  tokens.reserve(100);
  pos = 0;
  std::string line;
  while (getline(file, line)) {
    std::stringstream ss(line);
    std::string substr;
    while(getline(ss, substr, ' ')) {
      if (substr.find(";") == 0)
        break;
      if (substr.size() > 0) {
        tokens.push_back(substr);
      }
    }
  }
}

bool Tokenizer::empty() {
  return pos >= tokens.size();
}

std::string Tokenizer::next_peak() {
  return tokens[pos];
}

std::string Tokenizer::next_token() {
  return tokens[pos++];
}

bool Tokenizer::next_string(std::string str) {
  return next_token() == str;
}

int Tokenizer::next_int() {
  //std::cout << "to_int" << std::endl;
  return std::stoi(next_token());
}

double Tokenizer::next_real() {
  //std::cout << "to_real" << std::endl;
  return std::stod(next_token());
}

std::pair<int,int> Tokenizer::next_read_token() {
  std::vector<std::string> result;
  std::stringstream ss(next_token());

  std::string first, second;
  getline(ss, first, '.');
  getline(ss, second, '.');
  return std::pair<int,int>(std::stoi(first), std::stoi(second));
}

