#ifndef __EX_FILE_INDEX_H__ 
#define __EX_FILE_INDEX_H__ 

#ifdef __cplusplus
extern "C" {
#endif

struct ex_file_index_t {
    char *name;
    int addr;
    int len;
};

extern const struct ex_file_index_t ex_file_index[];

#define font_common_24_no_ch_addr (0x00000000)

#define font_common_32_addr (0x0005C6F7)

#define font_common_num_12_addr (0x002529D6)

#define font_common_num_18_addr (0x0025AF19)

#define font_common_num_24_addr (0x00263551)

#define font_common_num_32_addr (0x0026BDC6)

#define font_common_num_36_addr (0x0027492D)

#define font_common_num_42_addr (0x0027D445)

#define font_common_num_52_addr (0x0028620E)

#define font_common_num_64_addr (0x0028F4B2)

#define font_common_num_78_addr (0x00298F5C)

#define azkar_en_ER_00_index (0x00000000)
#define azkar_en_ER_01_index (0x00000001)
#define azkar_en_ER_02_index (0x00000002)
#define azkar_en_ER_03_index (0x00000003)
#define azkar_en_ER_04_index (0x00000004)
#define azkar_en_ER_05_index (0x00000005)
#define azkar_en_ER_06_index (0x00000006)
#define azkar_en_ER_07_index (0x00000007)
#define azkar_en_ER_08_index (0x00000008)
#define azkar_en_ER_09_index (0x00000009)
#define azkar_en_ER_10_index (0x0000000A)
#define azkar_en_ER_11_index (0x0000000B)
#define azkar_en_ER_12_index (0x0000000C)
#define azkar_en_ER_13_index (0x0000000D)
#define azkar_en_ER_14_index (0x0000000E)
#define azkar_en_ER_15_index (0x0000000F)
#define azkar_en_MR_00_index (0x00000010)
#define azkar_en_MR_01_index (0x00000011)
#define azkar_en_MR_02_index (0x00000012)
#define azkar_en_MR_03_index (0x00000013)
#define azkar_en_MR_04_index (0x00000014)
#define azkar_en_MR_05_index (0x00000015)
#define azkar_en_MR_06_index (0x00000016)
#define azkar_en_MR_07_index (0x00000017)
#define azkar_en_MR_08_index (0x00000018)
#define azkar_en_MR_09_index (0x00000019)
#define azkar_en_MR_10_index (0x0000001A)
#define azkar_en_MR_11_index (0x0000001B)
#define azkar_en_MR_12_index (0x0000001C)
#define azkar_en_MR_13_index (0x0000001D)
#define azkar_en_MR_14_index (0x0000001E)
#define azkar_en_MR_15_index (0x0000001F)
#define azkar_en_MR_16_index (0x00000020)
#define azkar_en_MR_17_index (0x00000021)
#define azkar_en_MR_18_index (0x00000022)
#define azkar_en_MR_19_index (0x00000023)
#define azkar_en_MR_20_index (0x00000024)
#define azkar_en_MR_21_index (0x00000025)
#define azkar_en_MR_22_index (0x00000026)
#define azkar_en_MR_23_index (0x00000027)
#define azkar_en_MR_24_index (0x00000028)
#define azkar_en_MR_25_index (0x00000029)
#define azkar_en_MR_26_index (0x0000002A)

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif