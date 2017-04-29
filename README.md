## LuaBz - A simple Lua binding for modern C++

Feature list:
 * Get/Set variable's value
 * Function call
 * Utilities
 * User defined class support


```cpp
  LuaBz script("test.lua");
```
#### Get/Set variable's value

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
#### Function Calls:
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
#### Utilities
```cpp
  script.open("test.lua");
  script.change("newTest.lua");
  script.close();
  //key = variable's name,value = variable's type
  std::map<std::string,std::string>info=script.tableInfo("Entity");
```
#### User defined class support
 In order to use a user defined type in with the wrapper you have to define a constructor that accepts as paramater a LuaTable (see file LuaHelpers.hpp) and
 also define a user defined conversion from your type to a LuaTable

