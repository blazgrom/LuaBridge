#ifndef LUA_STACK_TEMPLATE_IMPL
    #error This file should be included only from lua_stack.hpp
#endif
template <class T>
T lua_stack::top_element(bool popTopElement) const
{
    if (popTopElement)
    {
        //Experiment 1
        //Open new state on the same file with the same configuration
        // lua_State* d=luaL_newstate();
        // luaL_openlibs(d);
        // luaL_dofile(d,"src/core/tests/luascript_test.lua");
        // //Access the same variable to see if they have the same value
        // auto top=-1;
        // auto s=lua_tointeger(m_state,top);
        // lua_getglobal(d, "integer_var");
        // auto s1=lua_tointeger(d,top);
        // //Change the value from m_state to see if the value off d state changes
        // push(s+1);
        // lua_setglobal(m_state, "integer_var");
        // lua_getglobal(d, "integer_var");
        // s1=lua_tointeger(d,top);
        // lua_getglobal(m_state, "integer_var");
        // s=lua_tointeger(m_state,top);
        // //Transfer the updated value
        // lua_xmove(m_state,d,1);
        // lua_setglobal(d,"integer_var");
        // lua_getglobal(d, "integer_var");
        // s1=lua_tointeger(d,top);
        
        //Experiment 2
    //     lua_State* d=m_state;
    //     auto top=-1;
    //     push(100+25);
    //     lua_setglobal(m_state, "integer_var");
    //     lua_getglobal(d, "integer_var");
    //     auto s1=lua_tointeger(d,top);
    //     lua_getglobal(m_state, "integer_var");
    //     auto s=lua_tointeger(m_state,top);
    //     //Experiment 3
        
    //     //Resize stack to current size+4k
    //     auto can_contain=lua_checkstack(m_state,4000);
    //     lua_Debug* debug_info=nullptr;
    //     lua_getstack(m_state,0,debug_info);
    //     //Try pushing element to the stack,segmentation fault if cannot insert more elements
    //     for(int i=0;i!=12000;++i)
    //     {
    //         lua_pushinteger(m_state, i);
    //     }
    //     lua_State* thr=lua_newthread(m_state);
    //     lua_getglobal(thr,"integer_var");
    //     auto thr1=lua_tointeger(thr,top);
    //     //Experimenting 4
    //  //   lua_pop(m_state,5);
    //   //  lua_gc(m_state,LUA_GCCOLLECT,1);
    //     lua_getglobal(thr,"integer_var");
    //     thr1=lua_tointeger(thr,top);

    //   auto top=-1;
    //     lua_State* secondMainState=luaL_newstate();
    //     lua_State* thread1=lua_newthread(m_state);
    //     //Insert the new global table
    //     lua_newtable(thread1);
    //     //Create the new metatable
    //     //The __index of this table points to the old LUA_GLOBALSINDEX,
    //     //in order to be able to retrieve value already defined.
    //     //New values should be inserted in the new global table
    //     lua_newtable(thread1);
    //     lua_pushliteral(thread1,"__index");
    //     lua_pushvalue(thread1,LUA_GLOBALSINDEX);
    //     static const int new_metable_index=-3;
    //     lua_settable(thread1,new_metable_index);
    //     //Set the metable of the new global table
    //     static const int new_globaltable_index=-2;
    //     lua_setmetatable(thread1,new_globaltable_index);
    //     //Set the new global table
    //     lua_replace(thread1,LUA_GLOBALSINDEX);
    //     luaL_dofile(thread1,"src/core/tests/luascript_test.lua");

    //     lua_State* thread2=lua_newthread(m_state);
    //     //Set new __index metatable
    //     // lua_newtable(thread2);
    //     // lua_pushliteral(thread2,"__index");//Key
    //     // lua_pushvalue(thread2,LUA_GLOBALSINDEX);
    //     // lua_settable(thread,-3);

    //    lua_newtable(thread2); //new globals table
    //     lua_newtable(thread2); //metatable

    //     lua_pushliteral(thread2, "__index");
    //     lua_pushvalue(thread2, LUA_GLOBALSINDEX); //original globals 
    //     lua_settable(thread2, -3);
    //     lua_setmetatable(thread2, -2);
    //     //lua_replace(thread2, LUA_GLOBALSINDEX); //replace newState's globals
    //     luaL_dofile(thread2,"src/core/tests/luascript_test.lua");

    //     lua_pushinteger(thread2, 2222);
    //     lua_setglobal(thread2,"Somedummyvalue");
    //     lua_getglobal(m_state,"Somedummyvalue");
    //     auto s=LUA_GLOBALSINDEX;//This is deprecated from Lua5.2 onwards
    //     auto dummyMainState=lua_tointeger(m_state,top);
        
    //     lua_getglobal(thread1,"Somedummyvalue");
    //     auto dummyThreadState=lua_tointeger(thread1,top);

    //     lua_getglobal(thread2,"integer_var");
    //     auto dummyThread2State=lua_tointeger(thread2,top);



    //    lua_getglobal(secondMainState,"Somedummyvalue");
    //     auto dummysecondMainState=lua_tointeger(secondMainState,top);

        //Experimenting 5
        auto top=-1;
        auto file_name1="tests/core/luascript_dep.lua";
        auto file_name2="tests/core/luascript_dep2.lua";
        auto dep1state=LuaBz::detail::lua_state_factory::create_state(file_name1);
        lua_getglobal(dep1state,"dep1_integer");
        auto dep1=lua_tointeger(dep1state,top);
        
        auto dep2state=LuaBz::detail::lua_state_factory::create_state(file_name2);
        lua_getglobal(dep2state,"dep2_integer");
        auto dep2=lua_tointeger(dep2state,top);
        lua_getglobal(dep2state,"dep1_integer");
        dep1=lua_tointeger(dep2state,top);
        lua_pushinteger(dep1state, 11);
        lua_setglobal(dep1state,"dep1_integer");

        auto dep1reference=LuaBz::detail::lua_state_factory::create_state(file_name1);
        lua_getglobal(dep1reference,"dep1_integer");
        dep1=lua_tointeger(dep1reference,top);
        
        T result = get<T>(topElement);
        pop();
        return result;
    }
    return get<T>(topElement);
}
template <class T>
void lua_stack::set_element(const std::string& name, T&& value) const
{
    if (name.find('.') == std::string::npos)
    {
        set_global_variable(name, value);
    }
    else
    {
        set_table_element(name, value);
    }
}
template <class T>
T lua_stack::get_element(int index) const
{
    return get<T>(index);
}
template <class T>
typename std::enable_if<std::is_convertible<T, lua_table>::value>::type lua_stack::push(T val) const
{
    push(static_cast<lua_table>(val));
}
template <class T>
void lua_stack::set_global_variable(const std::string& name, T&& value) const
{
        push(value);
        lua_setglobal(m_state, name.c_str());
}
template <class T>
void lua_stack::set_table_element(const std::string& name, const T& val) const
{
    
    std::string tableName = name.substr(0, name.find_first_of('.')), tableField = name.substr(name.find_first_of('.') + 1);
    get_global_variable(tableName);//
    auto keepProcessing = true;
    while (keepProcessing)
    {
        auto dotPosition = tableField.find_first_of('.');
        if (dotPosition == std::string::npos)
        {
            push(val);
            lua_setfield(m_state, -2, tableField.c_str());
            keepProcessing = false;
        }
        else
        {
            std::string parent = tableField.substr(0, dotPosition);
            tableField = tableField.substr(dotPosition + 1);
            load_table_field(parent);
        }
    }
}
template <class T>
T lua_stack::get(int index) const
{
    using DispatchingTypes=std::tuple<detail::ClassType,detail::IntegralType,detail::FloatingPointType>;
    using Conditions=Utils::metafunction_list<std::is_class,std::is_integral,std::is_floating_point>;
    return get_impl<T>(index,typename Utils::static_type_if<T,Conditions,DispatchingTypes>::type{});
}
template<>
inline lua_table lua_stack::get<lua_table>(int index) const
{
    if (!lua_istable(m_state, index))
    {
        throw lua_error("The type you are trying to retrieve cannot be constructed with a lua_table");
    }
    return  create_lua_table();
}
template<>
inline std::string lua_stack::get<std::string>(int index) const
{
    size_t strLength = 0;
    const char* str = lua_tolstring(m_state, index, &strLength);
    std::string r{ str, strLength };
    return r;
}
template <>
inline char lua_stack::get<char>(int index) const
{
    auto str = get<std::string>(index);
    return str[0];
}
template <>
inline bool lua_stack::get<bool>(int index) const
{
    return lua_toboolean(m_state, index) != 0;
}
template <typename T>
T lua_stack::get_impl(int index,detail::ClassType) const
{
    static_assert(std::is_constructible<T, lua_table>::value, "Type cannot be constructed from a lua_table");
    return T{ get<lua_table>(index)};
}
template<typename T>
T lua_stack::get_impl(int index,detail::IntegralType) const
{
    return static_cast<T>(lua_tointeger(m_state, index));
}
template<typename T>
T lua_stack::get_impl(int index,detail::FloatingPointType) const
{
   return static_cast<T>(lua_tonumber(m_state, index));
}
//get specializations
