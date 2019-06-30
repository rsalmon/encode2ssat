#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "pomdp.hpp"

/*
void to_zander(basic_sat &sat index_t nstep, index_t naction, index_t nobserv, index_t nstate) {
  //POMDP pomdp(file);
  //to_zander(sat, pomdp.horizon, pomdp.naction, pomdp.nobserv, pomdp.nstate);
  //index_t nchance = nvar - (naction + nstep*(nobserv + naction));
  index_t nvar = sat.vars.size() - 1;
  index_t nclause = sat.clauses.size();
  cout << nstep << "\t;; number of time steps" << endl;
  cout << naction << "\t;; number of actions per time steps" << endl;
  cout << nstep > 1 ? nobserv : 0 << "\t;; number of observations per time steps" << endl;
  cout << nstate << "\t;; number of state variables per time steps" << endl;
  cout << nvar << "\t;; number of variables" << endl;
  cout << clause << "\t;; number of clauses" << endl;

  real_t &scale = sat.scale;
  vector<index_t> &order = sat.order;
  vector<basic_var> &vars = sat.vars;
  vector<vector<Token>> &clauses = sat.clauses;

  //cout << "# ssat " << nvar << " " << nclause << " " << scale << endl;
  for (int v = 1; v <= nvar; v++) {
    index_t id = order[v];
    basic_var &var = vars[id];
    cout << var.q << " " << id << " " << var.nvalue; //<< " (" << var.prob << ") ";
    if (var.q == 'r') {
      vector<vector<real_t>> &prob = sat.prob;
      for (int k = 0; k < var.nvalue; k++)
        cout << " " << prob[var.prob][k];
    }
    cout << endl;
  }
  printf("\n");

  for (int c = 0; c < nclause; c++) {
    vector<Token> &literal = clauses[c];
    index_t ntoken = literal.size();
    for (int l = 0; l < ntoken; l++) {
      Token token = literal[l];
      printf("%2d.%d ", token.id, token.value);
    }
    printf("0\n");
  }
}

void show_POMDP(POMDP * pomdp) {
  printf("discount: %.2f\n", pomdp->discount);
  printf("values: %s\n", pomdp->value);
  printf("actions: %2d\n", pomdp->naction);
  printf("states: %2d\n", pomdp->nstate);
  printf("observations: %2d\n", pomdp->nobserv);
  printf("\n");
  
  printf("start:");
  for (int i = 0; i < pomdp->nstate; i++)
    printf("%.2f ", pomdp->start[i]);
  printf("\n\n");

  printf("transition:\n");
  for (int k = 0; k < pomdp->naction; k++) {
    for (int i = 0; i < pomdp->nstate; i++) {
      for (int j = 0; j < pomdp->nstate; j++)
        printf("%.2f ", pomdp->trans[k][i][j]);
      printf("\n");
    }
    printf("\n");
  }

  printf("observ:\n");
  for (int k = 0; k < pomdp->naction; k++) {
    for (int i = 0; i < pomdp->nstate; i++) {
      for (int j = 0; j < pomdp->nobserv; j++)
        printf("%.2f ", pomdp->obs[k][i][j]);
      printf("\n");
    }
    printf("\n");
  }

  printf("reward:\n");
  for (int k = 0; k < pomdp->naction; k++) {
    for (int i = 0; i < pomdp->nstate; i++)
      printf("%.2f ", pomdp->reward[k][i]);
    printf("\n");
  }
  printf("\n");
}

*/

int main(int argc, char* argv[]) {
  if (argc <= 2) return 0;

  POMDP pomdp;
  std::string file = argv[1];
  int horizon = std::stoi(argv[2]);
  pomdp.read(file, horizon);
  //pomdp.info(file); return 0;

  pomdp.normalize();
  pomdp.to_sat();
  return 0;
}

