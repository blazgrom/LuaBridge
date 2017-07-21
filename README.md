## LuaBz - Binding between C++ and Lua

Feature list:
 * Getting/Setting variable's value
 * Registering C++ functions
 * Calling a function
 * User defined class support


```cpp
  LuaBz script("test.lua");
```
#### Getting/Setting variable's value

* Lua 
```lua
  width=500.0
  Position={x=100,y=100}
```
* C++
```cpp
  double width = script.get<double>("width");
  int pos_x = script.get<int>("Position.x");
  bool successWidth = script.set("width",newWidthValue);
  bool successPositionX = script.set("Position.x",newAxisXValue);
```
#### Registering C++ functions
```cpp
  //std::function
  std::function<int(int, int)> func = [](int a, int b)->int { return a+b; };
  script.register_function("func", func);
  //lambda
  script.register_function("lambda", [](int a, int b)->int { return a+b; });
  //normal function
  int f(int a,int b)
  {
    return a+b;
  }
  script.register_function("f",f);
  //Function object
  struct FunObj
  {
    int operator()(int a,int b)
    {
      return a+b;
    }
  };
  script.register_function("FunctionObject",FunObj{})
```
#### Calling a function:
##### No input params and no return values
* Lua
```lua
	
function helloWorld()
  print "Hello World"
end

function multipleReturnValues()
  return 1,2,3.0
end

function multipleInputNoResult(a,b,c)
  local x=a+b+c
  print x
end

function multipleInputMultipleResultValues(a,b,c)
  return a,b,c
end
```
* C++
```cpp
script.call(LuaFunction<void>("helloWorld"));

auto result=script.call(LuaFunction<int,int,double>("multipleReturnValues"));

script.call(LuaFunction<void>("multipleInputNoResult"), 1, 2, 3);

auto secondresult = script.call(LuaFunction<int, int, int> test("multipleInputMultipleResultValues"), 1, 2, 3); 
```
#### User defined class support
 In order to use a user defined type with Lua you have to a type that has a c-tor that accepts a LuaTable and a conversion from your type to a LuaTable.
 For more info on LuaTable see LuaTable.hpp.

#### Dependencies
* Lua 5.1.5
* [Utils](https://github.com/blazgrom/Utils)