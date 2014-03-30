#include "lua.h"
#include "lauxlib.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/file.h>
#include <string.h> // strerror
#include <errno.h>

static int l_dolock(lua_State* L) {
    int top = lua_gettop(L);
    const char* path = luaL_checkstring(L,1);
    size_t len = 0;
    const char* mode = luaL_checklstring(L,2,&len);
    struct flock fl;
    int fd;

    fl.l_type = *mode == 'r' ? F_RDLCK : F_WRLCK;
    fl.l_whence = SEEK_SET;
    if(top == 3) {
        fl.l_start = 0;
        fl.l_len = 0;
    } else if(top == 5) {
        fl.l_start = luaL_checkinteger(L,3);
        fl.l_len = luaL_checkinteger(L,4);
    } else {
        luaL_error(L,"Arguments are either path,mode,handler or path,mode,start,length,handler");
    }

    fd = open(path, O_WRONLY|O_TRUNC|O_CREAT, 0700);
    if(fd < 0) {
        luaL_error(L,"Couldn't open %s: %s",path,strerror(errno));
    }
    for(;;) {
        if(fcntl(fd,F_SETLKW,&fl)==0) {
            break;
        } else {
            if(errno == EINTR || errno == EAGAIN) {
                puts("Lock timed out, or got signalled or something, trying again in 1");
                lua_gc(L,LUA_GCCOLLECT,0); // may as well
                sleep(1);
            } else {
                close(fd);
                luaL_error(L,"File would not lock %s %s",path,strerror(errno));
            }
        }
    }

    // last argument is on top (i.e. the handler)

    int result = lua_pcall(L, 0, 0, 0);

    unlink(path);
    close(fd);

    if(result != 0) {
        // just pass the message on up
        lua_error(L);
    }
}

int luaopen_lock_dolock(lua_State* L) {
    lua_pushcfunction(L,l_dolock);
    return 1;
}
