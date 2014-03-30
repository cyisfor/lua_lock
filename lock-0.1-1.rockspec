-- vim: set syntax=lua:
package="lock"
version="0.1-1"
source = {
    url="git@github.com:cyisfor/lua_lock.git"
}
description = {
   summary = "Ways to lock so that processes can take turns nicely.",
   homepage = "http://github.com/cyisfor/lua_fancyrequire/",   
   detailed = [[dolock

dolock exports a function that takes a path for locking, a mode and a handler
The mode can either be 'r' or not 'r', the default being a write (exclusive) lock, 
and 'r' making it a read lock. The handler takes no arguments and returns nothing, 
but while in that handler, the file is most certainly definitely locked, and no 
other process has locked it.

dolock requires the fnctl call and F_SETLKW. If you're on Microsoft then there
is no way you can ever wait for a lock, so there's no point in calling dolock,
and having dolock be a noop seems disingenuous.

... network lock? shared memory lock? hardware locking? could add stuff here
]]
}
dependencies = {
   "lua >= 5.1",
}

build = {
   type="builtin",
   modules = {
       ["lock.dolock"] = {
           sources = {"dolock.c"}
       }
   }
}
