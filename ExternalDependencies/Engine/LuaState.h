#ifndef LUA_UTIL_HPP_INCLUDED
#define LUA_UTIL_HPP_INCLUDED

#include "lua.hpp"
#include <iostream>
#include <string>

#pragma warning(disable: 4800)
#pragma warning(disable: 4244)

using namespace std;

namespace utils
{

  class LuaState
  {

    lua_State *L;

    void endRead() {
      lua_settop(L, 0);
    }
  public:

    LuaState(const char* lua_config_filename){
		L = lua_open();
		if (luaL_loadfile(L, lua_config_filename) || lua_pcall(L, 0,0,0)) {
			cout << "error: " << lua_tostring(L,-1) << "\n";
		}
		lua_pushnil(L);
	}
    ~LuaState(){
		lua_close(L);
	}

    void open();
    void close();

    void clear_stack();

    int execute_file(const char* filename);

    int execute_string(const char * str);

    int execute_program();

    void report_errors(const int status);
    void report_error(const char * str);
    void error(const char * msg);

    template<typename T>
    T get(const char * varname) {
      static char temp[64];
      memset(temp, 0, sizeof(temp));
      int i=0;
      int j=0;
      int n=0;
      while (varname[i] != '\0') {
        char c = varname[i];
        if (c == '.') {
          if (n == 0)
            lua_getglobal(L, temp);
          else
            lua_getfield(L, -1, temp);
          ++n;
          memset(temp, 0, sizeof(temp));
          j = 0;

          if (lua_isnil(L, -1))
            return 0;
        }
        else {
          temp[j] = c;
          ++j;
        }
        ++i;
      }
      if (n == 0)
        lua_getglobal(L, temp);
      else
        lua_getfield(L, -1, temp);
      return lua_get<T>();
    }

    /*
     * Set global variables
     */
    void set(const char * varname, int val);
    void set(const char * varname, float val);
    void set(const char * varname, double val);
    void set(const char * varname, bool val);
    void set(const char * varname, const char * val);
    void set(const char * varname, const char * type, void * val);

    void call_function(const char * fname);
    void call_function_scoped(const char * scope, const char * fname);

    // Call a function with no args and return a string
    void call_function_res(const char * scope, const char * fname, std::string & res1);

    // User types (void*)
    void call_function_user(const char * fname, const char * type0, void * obj0);
    void call_function_scoped_user(const char * scope, const char * fname, const char * type0, void * obj0);

    // Primitive types (templates)

    template< typename T1 >
    void call_function_scoped(const char * scope, const char * fname, T1 param0) {
      lua_getfield(L, LUA_GLOBALSINDEX, scope);
      lua_getfield(L, -1, fname);
      push_field( param0 );
      int s = lua_pcall(L, 1, 0, 0);
      report_errors(s);
    }

    template< typename T1 >
    void call_function_scoped_res(const char * scope, const char * fname, T1 param0, std::string & res) {
      lua_getfield(L, LUA_GLOBALSINDEX, scope);
      lua_getfield(L, -1, fname);
      push_field( param0 );
      int s = lua_pcall(L, 1, 1, 0);
      if (!lua_isstring(L, -1))
        error("function must return a string");
      res = lua_tostring(L, -1);
      lua_pop(L, 1);
      report_errors(s);
    }

    template< typename T1, typename T2 >
    void call_function_scoped(const char * scope, const char * fname, T1 param0, T2 param1) {
      lua_getfield(L, LUA_GLOBALSINDEX, scope);
      lua_getfield(L, -1, fname);
      push_field( param0 );
      push_field( param1 );
      int s = lua_pcall(L, 2, 0, 0);
      report_errors(s);
    }

    template< typename T1, typename T2 >
    void call_function_scoped_res(const char * scope, const char * fname, T1 param0, T2 param1, std::string & res) {
      lua_getfield(L, LUA_GLOBALSINDEX, scope);
      lua_getfield(L, -1, fname);
      push_field( param0 );
      push_field( param1 );
      int s = lua_pcall(L, 2, 1, 0);
      if (!lua_isstring(L, -1))
        error("function must return a string");
      res = lua_tostring(L, -1);
      lua_pop(L, 1);
      report_errors(s);
    }

    template< typename T1, typename T2, typename T3>
    void call_function_scoped(const char * scope, const char * fname, T1 param0, T2 param1, T3 param2) {
      lua_getfield(L, LUA_GLOBALSINDEX, scope);
      lua_getfield(L, -1, fname);
      push_field( param0 );
      push_field( param1 );
      push_field( param2 );
      int s = lua_pcall(L, 3, 0, 0);
      report_errors(s);
    }

    template< typename T1, typename T2, typename T3, typename T4 >
    void call_function_scoped(const char * scope, const char * fname, T1 param0, T2 param1, T3 param2, T4 param3) {
      lua_getfield(L, LUA_GLOBALSINDEX, scope);
      lua_getfield(L, -1, fname);
      push_field( param0 );
      push_field( param1 );
      push_field( param2 );
      push_field( param3 );
      int s = lua_pcall(L, 4, 0, 0);
      report_errors(s);
    }

    inline operator lua_State*() {
      return L;
    }

  private:

    // Generic get
    template<typename T>
    T lua_get() {
      return 0;
    }

#if _MSC_VER
    // Specializations
    template <> float lua_get<float>() {
      return lua_tonumber(L, -1);
    }
    template <> double lua_get<double>() {
      return lua_tonumber(L, -1);
    }
    template <> bool lua_get<bool>() {
      return lua_toboolean(L, -1);
    }
    template <> int lua_get<int>() {
      return lua_tointeger(L, -1);
    }
    template <> unsigned int lua_get<unsigned int>() {
      return (unsigned int)lua_tonumber(L, -1);
    }
    template <> const char * lua_get<const char *>() {
      return lua_tostring(L, -1);
    }
#endif

    void push_field( int val ) {
      lua_pushinteger (L, val);
    }
    void push_field( float val ) {
      lua_pushnumber(L, val);
    }
    void push_field( bool val ) {
      lua_pushboolean (L, val);
    }
    void push_field( const char * val ) {
      lua_pushstring (L, val);
    }

    void get_global_or_scoped(const char * field, const char * scope = NULL);

  private:
    bool m_is_open;
  };

}  // namespace utils
#endif // LUA_UTIL_HPP_INCLUDED