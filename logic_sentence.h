#pragma once
#include <memory>
#include <list>
#include <string>
#include <sstream>

class Node;

class LogicSentence {
 public:
  virtual ~LogicSentence() {}
  virtual LogicSentence* asCNF() = 0;
  virtual bool isFinal() { return false; }
  virtual std::string asString() = 0;
  virtual LogicSentence* clone() = 0;
  virtual Node* BuildTree(Node* parent) = 0; //void?
};

typedef LogicSentence* LogicSPtr;

LogicSPtr AND(LogicSPtr s1, LogicSPtr s2);
LogicSPtr OR(LogicSPtr s1, LogicSPtr s2);
LogicSPtr IMP(LogicSPtr s1, LogicSPtr s2);
LogicSPtr EQ(LogicSPtr s1, LogicSPtr s2);
LogicSPtr NOT(LogicSPtr s);
LogicSPtr S(char x);

class SimpleSentence : public LogicSentence {
 public:
  SimpleSentence(char symbol)
    : symbol(symbol) {}
  virtual LogicSPtr asCNF();
  virtual bool isFinal() { return true; }
  virtual std::string asString();
  virtual LogicSPtr clone();
  virtual Node* BuildTree(Node* parent);

  char symbol;
};

class TwoOperandsSentence : public LogicSentence {
 public:
  TwoOperandsSentence(LogicSPtr s1, LogicSPtr s2)
    : s1(s1), s2(s2) {}
  virtual ~TwoOperandsSentence();

  LogicSPtr s1;
  LogicSPtr s2;
};

class OrSentence : public TwoOperandsSentence {
 public:
  OrSentence(LogicSPtr s1, LogicSPtr s2)
    : TwoOperandsSentence(s1, s2) {}
  virtual LogicSPtr asCNF();
  virtual std::string asString();
  virtual LogicSPtr clone();
  virtual Node* BuildTree(Node* parent);
};

class AndSentence : public TwoOperandsSentence {
 public:
  AndSentence(LogicSPtr s1, LogicSPtr s2)
    : TwoOperandsSentence(s1, s2) {}
  virtual LogicSPtr asCNF();
  virtual std::string asString();
  virtual LogicSPtr clone();
  virtual Node* BuildTree(Node* parent);
};

class ImplSentence : public TwoOperandsSentence {
 public:
  ImplSentence(LogicSPtr s1, LogicSPtr s2)
    : TwoOperandsSentence(s1, s2) {}
  virtual LogicSPtr asCNF();
  virtual std::string asString();
  virtual LogicSPtr clone();
  virtual Node* BuildTree(Node* parent);
};

class EqualSentence : public TwoOperandsSentence {
 public:
  EqualSentence(LogicSPtr s1, LogicSPtr s2)
    : TwoOperandsSentence(s1, s2) {}
  virtual LogicSPtr asCNF();
  virtual std::string asString();
  virtual LogicSPtr clone();
  virtual Node* BuildTree(Node* parent);
};

class NotSentence : public LogicSentence {
 public:
  NotSentence(LogicSPtr s)
    : s(s) {}
  virtual ~NotSentence();
  virtual LogicSPtr asCNF();
  virtual bool isFinal() { return s->isFinal(); }
  virtual std::string asString();
  virtual LogicSPtr clone();
  virtual Node* BuildTree(Node* parent);

  LogicSPtr s;
};

class Node {
public:
  Node() : l(NULL), r(NULL) {}
  std::string asString(int level = 0);
  void continueBuilding();
  static Node* BuildTree(LogicSPtr);
  
  bool isTautology();
  Node* l;
  Node* r;
  
  std::list<LogicSPtr> sentences;
};
