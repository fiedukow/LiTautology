#include <iostream>
#include <cassert>
#include <cstdlib>
#include <sstream>

// xxx ooo ooo
// ooo xxx ooo
// ooo ooo xxx

std::string genRows(const int N) {
  std::stringstream ss;
  for (int i = 0; i < N; ++i) {
    // every line
    // generate a1 or a2 or ... or an
    for (int j = 1; j <= N; ++j) {
      ss << j+(N*i) << " ";
    }
    ss << std::endl;
    // generate not aj or not ak, where j != k
    for (int j = 1; j <= N; ++j) {
      for (int k = j+1; k <= N; ++k) {
        ss << -(j+(N*i)) << " " << -(k+(N*i)) << std::endl;
      }
    }
  }
  return ss.str();
}

// xoo oxo oox
// xoo oxo oox
// xoo oxo oox
std::string genCols(const int N) {
  std::stringstream ss;
  for (int i = 1; i <= N; ++i) {
    // every column
    // generate a1 or a(1+N) or ... or a(1+N(N-1))
    for (int j = 0; j < N; ++j) {
      ss << i+(N*j) << " ";
    }
    ss << std::endl;
    // generate not aj or not ak, where j != k
    for (int j = 0; j < N; ++j) {
      for (int k = j+1; k < N; ++k) {
        ss << -(i + N*j) << " " << -(i + N*k) << std::endl;
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
std::string genDiag(const int N) {
  std::stringstream ss;
/*  for (int i = 1; i <= N*N; ++i) {
    for (int j = i+N+1; j <= N*N && i%N != 0; j += N+1) {
      if((j-(N+1))%N == 0)
        break;
      ss << -i << " " << -j << std::endl;
    }
  }*/
  
  for (int i = 1; i <= N*N; ++i) {
    for (int j = i+N-1; j <= N*N; j += N-1) {
      if(j%N == 0)
        break;
      ss << -i << " " << -j << std::endl;
    }
  }
  return ss.str();
}

int main(int argc, char* argv[]) {
  assert(argc > 1);
  const int N = atoi(argv[1]);
  std::cout << genDiag(N) << std::endl;
}
