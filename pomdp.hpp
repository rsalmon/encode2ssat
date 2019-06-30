#include <vector>
#include <sstream>

class POMDP {
  public:
  typedef std::vector<float> vector1;
  typedef std::vector<vector1> vector2;
  typedef std::vector<vector2> vector3;
  int naction, nstate, nobserv, horizon;
  std::string value;
  float discount;
  vector1 start;
  vector2 reward;
  vector3 trans, obs;

  bool read(std::string, int);
  void write();
  void info(std::string);
  void normalize();
  void to_sat();
  void to_sat(std::stringstream&);
  void print_var(std::stringstream&, int, int, int);
  POMDP to_goal();
};

