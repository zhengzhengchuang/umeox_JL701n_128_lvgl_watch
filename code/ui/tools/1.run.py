import os
import struct

#自定义头名字
head_name = 'file'

# 打开文件并读取内容
# file_path = "H-name.txt"  # 将文件路径替换为你的文件路径
# with open(file_path, "r", encoding="utf-8") as file:
#     content = file.read()
#     head_name = content
#     print(content)
    
#os.system('clean.bat')

#os.system('del /q 'bin_tmp\*.*"')

#os.system('del /q "bin_out\*.*"')

def is_ok_by_extension(file_path, name):
    # 获取文件的后缀名
    file_extension = file_path.split('.')[-1].lower()
    return file_extension == name

def get_image_dimensions(image_path):
    with open(image_path, 'rb') as image_file:
        image_data = image_file.read()
        if is_ok_by_extension(image_path,"jpg") and image_data[0:2] == b'\xff\xd8':  # Check for JPEG header
            # Find the height and width markers
            marker_idx = image_data.find(b'\xff\xc0')
            height = int.from_bytes(image_data[marker_idx + 5:marker_idx + 7], byteorder='big')
            width = int.from_bytes(image_data[marker_idx + 7:marker_idx + 9], byteorder='big')
            return width, height
        elif is_ok_by_extension(image_path,"png") and image_data[1:4] == b'PNG':  # Check for PNG header
            width = int.from_bytes(image_data[16:20], byteorder='big')
            height = int.from_bytes(image_data[20:24], byteorder='big')
            return width, height
        elif is_ok_by_extension(image_path,"bmp") and image_data[0:2] == b'BM':  # Check for BMP header
            width = int.from_bytes(image_data[18:22], byteorder='little')
            height = int.from_bytes(image_data[22:26], byteorder='little')
            return width, height
        elif is_ok_by_extension(image_path,"sjpg") and image_data[0:7] == b"_SJPG__": # Check for SJPG header
            width = int.from_bytes(image_data[14:18], byteorder='little')
            height = int.from_bytes(image_data[18:22], byteorder='little')
            return width, height    
        elif is_ok_by_extension(image_path,"gif") and image_data[0:3] == b"GIF":  # Check for gif header
            # 从字节 6 到 10 读取宽度和高度信息
            width = int.from_bytes(image_data[6:8], byteorder='little')
            height = int.from_bytes(image_data[8:10], byteorder='little')
            return width, height            
        else:
            print("不支持的格式")
            # 加入暂停
            input("按回车键继续执行...")   

# 定义输入文件夹和输出文件夹的路径
input_folder = "dat_tmp"
output_folder = "bin_tmp"

# 检查输出文件夹是否存在，如果不存在则创建
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

"""
# 获取输入文件夹中所有的bin文件
bin_files = [file for file in os.listdir(input_folder) if file.endswith(".bin")]

# 遍历每个bin文件并将其拷贝到输出文件夹
for bin_file in bin_files:
    input_path = os.path.join(input_folder, bin_file)
    output_path = os.path.join(output_folder, bin_file)
    
    # 打开输入文件和输出文件，分别读取和写入数据
    with open(input_path, "rb") as input_file, open(output_path, "wb") as output_file:
        output_file.write(input_file.read())
    
print("bin文件拷贝完成！")
"""

output_data = []  # 用于存储解包后的数据

# 遍历文件夹中的文件(IMB压缩资源文件)
for filename in os.listdir(input_folder):
    if filename.endswith('.dat'):
        file_path = os.path.join(input_folder, filename)
        with open(file_path, 'rb') as file:
            head = file.read(4)
            # print(head)
            # 读取文件头并按照BBHHHIIII格式解包
            header = file.read(struct.calcsize('BBHHHIIII'))
            unpacked_data = struct.unpack('BBHHHIIII', header)
            # print(unpacked_data)

            cf = unpacked_data[0]
            w = unpacked_data[3]
            h = unpacked_data[4]
            
            packed_data = [0,0,0,0]
            
            if cf == 8:
                packed_data[0] = 1    #RAW_ARGB
            else:
                packed_data[0] = 1    #RAW_RGB
                
            packed_data[1] = ((0x3F&w)<<2)
            packed_data[2] = ((0x7FF&w)>>6) + ((0x7&h)<<5)
            packed_data[3] = ((0x7FF&h)>>3)
            
            # 解包后的数据将是一个包含各个字段值的元组
            # tcf = packed_data[0]   # Color format
            # tw = (packed_data[1]>>2)  +  ((0x1F&packed_data[2])<<6)   # Width
            # th = ((0xE0&packed_data[2])>>5) + (packed_data[3]<<3) # Height
            # print("source", cf, w, h)
            # print("check", tcf, tw, th)
            
            # print(file_path,packed_data)
            lvhead = struct.pack('BBBB', *packed_data)
            
            file.seek(0)
            output_data = lvhead + file.read()
            
            output_path = os.path.join(output_folder, filename+'.bin')
            
            # 打开输入文件和输出文件，分别读取和写入数据
            with open(output_path, "wb") as output_file:
                output_file.write(output_data)
                
"""
# 遍历文件夹中的文件('.jpg','.bmp','.png','.sjpg','.gif') 没有加速功能
for filename in os.listdir(input_folder):
    if filename.endswith(('.jpg','.bmp','.png','.sjpg','.gif')):
        file_path = os.path.join(input_folder, filename)
        with open(file_path, 'rb') as file:
            image_path = file_path
            print(image_path)
            width, height = get_image_dimensions(image_path)
            print("宽度:", width)
            print("高度:", height)

            cf = 1#RAW_RGB
            w = width
            h = height
            
            packed_data = [0,0,0,0]
            
            if cf == 8:
                packed_data[0] = 2    #RAW_ARGB
            else:
                packed_data[0] = 1    #RAW_RGB
            
            packed_data[1] = ((0x3F&w)<<2) + 0x1
            packed_data[2] = ((0x7FF&w)>>6) + ((0x7&h)<<5)
            packed_data[3] = ((0x7FF&h)>>3)
            
            # 解包后的数据将是一个包含各个字段值的元组
            # tcf = packed_data[0]   # Color format
            # tw = (packed_data[1]>>2)  +  ((0x1F&packed_data[2])<<6)   # Width
            # th = ((0xE0&packed_data[2])>>5) + (packed_data[3]<<3) # Height
            # print("source", cf, w, h)
            # print("check", tcf, tw, th)
            
            print(packed_data)
            lvhead = struct.pack('BBBB', *packed_data)
            
            file.seek(0)
            output_data = lvhead + file.read()
            
            output_path = os.path.join(output_folder, filename+'.bin')
            
            # 打开输入文件和输出文件，分别读取和写入数据
            with open(output_path, "wb") as output_file:
                output_file.write(output_data)
"""


# Step 1: Read and sort bin files
bin_folder = "./bin_tmp/"
bin_files = sorted([f for f in os.listdir(bin_folder) if f.endswith(".bin")])
# print(bin_files)

# Step 2: Concatenate bin files
def clear_file_contents(file_path):
    with open(file_path, 'w') as file:
        file.truncate(0)

# clear_file_contents(f"./bin_out/{head_name}_addresses.txt")
clear_file_contents(f"./bin_out/{head_name}_index.c")
clear_file_contents(f"./bin_out/{head_name}.bin")

# Step 4: Generate C structure
struct_text = f'#include "{head_name}_index.h" \n\n'
# print(struct_text)
# struct_text += ''''''

struct_text += f"const struct file_index_t {head_name}_index[] = " + "{\n"

define_text = f'#ifndef __FILE_INDEX_H__ \n'
define_text += f'#define __FILE_INDEX_H__ \n'
define_text += '''
#ifdef __cplusplus
extern "C" {
#endif

struct file_index_t {
    char *name;
    int addr;
    int len;
};

extern const struct file_index_t file_index[];

'''
# print(define_text)

current_address = 0x00000000
output_bin = b""

# 添加字库
font_folder = "./font/"
font_list = []
for font in os.listdir(font_folder):
    font_list.append(font)

for i in range(len(font_list)):
    file_name = os.path.join(font_folder, font_list[i])
    with open(file_name, mode="rb") as font_bin:
        output_bin += font_bin.read()

    fontname_without_extension = os.path.splitext(font_list[i])[0] + "_address"
    #print(fontname_without_extension)
    define_text += f"#define {fontname_without_extension} (0x{current_address:08X})\n\n"

    file_size = os.path.getsize(file_name)
    # print(file_size)
    current_address += file_size

for index, filename in enumerate(bin_files):
    # print(index, filename)
    with open(os.path.join(bin_folder, filename), "rb") as bin_file:
        file_data = bin_file.read()
        output_bin += file_data
        # print(output_bin)

        # Step 3: Generate #define text
        # 去除文件名后缀
        filename_without_extension = os.path.splitext(filename)[0]
        # print(filename_without_extension)
        # 替换小数点为下划线
        processed_filename = filename_without_extension.replace(".dat", "_index")
        # print(processed_filename)

        define_text += f"#define {processed_filename} (0x{index:08X})\n"
        # print(define_text)
        
        # Step 3: Generate #define text
        # 去除文件名后缀
        filename_without_extension = os.path.splitext(filename)[0]
        # print(filename_without_extension)
        # 替换小数点为下划线
        processed_filename = filename_without_extension.replace(".dat", "_index")
        # print(processed_filename)
        struct_text += f"    {{ \"{processed_filename}\", 0x{current_address :08X} ,{len(file_data)}}},\n"
        # print(struct_text)
            
        # Align to 4 bytes
        padding = b"\x00" * (4 - (len(output_bin) % 4))
        output_bin += padding
        current_address += (len(padding) + len(file_data))
        

with open(f"./bin_out/{head_name}.bin", "wb") as o_file:
    o_file.write(output_bin)

struct_text += "};"

with open(f"./bin_out/{head_name}_index.c", "w") as struct_file:
    struct_file.write(struct_text)

define_text += '''
#ifdef __cplusplus
} /* extern "C" */
#endif
'''
define_text += f'#endif'
# print(define_text)
with open(f"./bin_out/{head_name}_index.h", "w") as define_file:
    define_file.write(define_text)
    # print(define_text[:-1])

# 加入暂停
#input("按回车键继续执行...")

