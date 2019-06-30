#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include "parser.hpp"
#include "pomdp.hpp"

bool POMDP::read(std::string file, int h) {
  Tokenizer parser(file);
  if (!parser.next_string("discount:")) return false;
  discount = parser.next_real();

  if (!parser.next_string("values:")) return false;
  value = parser.next_token();

  if (!parser.next_string("actions:")) return false;
  naction = parser.next_int();

  if (!parser.next_string("states:")) return false;
  nstate = parser.next_int();

  if (!parser.next_string("observations:")) return false;
  nobserv = parser.next_int();

  horizon = h;
  start.resize(nstate);
  trans.resize(naction);
  obs.resize(naction);
  reward.resize(naction);
  for (int k = 0; k < naction; k++) {
    trans[k].resize(nstate);
    obs[k].resize(nstate);
    reward[k].resize(nstate);
    for (int i = 0; i < nstate; i++) {
      reward[k][i] = 0;
      trans[k][i].resize(nstate);
      obs[k][i].resize(nobserv);
      for (int z = 0; z < nobserv; z++)
        obs[k][i][z] = 0;
      for (int j = 0; j < nstate; j++)
        trans[k][i][j] = 0;
    }
  }

  if (!parser.next_string("start:")) return false;
  for (int i = 0; i < nstate; i++)
    start[i] = parser.next_real();

  int a, i, j, z;
  while (!parser.empty()) {
    std::string trigger = parser.next_token();
    if (!parser.next_string(":")) return false;

    switch(trigger[0]) {
      case 'T':
        a = parser.next_int();
        if (!parser.next_string(":")) return false;
        i = parser.next_int();
        if (!parser.next_string(":")) return false;
        j = parser.next_int();
        trans[a][i][j] = parser.next_real();
        break;

      case 'O':
        a = parser.next_int();
        if (!parser.next_string(":")) return false;
        j = parser.next_int();
        if (!parser.next_string(":")) return false;
        z = parser.next_int();
        obs[a][j][z] = parser.next_real();
        break;

      case 'R':
        a = parser.next_int();
        if (!parser.next_string(":")) return false;
        i = parser.next_int();
        if (!parser.next_string(":")) return false;
        j = parser.next_int();
        if (!parser.next_string(":")) return false;
        z = parser.next_int();
        reward[a][i] += trans[a][i][j] * obs[a][j][z] * parser.next_real();
        break;

      default:
        break;
    }
  }

  return true;
}

void POMDP::write() {
  using std::cout;
  using std::endl;
  cout << "discount: " << discount << endl;
  cout << "values: " << value << endl;
  cout << "actions: " << naction << endl;
  cout << "states: " << nstate << endl;
  cout << "observations: " << nobserv << endl;
  cout << endl;
  std::string sp(" ");
  
  cout << "start:";
  for (int i = 0; i < nstate; i++)
    cout << sp << start[i];
  cout << endl << endl;

  cout << "transition:" << endl;
  for (int k = 0; k < naction; k++) {
    for (int i = 0; i < nstate; i++) {
      for (int j = 0; j < nstate; j++)
        if (trans[k][i][j] > 0)
          cout << "T : " << k << " : " << i << " : " << j << sp << trans[k][i][j] << endl;
    }
    cout << endl;
  }

  cout << "observ:" << endl;
  for (int k = 0; k < naction; k++) {
    for (int i = 0; i < nstate; i++) {
      for (int z = 0; z < nobserv; z++)
        if (obs[k][i][z] > 0)
          cout << "O : " << k << " : " << i << " : " << z << sp << obs[k][i][z] << endl;
    }
    cout << endl;
  }

  cout << "reward:" << endl;
  for (int k = 0; k < naction; k++) {
    for (int i = 0; i < nstate; i++)
      for (int j = 0; j < nstate; j++)
        for (int z = 0; z < nobserv; z++)
          cout << "T : " << k << " : " << i << " : " << j << " : " << z << sp << reward[k][i] << endl;
    cout << endl;
  }
  cout << endl;
}

void POMDP::normalize() {
  // find min and max of reward function
  double minr = 0, maxr = 0;
  for (int a = 0; a < naction; a++) {
    for (int i = 0; i < nstate; i++) {
      double r = reward[a][i];
      if (r < minr) minr = r;
      if (r > maxr) maxr = r;
    }
  }

  // compute normalize reward function
  for (int a = 0; a < naction; a++)
    for (int i = 0; i < nstate; i++)
      reward[a][i] = (reward[a][i] - minr) / (maxr - minr);

  double sum = 0;
  for (int a = 0; a < naction; a++)
    for (int i = 0; i < nstate; i++)
      sum += reward[a][i];
  for (int a = 0; a < naction; a++)
    for (int i = 0; i < nstate; i++)
      reward[a][i] /= sum;
}

void POMDP::print_var(std::stringstream &var_file, int id, int nvalue, int use_indicator) {
  if (use_indicator) {
    var_file << "i " << id << " " << nvalue << std::endl;
  } else {
    var_file << "r " << id << " " << nvalue;
    for (int i = 0; i < nvalue; i++)
      var_file << " " << 1.0/nvalue;
    var_file << std::endl;
  }
}
 
void POMDP::to_sat() {
  std::stringstream out;
  to_sat(out);
  std::cout << out.str();
}

void POMDP::to_sat(std::stringstream &stream) {
  using std::cout;
  using std::endl;

  // define var parameters
  std::stringstream var_file, clause_file, derive_file;
  std::string sp = " ", dot = ".";
  var_file << std::scientific;

  int A = (int)naction;
  int S = (int)nstate;
  int O = (int)nobserv;
  int H = (int)horizon;
  int D = 2;
  int use_indicator = false;

  // POLICY SELECTION
  int var_action_start = 1;
  int var_state_start = var_action_start + H;
  int var_reward_start = var_state_start + H;
  int var_discount_start = var_reward_start + H;

  int var_observ_start = var_discount_start + H;
  int var_state_dist_start = var_observ_start + H - 1;
  int var_observ_dist_start = var_state_dist_start + (H-1)*A*S;

  int nvar = var_observ_dist_start + (H-1)*A*S - 1;
  int nclause = 0;

  var_file << ";; action and observation variables" << endl;
  var_file << "e" << sp << var_action_start << sp << A << endl;
  print_var(var_file, var_discount_start, D, use_indicator);
  for (int h = 1; h < H; h++) {
    print_var(var_file, var_observ_start + (h-1), O, use_indicator);
    var_file << "e" << sp << var_action_start + h << sp << A << endl;
    print_var(var_file, var_discount_start + h, D, use_indicator);
  }
  var_file << endl;

  // POLICY EVALUATION
  var_file << ";; state variables" << endl;
  var_file << "r" << sp << var_state_start + 0 << sp << S;
  for (int i = 0; i < S; i++)
    var_file << sp << start[i];
  var_file << endl;

  for (int h = 1; h < H; h++)
    print_var(var_file, var_state_start + h, S, false);
  var_file << endl;


  derive_file << ";; constant state variables" << endl;
  for (int i = 0; i < S; i++)
    if (start[i] >= 1.0) {
      derive_file << var_state_start << dot << i << sp << 0 << endl;
      nclause++;
    }
  derive_file << endl;

  // discount var
  var_file << ";; policy variables" << endl;
  clause_file << ";; discount constraints" << endl;
  for (int h = 1; h < H; h++) {
    clause_file << var_discount_start + h-1 << dot << 1 << sp << var_discount_start + h << dot << 0 << sp << 0 << endl;
    nclause++;
  }

  clause_file << var_discount_start + H - 1 << dot << 0 << sp << 0 << endl;
  nclause++;
  var_file << endl;
  clause_file << endl;

  for (int h = 0; h < H-1; h++) {
    clause_file << var_discount_start + h << dot << 1 << sp << var_observ_start + h << dot << 0 << sp << 0 << endl;
    nclause++;

    clause_file << var_discount_start + h << dot << 1 << sp << var_state_start + h+1 << dot << 0 << sp << 0 << endl;
    nclause++;
    clause_file << endl;
  }
  clause_file << endl;

  // reward var
  var_file << ";; reward variables" << endl;
  clause_file << ";; reward constraints" << endl;
  for (int h = 0; h < H; h++) {
    var_file << "r" << sp << var_reward_start + h << sp << A*S;
    for (int a = 0; a < A; a++) {
      for (int i = 0; i < S; i++) {
        // nobserv - to map reward variables to same level as future rewards
        // nstate - account for extra state variables (excluding initial belief variable)
        float r = reward[a][i];
        var_file << sp << r;
        int action = -(var_action_start + h);
        int state = -(var_state_start + h);
        int reward = var_reward_start + h;
        int discount = var_discount_start + h;
        if (h == 0) {
          clause_file << action << dot << a << sp << state << dot << i << sp << reward << dot << a*S + i << sp << discount << dot << 1 << sp << 0 << endl;
        } else {
          clause_file << action << dot << a << sp << state << dot << i << sp << reward << dot << a*S + i << sp << discount - 1 << dot << 0 << sp << discount << dot << 1 << sp << 0 << endl;
        }
        nclause++;

        if (r >= 1.0) {
          derive_file << reward << dot << a*S + i << sp << 0 << endl;
          nclause++;
        }
      }
    }
    var_file << endl;
    clause_file << endl;
  }
  var_file << endl;

  if (H > 1) {
    var_file << ";; transition variables" << endl;
    clause_file << ";; transition constraints" << endl;
  }
  // transition var
  for (int h = 1; h < H; h++) {
    for (int a = 0; a < A; a++) {
      for (int i = 0; i < S; i++) {
        var_file << "r" << sp << var_state_dist_start + (h-1)*A*S + (a*S + i) << sp << S;
        for (int j = 0; j < S; j++) {
          var_file << sp << trans[a][i][j];
          int action = -(var_action_start + h-1);
          int statei = -(var_state_start + h-1);
          int statej = -(var_state_start + h);
          int distr = var_state_dist_start + (h-1)*A*S + (a*S + i);
          int discount = var_discount_start + h-1;
          clause_file << action << dot << a << sp << statei << dot << i << sp << statej << dot << j << sp << distr << dot << j << sp << discount << dot << 0 << sp << 0 << endl;
          nclause++;

          if (trans[a][i][j] >= 1.0) {
            derive_file << distr << dot << j << sp << 0 << endl;
            nclause++;
          }
        }
        var_file << endl;
      }
    }
    var_file << endl;
    clause_file << endl;
  }

  if (H > 1) {
    var_file << ";; observation variables" << endl;
    clause_file << ";; observation constraints" << endl;
  }
  // observation var
  for (int h = 1; h < H; h++) {
    for (int a = 0; a < A; a++) {
      for (int j = 0; j < S; j++) {
        var_file << "r" << sp << var_observ_dist_start + (h-1)*A*S + (a*S + j) << sp << O;
        for (int z = 0; z < O; z++) {
          var_file << sp << obs[a][j][z];
          int action = -(var_action_start + h-1);
          int state = -(var_state_start + h);
          int observ = -(var_observ_start + h-1);
          int distr = var_observ_dist_start + (h-1)*A*S + (a*S + j);
          int discount = var_discount_start + h-1;
          clause_file << action << dot << a << sp << state << dot << j << sp << observ << dot << z << sp << distr << dot << z << sp << discount << dot << 0 << sp << 0 << endl;
          nclause++;

          if (obs[a][j][z] >= 1.0) {
            derive_file << distr << dot << z << sp << 0 << endl;
            nclause++;
          }
        }
        var_file << endl;
      }
    }
    var_file << endl;
    clause_file << endl;
  }

  double scale = std::pow(2, horizon) * std::pow(nstate*nobserv, horizon - 1.0);
  stream << "# ssat " << nvar << " " <<  nclause << " " << scale << endl;
  stream << var_file.str();
  stream << ";; ---------------------------------------------------------------" << endl;
  stream << clause_file.str();
  stream << ";; ---------------------------------------------------------------" << endl;
  stream << derive_file.str();
}

void POMDP::info(std::string file) {
  using std::cout;
  using std::endl;
  std::string comma(", ");
  cout << file << comma;
  cout << horizon << comma;
  cout << naction << comma;
  cout << nstate << comma;
  cout << nobserv << comma;
  cout << endl;
}
