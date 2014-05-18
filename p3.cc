#include <vector>
#include <string>
#include <fstream>
#include <exception>
#include <iostream>
#include <ostream>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

typedef std::vector<int> Diff;

bool operator<(const Diff& d1, const Diff& d2) {
  for (int i : d1) {
    bool has = false;
    for (int j : d2) {
      if (i == j) {
        has = true;
        break; 
      }
    }
    if(!has)
      return false;
  }
  return true;
}

void InsertWithReduction(std::vector<Diff>& v, const Diff& d) {
  for (const Diff& di : v)
    if (di < d)
      return;
  for (std::vector<Diff>::iterator i = v.begin(); i < v.end();) {    
    if (d < (*i))
      i = v.erase(i);
    else
      i++;
  }
  v.push_back(d);
  return;
}

struct DiffWithId {
  DiffWithId(const Diff& diff, int id1, int id2)
    : diff(diff), id1(id1), id2(id2) {
  }
  Diff diff;
  int id1;
  int id2;

  friend std::ostream& operator<<(std::ostream& os, const DiffWithId& line);
};

class Line {
public:
  Line(const std::string& line);
  Diff diff(const Line& other) const;
 
  friend std::ostream& operator<<(std::ostream& os, const Line& line);
  
  std::vector<std::string> info;
  bool decision;  
};

class Table {
public:
  Table(const std::string& filePath);

  friend std::ostream& operator<<(std::ostream& os, const Table& t);

  std::vector<Line> lines;
};

class DiffTable {
public:
  DiffTable(const Table& t, bool info_system = true);
  std::vector<Diff> reducts() const;

  friend std::ostream& operator<<(std::ostream& os, const DiffTable& t);

private:
  const Table& t;
  std::vector<DiffWithId> diffs;
};

DiffTable::DiffTable(const Table& t, bool info_system)
    : t(t) {
  for (int i = 0; i < t.lines.size(); ++i)
    for (int j = i + 1; j < t.lines.size(); ++j)
      if (info_system || t.lines[i].decision != t.lines[j].decision)
        diffs.push_back(DiffWithId(t.lines[i].diff(t.lines[j]), i, j));
}

std::vector<Diff> DiffTable::reducts() const {
  std::vector<Diff> reducts;
  for (const DiffWithId& di : diffs)
    InsertWithReduction(reducts, di.diff);
  return reducts;
}

std::ostream& operator<<(std::ostream& os, const DiffTable& t) {
  for(DiffWithId diff : t.diffs)
    os << diff << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const DiffWithId& diff) {
  os << diff.id1 << " x " << diff.id2 << ": ";
  for (int i : diff.diff)
    os << i << " ";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Line& line) {
  for(const std::string& i : line.info)
    os << i << ", ";
  os << (line.decision ? "YES" : "NO");
  return os;
}

std::ostream& operator<<(std::ostream& os, const Table& t) {
  for(const Line& l : t.lines)
    os << l << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Diff> diffs) {
  for (const Diff& d : diffs) {
    os << "(";
    for (int i = 0; i < d.size(); ++i) {
      os << d[i] << (i == d.size()-1 ? "" : "+");
    }
    os << ")";    
  }
  os << std::endl;
  return os;
}

Table::Table(const std::string& filePath) {
  std::ifstream fs_in(filePath.c_str());
  std::string line;
  while(!fs_in.eof() && !fs_in.bad() && std::getline(fs_in, line) && line != "") {
    lines.push_back(Line(line));
  }
  fs_in.close();
}

Line::Line(const std::string& line) {
  boost::split(info, line, boost::is_any_of("\t "));
  decision = boost::lexical_cast<bool>(info.back());
  info.pop_back();
}

Diff Line::diff(const Line& other) const {
  assert(info.size() == other.info.size());
  Diff result;
  for (int i = 0; i < info.size(); ++i)
    if (info[i] != other.info[i])
      result.push_back(i);
  return result;
}

int main() {
  Table t("indec");
  std::cout << t << std::endl;
  DiffTable dt(t, false);
  std::cout << dt << std::endl;
  std::vector<Diff> diffs = dt.reducts();
  std::cout << "Redukty: " << diffs;
}
