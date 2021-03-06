## luabz - C++ <=> Lua
[![Build Status](https://travis-ci.org/blazgrom/luabz.svg?branch=master)](https://travis-ci.org/blazgrom/luabz)

[![codecov](https://codecov.io/gh/blazgrom/luabz/branch/master/graph/badge.svg)](https://codecov.io/gh/blazgrom/luabz)


#### Getting a Lua variable's value

```lua
--my_script.lua
variable=1;
mytable={
  variable=2
};
```
```cpp
#include "script.hpp"

int main()
{

  luabz::script my_script("my_script.lua")
  int variable=my_script["variable"]; //Getting the value of a global Lua variable
  int mytable_variable=my_script["my_table"]["variable"]; // Getting the value of a Lua table's field
  return 0;
}
```

#### Setting a Lua variable's value
```lua
--my_script.lua
variable=2;
mytable={
  variable=2
};
```

```cpp
#include "script.hpp"

int main()
{

  luabz::script my_script("my_script.lua")
  my_script["variable"]=1;//Setting global varialbe
  my_script["my_table"]["variable"]=1; // Setting the value of a Lua table's field
  return 0;
}
```

#### Run any Lua code
```cpp
#include "script.hpp"

int main()
{

  luabz::script my_script("my_script.lua")
  my_script("Your Lua code");
  return 0;
}
```

#### Registering a std::function to be called from lua
```cpp
#include "script.hpp"
int main()
{
  luabz::script my_script("my_script.lua")
  std::function<int()> f = [&]() {
        return 100;
    };
  my_script["std_function"].assign(f);
  int result=script["std_function"](); //100
  return 0;
}
```
#### Registering a normal function to be called from lua
```cpp
#include "script.hpp"
int normal_function()
{
    return 77;
}
int main()
{
  luabz::script my_script("my_script.lua")
  my_script["normal_function"].assign(normal_function);
  int result =script["normal_function"](); //77

  return 0;
}
```

#### Registering a C++ lambda to be called from lua
```cpp
#include "script.hpp"
int normal_function()
{
    return 77;
}
int main()
{
  luabz::script my_script("my_script.lua")
  script["lambda"].assign([](int a) mutable {
      return a;
  });
  int result =script["lambda"](1000); //1000
  return 0;
}
```
#### Dependencies
* Lua 5.1
* [Utils](https://github.com/blazgrom/Utils)
#### Building
1. mkdir build && cd build
2. cmake ..
3. make

#### Building flags
- ENABLE_TEST: Creates the 'make test' target, which build all the tests.
- ENABLE_DOC: Creates the 'make doc' target, which generates doxygen documentation.
- ENABLE_CLANG_FORMAT: Creates the 'make clang-format' target, which runs clang-format on the project. For more information on the styling see .clang-format.
- ENABLE_CLANG_TIDY: Creates the 'make clang-tidy' target, which runs clang-tidy on the project.
- ENABLE_CPPCHECK: Creates the 'make cppcheck' taget, which runs cppcheck on the project.
- USE_CPP_EXCEPTIONS: Every time there is an error instead of false assertion and logging the error into a file an cpp exception will be throw.


#### Notes
For  loading script dependencies use [lua modules] (http://lua-users.org/wiki/ModulesTutorial)
