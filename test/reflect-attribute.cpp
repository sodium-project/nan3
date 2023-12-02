// RUN: split-file %s %t
// RUN: clang -cc1 -load %shlibdir/nan3%shlibext -fsyntax-only -ast-dump -verify %t/good_attr.cpp | FileCheck %s
// RUN: clang -cc1 -load %shlibdir/nan3%shlibext -fsyntax-only -ast-dump -verify %t/bad_attr.cpp
//--- good_attr.cpp
// expected-no-diagnostics
class [[reflect]] c1 {};
class [[nan3::reflect]] c2 {};
class __attribute__((reflect)) c3 {};

struct [[reflect]] s1 {};
struct [[nan3::reflect]] s2 {};
struct __attribute__((reflect)) s3 {};

union [[reflect]] u1 {};
union [[nan3::reflect]] u2 {};
union __attribute__((reflect)) u3 {};
// CHECK-COUNT-9: -AnnotateAttr 0x{{[0-9a-z]+}} {{<col:[0-9]+(, col:[0-9]+)?>}} "reflect"

//--- bad_attr.cpp
int var1 __attribute__((reflect)) = 1; // expected-warning {{'reflect' attribute only applies to enums or classes}}
[[reflect]] int var2 = 2; // expected-warning {{'reflect' attribute only applies to enums or classes}}
[[nan3::reflect]] int var3 = 3; // expected-warning {{'reflect' attribute only applies to enums or classes}}