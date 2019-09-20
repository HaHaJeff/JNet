#include <vector>
#include <string>
#include "unit_test.h"

struct Test {
  const char* base;
  const char* name;
  void (*func)();
};

std::vector<Test>* GetVectorTest() {
  static std::vector<Test> tests;
  return &tests;
}

bool RegisterTest(const char* base, const char* name, void(*func)()) {
  std::vector<Test>* tests = GetVectorTest();

  Test t;
  t.base = base;
  t.name = name;
  t.func = func;
  tests->push_back(t);
  return true;
}

int RunAllTests() {
  std::vector<Test>* tests = GetVectorTest();

  if (tests->size() != 0) {
    for (auto elem : *tests) {
      //fprintf(stderr, "==== Test %s.%s\n", elem.base, elem.name);
      elem.func();
    }
  }
  return 0;
}

