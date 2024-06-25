#ifndef __BTCTRLER_API_FOR_UPDATE_H__
#define __BTCTRLER_API_FOR_UPDATE_H__

extern u32 bt_updata_get_flag(void);
extern void bt_updata_clr_flag(u8 bit_flag);
extern u16 bt_updata_control(u8 btupdata_sw, u16 len, u8 seek_type, u32 seek_offsize);
extern struct lmp_connection *lmp_conn_for_address(u8 *address);
extern void lmp_update_init(void);
extern void bt_rf_init(void);
extern void bredr_bd_init(void);
extern void bredr_link_init(void);
extern void lmp_init(void);
extern int lmp_hci_write_scan_enable(u8 enable);
extern void btctler_nv_memory_apply(void);
extern int link_bulk_init(void);
extern void stack_run_loop_resume(void);
extern void init_soft_interrupt(void);
extern void reg_revic_buf_addr(void *addr);
extern void lmp_hci_write_local_name(const char *name);
extern void lmp_hci_write_local_address(const u8 *addr);
extern void lmp_hci_write_super_timeout(u16 timeout);
extern void bt_max_pwr_set(u8 pwr, u8 pg_pwr, u8 iq_pwr, u8 ble_pwr);
extern void hci_controller_destory(void);

#endif
