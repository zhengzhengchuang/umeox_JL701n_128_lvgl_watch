#ifndef __IMB_H__
#define __IMB_H__

#include "generic/typedef.h"
#include "generic/list.h"
#include "generic/rect.h"
#include "fs/fs.h"


//图层格式定义
enum LAYER_FORMAT {
	/* 0 */LAYER_FORMAT_ARGB8888,
	/* 1 */LAYER_FORMAT_RGB888,
	/* 2 */LAYER_FORMAT_RGB565,
	/* 3 */LAYER_FORMAT_L8,
	/* 4 */LAYER_FORMAT_AL88,
	/* 5 */LAYER_FORMAT_AL44,
	/* 6 */LAYER_FORMAT_A8,
	/* 7 */LAYER_FORMAT_L1,
	/* 8 */LAYER_FORMAT_ARGB8565,
	/* 9 */LAYER_FORMAT_OSD16,
	/* 10 */LAYER_FORMAT_SOLID,
	/* 11 */LAYER_FORMAT_JPEG,
};

enum OUTPUT_FORMAT {
	/* 0 */OUTPUT_FORMAT_RGB888,
	/* 1 */OUTPUT_FORMAT_RGB565,
};

enum DATA_SRC {
	DATA_SRC_FLASH,
	DATA_SRC_SRAM,
	DATA_SRC_PSRAM,
	DATA_SRC_NONE,
};

enum DATA_TYPE {
	DATA_NOZIP,
	DATA_ZIP,
};

struct imb_file_info {
	u32 *tab;
	u16 tab_size;
	u32 offset;
	u32 last_tab_data_len;
};

//imb输出配置
struct imb_output {
	int width;
	int height;
	int stride;
	enum OUTPUT_FORMAT format;
	u8 *buf_addr;
};

struct imb_task_param {
	struct imb_output *output;
};

#define RING_MAX_TASK   20

#define rect_left(r)        ((r)->left)
#define rect_top(r)         ((r)->top)
#define rect_right(r)       ((r)->left + (r)->width)
#define rect_bottom(r)      ((r)->top + (r)->height)

//任务块结构(与硬件对应，禁止修改该结构体)
struct imb_task {
	u32 src_w : 10;
	u32 src_h : 9;
	u32 des_w : 10;
	u32 cur_osd0_en : 1;
	u32 rgb_tsk_en : 1;
	u32 zip_en : 1;

	s32 x_offset : 11;
	s32 y_offset : 10;
	u32 des_h : 9;
	u32 scale_en : 1;
	u32 nxt_wait_en : 1;

	u32 rotate_angle : 9;
	u32 rotate_c_x : 10;
	u32 rotate_c_y : 9;
	u32 cur_format : 4;

	u32 adr_map_adr;
	u32 dat_src_adr;
	u32 nxt_tsk_adr;

	u32 b_from_tsk : 8;
	u32 g_from_tsk : 8;
	u32 r_from_tsk : 8;
	u32 a_from_tsk : 8;

	u32 rotate_dx : 13;
	u32 rotate_dy : 13;
	u32 rotate_en : 1;
	u32 osd0_in_buf : 1;
	u32 cur_in_flash : 1;
	u32 cur_tsk_valid : 1;
	u32 reserved1 : 2;

	// u32 resize_des_w : 10;
	// u32 resize_des_h : 9;
	// u32 osd16_alpha : 8;
	// u32 reserved0 : 5;

	u32 src_w_pre : 10;
	u32 src_h_pre : 9;
	u32 reserved2 : 13;

	u32 x_src_offset : 10;
	u32 y_src_offset : 9;
	u32 reserved3 : 13;

	struct list_head entry;
	struct list_head entry_h;
	struct rect rect;
	struct rect crop;
	u8 jpeg_en : 1;
	u8 task_en : 1;
	u8 rotate_chunk : 1;
	u8 user_cb : 1;
	u8 ui_malloc : 1;
	u8 root_index : 1;
	u8 quote : 2;
	u16 adr_map_tab_size;
	u32 id;
	u32 elm_id;
	u32 *lut_tab;
	u32 resfile_addr;
	int index;
	char name[8];
	char *filename;
	// struct imb_file_info *flash_file_info;
	void *flash_file_info;
	enum DATA_SRC data_src;
	void *(*cb)(void *priv);
	void *cb_priv;
	int cb_id;
	u32 priv_buf[22];
	u32 crc;
};

enum {
	STATE_SYNTHESIS_AND_DRAW, //合成并推屏显示
	STATE_SYNTHESIS, //合成
	STATE_DRAW, //推屏
};

struct imb_task_head {
	struct list_head entry;
	struct list_head entry_h;
	struct imb_task *head;
	struct rect rect_bg;
	struct rect rect;
	struct rect canv;
	struct rect rect_ref;
	struct rect curr_draw;
	u8 *dispbuf;
	u32 buflen;
	u16 lines;
	u8 bufnum;
	u16 screen_width;
	u16 screen_height;
	u16 buf_stride;
	int task_num;
	int index;
	u8 state;
	u8 format;
	u8 active;
	struct imb_output output;
	u8 *buf_addr;

	u8 *ring_buf[2];
	struct imb_task *task_tab[RING_MAX_TASK];
	struct imb_task *curr_task;
};

struct imb_task_out {
	u8 *outbuf;
	struct rect rect;
	struct rect root_rect;
	struct imb_task *task;
	u8 format;
	void *priv;
};

enum {
	L1_TEXT,
	L1_IMAGE,
};

struct imb_task_info {
	//输入图像
	u16 src_w;//输入宽高，没有坐标
	u16 src_h;

	//输出图像
	s16 x_offset;
	s16 y_offset;
	u16 des_w;
	u16 des_h;

	u8 cur_osd0_en: 1;
	u8 rgb_tsk_en: 1;
	u8 zip_en: 1;
	u8 scale_en: 1;
	u8 next_wait_en: 1;
	u8 rotate_en: 1;
	u8 osd0_in_buf: 1;
	u8 cur_in_flash: 1;//原始数据放置位置
	u8 task_invisible;
	u32 rgb_task_color;

	//旋转相关配置
	u16 rotate_c_x;
	u16 rotate_c_y;
	u16 rotate_angle;
	u16 rotate_dx;
	u16 rotate_dy;
	u16 rotate_chunk : 15;
	u16 rotate_kick : 1;
	u16 rotate_chunk_num;
	u16 rotate_chunk_offset;
	u16 rotate_chunk_offset_cur;
	u8 *rotate_buffer;
	u8 *rotate_buffer_0;
	u8 *rotate_buffer_1;
	u32 x_src_offset;
	u32 y_src_offset;

	//缩放相关配置
	// u16 resize_des_w;
	// u16 resize_des_h;
	float ratio_w;
	float ratio_h;

	u8 lcd_format;
	u8 quote;
	u8 user_cb;
	u8 l1_type;
	u8 ui_malloc;
	u16 lcd_width;
	u16 lcd_height;
	u32 id;
	u32 elm_id;
	u32 priv;
	struct imb_task *task;
	struct rect crop;
	struct rect rect;
	struct rect draw;
	int text_color;
	void *ff_info;// struct flash_file_info *ff_info;
	enum LAYER_FORMAT in_format;
	enum OUTPUT_FORMAT out_format;
	enum DATA_SRC data_src;
	void *(*cb)(void *cb_priv);
	void *cb_priv;
	int cb_id;
};


void imb_task_enable(struct imb_task *task, int en);
// u8 *get_image_data(struct imb_task *task, struct image_file *f, struct imb_task_info tsk_info);
void imb_task_list_init(struct imb_task_head *root, struct imb_task_info *info);
struct imb_task *imb_task_list_add(struct imb_task_head *root, struct imb_task_info *info);
void imb_start(struct imb_task_head *root, struct rect *limit, struct rect disp, u16 colums, u16 lines);
void imb_task_all_destroy(struct imb_task_head *root);
struct imb_task *imb_task_search(struct imb_task_head *root, char *task_name);
struct imb_task *imb_task_search_by_id(struct imb_task_head *root, u32 elm_id, u8 index);
void imb_task_reset(struct imb_task_head *root, struct imb_task *p_task, struct imb_task_info *info);
int get_elm_task_num(struct imb_task_head *root, u32 elm_id);
int imb_task_delete_by_id(struct imb_task_head *root, u32 elm_id, u32 id);
void set_imb_output_buffer(struct imb_task_head *root, void *buffer);
u8 *get_imb_output_buf(struct imb_task_head *root);
int imb_task_delete_invalid(struct imb_task_head *root, u32 elm_id, u8 elm_index);
struct imb_task_head *imb_task_head_get(int idx);
void imb_task_head_config(struct imb_task_head *root, struct rect *rect, struct rect *canv, struct rect *rect_ref, u8 state);
void imb_task_head_set_buf(struct imb_task_head *root, u8 *buf, u32 bufsize, u16 width, u16 height, u16 stride, u16 lines, u8 buf_num);
int imb_task_head_check(struct imb_task_head *root);
struct imb_task *imb_task_enable_by_id(struct imb_task_head *root, u32 elm_id, u8 en);
int imb_task_delete_by_elm(struct imb_task_head *root, u32 elm_id);

void imb_set_memory_func(void *(*imb_malloc)(int size), void (*imb_free)(void *p));

void *imb_malloc(int size, int clr);
void imb_free(void *p);

void imb_task_lut_tab_release(struct imb_task *task);
void imb_task_file_info_release(struct imb_task *task);
void imb_task_src_adr_release(struct imb_task *task);

void imb_meld_buf_release(void);

void imb_task_protect(int enable);

#endif

