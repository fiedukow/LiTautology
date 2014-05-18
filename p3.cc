// Spagethi-coding-fest...
#include <vector>
#include <string>
#include <fstream>
#include <exception>
#include <iostream>
#include <ostream>
#include <cassert>
#include <set>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

typedef std::vector<int> Diff;

struct Reduct {
  std::vector<int> mul;
};

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
  void fillSoV(std::vector<std::set<std::string>>& toFill) const;
 
  friend std::ostream& operator<<(std::ostream& os, const Line& line);
  
  std::vector<std::string> info;
  bool decision;  
};

struct Rule {
public:
  bool operator()(const Line&);
  
  std::map<int, std::string> rule;
  bool decision;
};

class Table {
public:
  Table(const std::string& filePath);
  //set of values
  void generateSoV();
  std::vector<Rule> generateRules(const std::vector<Reduct>&);
  std::vector<Rule> generateRules(Reduct);

  friend std::ostream& operator<<(std::ostream& os, const Table& t);
  
  std::vector<std::set<std::string>> sov;
  std::vector<Line> lines;
};

class DiffTable {
public:
  DiffTable(const Table& t, bool info_system = true);
  std::vector<Reduct> reducts() const;

  friend std::ostream& operator<<(std::ostream& os, const DiffTable& t);

private:
  const Table& t;
  std::vector<DiffWithId> diffs;
};

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

std::ostream& operator<<(std::ostream& os, const Reduct& rd) {
  for (int i = 0; i < rd.mul.size(); ++i) {
    os << rd.mul[i] << (i == rd.mul.size()-1 ? "" : "*");
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Reduct>& rds) {
  for (int i = 0; i < rds.size(); ++i) {
    os << rds[i] << (i == rds.size()-1 ? "" : "+");
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Rule& r) {
  os << "if (";
  int i = 0;
  for (auto e : r.rule) {
    i++;
    os << e.first << " = " << e.second << (i == r.rule.size() ? "" : " && ");
  }
  os << ") then " << (r.decision ? "YES" : "NO");
  return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T> v) {
  for (const T& e : v)
    os << e << std::endl;
  return os;
}

std::vector<Rule> Table::generateRules(const std::vector<Reduct>& rds) {
  std::vector<Rule> rules;
  for (const Reduct& rd : rds) {
    std::vector<Rule> rld = generateRules(rd);
    rules.insert(rules.end(), rld.begin(), rld.end());
  }
  return rules;
}

std::vector<Rule> Table::generateRules(Reduct rd) {
  std::vector<Rule> result;
  int l = rd.mul.back();
  rd.mul.pop_back();  
  if (rd.mul.empty()) {
    for (std::string s : sov[l]) {
      Rule r_p;
      r_p.rule[l] = s;
      Rule r_n = r_p;
      r_p.decision = true;
      r_n.decision = false;
      result.push_back(r_p);
      result.push_back(r_n);
    }
    return result;
  }
  std::vector<Rule> smaller = generateRules(rd);
  for (Rule r : smaller) {
    for (std::string s : sov[l]) {
      Rule nr = r;
      nr.rule[l] = s;
      result.push_back(nr);
    }
  }
  return result;
}

DiffTable::DiffTable(const Table& t, bool info_system)
    : t(t) {
  for (int i = 0; i < t.lines.size(); ++i)
    for (int j = i + 1; j < t.lines.size(); ++j)
      if (info_system || t.lines[i].decision != t.lines[j].decision)
        diffs.push_back(DiffWithId(t.lines[i].diff(t.lines[j]), i, j));
}

std::vector<Reduct> operator*(const Reduct& r, const Diff sum) {
  std::vector<Reduct> rs;
  for (int d : sum) {
    Reduct rn = r;
    rn.mul.push_back(d);
    rs.push_back(rn);
  }
  return rs;
}

std::vector<Reduct> operator*(const std::vector<Reduct>& r, const Diff& diff) {
  std::vector<Reduct> result;
  if (r.empty()) {
    result = Reduct()*diff;
    return result;
  }
  for (const Reduct& ri : r) {
    std::vector<Reduct> res = ri * diff;
    result.insert(result.end(), res.begin(), res.end());
  }
  return result;
}

std::vector<Reduct> ToReducts(const std::vector<Diff>& sums) {
  std::vector<Reduct> result;
  for (Diff diff : sums) {
    result = result * diff;
  }
  return result;
}

std::vector<Reduct> reduce(std::vector<Reduct> red) {
  // NEVER TESTED
  for (Reduct& r : red) {
    for (std::vector<int>::iterator i = r.mul.begin(); i != r.mul.end(); ++i) {
      for (std::vector<int>::iterator j = i+1; j != r.mul.end();) {
        if (*i == *j)
          j = r.mul.erase(j);
        else
          j++;
      }
    }
  }
  return red;
}


std::vector<Reduct> DiffTable::reducts() const {
  std::vector<Diff> sums;
  for (const DiffWithId& di : diffs)
    InsertWithReduction(sums, di.diff);
  std::cout << sums << std::endl;
  return reduce(ToReducts(sums));
}

std::ostream& operator<<(std::ostream& os, const DiffTable& t) {
  for(DiffWithId diff : t.diffs)
    os << diff << std::endl;
  return os;
}

Table::Table(const std::string& filePath) {
  std::ifstream fs_in(filePath.c_str());
  std::string line;
  while(!fs_in.eof() && !fs_in.bad() && std::getline(fs_in, line) && line != "") {
    lines.push_back(Line(line));
  }
  fs_in.close();
  generateSoV();
  for (std::set<std::string> s : sov) {
    for (std::string ss : s)
      std::cout << ss << " ";
    std::cout << std::endl;
  }
}

void Table::generateSoV() {
  for (int i = 0; i < lines[0].info.size(); ++i)
    sov.push_back(std::set<std::string>());
  for (const Line& l : lines)
    l.fillSoV(sov);
}

void Line::fillSoV(std::vector<std::set<std::string>>& toFill) const {
  for (int i = 0; i < toFill.size(); ++i)
    toFill[i].insert(info[i]);
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
  std::vector<Reduct> diffs = dt.reducts();
  std::cout << "Redukty: " << diffs << std::endl;
  std::vector<Rule> rules = t.generateRules(diffs);
  std::cout << rules << std::endl;
}
