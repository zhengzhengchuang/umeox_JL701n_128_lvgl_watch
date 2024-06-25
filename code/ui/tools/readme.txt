文件夹内容：
1.inputfile：指的是输入需要打包的文件，支持bin、jpg、sjpg、dat、gif等图片。

2.binfile：指的是将inputfile的图片文件转换成加入头信息lv_img_dsc_t结构体后的bin文件，如果原先是bin文件已经包含了lv_img_dsc_t则不转换，用于lvgl程序初始化索引资源。

3.outputfile：生成的总的资源bin文件，索引信息表c文件以及资源地址的宏定义文件

IMB_PIC_RES(.dat)：  IMB图片压缩工具，pic支持输入png、bmp原图，支持转换成ARGB8565、RGB565、ARGB8888、RGB888等格式，生成文件后缀为dat。

LVGL_BIN_RES(.bin)：lvgl非压缩bin图片预处理工具，lvgl_input_file输入https://lvgl.io/tools/imageconverter官方工具生成的bin格式图片，宽度转换成4字节对齐格式以适配IMB加速。

NORMAL_PIC(.jpg .gif .sjpg .png .bmp and more)：原图格式，需要开启对应解码器

H-name.txt：用于配置资源名字，用于客户自定义分组，默认是”FILE“
e.g.	
设置的名字：		“FILE”
生成的资源：
	资源总文件：	FILE.bin		
	文件地址表：	FILE_addresses.txt	宏定义 -> #define FILE_xxxx_img    
	结构体索引：	FILE_index.c		code索引表结构体 -> FILE_index[]


图片格式说明（指的是inputfile文件夹里面的）
.bin .jpg .dat支持硬件加速，其中.dat是IMB工具生成的压缩图片，支持硬件解压缩，bin和jpg需要转换一下适应IMB的格式。

.gif 需要预留图片 RAM大小 = 宽度*高度*4，如200*200就需要占用160K的RAM，适用比较小的图，大图建议用JPG轮播。

.png .bmp等标准图片文件开启LVGL对应的解码器理论上可以支持，但很占RAM，一般不建议使用，可以转成.dat或者.bin格式图片使用。


使用步骤
1.下载官方python最新的版本

2.根据需求分别把原图放进IMB_PIC_RES\pic、LVGL_BIN_RES\lvgl_input_file、NORMAL_PIC\IMG路径上，其中JPG图片需要点击"工程源文件升级工具.exe"转换一下。

3.点击“一键生成.bat”，输出总资源文件夹在outputfile，相关资源和索引文件会自动copy到SDK上。


###工具代码已全部开源，有需求可以自行修改！！
