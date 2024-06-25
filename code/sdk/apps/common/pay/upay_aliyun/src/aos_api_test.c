#include "aos_system.h"
#include "aos/kernel.h"
/* #include "os/os_api.h" */
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

#define TEST_SLEEP_MS 		3000

static aos_sem_t sem = 0;
static aos_queue_t queue = 0;

enum {
    SEND_SEM,
    SEND_QUEUE,
};

static void aos_timer_handler(void *hdl, void *arg)
{
    u32 cmd = (u32)arg;
    aos_printf("%s:%d, hdl: 0x%x, cmd: 0x%x", __func__, __LINE__, hdl, cmd);
    u32 msg[20];
    u32 cnt = 0;
    u32 i = 0;

    switch (cmd) {
    case SEND_SEM:
        aos_printf("%s:%d send sem", __func__, __LINE__);
        aos_sem_signal_all(&sem);
        break;

    case SEND_QUEUE:
        cnt = aos_rand() & 0xF;
        cnt = (cnt == 0) ? 1 : cnt;
        for (i = 0; i < cnt; i++) {
            msg[i] = cnt;
        }
        aos_printf("%s:%d send queue, cnt: 0x%x, size: 0x%x", __func__, __LINE__, cnt, cnt * sizeof(u32));
        aos_queue_send(&queue, msg, cnt * sizeof(u32));
        break;
    default:
        break;
    }
}

static void aos_test_task(void *priv)
{
    aos_timer_t timer = 0;
    aos_status_t ret = 0;
    aos_mutex_t mutex = 0;
    size_t size = 0;

    int msg[20];

    aos_printf("%s:%d start", __func__, __LINE__);
    aos_printf("%s:%d enter sleep %d ms, current time: %d", __func__, __LINE__, TEST_SLEEP_MS, aos_now_ms());

    //delay:
    aos_msleep(TEST_SLEEP_MS);
    /* os_time_dly(300); */
    aos_printf("%s:%d exit sleep, current time: %d", __func__, __LINE__, aos_now_ms());

    //queue:
    aos_printf("create queue: ");
    ret = aos_queue_new(&queue, NULL, 200, 10);
    if (ret == 0) {
        aos_printf("queue have created succ.");
    } else {
        aos_printf("queue create failed.");
    }

    //timer:
    aos_printf("timer add");
    aos_timer_create(&timer, aos_timer_handler, (void *)SEND_SEM, 2000, AOS_TIMER_REPEAT);

    aos_printf("timer start");
    aos_timer_start(&timer);

    //sem:
    aos_printf("sem new");
    aos_sem_new(&sem, 0);

    aos_printf("sem wait");
    ret = aos_sem_wait(&sem, 3000);
    if (ret == 0) {
        aos_printf("sem wait succ");
    } else {
        aos_printf("sem wait timeout");
    }

    //delete timer:
    aos_timer_free(&timer);

    //trigger timeout:
    aos_printf("trigger sem timeout");
    ret = aos_sem_wait(&sem, 2000);
    if (ret == 0) {
        aos_printf("sem wait succ");
    } else {
        aos_printf("sem wait timeout");
    }

    //mutex:
    aos_printf("mutex new");
    aos_mutex_new(&mutex);
    ret = aos_mutex_lock(&mutex, AOS_WAIT_FOREVER);
    if (ret == 0) {
        aos_printf("mutex lock succ.");
    } else {
        aos_printf("mutex lock failed.");
    }
    aos_mutex_unlock(&mutex);

    timer = 0;
    aos_timer_create(&timer, aos_timer_handler, (void *)SEND_QUEUE, 2000, AOS_TIMER_AUTORUN | AOS_TIMER_REPEAT);

    while (1) {
        //aos_msleep(1000);

        ret = aos_queue_recv(&queue, AOS_WAIT_FOREVER, msg, &size);
        if (ret == 0) {
            aos_printf("receive queue succ, size: 0x%x", size);
            put_buf(msg, size);
        } else {
            aos_printf("receive queue fail");
        }

        aos_printf("%s:%d", __func__, __LINE__);
    }
}

void aos_api_test(void)
{
    //task:
    aos_task_new("aos_test", aos_test_task, NULL, 0x1000);
}



#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
