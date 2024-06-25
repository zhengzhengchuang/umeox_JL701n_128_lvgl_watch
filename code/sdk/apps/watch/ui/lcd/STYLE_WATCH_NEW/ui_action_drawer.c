#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "res/resfile.h"
#include "ui/res_config.h"
#include "ui/ui_resource.h"
#include "ui/ui_sys_param.h"
#include "asm/imb.h"
#include "ui/ui_effect.h"




#if TCFG_PSRAM_UI_EFFECT

#define STYLE_NAME  JL



/* static LIST_HEAD(drawer_head) */
static struct drawer_ctl drawer = {0};

static u32 drawer_page_list[] = {ID_WINDOW_SET, ID_WINDOW_SLEEP, ID_WINDOW_CALCULATOR, ID_WINDOW_MESS, ID_WINDOW_CALCULAGRAPH, DIAL_PAGE_0, ID_WINDOW_TRAIN, ID_WINDOW_MUSIC, ID_WINDOW_WEATHER, ID_WINDOW_HEART, ID_WINDOW_COMPASS};


int drawer_add(int page_id, struct list_head *prev, struct list_head *next);
int drawer_add_tail(int page_id);
int drawer_del(int page_id);
int drawer_item_switch(struct drawer_item *node_prev, struct drawer_item *node_next);
bool drawer_end_item(struct drawer_item *node);
extern int ui_drawer_effect_post(struct drawer_ctl *drawer);

extern void free_psram(void *pv);
extern void put_float(double fv);

#define SCR_W	454
#define SCR_H	454


static int drawer_init()
{
    int num, i;
    struct drawer_item *p;
    int scr_w = SCR_W;
    int scr_h = SCR_H;
    int img_h;

    printf("drawer init\n");

    if (drawer.init == 0) {
        INIT_LIST_HEAD(&drawer.head);
        num = sizeof(drawer_page_list) / sizeof(drawer_page_list[0]);
        printf("orig num %d\n", num);
        for (i = 0; i < num; i++) {
            drawer_add_tail(drawer_page_list[i]);
        }

        i = 0;
        list_for_each_entry(p, &drawer.head, entry) {
            if (i == 0) {
                drawer.first_ditem = p;
            }
            printf("%x, %x, %d\n", (u32)p, p->page_id, (u32)p->index);
            i++;
        }

        drawer.init = 1;
    }

    float min_sca = 0.6f;
    float max_sca = 0.9f;
    float limit_sca = 0.5f;
    int last_h, last_mid_top;
    float tmp_rat;

    drawer.ratio = 0.6;
    img_h = scr_h * drawer.ratio;

    drawer.max_cy_limit = scr_h + (img_h * max_sca) / 2;
    drawer.min_cy_limit = 30 + (img_h * min_sca) / 2;

    i = 0;
    tmp_rat = 0;
    last_h = 0;
    last_mid_top = drawer.min_cy_limit;
    p = drawer.first_ditem;
    do {
        p->ccoor.top = last_mid_top + last_h * tmp_rat;
        p->sca = min_sca + (float)((p->ccoor.top - drawer.min_cy_limit) * (max_sca - min_sca) / (float)(drawer.max_cy_limit - drawer.min_cy_limit));

        p->sca += 0.1;


        p->sca = (p->sca < limit_sca) ? limit_sca :  p->sca;
        p->ccoor.top = (p->ccoor.top < drawer.min_cy_limit) ? drawer.min_cy_limit : p->ccoor.top;

        p->ccoor.left = (scr_w - scr_w * p->sca) / 2;
        p->ccoor.width = scr_w * p->sca;
        p->ccoor.height = img_h * p->sca;

        last_h = p->ccoor.height;
        last_mid_top = p->ccoor.top;
        tmp_rat = (i == 1) ? 1.0 : 0.7;

        if (p->entry.next != (&drawer.head)) {
            p = list_entry(p->entry.next, struct drawer_item, entry);
        } else {
            p = NULL;
        }

    } while (p && (++i < 3));

    drawer.has_new_item = 1;

    return 0;
}

static int drawer_move(struct drawer_ctl *drawer, int diff_x, int diff_y)
{
    int i;
    struct drawer_item *p;
    int scr_w = SCR_W;
    int scr_h = SCR_H;
    int img_h = scr_h * drawer->ratio;
    float min_sca = 0.6f;
    float max_sca = 0.9f;
    float limit_sca = 0.5f;
    int last_h, last_mid_top;
    float tmp_rat;
    int first_mid_top;

    printf("\n\ndiff_x %d, diff_y %d\n\n", diff_x, diff_y);

    i = 0;
    tmp_rat = 0;
    last_h = 0;
    p = drawer->first_ditem;
    first_mid_top = p->ccoor.top;
    /* last_mid_top = drawer->min_cy_limit; */
    last_mid_top = p->ccoor.top + diff_y;
    do {
        p->ccoor.top = last_mid_top + last_h * tmp_rat;
        p->sca = min_sca + (float)((p->ccoor.top - drawer->min_cy_limit) * (max_sca - min_sca) / (float)(drawer->max_cy_limit - drawer->min_cy_limit));

        p->sca += 0.1;

        p->sca = (p->sca < limit_sca) ? limit_sca :  p->sca;
        p->ccoor.top = (p->ccoor.top < drawer->min_cy_limit) ? drawer->min_cy_limit : p->ccoor.top;

        p->ccoor.left = (scr_w - scr_w * p->sca) / 2;
        p->ccoor.width = scr_w * p->sca;
        p->ccoor.height = img_h * p->sca;

        last_h = p->ccoor.height;
        last_mid_top = p->ccoor.top;
        tmp_rat = (i == 1) ? 1.0 : 0.7;

        if (p->entry.next != (&drawer->head)) {
            p = list_entry(p->entry.next, struct drawer_item, entry);
        } else {
            p = NULL;
        }

    } while (p && (++i < 3));

    //从上往下滑动,第一项在没出现新项时中心坐标不动
    p = drawer->first_ditem;
    if ((diff_y > 0) && (p == drawer->first_ditem)) {
        p->ccoor.top = first_mid_top;
    }


    drawer->has_new_item = 0;
    ui_drawer_effect_post(drawer);
    return 0;
}



int drawer_add(int page_id, struct list_head *prev, struct list_head *next)
{
    struct drawer_item *item;
    struct drawer_item *p;
    int i;

    item = zalloc(sizeof(struct drawer_item));
    if (item == NULL) {
        printf("item malloc_err\n");
        return -1;
    }

    item->page_id = page_id;
    __list_add(&item->entry, prev, next);

    i = 0;
    list_for_each_entry(p, &drawer.head, entry) {
        p->index = i++;
    }
    return 0;
}

int drawer_add_tail(int page_id)
{
    int i;
    struct drawer_item *p;
    struct drawer_item *item = zalloc(sizeof(struct drawer_item));
    if (item == NULL) {
        printf("item malloc_err\n");
        return -1;
    }
    item->page_id = page_id;
    list_add_tail(&item->entry, &drawer.head);

    i = 0;
    list_for_each_entry(p, &drawer.head, entry) {
        p->index = i++;
    }
    return 0;
}

int drawer_del(int page_id)
{
    int i;
    struct drawer_item *p, *n;
    int ret = -1;

    list_for_each_entry_safe(p, n, &drawer.head, entry) {
        if (page_id == p->page_id) {
            list_del(&p->entry);
            free(p);
            ret = 0;
            break;
        }
    }

    if (ret == 0) {
        i = 0;
        list_for_each_entry(p, &drawer.head, entry) {
            p->index = i++;
        }
    }

    return ret;
}
int drawer_item_switch(struct drawer_item *node_prev, struct drawer_item *node_next)
{
    int i;
    struct drawer_item *p;

    node_prev->entry.prev->next = &node_next->entry;
    node_next->entry.next->prev = &node_prev->entry;

    node_prev->entry.next = node_next->entry.next;
    node_next->entry.prev = node_prev->entry.prev;

    node_prev->entry.prev = &node_next->entry;
    node_next->entry.next = &node_prev->entry;

    i = 0;
    list_for_each_entry(p, &drawer.head, entry) {
        p->index = i++;
    }
    return 0;
}


bool drawer_end_item(struct drawer_item *node)
{
    if ((!list_empty(&drawer.head)) && (drawer.head.prev == &node->entry)) {
        return true;
    }

    return false;
}

bool drawer_first_item(struct drawer_item *node)
{
    if ((!list_empty(&drawer.head)) && (node == list_first_entry(&drawer.head, struct drawer_item, entry))) {
        return true;
    }
    return false;
}



static int drawer_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct layout *layout = (struct layout *)_ctrl;
    struct element *layout_elm = &layout->elm;
    struct element *p;
    struct rect rect;

    switch (event) {
    case ON_CHANGE_INIT:
        /* key_ui_takeover(1); */
        ui_window_effect(layout_elm->parent->id, EFFECT_MODE_DRAWER, NULL, NULL);
        drawer_init();
        ui_drawer_effect_post(&drawer);
        break;
    case ON_CHANGE_RELEASE:
        /* key_ui_takeover(0); */
        if (drawer.buf[0]) {
            free_psram(drawer.buf[0]);
            drawer.buf[0] = NULL;
        }
        if (drawer.buf[1]) {
            free_psram(drawer.buf[1]);
            drawer.buf[1] = NULL;
        }
        if (drawer.buf[2]) {
            free_psram(drawer.buf[2]);
            drawer.buf[2] = NULL;
        }

        break;
    default:
        return false;
    }
    return false;
}

static int drawer_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct layout *layout = (struct layout *)ctrl;
    struct element *p;
    struct rect rect;
    static struct position pos = {0};
    int diff_x, diff_y;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:

        pos.x = e->pos.x;
        pos.y = e->pos.y;

        break;
    case ELM_EVENT_TOUCH_UP:
        printf("drawer touch up \n");
        break;
    case ELM_EVENT_TOUCH_MOVE:

        diff_x = e->pos.x - pos.x;
        diff_y = e->pos.y - pos.y;

        drawer_move(&drawer, diff_x, diff_y);

        break;
    case ELM_EVENT_TOUCH_L_MOVE:
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        break;
    default:
        break;
    }
    return true;
}


static int drawer_onkey(void *ctr, struct element_key_event *e)
{
    struct layout *layout = (struct layout *)ctr;

    switch (e->value) {
    case KEY_OK:
        break;
    case KEY_UI_MINUS:
    case KEY_UI_HOME:
        break;
    case KEY_UI_PLUS:
    case KEY_UI_SHORTCUT:
        break;
    default:
        return false;
    }
    return false;

}

REGISTER_UI_EVENT_HANDLER(LAYOUT_DRAWER)
.onchange = drawer_onchange,
 .onkey = drawer_onkey,
  .ontouch = drawer_ontouch,
};



#endif





