#include "logic_sentence.h"
#include <iostream>
#include <memory>
#include <cassert>

LogicSPtr AND(LogicSPtr s1, LogicSPtr s2) {
  return new AndSentence(s1, s2);
}
LogicSPtr OR(LogicSPtr s1, LogicSPtr s2) {
  return new OrSentence(s1, s2);
}
LogicSPtr IMP(LogicSPtr s1, LogicSPtr s2) {
  return new ImplSentence(s1, s2);
}
LogicSPtr EQ(LogicSPtr s1, LogicSPtr s2) {
  return new EqualSentence(s1, s2);
}
LogicSPtr NOT_cnf(LogicSPtr s) {
  NotSentence* not_s = dynamic_cast<NotSentence*>(s);
  if (not_s == NULL) {
    NotSentence* ns = new NotSentence(s);//leak
    return ns->s->isFinal() ? ns : ns->asCNF();
  } else {
    return not_s->s->clone();
  }
}
LogicSPtr NOT(LogicSPtr s) {
  NotSentence* not_s = dynamic_cast<NotSentence*>(s);
  if (not_s == NULL)
    return new NotSentence(s);
  else
    return not_s->s->clone();
}

LogicSPtr S(char x) {
  return new SimpleSentence(x);
}

TwoOperandsSentence::~TwoOperandsSentence() {
  delete s1;
  delete s2;
}

NotSentence::~NotSentence() {
  delete s;
}

LogicSPtr SimpleSentence::asCNF() {
  return clone();
}

std::string SimpleSentence::asString() {
  std::stringstream ss;
  ss << symbol;
  return ss.str();
}

LogicSPtr SimpleSentence::clone() {
  return new SimpleSentence(*this);
}

Node* SimpleSentence::BuildTree(Node* parent) {
  //std::cout << "SS::BT" << std::endl;
  //parent->sentences.push_back(this);
  return parent;
}

LogicSPtr NotSentence::asCNF() {
  LogicSPtr inside = s->asCNF();
  AndSentence* ai = dynamic_cast<AndSentence*>(inside);
  OrSentence* oi = dynamic_cast<OrSentence*>(inside);
  // Prawa de'Morgana
  if (ai != NULL) {
    inside = OR(NOT_cnf(ai->s1), NOT_cnf(ai->s2));
    //delete ai;
  } else if (oi != NULL) {
    inside = AND(NOT_cnf(oi->s1), NOT_cnf(oi->s2));
    //delete oi;
  } else {
    return NOT_cnf(inside);
  }
  return inside;
}

std::string NotSentence::asString() {
  std::stringstream ss;
  ss << "!(" << s->asString() << ")";
  return ss.str();
}

LogicSPtr NotSentence::clone() {
  return new NotSentence(*this);
}

Node* NotSentence::BuildTree(Node* parent) {
  assert(isFinal());
  //std::cout << "NS::BT" << std::endl;
  //parent->sentences.push_back(this);
  return parent;
}

LogicSPtr OrSentence::asCNF() {
  return OR(s1->asCNF(), s2->asCNF());
}

std::string OrSentence::asString() {
  std::stringstream ss;
  ss << "(" << s1->asString() << " + " << s2->asString() << ")";
  return ss.str();
}

LogicSPtr OrSentence::clone() {
  return new OrSentence(*this);
}

Node* OrSentence::BuildTree(Node* parent) {
  //std::cout << "OR::BT" << std::endl;
  parent->sentences.remove(this);
  parent->sentences.push_back(s1);
  parent->sentences.push_back(s2);
  s1->BuildTree(parent);
  s2->BuildTree(parent);
  return parent;
}

LogicSPtr AndSentence::asCNF() {
  return AND(s1->asCNF(), s2->asCNF());
}

std::string AndSentence::asString() {
  std::stringstream ss;
  ss << "(" << s1->asString() << " * " << s2->asString() << ")";
  return ss.str();
}

LogicSPtr AndSentence::clone() {
  return new AndSentence(*this);
}

Node* AndSentence::BuildTree(Node* parent) {
 // std::cout << "AND::BT" << std::endl;
  parent->sentences.remove(this);
  parent->l = new Node;
  parent->r = new Node;
  parent->l->sentences = parent->sentences;
  parent->l->sentences.push_back(s1);
  parent->r->sentences = parent->sentences;
  parent->r->sentences.push_back(s2);
  s1->BuildTree(parent->l);
  s2->BuildTree(parent->r);
  return parent;
}

LogicSPtr ImplSentence::asCNF() {
  return OR(NOT_cnf(s1->asCNF()), s2->asCNF());
}

std::string ImplSentence::asString() {
  std::stringstream ss;
  ss << "(" << s1->asString() << " => " << s2->asString() << ")";
  return ss.str();
}

LogicSPtr ImplSentence::clone() {
  return new ImplSentence(*this);
}

Node* ImplSentence::BuildTree(Node* parent) {
  assert(false);
  return NULL;
}

LogicSPtr EqualSentence::asCNF() {
  return OR(AND(s1->asCNF(), s2->asCNF()), AND(NOT_cnf(s1->asCNF()), NOT_cnf(s2->asCNF())));
}

std::string EqualSentence::asString() {
  std::stringstream ss;
  ss << "(" << s1->asString() << " <=> " << s2->asString() << ")";
  return ss.str();
}

LogicSPtr EqualSentence::clone() {
  return new EqualSentence(*this);
}

Node* EqualSentence::BuildTree(Node* parent) {
  assert(false);
  return NULL;
}

Node* Node::BuildTree(LogicSPtr s) {
  Node* root = new Node;
  root->sentences.push_back(s);
  s->BuildTree(root);
  return root;
}

std::string Node::asString(const int level) {
  std::stringstream ss;
  ss << "->";
  for(int i = 0; i < level; ++i) ss << "  ";
  for(auto s : sentences)
    ss << s->asString() << ";";

  if (l != NULL)
    ss << std::endl << l->asString(level + 1);
  if (r != NULL)
    ss << std::endl << r->asString(level + 1);
  if (l == NULL && r == NULL) {
    ss << "L";
  }
  return ss.str();
}

bool Node::isTautology() {
  if (l == NULL && r == NULL) {
    for (auto s1 : sentences) {
      for (auto s2 : sentences) {
        NotSentence* ns1 = dynamic_cast<NotSentence*>(s1); 
        SimpleSentence* ss2 = dynamic_cast<SimpleSentence*>(s2);
        if (ns1 != NULL && ss2 != NULL) {
          SimpleSentence* ins1 = dynamic_cast<SimpleSentence*>(ns1->s);
          assert(ins1);
          if (ins1->symbol == ss2->symbol)
            return true;
        }
      }
    }
    return false;
  } else {
    assert(l != NULL && r != NULL);
    return l->isTautology() && r->isTautology();
  }
}

bool isTautology(LogicSPtr s) {
  std::cout << Node::BuildTree(s->asCNF())->asString() << std::endl;
  return Node::BuildTree(s->asCNF())->isTautology(); // yet another leak
}

bool isSatisfiable(LogicSPtr s) {
  return !isTautology(NOT(s));
}

bool testSentence(LogicSPtr s, bool ex_t, bool ex_s) {
  std::cout << "=== BEGIN OF TEST CASE ===" << std::endl;
  std::cout << s->asString() << std::endl;
  std::cout << s->asCNF()->asString() << std::endl;
  std::cout << "Tautology Tree:" << std::endl;
  bool tr = isTautology(s->asCNF());
  std::cout << "Satisfiable Tree:" << std::endl;
  bool sr = isSatisfiable(s->asCNF());

  if (tr)
    std::cout << "This formula is tautology." << std::endl;
  else if (sr)
    std::cout << "This formula is satisfiable." << std::endl;
  else
    std::cout << "This formula is NOT staisfiable." << std::endl;

  if (ex_t == tr && ex_s == sr)
    std::cout << "[OK]" << std::endl;
  else
    std::cout << "[FAILED]" << std::endl;

  std::cout << "=== END OF TEST CASE ===" << std::endl;
}

void tests() {
  testSentence(NOT(EQ(S('q'), IMP(NOT(S('p')), S('q')))), false, true);
  testSentence(AND(NOT(S('p')), S('p')), false, false);
  testSentence(OR(NOT(S('p')), S('p')), true, true);
  testSentence(EQ(IMP(IMP(OR(S('p'), S('q')), S('r')), NOT(OR(S('q'),S('r')))), NOT(S('r'))), true, true);
}

int main() {
  tests();
} 


