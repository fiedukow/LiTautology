#include <iostream>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <zlib.h>
#include <cstdio>

#include "utils/System.h"
#include "utils/ParseUtils.h"
#include "utils/Options.h"
#include "core/Dimacs.h"
#include "simp/SimpSolver.h"

using namespace Minisat;

// xxx ooo ooo
// ooo xxx ooo
// ooo ooo xxx

std::string genRows(const int N, int* n) {
  std::stringstream ss;
  for (int i = 0; i < N; ++i) {
    // every line
    // generate a1 or a2 or ... or an
    for (int j = 1; j <= N; ++j) {
      ss << j+(N*i) << ((j < N) ? " " : "");
    }
    ss << "  0" << std::endl;
    (*n)++;
    // generate not aj or not ak, where j != k
    for (int j = 1; j <= N; ++j) {
      for (int k = j+1; k <= N; ++k) {
        ss << -(j+(N*i)) << " " << -(k+(N*i)) << "  0" << std::endl;
        (*n)++;
      }
    }
  }
  return ss.str();
}

// xoo oxo oox
// xoo oxo oox
// xoo oxo oox
std::string genCols(const int N, int* n) {
  std::stringstream ss;
  for (int i = 1; i <= N; ++i) {
    // every column
    // generate a1 or a(1+N) or ... or a(1+N(N-1))
    for (int j = 0; j < N; ++j) {
      ss << i+(N*j) << ((j < N-1) ? " " : "");
    }
    ss << "  0" << std::endl;
    (*n)++;
    // generate not aj or not ak, where j != k
    for (int j = 0; j < N; ++j) {
      for (int k = j+1; k < N; ++k) {
        ss << -(i + N*j) << " " << -(i + N*k) << "  0" << std::endl;
        (*n)++;
      }
    }
  }
  return ss.str();
}

// xoo oxo oox
// oxo oox ooo
// oox ooo ooo
//
// ooo ooo
// xoo ooo
// oxo xoo
//
// oox oxo xoo
// oxo xoo ooo
// xoo ooo ooo
//
// ooo ooo
// oox ooo
// oxo oox
std::string genDiags(const int N, int* n) {
  std::stringstream ss;
  for (int i = 1; i <= N*N; ++i) {
    for (int j = i+N+1; j <= N*N && i%N != 0; j += N+1) {
      if((j-(N+1))%N == 0)
        break;
      ss << -i << " " << -j << "  0" << std::endl;
      (*n)++;
    }
  }
  
  for (int i = 1; i <= N*N; ++i) {
    for (int j = i+N-1; j <= N*N; j += N-1) {
      if(j%N == 0)
        break;
      ss << -i << " " << -j << "  0" << std::endl;
      (*n)++;
    }
  }
  return ss.str();
}

std::string genAll(const int N) {
  std::stringstream ss;
  int n = 0;
  const std::string rows  = genRows (N, &n);
  const std::string cols  = genCols (N, &n);
  const std::string diags = genDiags(N, &n);
  ss << "p cnf " << N*N << " " << n << std::endl;
  ss << rows;
  ss << cols;
  ss << diags;
  return ss.str();
}

void printStats(Solver& solver)
{
    double cpu_time = cpuTime();
    double mem_used = memUsedPeak();
    printf("restarts              : %"PRIu64"\n", solver.starts);
    printf("conflicts             : %-12"PRIu64"   (%.0f /sec)\n", solver.conflicts   , solver.conflicts   /cpu_time);
    printf("decisions             : %-12"PRIu64"   (%4.2f %% random) (%.0f /sec)\n", solver.decisions, (float)solver.rnd_decisions*100 / (float)solver.decisions, solver.decisions   /cpu_time);
    printf("propagations          : %-12"PRIu64"   (%.0f /sec)\n", solver.propagations, solver.propagations/cpu_time);
    printf("conflict literals     : %-12"PRIu64"   (%4.2f %% deleted)\n", solver.tot_literals, (solver.max_literals - solver.tot_literals)*100 / (double)solver.max_literals);
    if (mem_used != 0) printf("Memory used           : %.2f MB\n", mem_used);
    printf("CPU time              : %g s\n", cpu_time);
}


int main(int argc, char* argv[]) {
  assert(argc > 1);
  const int N = atoi(argv[1]);
  std::ofstream fout;
  fout.open("formula", std::ofstream::out);
  std::cout << "Generating... " << std::endl;
  fout << genAll(N) << std::endl;
  fout.close();
  gzFile in = gzopen("formula", "rb");

  SimpSolver  S;
  std::cout << "Parsing... " << std::endl;
  parse_DIMACS(in, S);
  gzclose(in);
  std::cout << "Eliminating..." << std::endl;
  S.eliminate(true);
  vec<Lit> dummy;
  std::cout << "Solving... " << std::endl;
  lbool ret = S.solveLimited(dummy);
  printStats(S);
  printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");
  FILE* res = stdout;
  if (res != NULL){
    if (ret == l_True){
        fprintf(res, "SAT\n");
        for (int i = 0; i < S.nVars(); i++)
            if (S.model[i] != l_Undef)
                fprintf(res, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
        fprintf(res, " 0\n");
    }else if (ret == l_False)
        fprintf(res, "UNSAT\n");
    else
        fprintf(res, "INDET\n");
    fclose(res);
  }   
}
