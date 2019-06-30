#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

class Table {
  public:
  int id, nparent, nvalues;
  vector<int> parents;
  vector<double> values;
};

class Network {
  public:
  int nvar;
  vector<int> nvalue;
  int ncpt;
  vector<Table> cpt;

  Network() : nevidence(0) {};
  void write();
  bool read(string);
  void to_sat();

  int nevidence;
  vector<pair<int, int>> evidence;
  bool read_evidence(string file);
};

