#ifndef __IMAGE_H__
#define __IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

// dll头文件
#ifdef IMAGE_EXPORTS
#define IMAGE_API __declspec(dllexport)
#else
#define IMAGE_API __declspec(dllimport)
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//图像数据格式
enum PIXEL_FMT {
	PIXEL_FMT_ARGB8888,		//0
	PIXEL_FMT_RGB888,		//1
	PIXEL_FMT_RGB565,		//2
	PIXEL_FMT_L8,			//3
	PIXEL_FMT_AL88,			//4
	PIXEL_FMT_AL44,			//5
	PIXEL_FMT_A8,			//6
	PIXEL_FMT_L1,			//7
	PIXEL_FMT_ARGB8565,		//8
	PIXEL_FMT_OSD16,		//9
	PIXEL_FMT_UNKNOW,		//10
};

struct image_info {
	int width;  //图片宽度
	int height; //图片高度
	enum PIXEL_FMT format;
	u8 *buf;    //位图数据（与format一致）
	int len;    //位图数据长度
};

struct image_buf {
	int width;	//图片宽度
	int height; //图片高度
	enum PIXEL_FMT format; //图片格式
	int compress;//是否压缩 0：非压缩， 1：压缩
	u8 *buf;    //压缩数据指针
	int len;    //压缩数据长度	
};

/*----------------------------------------------------------------------------*/
/**@brief   获取库版本号
   @param   void
   @return  版本号
   @note     
*/
/*----------------------------------------------------------------------------*/
IMAGE_API char *image_version();
/*----------------------------------------------------------------------------*/
/**@brief    指定位图数据进行压缩
   @param    info : 位图数据结构
   @return   压缩数据
   @note     
*/
/*----------------------------------------------------------------------------*/
IMAGE_API struct image_buf *image_compress(struct image_info *info);
/*----------------------------------------------------------------------------*/
/**@brief    指定图片路径以及格式进行压缩
   @param    path : 图片路径
   @param    format : 图片格式, 例如"ARGB8565"、"RGB565"
   @param    compress : 是否压缩
   @return   压缩数据 或者 非压缩数据
   @note     
*/
/*----------------------------------------------------------------------------*/
IMAGE_API struct image_buf *image_compress_by_path(TCHAR *path, TCHAR *format, int compress);
/*----------------------------------------------------------------------------*/
/**@brief   释放压缩数据所占内存 
   @param   image : 压缩数据信息
   @return  
   @note     
*/
/*----------------------------------------------------------------------------*/
IMAGE_API int image_buff_free(struct image_buf *image);

#ifdef __cplusplus
}
#endif

#endif
