#ifndef __LSH_H__
#define __LSH_H__

typedef struct _cmd_opt_map {
    const char *opt;
    void (*func)(int argc, char const *argv[]);
}cmd_opt_map_t;

#endif
