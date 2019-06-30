#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "network.hpp"

int main(int argc, char* argv[]) {
  if (argc >= 2) {
    Network network;
    network.read(argv[1]);

    if (argc >= 3)
      network.read_evidence(argv[2]);
    else { // if only 1 file try to see if .evid extension by same name is in same path
      string arg(argv[1]);
      arg += ".evid";
      ifstream file(arg);
      if (file.good()) {
        network.read_evidence(arg);
      }
    }
    network.to_sat();
  }
  return 0;
}

