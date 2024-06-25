import os, re, ctypes
from ctypes import *
import ctypes as c
import shutil
import struct
from PIL import Image

'''
最终输出文件：output.bin

打包格式：
-------------------------------------------------------------------
|                                                                   
|                           "IMB\0"(4BYTE)          
|
-------------------------------------------------------------------
|
|                      file_info(24BYTE)   
|
-------------------------------------------------------------------
|
|                         DATA
|
-------------------------------------------------------------------
'''

# 加载C动态库
lib = ctypes.cdll.LoadLibrary('./image.dll')


# /------------------------------------重要接口、结构体定义------------------------------------/
'''
//图像数据格式
enum PIXEL_FMT {
    PIXEL_FMT_ARGB8888,     //0
    PIXEL_FMT_RGB888,       //1
    PIXEL_FMT_RGB565,       //2
    PIXEL_FMT_L8,           //3
    PIXEL_FMT_AL88,         //4
    PIXEL_FMT_AL44,         //5
    PIXEL_FMT_A8,           //6
    PIXEL_FMT_L1,           //7
    PIXEL_FMT_ARGB8565,     //8
    PIXEL_FMT_OSD16,        //9
    PIXEL_FMT_UNKNOW,       //10
};
'''

'''
struct image_info {
    int width;  //图片宽度
    int height; //图片高度
    enum PIXEL_FMT format;
    u8 *buf;    //位图数据（与format一致）
    int len;    //位图数据长度
};
'''
# ImageInfo
class ImageInfo(ctypes.Structure):
    _fields_ = [
        ("width", ctypes.c_int),
        ("height", ctypes.c_int),
        ("format", ctypes.c_int),  # 这里假设 PIXEL_FMT 是一个整数类型
        ("buf", ctypes.POINTER(ctypes.c_ubyte)),
        ("len", ctypes.c_int)
    ] 


'''
struct image_buf {
    int width;  //图片宽度
    int height; //图片高度
    enum PIXEL_FMT format; //图片格式
    int compress;//是否压缩 0：非压缩， 1：压缩
    u8 *buf;    //压缩数据指针
    int len;    //压缩数据长度    
};
'''
# ImageBuf结构体
class ImageBuf(ctypes.Structure):
    _fields_ = [
        ("width", ctypes.c_int),
        ("height", ctypes.c_int),
        ("format", ctypes.c_int),  # 这里假设 PIXEL_FMT 是一个整数类型
        ("compress", ctypes.c_int),
        ("buf", ctypes.POINTER(ctypes.c_ubyte)),
        ("len", ctypes.c_int)
    ] 


'''
/*----------------------------------------------------------------------------*/
/**@brief   获取库版本号
   @param   void
   @return  版本号
   @note     
*/
/*----------------------------------------------------------------------------*/
IMAGE_API char *image_version();
'''
# 将dll中的image_version赋予py_image_version
py_image_version = lib.image_version
# 修改返回值为char类型
lib.image_version.restype = c_char_p


'''
/*----------------------------------------------------------------------------*/
/**@brief    指定位图数据进行压缩
   @param    info : 位图数据结构
   @return   压缩数据
   @note     
*/
/*----------------------------------------------------------------------------*/
IMAGE_API struct image_buf *image_compress(struct image_info *info);
'''
# 将dll中的image_compress_by_path赋予py_image_compress_by_path
py_image_compress = lib.image_compress
# 修改输入参数类型为char *类型和int类型
py_image_compress.argtypes = [c.POINTER(ImageInfo)]
# 修改返回参数为struct *ImageBuf类型
py_image_compress.restype = c.POINTER(ImageBuf)


'''
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
'''
# 将dll中的image_compress_by_path赋予py_image_compress_by_path
py_image_compress_by_path = lib.image_compress_by_path
# 修改输入参数类型为char *类型和int类型
py_image_compress_by_path.argtypes = [c.c_wchar_p, c.c_wchar_p, c.c_int]
# 修改返回参数为struct *ImageBuf类型
py_image_compress_by_path.restype = c.POINTER(ImageBuf)
# /------------------------------------重要接口、结构体定义------------------------------------/


# /------------------------------------重要变量定义-------------------------------------------/

# 保存路径，用于创建输出文件夹
save_path = '.\\output\\'

# 输出长度信息文件名
len_info_file_name = 'len_info.txt'

# 输出合并bin文件名
output_bin_file_name = 'merged.bin' 

# 包含文件数据信息文件名
output_file_name = 'output_noname.bin'

# 输出文件名和文件名长度tab表
name_tab_file_name = 'tab.txt' 

# 输出文件名和文件名长度tab表
id_tab_file_name = 'ID_TABLE.txt' 

# 相关路径
len_info_path = save_path + '\\' + len_info_file_name
output_merged_bin_path = save_path + '\\' + output_bin_file_name
name_tab_path = save_path + '\\' + name_tab_file_name
output_file_path = save_path + '\\' + output_file_name
id_tab_file_path = save_path + '\\' + id_tab_file_name

# 遍历路径下png图片数量
png_num = 0

# 遍历路径下bmp图片数量
bmp_num = 0

# 文件信息字典
file_info_dict = {}

# 名字长度列表
len_of_name_list = []

# image_file结构体字典
image_file_dict = {}

# 文件名长度最大值
name_tab_max_len = 0

# 输出bin文件总长度
output_bin_file_total_len = 0

# 合并bin文件信息
merged_bin_data = b''

# 基地址
base_addr = 0

# /------------------------------------重要变量定义-------------------------------------------/


# /------------------------------------工具函数定义------------------------------------------/

def buf_data_align(data, n):
    """输出buf n字节对齐

    Args:
        data (_type_): buf数据
        n (_type_): n字节对齐

    Returns:
        _type_: n字节对齐后数据
    """
    if len(data) % n != 0:
        data += b'\x00' * (n - len(data) % n)
    return data

def num_align_4byte(num):
    """数字4字节对齐

    Args:
        num (_type_): 需进行4字节对齐的数字

    Returns:
        _type_: 4字节对齐后的16进制数字
    """
    return format(num, '08X')

def hex_to_little_edian(hex_num):
    """十六进制转小端

    Args:
        hex_num (_type_): 输入十六进制数据，不能以0x开头，正确输入格式如“00000003”

    Returns:
        _type_: 转小端后的字符串
    """
    byte_str = bytes.fromhex(str(hex_num))
    little_edian_binary = byte_str[::-1]
    little_edian = ' '.join(format(byte, '02X') for byte in little_edian_binary)
    return little_edian
# /------------------------------------工具函数定义------------------------------------------/


def make_dir(path):
    """创建目录

    Args:
        path (_type_): 需要创建的目录名称

    Returns:
        _type_: 创建成功则返回True，否则返回False
    """
    # 判断路径是否存在
    is_exists = os.path.exists(path)
    if not is_exists:
        # 如果不存在，则创建目录（多层）
        os.makedirs(path)
        # print('make dir sucess！')
        return True
    else:
        # 如果目录存在则不创建，并提示目录已存在
        # print('dir has been existed！')
        return False
    

def del_dir(path):
    """删除目录，包括子目录

    Args:
        path (_type_): 需要删除的目录名
    """
    is_exists = os.path.exists(path)
    if is_exists:
        # 遍历子文件和子文件夹
        for root, dirs, files in os.walk(path):
            for file in files:
                # 删除文件
                os.remove(os.path.join(root, file))
            for dir in dirs:
                # 删除子文件夹
                shutil.rmtree(os.path.join(root, dir))


def pic_compress(file_name, file_path, format, output_file_name, addr_offset):
    """图片压缩

    Args:
        file_name (_type_): 文件名
        file_path (_type_): 需要压缩的图片的路径
        format (_type_): 压缩格式
        output_file_name (_type_): 输出的合并后的bin文件名
        addr_offset (_type_): 地址偏移

    Returns:
        _type_: 返回地址偏移
    """
    # 调用dll后的返回值
    result = py_image_compress_by_path(file_path, format, 1)
    #print(result.contents.buf)
    
    # 构建image_file结构体字典
    image_file_dict = {}
    image_file_dict['format'] = result.contents.format
    image_file_dict['compress'] = result.contents.compress
    image_file_dict['data_crc'] = 0
    image_file_dict['width'] = result.contents.width
    image_file_dict['height'] = result.contents.height
    image_file_dict['offset'] = 0
    image_file_dict['len'] = result.contents.len
    image_file_dict['unzipOffset'] = 0
    image_file_dict['unzipLen'] = 0 
    
    print(image_file_dict)
    
    tab_data = struct.pack('<B', image_file_dict['format'])
    tab_data += struct.pack('<B', image_file_dict['compress'])
    tab_data += struct.pack('<H', image_file_dict['data_crc'])
    tab_data += struct.pack('<H', image_file_dict['width'])
    tab_data += struct.pack('<H', image_file_dict['height'])
    tab_data += struct.pack('<I', image_file_dict['offset'])
    tab_data += struct.pack('<I', image_file_dict['len'])
    tab_data += struct.pack('<I', image_file_dict['unzipOffset'])
    tab_data += struct.pack('<I', image_file_dict['unzipLen'])
        
    # 将字符串转换为字节（bytes）使用UTF-8编码
    text = "IMB\0"
    head_data = text.encode('utf-8')

    #print(head_data)

    #print(image_file_dict)    
    
    print(head_data, result.contents.len)
    print(output_file_name)
    # 拼接成buf
    data = head_data + tab_data + c.string_at(result.contents.buf, result.contents.len) 
    print(output_file_name)
    # 将每个转换后的图片输出为单个bin文件
    with open(output_file_name, 'wb') as single_file:  # 打开/创建相应.bin文件
        data = buf_data_align(data, 4) # 对输出buf数据进行4字节对齐
        single_file.write(data) # 将buf写入bin文件

    #print(image_file_dict)
    
    return addr_offset
    

def find_png_File(path):
    """遍历png文件

    Args:
        path (_type_): 需要遍历png的路径

    Yields:
        _type_: full_name：路径全名， f：文件名
    """
    for root, ds, fs in os.walk(path):
        # 这里使用默认的字母顺序
        # fs.sort()
        for f in fs:
            #过滤出png图片
            if f.endswith('.png'):
                global png_num 
                png_num += 1
                full_name = os.path.join(root, f)
                yield full_name, f



def find_bmp_File(path):
    """遍历bmp图片

    Args:
        path (_type_): 需要遍历bmp图片的路径

    Yields:
        _type_: full_name：路径全名， f：文件名
    """
    for root, ds, fs in os.walk(path):
        for f in fs:
            #过滤出bmp图片
            if f.endswith('.bmp'):
                global bmp_num 
                bmp_num += 1
                full_name = os.path.join(root, f)
                yield full_name, f

def find_all_File(path):
    """遍历bmp图片

    Args:
        path (_type_): 需要遍历bmp图片的路径

    Yields:
        _type_: full_name：路径全名， f：文件名
    """
    for root, ds, fs in os.walk(path):
        for f in fs:
            #过滤出bmp图片
            if f.endswith('.png') or f.endswith('.bmp'):
                global bmp_num 
                #bmp_num += 1
                full_name = os.path.join(root, f)
                yield full_name, f   
            else:
                print("输入图片格式不支持："+f)

def main():
    # 设置为当前路径下
    base_path = '.\pic'

    # bin文件偏移，每次以当前bin文件长度进行偏移
    addr_offset = 0

    # 清空字典
    file_info_dict.clear()

    # 在png文件中遍历
    for name_path, name in find_png_File(base_path):

        # 用列表记录文件名长度
        global len_of_name_list
        #len_of_name_list.append(len(name) + 1)
        len_of_name_list.append(0)
        
        
        rename = name_path
        
        #print(name)
        print(name_path)
        
        # 删除路径中的.png
        
        rename = rename.replace(".png", "", 1)  
        #rename = rename.replace(".bmp", "", 1)        
        rename = rename.replace(base_path+"\\", "")
        #outformat = rename.replace(base_path+"\\", "") 
        #outformat = rename.split("\\")[0]
        rename = rename.replace("\\", "_")
        print(rename)
        
        #outformat = "RGB565"
        #print(outformat)
        
        #print(rename)
        
        # 打开图像
        image = Image.open(name_path, mode='r')

        # 获取位深度
        depth = image.mode

        # 打印位深度
        print("图像的位深度为:", depth)
      
        # 将名字拼接成.bin
        output_name = save_path + rename + '.dat' 
     
        outformat = "RGB565"
        
        if depth == "RGBA":
            outformat = "ARGB8565"
      
        format = outformat#ARGB8565" # 使用argb8565格式
        print(format)
        
       
        # 压缩图片并返回地址偏移
        addr_offset = pic_compress(name, name_path, format, output_name, addr_offset)
        
    # 获取所有bin文件最终长度
    output_bin_file_total_len = addr_offset
    # print("output_bin_file_total_len: ", hex(output_bin_file_total_len))

    # print(file_info_dict)
    
    # 求出最长文件名
    global name_tab_max_len
    name_tab_max_len = max(len_of_name_list)

    # 将文件信息写入txt
    # with open(name_tab_path, 'a') as name_tab_file: # 打开或创建.txt文件    
        # for key, value in file_info_dict.items():
            # name_tab_file.write(key + ': ' + str(value) + '\n')
            
    # with open(id_tab_file_path, 'a') as name_tab_file: # 打开或创建.txt文件
        # for index, (key, value) in enumerate(file_info_dict.items()):
            # id_name = key.upper().replace(".", "_").replace("~", "_")
            # name_tab_file.write(f"#define {id_name}\t\t\t\"{index}\"\n") 
   

    # 打包输出tab_data，包含bin文件总长度、文件总数、文件名最长长度和image_file表

    # 1.打包输出bin文件总长度
    tab_data = struct.pack('<I', output_bin_file_total_len)

    # 2.打包文件总数
    tab_data += struct.pack('<I', png_num)

    # 3.打包文件名最长长度
    tab_data += struct.pack('<I', name_tab_max_len)

    # 遍历文件信息字典
    for file_name, image_file_dict in file_info_dict.items():
        # 4.按最长文件名长度打包文件名
        file_name_bytes = str(file_name).encode('utf-8')
        file_name_bytes = file_name_bytes[:name_tab_max_len]
        tab_data += struct.pack(f"<{name_tab_max_len}s", file_name_bytes)

        # 5.遍历当前文件名下的image_file结构体成员并打包
        #for key, value in image_file_dict.items():
        # struct image_file {
            # u8 format;
            # u8 compress;
            # u16 data_crc;
            # u16 width;
            # u16 height;
            # u32 offset;
            # u32 len;
            # u32 unzipOffset;
            # u32 unzipLen;
        # };
        
        
        print(image_file_dict)
        tab_data += struct.pack('<B', image_file_dict['format'])
        tab_data += struct.pack('<B', image_file_dict['compress'])
        tab_data += struct.pack('<H', image_file_dict['data_crc'])
        tab_data += struct.pack('<H', image_file_dict['width'])
        tab_data += struct.pack('<H', image_file_dict['height'])
        tab_data += struct.pack('<I', image_file_dict['offset'])
        tab_data += struct.pack('<I', image_file_dict['len'])
        tab_data += struct.pack('<I', image_file_dict['unzipOffset'])
        tab_data += struct.pack('<I', image_file_dict['unzipLen'])

        # image_file_dict['format'] = result.contents.format
        # image_file_dict['compress'] = result.contents.compress
        # image_file_dict['data_crc'] = 0
        # image_file_dict['width'] = result.contents.width
        # image_file_dict['height'] = result.contents.height
        # image_file_dict['offset'] = addr_offset
        # image_file_dict['len'] = result.contents.len
        # image_file_dict['unzipoffset'] = 0
        # image_file_dict['unziplen'] = 0 

        #image_file_dict['start_addr'] = start_addr 

        #file_info_dict[file_name] = image_file_dict

    # 打包output.bin文件

    # 1.添加第一个bin文件起始地址，即tab_data长度的偏移 + 4 + 基地址
    binary_data = struct.pack('<I', len(tab_data) + 4 + base_addr)

    # 2.包含tab_data内容
    binary_data += tab_data

    # 3.包含对齐后的bin文件集合
    binary_data += merged_bin_data

    # with open(output_file_path, 'wb') as output_file:
        # output_file.write(binary_data)
        

if __name__ == '__main__':
    # 打印版本号       
    print("version: ", py_image_version()); 

    # 清除保存路径内容   
    del_dir(save_path)

    # 创建保存路径文件夹
    make_dir(save_path)    

    main()


    
# 暂停，等待用户按回车继续
#input("生成步骤结束 按回车键继续...")
 