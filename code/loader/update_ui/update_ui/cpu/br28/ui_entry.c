#include "typedef.h"
#include "app_config.h"
#include "generic/gpio.h"
//#include "asm/led.h"
//#include "asm/spi.h"
#include "ui/lcd_spi/lcd_drive.h"
#include "update_status.h"
struct _user_api_param_t {
    u8 version;         //版本号：0
    u8 progress_bar;    //等待中的循环进度，可用于ui显示
    u8 addr[6];         //蓝牙地址
} user_api_param;


typedef int (*__print)(const char *format, ...) ;
static __print _print;

typedef void (*__request_irq)(u8 index, u8 priority, void (*handler)(void), u8 cpu_id);
extern void draw_ring(int center_x, int center_y, int radius, u16 color, int percent);
static __request_irq _request_irq;

ex_api_t *p_ex_api = NULL;

static void (*my_irq_enable)(void) = NULL;
static void (*my_irq_disable)(void) = NULL;
static void (*my_mdelay)(u32) = NULL;

void my_irq_enable_func(void)
{
    if (my_irq_enable) {
        my_irq_enable();
    }
}

void my_irq_disable_func(void)
{
    if (my_irq_disable) {
        my_irq_disable();
    }
}

typedef struct _user_api_t {
	char magic[8];
	void (*ex_api_reg_hdl)(ex_api_t *api);
	void (*ui_disp_hdl)(UPDATA_STATUS st, u32 param);
} user_api_t;


void request_irq(u8 index, u8 priority, void (*handler)(void), u8 cpu_id)
{
	_request_irq(index,priority,handler,cpu_id);
}



#if 0
int printf(const char *format, ...)
{
	int ret  = 0;


	// va_list args;
	// va_start(args, format);
	//if(_print)
	//ret = _print(format,args);
	return ret;
}

void log_print(int level, const char *tag, const char *format, ...)
{
	int ret = 0;
	// va_list args;
	//va_start(args, format);
	//if(_print)
	// ret = _print(format, args);
	return ;
}
#endif


void _isatty() {}

void delay_2ms(int cnt)
{
	int count;
	for (int i = 0; i < cnt; i++) {
        if (my_mdelay) {
            my_mdelay(2);
        } else {
            count = 110 * 50;
            while (count--) {
                __asm__ volatile("nop");
                __asm__ volatile("nop");
                __asm__ volatile("nop");
            }
        }
	}

}


static void delay_nop(int cnt)
{
	while (cnt--) {
		__asm__ volatile("nop");
	}
}

static void ui_flash_handler(void)
{
	if (p_ex_api && p_ex_api->printf) {
		p_ex_api->printf("@");
	}

	JL_PORTA->DIR &= ~BIT(6);
	JL_PORTA->OUT ^= BIT(6);
}


extern int _bss_begin[];
extern int _bss_size[];
void spi_regsiter_dump(void);


struct ui_display {
	struct lcd_interface *lcd;
};
static struct ui_display display = {0};
#define __this (&display)

extern int ldo_power_ctrl(uint8_t on);

void lcd_screen_init(void *arg)
{
    ldo_power_ctrl(1);

	//屏初始化
	struct lcd_info info = {0};
	__this->lcd = lcd_get_hdl();
	ASSERT(__this->lcd);

	// if (__this->lcd->power_ctrl) {
	// 	/* printf("0x%x, 0x%x, 0x%x\n", __this, __this->lcd, __this->lcd->power_ctrl); */
	// 	__this->lcd->power_ctrl(true);
	// }

	if (__this->lcd->init) {
		__this->lcd->init(arg);
	}

	if (__this->lcd->clear_screen) {
		__this->lcd->clear_screen(0x000000);
	}

	if (__this->lcd->backlight_ctrl) {
		__this->lcd->backlight_ctrl(100);
	}

	if (__this->lcd->get_screen_info) {
		__this->lcd->get_screen_info(&info);
	}

    u8 *buf = NULL;
	u32 len;
	if (__this->lcd->buffer_malloc)
    {
		__this->lcd->buffer_malloc(&buf, &len);

		printf("buf : 0x%x , len : %d\n", buf, len);

		void ui_buffer_init(u8 * buf, u32 len);
		ui_buffer_init(buf, len);
	}

    printf("+++++++++%s\n", __func__);
	/*
	    if (__this->lcd->buffer_malloc) {
	        __this->lcd->buffer_malloc(&__this->buf, &__this->len);
	        __this->disp_buffer = (color_t *)__this->buf;
	        __this->pixel_len = (__this->len/4*4) / sizeof(color_t) / 2;
	        __this->disp_buffer_0 = __this->disp_buffer;
	        __this->disp_buffer_1 = &__this->disp_buffer[__this->len];
	    }
	    */

    return;
}

enum ui_devices_type {
	LED_7,
	LCD_SEG3X9,
	TFT_LCD,//彩屏
	DOT_LCD,//点阵屿
};

//板级配置数据结构
struct ui_devices_cfg {
	enum ui_devices_type type;
	void *private_data;
};


LCD_SPI_PLATFORM_DATA_BEGIN(lcd_spi_data)
#if TCFG_LCD_QSPI_FT2308_ENABLE
    .pin_reset = IO_PORTA_02,
    .pin_cs = IO_PORTA_06,
    .pin_bl = NO_CONFIG_PORT,//IO_PORTB_10,//NO_CONFIG_PORT,
    .pin_dc = NO_CONFIG_PORT,
    .pin_en = IO_PORTB_10,
    .pin_te = NO_CONFIG_PORT,//IO_PORTA_03,//NO_CONFIG_PORT,//IO_PORTA_03,
#endif

#if TCFG_LCD_SPI_SH8601A_ENABLE
    .pin_reset = IO_PORTA_02,
    .pin_cs = IO_PORTA_06,
    .pin_bl = IO_PORTB_10,//NO_CONFIG_PORT,
    .pin_dc = NO_CONFIG_PORT,
    .pin_en = IO_PORTB_10,
    .pin_te = IO_PORTA_03,//NO_CONFIG_PORT,//IO_PORTA_03,
#endif

#if TCFG_LCD_QSPI_ST77903_ENABLE || TCFG_LCD_QSPI_ST77903_V1_ENABLE || TCFG_LCD_QSPI_ST77903_V2_ENABLE
    .pin_reset= IO_PORTA_02,
    .pin_cs	= IO_PORTA_06,
    .pin_bl = IO_PORTB_10,//TCFG_BACKLIGHT_PWM_IO,
    .pin_dc	= NO_CONFIG_PORT,
    .pin_en	= NO_CONFIG_PORT,
    .pin_te = IO_PORTA_03,
#endif

LCD_SPI__PLATFORM_DATA_END()

const struct ui_devices_cfg ui_cfg_data = {
	.type = TFT_LCD,
	.private_data = (void *) &lcd_spi_data,
};

static void ex_api_register(ex_api_t *api)
{
	//extern u32 bss_begin;
	//extern u32 bss_size;
	printf("bss_begin :0x%x, bss_size : 0x%x\n", _bss_begin, _bss_size);
	memset((void *)_bss_begin, 0, (int)_bss_size);

    P3_IOV2_CON = 0x57;
	printf("[%s] 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", __FUNCTION__, api->printf, api->put_buf, api->sys_timer_add, api->sys_timeout_add, api->sys_timer_del, api->irq_disable, api->irq_enable, api->mdelay, api->request_irq_func);
	if (api->irq_disable) {
        my_irq_disable = api->irq_disable;
	}

	if (api->irq_enable) {
        my_irq_enable = api->irq_enable;
	}

	if (api->mdelay) {
        my_mdelay = api->mdelay;
	}

	if(api->request_irq_func) {
		_request_irq = api->request_irq_func;
	}

	lcd_screen_init(&ui_cfg_data);

//	draw_ring(227, 227, 227, 0x07e0, 0);//参数 输入：x坐标 、y坐标、半径、颜色、百分百(0~100)
};
//extern u8 update_text_buf[9][20];
static u8 filter_cnt;
#define LCD_WIDTH  (368)
#define LCD_HEIGHT (448)
extern const unsigned char gImage_ota_icon[20000];
static void ui_disp(UPDATA_STATUS st, u32 param)
{

	printf("[%s] st : %d, param : %d\n", __FUNCTION__, st, param);
    static u8 last_status = 0;
    //int text_x,text_y,text_width,text_height;
    //u8 str_buf[32] = {0};
    int i = 0;

	switch (st) {
	case UPDATE_START:
        printf("UPDATE_START.param:%d\n",param);

        if(param == 0)
        {
            ui_draw_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, 0x0000);
            ui_draw_image(134, 124, 100, 100, &gImage_ota_icon, 20000);
            ui_draw_bar(0);

            filter_cnt = 0;
        }
        // if(param == 0){
        //     ui_draw_rect(0,0,454,454);
        //     draw_ring(227, 227, 227, 0x07e0, 0);
        // }else if(param == 1){
        //     ui_draw_rect(0,0,454,454);
            //  get_text_rect(&update_text_buf[7][0],strlen(&update_text_buf[7][0]), &text_width, &text_height);
        //     text_x = 227-text_width/2;
        //     ui_draw_text(text_x,210,text_width,text_height,0xffff,&update_text_buf[7][0],strlen(&update_text_buf[7][0]));

        // }else if(param == 2){
        //     ui_draw_rect(0,0,454,454);
        //     get_text_rect(&update_text_buf[8][0],strlen(&update_text_buf[8][0]), &text_width, &text_height);
        //     text_x = 227-text_width/2;
        //     ui_draw_text(text_x,210,text_width,text_height,0xffff,&update_text_buf[8][0],strlen(&update_text_buf[8][0]));

        // }
		break;
	case UPDATE_PROCESS:
        printf("UPDATE_PROCESS.\n");
        int percent = param/100;
        if(filter_cnt <= 4)
        {
            filter_cnt++;
            return;
        }
        //printf("percent = %d\n", percent);
        ui_draw_bar(percent);
        // if(percent == 100)
        //     ldo_power_ctrl(0);
		//memset((void *)_bss_begin, 0, (int)_bss_size);
		//draw_ring(227, 227, 227, 0x07e0, param / 100);
        //draw_ring(368/2, 448/2, 368/2, 0x07e0, param / 100);
		break;
	case UPDATE_STOP:
	    printf("UPDATE_STOP.\n");
		// if (param == UPDATE_ERR_NONE) {

		// } else {
		// 	printf("error code:0x%x\n", param);
		// }
		break;
    case EX_API_UPDATE_TIPS_WAIT_CONN:

        if((!user_api_param.progress_bar)||(last_status!=st))
        {
            ui_draw_rect(0,0,LCD_WIDTH,LCD_HEIGHT, 0x0000);
        }

        printf("EX_API_UPDATE_TIPS_WAIT_CONN\n");
        printf("ADDR:%X:%X:%X:%X:%X:%X",user_api_param.addr[0],user_api_param.addr[1],user_api_param.addr[2],user_api_param.addr[3],user_api_param.addr[4],user_api_param.addr[5]);
        memcpy(&user_api_param,param,sizeof(struct _user_api_param_t));

        // get_text_rect(&update_text_buf[0][0],strlen(&update_text_buf[0][0]), &text_width, &text_height);
        // text_x = 227-text_width/2;
        // ui_draw_text(text_x,180,text_width,text_height,0xffff,&update_text_buf[0][0],strlen(&update_text_buf[0][0]));

        // memcpy(str_buf, &update_text_buf[4][0],strlen(&update_text_buf[4][0]));
        // bt_addr_to_str(&str_buf[strlen(str_buf)],user_api_param.addr);

		// printf("str_buf[%d]",strlen(str_buf));
		// put_buf(str_buf,strlen(str_buf));
        // get_text_rect(str_buf,strlen(str_buf), &text_width, &text_height);
        // text_x = 227-text_width/2;
        // ui_draw_text(text_x,240,text_width,text_height,0xffff,str_buf,strlen(str_buf));

        // for(i = 0;i < user_api_param.progress_bar;i++){
        //     get_text_rect(&update_text_buf[6][0],strlen(&update_text_buf[6][0]), &text_width, &text_height);
        //     ui_draw_text(100 + (text_width+5) * i,300,text_width,text_height,0xffff,&update_text_buf[6][0],strlen(&update_text_buf[6][0]));
        // }
        break;
    case EX_API_UPDATE_TIPS_WAIT_UPDATE:
        // if((!user_api_param.progress_bar)||(last_status!=st))
        // {
        //     ui_draw_rect(0,0,LCD_WIDTH,LCD_HEIGHT, 0x0000);
        // }
        printf("EX_API_UPDATE_TIPS_WAIT_UPDATE\n");
        memcpy(&user_api_param,param,sizeof(struct _user_api_param_t));

        // get_text_rect(&update_text_buf[1][0],strlen(&update_text_buf[1][0]), &text_width, &text_height);
        // text_x = 227-text_width/2;
        // ui_draw_text(text_x,180,text_width,text_height,0xffff,&update_text_buf[1][0],strlen(&update_text_buf[1][0]));

        // memcpy(str_buf, &update_text_buf[4][0],strlen(&update_text_buf[4][0]));
        // bt_addr_to_str(&str_buf[strlen(str_buf)],user_api_param.addr);

//		printf("str_buf[%d]",strlen(str_buf));
//		put_buf(str_buf,strlen(str_buf));
        // get_text_rect(str_buf,strlen(str_buf), &text_width, &text_height);
        // text_x = 227-text_width/2;
        // ui_draw_text(text_x,240,text_width,text_height,0xffff,str_buf,strlen(str_buf));
		// for(i = 0;i < user_api_param.progress_bar;i++){
        //     get_text_rect(&update_text_buf[6][0],strlen(&update_text_buf[6][0]), &text_width, &text_height);
        //     ui_draw_text(100 + (text_width+5) * i,300,text_width,text_height,0xffff,&update_text_buf[6][0],strlen(&update_text_buf[6][0]));
        // }
        break;
    case EX_API_UPDATE_TIPS_WAIT_START_UPDATE:
        // if((!user_api_param.progress_bar)||(last_status!=st))
        // {
        //     ui_draw_rect(0,0,LCD_WIDTH,LCD_HEIGHT, 0x0000);
        // }
        printf("EX_API_UPDATE_TIPS_WAIT_START_UPDATE\n");
        memcpy(&user_api_param,param,sizeof(struct _user_api_param_t));

        // get_text_rect(&update_text_buf[2][0],strlen(&update_text_buf[2][0]), &text_width, &text_height);
        // text_x = 227-text_width/2;
        // ui_draw_text(text_x,140,text_width,text_height,0xffff,&update_text_buf[2][0],strlen(&update_text_buf[2][0]));

        // get_text_rect(&update_text_buf[3][0],strlen(&update_text_buf[3][0]), &text_width, &text_height);
        // text_x = 227-text_width/2;
        // ui_draw_text(text_x,200,text_width,text_height,0xffff,&update_text_buf[3][0],strlen(&update_text_buf[3][0]));

        // memcpy(str_buf, &update_text_buf[4][0],strlen(&update_text_buf[4][0]));
        // bt_addr_to_str(&str_buf[strlen(str_buf)],user_api_param.addr);

		// put_buf(str_buf,strlen(str_buf));
        // get_text_rect(str_buf,strlen(str_buf), &text_width, &text_height);
        // text_x = 227-text_width/2;
        // ui_draw_text(text_x,260,text_width,text_height,0xffff,str_buf,strlen(str_buf));

		// for(i = 0;i < user_api_param.progress_bar;i++){
        //     get_text_rect(&update_text_buf[6][0],strlen(&update_text_buf[6][0]), &text_width, &text_height);
        //     ui_draw_text(100 + (text_width+5) * i,320,text_width,text_height,0xffff,&update_text_buf[6][0],strlen(&update_text_buf[6][0]));
        // }
        break;
	default:

		break;
	}
	last_status = st;
}

__attribute__((section(".api_tab"), used))
static  const user_api_t user_api_ins = {
	.magic = {'E', 'X', 'A', 'P',},
	.ex_api_reg_hdl = ex_api_register,
	.ui_disp_hdl = ui_disp,
};


