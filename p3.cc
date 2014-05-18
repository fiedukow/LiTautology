#include <vector>
#include <string>
#include <fstream>
#include <exception>
#include <iostream>
#include <ostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


class Line {
public:
  Line(const std::string& line);
  std::vector<std::string> diff(const Line& other) const;
 
  friend std::ostream& operator<<(std::ostream& os, const Line& line);
  
private:
  std::vector<std::string> info;
  bool decision;  
};

class Table {
public:
  Table(const std::string& filePath);

  friend std::ostream& operator<<(std::ostream& os, const Table& t);

private:
  std::vector<Line> lines;
};

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

int main() {
  Table t("indec");
  std::cout << t << std::endl;
}
