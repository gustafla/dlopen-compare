#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>

#define LIB_ERR() {fprintf(stderr, "%s\n", dlerror()); return 1;}

int main() {
    while (1) {
        void *lib = dlopen("./libsomething.so", RTLD_LAZY);

        if (!lib)
            LIB_ERR();

        void (*function)(void) = dlsym(lib, "call_me");

        if (!function)
            LIB_ERR();

        function();

        if (dlclose(lib))
            LIB_ERR();

        sleep(1);
    }

    return 0;
}
