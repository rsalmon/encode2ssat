#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "network.hpp"
#include "parser.hpp"

void Network::write() {
  cout << "BAYES" << endl;
  cout << nvar << endl;
  cout << nvalue[0];
  for (int v = 1; v < nvar; v++)
    cout << " " << nvalue[v];
  cout << endl << endl;

  cout << ncpt << endl;
  for (int i = 0; i < ncpt; i++) {
    Table &pt = cpt[i];
    cout << pt.nparent + 1;
    for (int j = 0; j < pt.nparent; j++)
      cout << " " << pt.parents[j];
    cout << " " << pt.id << endl;
  }
  cout << endl;

  for (int i = 0; i < ncpt; i++) {
    Table &pt = cpt[i];
    cout << pt.nvalues << endl;
    int id_nvalue = nvalue[pt.id];
    for (int j = 0; j < pt.nvalues/id_nvalue; j++) {
      cout << pt.values[j*id_nvalue + 0];
      for (int k = 1; k < id_nvalue; k++)
        cout << " " << pt.values[j*id_nvalue + k];
      cout << endl;
    }
    cout << endl;
  }
}

bool Network::read_evidence(string file) {
  Tokenizer parse(file);
  nevidence = parse.next_int();
  evidence.reserve(nevidence);
  for (int v = 0; v < nevidence; v++) {
    int var = parse.next_int();
    int value = parse.next_int();
    evidence.push_back({var, value});
  }

  return true;
}

bool Network::read(string file) {
  Tokenizer parse(file);
  if (!parse.next_string("BAYES")) return false;

  nvar = parse.next_int();
  nvalue.reserve(nvar);
  for (int v = 0; v < nvar; v++)
    nvalue[v] = parse.next_int();

  ncpt = parse.next_int();
  cpt.resize(ncpt);
  for (int i = 0; i < ncpt; i++) {
    Table &pt = cpt[i];
    pt.nparent = parse.next_int() - 1; // exclude current variable
    pt.parents.reserve(pt.nparent);
    for (int j = 0; j < pt.nparent; j++)
      pt.parents[j] = parse.next_int();
    pt.id = parse.next_int();
  }

  for (int i = 0; i < ncpt; i++) {
    Table &pt = cpt[i];
    pt.nvalues = parse.next_int();
    pt.values.reserve(pt.nvalues);
    int id_nvalue = nvalue[pt.id];
    for (int j = 0; j < pt.nvalues/id_nvalue; j++)
      for (int k = 0; k < id_nvalue; k++)
        pt.values[j*id_nvalue + k] = parse.next_real();
  }

  return true;
}

void Network::to_sat() {
  // define var parameters
  std::stringstream var_file, exist_file, clause_file, evidence_file;
  int nclause = 0;
  double scale = 1.0;

  // POLICY SELECTION
  int cpt_var = nvar + 1;
  exist_file << ";; existential variables" << endl;
  for (int v = 0; v < nvar; v++) {
    Table &pt = cpt[v];
    var_file << ";; cpt for variable " << pt.id + 1 << endl;
    exist_file << "e " << v + 1 << " " << nvalue[v] << endl;

    int nvalue_cpt = pt.nvalues/nvalue[v];
    clause_file << ";; constants for variable " << pt.id + 1 << endl;
    for (int i = 0; i < nvalue_cpt; i++) {
      var_file << "r " << cpt_var << " " << nvalue[v];
      for (int j = 0; j < nvalue[v]; j++) {
        int index = i*nvalue[v] + j;
        var_file << " " << pt.values[index];
        if (pt.values[index] >= 1.0) {
          clause_file << cpt_var << "." << j << " 0" << endl;
          nclause++;
        }
      }
      var_file << endl;
      cpt_var++;
    }
    var_file << endl;
    clause_file << endl;

    int total = nvalue[pt.id];
    int parents[pt.nparent];
    for (int i = 0; i < pt.nparent; i++) {
      parents[pt.nparent - i - 1] = total;
      total *= nvalue[pt.parents[pt.nparent - i - 1]];
    }

    clause_file << ";; constraints for variable " << pt.id + 1 << endl;
    for (int i = 0; i < pt.nvalues; i++) {
      int remainder = i;
      for (int j = 0; j < pt.nparent; j++) {
        int index = parents[j];
        clause_file << -(pt.parents[j] + 1) << "." << remainder / index << " ";
        remainder = remainder % index;
      }
      clause_file << -(pt.id + 1) << "." << i % nvalue[v] << " ";
      clause_file << (cpt_var - nvalue_cpt + i / nvalue[v]) << "." << i % nvalue[v] << " ";
      clause_file << 0 << endl;
      nclause++;
    }
    clause_file << endl;
  }
  exist_file << endl;

  evidence_file << ";; evidence variables" << endl;
  for (int v = 0; v < nevidence; v++) {
    pair<int, int> evid = evidence[v];
    evidence_file << evid.first + 1 << "." << evid.second << " 0" << endl;
    nclause++;
  }

  int nvar_ssat = cpt_var - 1;
  cout << "# ssat " << nvar_ssat << " " << nclause << " " << scale << endl;
  cout << var_file.str() << exist_file.str() << clause_file.str() << evidence_file.str();
}

