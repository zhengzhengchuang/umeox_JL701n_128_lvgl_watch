
#ifndef __WATCH_SYSCFG_MANAGE_H__
#define __WATCH_SYSCFG_MANAGE_H__

struct watch_syscfg_operataions {
    const char *name;
    int (*read)(void);
    int (*write)(void *priv);
};

#define SYSCFG_WRITE_ERASE_STATUS		((void *)-1)

#define REGISTER_WATCH_SYSCFG(node) \
    const struct watch_syscfg_operataions node sec(.watch_syscfg)

void watch_syscfg_read(const char *name);
void watch_syscfg_write(const char *name, void *arg);
void watch_syscfg_read_all();
void watch_syscfg_write_all(void *p);

#endif

