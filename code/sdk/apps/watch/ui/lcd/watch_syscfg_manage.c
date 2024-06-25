#include "system/includes.h"
#include "ui/watch_syscfg_manage.h"

extern const struct watch_syscfg_operataions watch_syscfg_begin[];
extern const struct watch_syscfg_operataions watch_syscfg_end[];

#define list_for_each_watch_syscfg(p) \
    for (p = watch_syscfg_begin; p < watch_syscfg_end; p++)


void watch_syscfg_read(const char *name)
{
    int ret = 0;
    const struct watch_syscfg_operataions *p;
    list_for_each_watch_syscfg(p) {
        if (!ASCII_StrCmp(p->name, name, -1)) {
            if (p->read) {
                ret = p->read();
            }
            if (ret < 0) {
                log_e("watch_syscfg_read err\n");
            }
            return;
        }
    }
}

void watch_syscfg_write(const char *name, void *arg)
{
    int ret = 0;
    const struct watch_syscfg_operataions *p;
    list_for_each_watch_syscfg(p) {
        if (!ASCII_StrCmp(p->name, name, -1)) {
            if (p->write) {
                ret = p->write(arg);
            }
            if (ret < 0) {
                log_e("watch_syscfg_write err\n");
            }
            return;
        }
    }
}

void watch_syscfg_read_all()
{
    int ret = 0;
    const struct watch_syscfg_operataions *p;
    list_for_each_watch_syscfg(p) {
        /* g_printf("%s %s",__FUNCTION__,p->name); */
        if (p->read) {
            ret = p->read();
        }
        if (ret < 0) {
            log_e("watch_syscfg_read err:%s\n", p->name);
        }
    }
}

void watch_syscfg_write_all(void *priv)
{
    int ret = 0;
    const struct watch_syscfg_operataions *p;
    list_for_each_watch_syscfg(p) {
        /* g_printf("%s %s",__FUNCTION__,p->name); */
        if (p->write) {
            ret = p->write(priv);
        }
        if (ret < 0) {
            log_e("watch_syscfg_write err:%s\n", p->name);
        }
    }
}

