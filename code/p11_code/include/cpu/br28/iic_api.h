#ifndef __IIC_API_H__
#define __IIC_API_H__

//=========================================================================//
//                             IIC MASTER API                              //
//=========================================================================//
int hw_iic_init(void);
void hw_iic_close(void);
void hw_iic_start(void);
void hw_iic_stop(void);
u8 hw_iic_master_tx_byte(u8 data);
u8 hw_iic_master_rx_byte(u8 ack);
int hw_iic_master_read_buf(void *buf, int len);
int hw_iic_master_write_buf(const void *buf, int len);

//=========================================================================//
//                              IIC SLAVE API                              //
//=========================================================================//
// void iic_slave_init(void);


#endif /* #ifndef __IIC_API_H__ */
