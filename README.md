# LuaBz - Simple Lua wrapper for Modern C++ 

Feature list:

 * Retrieve Lua variable's value
 * Set Lua variable's value
 * Function calls
 * Table info
 * User defined class support
 * Utility

```cpp
		LuaBz script("test.lua");
```
#### Retrieve lua variable's value:

* Lua 
```lua
	width=500.0
```
* C++
```cpp
	double width = script.get < double >("width");
```
* Lua 
```lua
	Position={
                x=100,
                y=100
              }
```
* C++
```cpp
	int pos_x = script.get < int >("Position.x");
```

####  Set Lua variable's value:

```cpp
	bool success = script.set("width",newWidthValue);
```
```cpp
	bool success = script.set("Position.x",newAxisXValue);
```
#### Function Calls:
##### No input params and no return values
* Lua
```lua
	function helloWorld()
		print "Hello World"
	end
```

* C++
```cpp
	script.call(LuaFunction<void>("helloWorld"));
```
##### No input params and N return values
* Lua
```lua
	function multipleReturnValues()
		return 1,2,3
    end
```
* C++
```cpp
    std::tuple<int,int,int> result=script.call(LuaFunction<int,int,int>("multipleReturnValues"));
```
##### N input params and no return values
* Lua
```lua
	function Sum(a,b,c)
    	local x=a+b+c
        print x
    end
```
* C++
```cpp
    script.call(LuaFunction<void>("Sum"), 1, 2, 3);
```

##### N input params and N return values

* Lua
```lua
	function Test(a,b,c)
         return a,b,c
    end
```
* C++
```cpp
	int returnValuesCount=3;
    LuaFunction<int, int, int> test("Test");
   	auto result = script.call(test, 1, 2, 3); 
```
#### Table info
```cpp
	std::map<std::string,std::string>info;
	info=script.getTableInfo("Entity");
   /*
    std::pair<const std::string,std::string> singleElement;
    singleElement.first - the name of the field
    singleElement.second - field's type
   */
```

#### User defined class support

 In order to use user defined classes you have to implement the following member function ( support for nested classes is not provided)
* luaPack
* luaUnpack() const

Examples
```cpp
struct Position {
	double x;
	double y;
	//The value in the map returned by luaUnpack can be of any simple type except user defined types
	std::map<string, double> luaUnpack() const
	{
		std::map<string, double> data;
		data["x"] = x;
		data["y"] = y;
		return data;
	}
	void luaPack(const std::map<string,string>& data)
	{
		x = std::stod(data.at("x"));
		y = std::stod(data.at("y"));
	}
};
```
#### Utility

* Close File

```cpp
	script.closeFile()
};
```
* Open file 
```cpp
	script.openFile("test.lua")
};
```

* Change file ( If the new file cannot be loaded, the old one is left active )
```cpp
	script.changeFile("newTest.lua")
};
```
	
