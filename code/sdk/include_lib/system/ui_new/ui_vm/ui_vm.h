#ifndef __UI_F_VM_H__
#define __UI_F_VM_H__

#include "includes.h"

#define F_TYPE_BASE                 (1)

#define F_TYPE_PHONEBOOK            (F_TYPE_BASE)
#define F_TYPE_SPORTRECORD          (F_TYPE_BASE+1)
#define F_TYPE_HEART                (F_TYPE_BASE+2)
#define F_TYPE_BLOOD_OXYGEN         (F_TYPE_BASE+3)
#define F_TYPE_SLEEP                (F_TYPE_BASE+4)
#define F_TYPE_MESSAGE              (F_TYPE_BASE+5)
#define F_TYPE_WEATHER              (F_TYPE_BASE+6)
#define F_TYPE_CALL_LOG				(F_TYPE_BASE+7)
#define F_TYPE_STEP					(F_TYPE_BASE+8)
#define F_TYPE_HEART_SINGLE			(F_TYPE_BASE+9)
#define F_TYPE_BLOOD_OXYGEN_SINGLE  (F_TYPE_BASE+10)

#define F_TYPE_MAX                  (F_TYPE_BLOOD_OXYGEN_SINGLE)
#define F_TYPE_COUNT                (F_TYPE_MAX +1- F_TYPE_BASE)

#define F_TYPE_MASK    62//(已经固定)

extern int flash_common_init(void **handle, const u8 *name, u16 type, u16 max, int start, int end);
extern int flash_common_reset(void **handle, const u8 *name, u16 type, u16 max, int start, int end);
extern int flash_common_write_file(void *handle, int id, int len, u8 *data);
extern int flash_common_read_by_id(void *handle, int id, int offset, int len, u8 *data);
extern int flash_common_read_by_index(void *handle, int index, int offset, int len, u8 *data);
extern int flash_common_get_id_table(void *handle, int len, u8 *data);
extern int flash_common_get_total(void *handle);
extern int flash_common_delete_by_id(void *handle, int id);
extern int flash_common_delete_by_index(void *handle, int index);
extern int flash_common_get_file_size_by_index(void *handle, int index);
extern int flash_common_get_file_size_by_id(void *handle, int id);
extern int flash_common_file_defrag(void *handle, int len);

extern int flash_common_open_id(void *handle, int id, int len);

extern int flash_common_write_packet(void *handle, int id, int len, u8 *data);

extern int flash_common_write_push(void *priv, int id);

extern int flash_common_update_by_id(void *handle, int id, int offset, int len, u8 *data);









#endif
