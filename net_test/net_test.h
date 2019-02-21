#ifndef NET_TEST_H
#define NET_TEST_H

int RunAllTests();

bool RegisterTest(const char* base, const char* name, void(*func)());

//为什么这么写？
//如果不通过宏定义调用宏定义，那么被调用的宏定义将不会被展开
#define TCONCAT(a, b) TCONCAT1(a, b)
#define TCONCAT1(a, b) a##b

//自动生成代码
#define TEST(base, name)                                     \
class TCONCAT(_Test_, name) : public base {                  \
  public:                                                    \
    void Run();                                              \
    static void RunIt() {                                    \
      TCONCAT(_Test_, name) t;                               \
      t.Run();                                               \
    }                                                        \
};                                                           \
bool TCONCAT(_Test_ignored_, name) =                         \
  RegisterTest(#base, #name, &TCONCAT(_Test_, name)::RunIt); \
void TCONCAT(_Test_, name)::Run()

class TestBase {};

#endif
