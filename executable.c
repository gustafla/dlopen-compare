#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <SDL.h>
#include <sys/wait.h>

#define LIB_ERR() {fprintf(stderr, "%s\n", dlerror()); exit(1);}
#define CP_ERR() {fprintf(stderr, "Copying temporary file failed\n"); exit(1);}

const char *LIB_PATH = "./libsomething.so";
const char *TMP_TEMPLATE = "/tmp/XXXXXX.so";
char *name;

void *load_lib() {
    name = malloc(strlen(TMP_TEMPLATE)+1);
    strcpy(name, TMP_TEMPLATE);
    if (close(mkstemps(name, 3))) {
        fprintf(stderr, "Calling close on mkstemp fd failed\n");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0) {
        execl("/bin/cp", "/bin/cp", LIB_PATH, name, NULL);
    } else if (pid < 0) {
        CP_ERR();
    } else {
        int status;
        wait(&status);
    }

    void *lib = dlopen(name, RTLD_NOW);

    if (!lib) {
        LIB_ERR();
    }

    return lib;
}

void close_lib(void *lib) {
    if (dlclose(lib))
        LIB_ERR();

    if (unlink(name)) {
        fprintf(stderr, "unlink errno: %d\n", errno);
        exit(1);
    }

    free(name);
}

void (*load_func(void *lib))(void) {
    void (*function)(void) = dlsym(lib, "call_me");
    if (!function)
        LIB_ERR();
    return function;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("-", 0, 0, 200, 200, SDL_WINDOW_SHOWN);
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, 0);
    SDL_Event e;
    int running = 1;

    void *lib = load_lib();
    void (*function)(void) = load_func(lib);

    while (running) {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) {
            break;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_q:
                case SDLK_ESCAPE: running = 0; break;
                case SDLK_r:
                                  close_lib(lib);
                                  lib = load_lib();
                                  function = load_func(lib);
            }
        }

        function();

        SDL_RenderPresent(rend);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    close_lib(lib);

    return 0;
}
