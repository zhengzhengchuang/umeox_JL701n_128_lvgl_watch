


idle = ABSOLUTE(0x8000);
standby = ABSOLUTE(0x800a);
sleep = ABSOLUTE(0x801a);
deep_sleep = ABSOLUTE(0x802a);
ISR_BASE = 0x0;

P11_CODE_BEGIN = 0x80;

P11_RAM_LIMIT_H = 0x8000;
P11_RAM_LIMIT_L = 0x80;
P11_RAM_SIZE = P11_RAM_LIMIT_H - P11_RAM_LIMIT_L;
P11_RAM_END = P11_RAM_LIMIT_H;

P11_POWEROFF_RAM_SIZE = 0x14 + 0xc;
P11_POWEROFF_RAM_BEGIN = P11_RAM_END - P11_POWEROFF_RAM_SIZE;

P2M_MESSAGE_SIZE = 0x40;
M2P_MESSAGE_SIZE = 0x60;
P11_MESSAGE_SIZE = M2P_MESSAGE_SIZE + P2M_MESSAGE_SIZE;
P11_MESSAGE_BEGIN = P11_POWEROFF_RAM_BEGIN - P11_MESSAGE_SIZE;




P11_VAD_RAM_SIZE = 0;

P11_VAD_RAM_BEGIN = P11_MESSAGE_BEGIN - P11_VAD_RAM_SIZE - 0x20;

P11_RAM_CODE_BEGIN = P11_CODE_BEGIN;
P11_RAM_CODE_END = P11_VAD_RAM_BEGIN;
P11_RAM_CODE_SIZE = P11_RAM_CODE_END - P11_RAM_CODE_BEGIN;

MEMORY
{
    p11_ram(rwx) : ORIGIN = P11_RAM_CODE_BEGIN, LENGTH = P11_RAM_CODE_SIZE
 p11_poweroff_ram(rw) : ORIGIN = P11_POWEROFF_RAM_BEGIN, LENGTH = P11_POWEROFF_RAM_SIZE
    p11_message_ram(rw) : ORIGIN = P11_MESSAGE_BEGIN, LENGTH = P11_MESSAGE_SIZE
    p11_vad_ram(rw) : ORIGIN = P11_VAD_RAM_BEGIN, LENGTH = P11_VAD_RAM_SIZE
}

ENTRY(_start)

SECTIONS
{
    . = ORIGIN(p11_ram);
    .text ALIGN(32):
    {
        *startup.o(.text)

  *(.ram0.data.bss)
  *(.ram0.data)
  *(.ram0.text.const)
  *(.ram0.text)

  *(.*.text)
  *(.*.const)


        . = ALIGN(4);
        gsensor_dev_begin = .;
        KEEP(*(.gsensor_dev))
       gsensor_dev_end = .;

        . = ALIGN(4);
        hrsensor_dev_begin = .;
        KEEP(*(.hrsensor_dev))
        hrsensor_dev_end = .;

        *(.rodata*)
        . = ALIGN(4);
  PROVIDE(low_power_target_begin = .);
  *(.lp_target.2.ops)
  *(.lp_target.1.ops)
  *(.lp_target.0.ops)
  PROVIDE(low_power_target_end = .);


        . = ALIGN(4);
        loop_detect_handler_begin = .;
        KEEP(*(.loop_detect_region))
        loop_detect_handler_end = .;

  . = ALIGN(4);
  *(.*.data)
  . = ALIGN(4);
        *(.data*)
  . = ALIGN(4);
    } > p11_ram

 .data ALIGN(32):
 {

 } > p11_ram

    .bss ALIGN(4):
    {
        *(.bss)
  *(.*.bss)
        *(COMMON)
    } > p11_ram

    .stack_bss ALIGN(32):
    {
        *(.stack_magic0)
        PROVIDE(_stack_begin = .);
        PROVIDE(_sstack_begin = .);
        *(.sstack)
        PROVIDE(_sstack_end = .);

        PROVIDE(_ustack_begin = .);
        *(.ustack)
        PROVIDE(_ustack_end = .);

        PROVIDE(_stack_end = .);
        *(.stack_magic1)
    } > p11_ram

 PROVIDE(overlay_demo_begin = .);
 OVERLAY : AT(0x200000) SUBALIGN(4)
 {
  .overlay_bank_demo_init
  {
            LONG(0xffffffff);
            *(.demo.code.bank.0)
            . = ALIGN(4);
  }
  .overlay_bank_demo_normal0
  {
            LONG(0xffffffff);
            *(.demo.code.bank.1)
  }

  .overlay_bank_demo_normal1
  {
            LONG(0xffffffff);
            *(.demo.code.bank.2)
  }
 } > p11_ram
 PROVIDE(overlay_demo_end = .);


 PROVIDE(overlay_sensor_begin = .);
 OVERLAY : AT(0x210000) SUBALIGN(4)
 {

  .overlay_bank_sensor_init
  {
            LONG(0xffffffff);
            *(.sensor.code.bank.0)
            . = ALIGN(4);
  }
  .overlay_bank_sensor_normal0
  {
            LONG(0xffffffff);
            *(.sensor.code.bank.1)
  }
  .overlay_bank_sensor_normal1
  {
            LONG(0xffffffff);
            *(.sensor.code.bank.2)
  }

 } > p11_ram
 PROVIDE(overlay_sensor_end = .);

 PROVIDE(overlay_sys_begin = .);
 OVERLAY : AT(0x220000) SUBALIGN(4)
 {

  .overlay_bank_sys_init
  {
            LONG(0xffffffff);
            *(.sys.code.bank.0)
            . = ALIGN(4);
  }
  .overlay_bank_sys_normal0
  {
            LONG(0xffffffff);
            *(.sys.code.bank.1)
  }
 } > p11_ram
 PROVIDE(overlay_sys_end = .);

 PROVIDE(p11_heap_begin = .);
  p11_heap_end = P11_RAM_CODE_END;


    . = ORIGIN(p11_message_ram);
    .p11_message ALIGN(32):
    {
        *(.p2m_data)
        *(.m2p_data)
    } > p11_message_ram

    . = ORIGIN(p11_vad_ram);
    .p11_vad ALIGN(4):
 {
        *(.m2p_dvad_config)
  . = ALIGN(4);
        *(.m2p_avad_config)
  . = ALIGN(4);
        *(.p2m_cbuf_data)
    } > p11_vad_ram
}

text_begin = ADDR(.text);
text_size = SIZEOF(.text);
text_end = text_begin + text_size;

bss_begin = ADDR(.bss);
bss_size = SIZEOF(.bss);

data_addr = ADDR(.data);
data_begin = text_begin + text_size;
data_size = SIZEOF(.data);

p11_message_data_addr = ADDR(.p11_message);
p11_message_data_size = SIZEOF(.p11_message);

p11_heap_size = p11_heap_end - p11_heap_begin;
ASSERT(p11_heap_size >= 200, "p11 heap space too small !")
