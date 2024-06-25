#include "system/init.h"
//#include "system/includes.h"
#include "fs/fs.h"
#include "irq.h"
#include "version.h"
#include "jiffies.h"
//#include "boot.h"
//#include "asm/adc_api.h"
#include "device.h"
#include "asm/wdt.h"
#include "asm/debug.h"
#include "debug.h"
volatile unsigned long jiffies = 10;

BOOT_INFO boot_info AT(.boot_info);

extern void setup_arch();
extern void app_core_init();
extern void app_core_msg_handler(int *);
extern void mkey_dummy_func();

static void do_initcall()
{
	__do_initcall(initcall);
}

static void do_early_initcall()
{
	__do_initcall(early_initcall);
}

static void do_late_initcall()
{
	__do_initcall(late_initcall);
}

static void do_platform_initcall()
{
	__do_initcall(platform_initcall);
}

static void do_module_initcall()
{
	__do_initcall(module_initcall);
}

void __attribute__((weak)) app_version_check()
{

}

void __attribute__((weak)) board_init()
{

}
void __attribute__((weak)) board_early_init()
{
#if 0       //test code
	local_irq_disable();
	wdt_close();
	/* q32DSP(core_num())->PMU_CON1 &= ~BIT(8); */
	/* q32DSP(core_num())->PMU_CON1 |= BIT(8); */
	int coremark_main(void);
	coremark_main();
#endif
}

_NOINLINE_
__BANK_INIT_ENTRY
void app_task_init()
{
	app_version_check();
	do_early_initcall();
	do_platform_initcall();

	board_init();

	do_initcall();

	do_module_initcall();
	do_late_initcall();

	app_core_init();
}

static void app_task_handler(void *p)
{
	int res;
	int msg[32];

	app_task_init();

	while (1) {
		res = os_taskq_pend("taskq", msg, ARRAY_SIZE(msg));
		if (res != OS_TASKQ) {
			continue;
		}
		app_core_msg_handler(msg);
	}
}

#ifdef CONFIG_CPU_BR22
void *boot_info_init_pre(void *_info)
{
#if (USE_SDFILE_NEW == 1)
	BOOT_DEVICE_INFO *info = (BOOT_DEVICE_INFO *)(IRQ_MEM_ADDR + 8);
	memcpy(info, _info, sizeof(BOOT_DEVICE_INFO));
	struct flash_head *fs_info = (struct flash_head *)(info + 1);
	memcpy(fs_info, info->fs_info, sizeof(struct flash_head));
	info->fs_info = fs_info;

#else
	BOOT_INFO *info = (BOOT_INFO *)(IRQ_MEM_ADDR + 8);
	memcpy(info, _info, sizeof(BOOT_INFO));
#endif

	return (void *)info;
}
#endif

void printf_boot_info()
{
	put_buf((void *) &boot_info, sizeof(boot_info));
}

_WEAK_
void boot_init_hook(void *_info)
{
}
void boot_info_init(void *_info)
{
	boot_init_hook(_info);

#if (USE_SDFILE_NEW == 1)
	BOOT_DEVICE_INFO *info = (BOOT_DEVICE_INFO *)_info;
	memcpy(&(boot_info.sfc), &(info->sfc), sizeof(struct sfc_info));
	boot_info.flash_size = info->fs_info->FlashSize;
	boot_info.vm.align = info->fs_info->align;
	boot_info.chip_id = info->chip_id;
	boot_info.trim_value = info->trim_value;
	memcpy(&(boot_info.mac), info->bt_mac_addr, sizeof(boot_info.mac));
#else
	BOOT_INFO *info = (BOOT_INFO *)_info;
	memcpy(&boot_info, info, sizeof(BOOT_INFO));
#endif
	mkey_dummy_func();
}


u32 boot_info_get_sfc_base_addr(void)
{
	return boot_info.sfc.sfc_base_addr;
}

#if (OS_CPU_NUM > 1)

extern void cpu_sfc_critical_handle(void);

extern void exception_irq_handler(void);

void __attribute__((weak)) cpu1_setup_arch()
{
	request_irq(IRQ_EXCEPTION_IDX, 7, exception_irq_handler, 1);

	request_irq(IRQ_SOFT1_IDX, 6, cpu_sfc_critical_handle, 1);

	// request_irq(0, 7, the_debug_isr, 1);

	debug_init();
}

void cpu1_main()
{
	extern void cpu1_run_notify(void);
	cpu1_run_notify();
//	while(1) ;
//	puts("cpu1 start -----------------------------------") ;
#if 0
	while (1) {//单核死循环操作, 不可用操作系统接口
		__asm__ volatile("idle");
	}
#endif

	interrupt_init();

	cpu1_setup_arch();

	os_start();

	log_e("os err \r\n") ;

	while (1) {
		__asm__ volatile("idle");
	}
}
#else
void cpu1_main()
{
}
#endif

__attribute__((weak))
int main()
{
	wdt_close();

	os_init();

	setup_arch();

	board_early_init();

	task_create(app_task_handler, NULL, "app_core");

	os_start();

	log_e("os err \r\n") ;

	while (1) {
		asm("idle");
	}

	return 0;
}





