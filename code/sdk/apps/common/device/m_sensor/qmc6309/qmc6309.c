
#include "qmc6309.h"
#include "ICAL_GDF30X.h"
#include "ICFG_GDF30X.h"

static qmc6309_data_t p_mag;
int qmc6309_recover(void);

int qmc6309_read_block(unsigned char addr, unsigned char *data, unsigned char len)
{
	if(!data || len == 0)
        return false;

    u8 retry = 5;
    bool succ_ret = false;

	u8 slave_addr = p_mag.slave_addr;

    succ_ret = sensor_iic_u8addr_rx_buf(slave_addr, addr, data, len);

    while(retry && !succ_ret)
    {
        succ_ret = sensor_iic_u8addr_rx_buf(slave_addr, addr, data, len);

        retry--;
    }

    return succ_ret;
}

int qmc6309_write_reg(unsigned char addr, unsigned char data)
{
	u8 retry = 5;
    bool succ_ret = false;

	u8 slave_addr = p_mag.slave_addr;

    succ_ret = sensor_iic_u8addr_tx_byte(slave_addr, addr, data);

    while(retry && !succ_ret)
    {
        succ_ret = sensor_iic_u8addr_tx_byte(slave_addr, addr, data);

        retry--;
    }

    return succ_ret;
}

void qmc6309_delay(unsigned int ms)
{
	mdelay(ms);

	return;
}

void qmc6309_axis_convert(float data[3], int layout)
{
	float raw[3];

	raw[0] = data[0];
	raw[1] = data[1];
	//raw[2] = data[2];
	if(layout >=4 && layout <= 7)
	{
		data[2] = -data[2];
	}
	//else
	//{
	//	data[2] = raw[2];
	//}

	if(layout%2)
	{
		data[0] = raw[1];
		data[1] = raw[0];
	}
	else
	{
		data[0] = raw[0];
		data[1] = raw[1];
	}

	if((layout==1)||(layout==2)||(layout==4)||(layout==7))
	{
		data[0] = -data[0];
	}
	if((layout==2)||(layout==3)||(layout==6)||(layout==7))
	{
		data[1] = -data[1];
	}
}


#if defined(QMC6309_MODE_SWITCH)
void qmc6309_setrst_auto_mode(short hw_d[3])
{
	int ret = QMC6309_FAIL;

	if(p_mag.set_ctl.mode == 0)
	{// set reset on
		if((QMC6309_ABS(hw_d[0]) > 6000)||(QMC6309_ABS(hw_d[1]) > 10000))
		{
			p_mag.set_ctl.count++;
			if(p_mag.set_ctl.count >= 2)
			{
				// read config reg
				ret = qmc6309_read_block(QMC6309_CTL_REG_ONE, &p_mag.ctrl1.value, 1);
				ret = qmc6309_read_block(QMC6309_CTL_REG_TWO, &p_mag.ctrl2.value, 1);
				// read config reg
				ret = qmc6309_write_reg(QMC6309_CTL_REG_ONE, 0x00);
				QMC6309_CHECK_ERR(ret);
				//ret = qmc6309_config_setrst(QMC6309_SET_ON);
				p_mag.ctrl2.bit.set_rst = QMC6309_SET_ON;
				ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, p_mag.ctrl2.value);
				QMC6309_CHECK_ERR(ret);
				qmc6309_delay(1);
				ret = qmc6309_write_reg(QMC6309_CTL_REG_ONE, p_mag.ctrl1.value);
				QMC6309_CHECK_ERR(ret);
				qmc6309_delay(1);
				p_mag.set_ctl.mode = 1;
				p_mag.set_ctl.count = 0;
				{
					unsigned char reg[2];
					ret = qmc6309_read_block(QMC6309_CTL_REG_ONE, reg, 2);
					//ret = qmc6309_read_block(QMC6309_CTL_REG_TWO, &p_mag.ctrl2.value, 1);
					QMC6309_LOG("qmc6309 mode switch to set only reg[0x%x 0x%x]\r\n",reg[0],reg[1]);
				}
				qmc6309_delay(10);
			}
		}
		else
		{
			p_mag.set_ctl.count = 0;
		}
	}
	else
	{// set only
		int force_switch = 0;
		p_mag.set_ctl.count++;
		if(p_mag.set_ctl.count >= 200)
		{
			p_mag.set_ctl.count = 0;
			force_switch = 1;
		}
		if(((QMC6309_ABS(hw_d[0]) < 5000)&&(QMC6309_ABS(hw_d[1]) < 8000)) || force_switch)
		{
			// read config reg
			ret = qmc6309_read_block(QMC6309_CTL_REG_ONE, &p_mag.ctrl1.value, 1);
			ret = qmc6309_read_block(QMC6309_CTL_REG_TWO, &p_mag.ctrl2.value, 1);
			// read config reg	
			ret = qmc6309_write_reg(QMC6309_CTL_REG_ONE, 0x00);
			QMC6309_CHECK_ERR(ret);
			//ret = qmc6309_config_setrst(QMC6309_SET_RESET_ON);			
			p_mag.ctrl2.bit.set_rst = QMC6309_SET_RESET_ON;
			ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, p_mag.ctrl2.value);
			QMC6309_CHECK_ERR(ret);
			qmc6309_delay(1);
			ret = qmc6309_write_reg(QMC6309_CTL_REG_ONE, p_mag.ctrl1.value);
			QMC6309_CHECK_ERR(ret);
			qmc6309_delay(1);
			p_mag.set_ctl.mode = 0;
			p_mag.set_ctl.count = 0;
			//QMC6309_LOG("qmc6309 mode switch to set-reset force:%d reg[0x%x 0x%x]\r\n",force_switch,p_mag.ctrl1.value,p_mag.ctrl2.value);
			
			{
				unsigned char reg[2];
				ret = qmc6309_read_block(QMC6309_CTL_REG_ONE, reg, 2);
				//ret = qmc6309_read_block(QMC6309_CTL_REG_TWO, &p_mag.ctrl2.value, 1);
				QMC6309_LOG("qmc6309 mode switch to set-reset force:%d reg[0x%x 0x%x]\r\n",force_switch, reg[0],reg[1]);
			}
			qmc6309_delay(10);
		}
	}

}
#endif

int qmc6309_get_chipid(void)
{
	int ret = QMC6309_FAIL;
	int i=0, retry=0;
	unsigned char chip_id = 0x00;

	retry = 0;
	while((chip_id != 0x90) && (retry++<5))
	{
		for(i=0; i<10; i++)
		{
			ret = qmc6309_read_block(QMC6309_CHIP_ID_REG, &chip_id, 1);
			if(ret == QMC6309_OK)
			{
				break;
			}
		}
	}

	if(chip_id == 0x90)
	{
		QMC6309_LOG("qmc6309_get_chipid slave:0x%x chipid = 0x%x\r\n", \
			p_mag.slave_addr, chip_id);
		return 1;
	}
	else
	{
		QMC6309_LOG("qmc6309_get_chipid slave:0x%x fail\r\n", \
			p_mag.slave_addr);
		return 0;
	}
}


void qmc6309_init_para(void)
{
	p_mag.slave_addr = QMC6309_IIC_ADDR;

#if 0
	p_mag.ctrl1.bit.mode = QMC6309_MODE_HPFM;	// QMC6309_MODE_HPFM; QMC6309_MODE_NORMAL
	p_mag.ctrl1.bit.zdbl_enb = QMC6309_ZDBL_ENB_OFF;	// QMC6309_ZDBL_ENB_ON;
	p_mag.ctrl1.bit.osr1 = QMC6309_OSR1_1;		// QMC6309_OSR1_8
	p_mag.ctrl1.bit.osr2 = QMC6309_OSR2_2;		// QMC6309_OSR2_4

	p_mag.ctrl2.bit.set_rst = QMC6309_SET_RESET_ON;
	p_mag.ctrl2.bit.range = QMC6309_RNG_32G;
	p_mag.ctrl2.bit.odr = QMC6309_ODR_HPFM;	//QMC6309_ODR_HPFM; QMC6309_ODR_200HZ
	p_mag.ctrl2.bit.soft_rst = 0;
#endif
#if 0
// 100Hz
	p_mag.ctrl1.bit.mode = QMC6309_MODE_NORMAL;	// QMC6309_MODE_HPFM; QMC6309_MODE_NORMAL
	p_mag.ctrl1.bit.zdbl_enb = QMC6309_ZDBL_ENB_OFF;	// QMC6309_ZDBL_ENB_ON;
	p_mag.ctrl1.bit.osr1 = QMC6309_OSR1_8;		// QMC6309_OSR1_8
	p_mag.ctrl1.bit.osr2 = QMC6309_OSR2_2;		// QMC6309_OSR2_4

	p_mag.ctrl2.bit.set_rst = QMC6309_SET_RESET_ON;
	p_mag.ctrl2.bit.range = QMC6309_RNG_32G;
	p_mag.ctrl2.bit.odr = QMC6309_ODR_100HZ; //QMC6309_ODR_HPFM; QMC6309_ODR_200HZ
	p_mag.ctrl2.bit.soft_rst = 0;
	p_mag.odr = 100;
#endif
// HPF
#if 1
	p_mag.ctrl1.bit.mode = QMC6309_MODE_HPFM;	// QMC6309_MODE_HPFM; QMC6309_MODE_NORMAL
	p_mag.ctrl1.bit.zdbl_enb = QMC6309_ZDBL_ENB_ON;
	p_mag.ctrl1.bit.osr1 = QMC6309_OSR1_8;
	p_mag.ctrl1.bit.osr2 = QMC6309_OSR2_4;

	p_mag.ctrl2.bit.set_rst = QMC6309_SET_RESET_ON;
	p_mag.ctrl2.bit.range = QMC6309_RNG_32G;
	p_mag.ctrl2.bit.odr = QMC6309_ODR_HPFM;
	p_mag.ctrl2.bit.soft_rst = 0;
	p_mag.odr = 200;
#endif
}

void qmc6309_check_nvram(void)
{
	int retry = 0;
	unsigned char status = 0x00;

	retry = 0;
	while(retry++<5)
	{
		qmc6309_read_block(QMC6309_STATUS_REG, &status, 1);
		QMC6309_LOG("qmc6309 status 0x%x\r\n", status);
		if((status & 0x10)&&(status & 0x08))
		{
			QMC6309_LOG("qmc6309 NVM load done!\r\n");
			break;
		}
	}
}

void qmc6309_soft_reset(void)
{
	int ret = QMC6309_FAIL;

	QMC6309_LOG("qmc6309_soft_reset!\r\n");
	ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, 0x80);
	QMC6309_CHECK_ERR(ret);
	qmc6309_delay(5);
	ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, 0x00);
	QMC6309_CHECK_ERR(ret);
	qmc6309_delay(5);
}

int qmc6309_read_mag_xyz(float data[3])
{
	static int qmc6309_fail_num = 0;
	int res;
	unsigned char mag_data[6];
	short hw_d[3] = {0};
	int t1 = 0;
	unsigned char rdy = 0;

	/* Check status register for data availability */
	while(!(rdy & QMC6309_STATUS_DRDY) && (t1++ < 5))
	{
		res = qmc6309_read_block(QMC6309_STATUS_REG, &rdy, 1);
		qmc6309_delay(1);
	}

	if((res == QMC6309_FAIL)||(!(rdy & QMC6309_STATUS_DRDY)))
  	{
		QMC6309_LOG("qmc6309_read_mag_xyz drdy fail! res=%d rdy=0x%x\r\n",res,rdy);
		hw_d[0] = p_mag.last_data[0];
		hw_d[1] = p_mag.last_data[1];
		hw_d[2] = p_mag.last_data[2];
		if(qmc6309_fail_num++ > 3)
		{
			//qmc6309_soft_reset();
			qmc6309_recover();
			qmc6309_enable();
			qmc6309_fail_num = 0;
		}
		//return QMC6309_FAIL;
	}else
	{
		qmc6309_fail_num = 0;
		mag_data[0] = QMC6309_DATA_OUT_X_LSB_REG;
		res = qmc6309_read_block(QMC6309_DATA_OUT_X_LSB_REG, mag_data, 6);
		if(res == QMC6309_FAIL)
	  	{
			QMC6309_LOG("qmc6309_read_mag_xyz drdy read 0x01 fail! res=%d\r\n",res);
			hw_d[0] = p_mag.last_data[0];
			hw_d[1] = p_mag.last_data[1];
			hw_d[2] = p_mag.last_data[2];
			//return QMC6309_FAIL;
		}
		else
		{
			hw_d[0] = (short)(((mag_data[1]) << 8) | mag_data[0]);
			hw_d[1] = (short)(((mag_data[3]) << 8) | mag_data[2]);
			hw_d[2] = (short)(((mag_data[5]) << 8) | mag_data[4]);
		}
	}
	p_mag.last_data[0] = hw_d[0];
	p_mag.last_data[1] = hw_d[1];
	p_mag.last_data[2] = hw_d[2];
#if defined(QMC6309_MODE_SWITCH)
	qmc6309_setrst_auto_mode(hw_d);
#endif
	data[0] = (float)((float)hw_d[0] / ((float)p_mag.ssvt/100.f));		// ut
	data[1] = (float)((float)hw_d[1] / ((float)p_mag.ssvt/100.f));		// ut
	data[2] = (float)((float)hw_d[2] / ((float)p_mag.ssvt/100.f));		// ut

	qmc6309_axis_convert(data, 2);

	if(p_mag.ctrl1.bit.mode == QMC6309_MODE_SINGLE)
	{
		res = qmc6309_write_reg(QMC6309_CTL_REG_ONE, p_mag.ctrl1.value);
		QMC6309_CHECK_ERR(res);
		qmc6309_delay(1);
	}

	return res;
}


int qmc6309_read_mag_raw_xyz(short data[3])
{
	int res;
	unsigned char mag_data[6];
//	short hw_d[3] = {0};
	int t1 = 0;
	unsigned char rdy = 0;

	/* Check status register for data availability */
	//res = qmc6309_read_block(QMC6309_STATUS_REG, &rdy, 1);
	while(!(rdy & QMC6309_STATUS_DRDY) && (t1++ < 5))
	{
		res = qmc6309_read_block(QMC6309_STATUS_REG, &rdy, 1);
		qmc6309_delay(1);
	}
	if((res == QMC6309_FAIL)||(!(rdy & QMC6309_STATUS_DRDY)))
  	{
		QMC6309_LOG("qmc6309_read_mag_xyz drdy fail! res=%d rdy=0x%x\r\n",res,rdy);
		return QMC6309_FAIL;
	}

	mag_data[0] = QMC6309_DATA_OUT_X_LSB_REG;
	res = qmc6309_read_block(QMC6309_DATA_OUT_X_LSB_REG, mag_data, 6);
	if(res == QMC6309_FAIL)
  	{
		return QMC6309_FAIL;
	}

	data[0] = (short)(((mag_data[1]) << 8) | mag_data[0]);
	data[1] = (short)(((mag_data[3]) << 8) | mag_data[2]);
	data[2] = (short)(((mag_data[5]) << 8) | mag_data[4]);

	return res;
}

int qmc6309_enable(void)
{
	int ret = 0;

	//QMC6309_LOG("qmc6309_enable!\r\n");
	if(p_mag.ctrl1.bit.mode == QMC6309_MODE_NORMAL)
	{
#if 0
		if(odr <= 1)
		{
			p_mag.ctrl2.bit.odr = QMC6309_ODR_1HZ;
		}
		else if(odr <= 10)
		{ 
			p_mag.ctrl2.bit.odr = QMC6309_ODR_10HZ;
		}
		else if(odr <= 50)
		{
			p_mag.ctrl2.bit.odr = QMC6309_ODR_50HZ;
		}
		else if(odr <= 100)
		{ 
			p_mag.ctrl2.bit.odr = QMC6309_ODR_100HZ;
		}
		else if(odr <= 200)
		{
			p_mag.ctrl2.bit.odr = QMC6309_ODR_200HZ;
		}
		else
		{
			p_mag.ctrl2.bit.odr = QMC6309_ODR_200HZ;
		}
#endif
	}
	
	switch(p_mag.ctrl2.bit.range)
	{
		case QMC6309_RNG_32G:
			p_mag.ssvt = 1000;
			break;
		case QMC6309_RNG_16G:
			p_mag.ssvt = 2000;
			break;
		case QMC6309_RNG_8G:
			p_mag.ssvt = 4000;
			break;
		default:
			p_mag.ssvt = 1000;
			break;			
	}

	//unsigned char reg = 0;
	//qmc6309_read_block(0x40, &reg, 1);
	//QMC6309_LOG("0x40=0x%x\r\n", reg);
	//ret = qmc6309_write_reg(0x40, (reg | 0x80));		// IO SET VBAT
	//ret = qmc6309_write_reg(0x40, (reg & 0xFE));		// SR SWITCH SINGLE

	ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, p_mag.ctrl2.value);
	QMC6309_CHECK_ERR(ret);
	qmc6309_delay(1);
	ret = qmc6309_write_reg(QMC6309_CTL_REG_ONE, p_mag.ctrl1.value);
	QMC6309_CHECK_ERR(ret);
	qmc6309_delay(1);

	// printf("%s:ctrl1=0x%02x, ctrl2=0x%02x\n", \
	// 	__func__, p_mag.ctrl1.value, p_mag.ctrl2.value);

	return ret;
}

int qmc6309_disable(void)
{
	int ret = 0;

	ret = qmc6309_write_reg(QMC6309_CTL_REG_ONE, 0x00);
	QMC6309_CHECK_ERR(ret);

	return ret;
}

static void qmc6309_dump_reg(void)
{
	unsigned char ctrl_value[4];
	unsigned char version_id;
	unsigned char wafer_id;
	unsigned char die_id[2];

	
	qmc6309_read_block(0x12, &version_id, 1);
	QMC6309_LOG("qmc6309  version id:0x%x \r\n", version_id);
	qmc6309_read_block(0x37, &wafer_id, 1);
	QMC6309_LOG("qmc6309  wafer id:0x%x \r\n", wafer_id&0x1f);
	qmc6309_read_block(0x38, die_id, 2);
	QMC6309_LOG("qmc6309  die id:0x%x \r\n", (die_id[1]<<8)|die_id[0]);

	qmc6309_read_block(QMC6309_CTL_REG_ONE, ctrl_value, 2);
	QMC6309_LOG("qmc6309 ctrlreg [0x0a=0x%x 0x0b=0x%x] \r\n", \
		ctrl_value[0], ctrl_value[1]);
	//qmc6309_read_block(QMC6309_CTL_REG_THREE, &ctrl_value, 1);
	//QMC6309_LOG("qmc6309  0x%x=0x%x \r\n", QMC6309_CTL_REG_THREE, ctrl_value);
	
	qmc6309_read_block(0x40, ctrl_value, 1);
	QMC6309_LOG("0x40 = 0x%x \r\n", ctrl_value[0]);
}

void qmc6309_fifo_config(qmc6309_fifo_mode mode, qmc6309_fifo_ch ch, int wmk)
{
	unsigned char fifo_reg = 0x00;
	int ret = QMC6309_OK;

	fifo_reg = (mode|ch|(wmk<<3));
	p_mag.fifo_ctrl = fifo_reg;
	ret = qmc6309_write_reg(QMC6309_FIFO_REG_CTRL, fifo_reg);
	QMC6309_CHECK_ERR(ret);
	qmc6309_delay(1);
	p_mag.fifo_frame_len = 0;
	if(ch & QMC6309_FIFO_CH_X)
	{
		p_mag.fifo_frame_len+=2;
	}	
	if(ch & QMC6309_FIFO_CH_Y)
	{
		p_mag.fifo_frame_len+=2;
	}
	if(ch & QMC6309_FIFO_CH_Z)
	{
		p_mag.fifo_frame_len+=2;
	}
}

int qmc6309_fifo_read(unsigned char *f_data)
{
	unsigned char fifo_status = 0;
	unsigned char fifo_level = 0;
	int ret = QMC6309_OK;

	ret = qmc6309_read_block(QMC6309_FIFO_REG_STATUS, &fifo_status, 1);
	QMC6309_CHECK_ERR(ret);
	fifo_level = (fifo_status >> 4);
	if(fifo_level)
	{
		ret = qmc6309_read_block(QMC6309_FIFO_REG_DATA, (unsigned char*)f_data, p_mag.fifo_frame_len*fifo_level);
		//for(int index=0; index<fifo_level; index++)
		//{
		//	qmc6309_read_block(QMC6309_FIFO_REG_DATA, &f_data[index*6], 6);
		//}
		ret = qmc6309_write_reg(QMC6309_FIFO_REG_CTRL, p_mag.fifo_ctrl);
		QMC6309_LOG("fifo-level:%d\r\n", fifo_level);
		QMC6309_CHECK_ERR(ret);
	}
	//QMC6309_LOG("status 0x%x count=%d	int=%d\r\n", fifo_status, fifo_level, fifo_status&0x07);

	return (int)fifo_level;
}

int qmc6309_fifo_read_wait(unsigned char *f_data)
{
	unsigned char fifo_status = 0;
	unsigned char fifo_level = 0;
	int ret = QMC6309_OK;

	while(!(fifo_status&QMC6309_FIFO_STATUS_WMK))
	{
		ret = qmc6309_read_block(QMC6309_FIFO_REG_STATUS, &fifo_status, 1);
		QMC6309_CHECK_ERR(ret);
	}

	if(fifo_status&QMC6309_FIFO_STATUS_WMK)
	{
		fifo_level = (fifo_status >> 4);
		ret = qmc6309_read_block(QMC6309_FIFO_REG_DATA, (unsigned char*)f_data, 6*fifo_level);
		QMC6309_CHECK_ERR(ret);
		//for(int index=0; index<fifo_level; index++)
		//{
		//	qmc6309_read_block(QMC6309_FIFO_REG_DATA, &f_data[index*6], 6);
		//}
		ret = qmc6309_write_reg(QMC6309_FIFO_REG_CTRL, p_mag.fifo_ctrl); 
		QMC6309_CHECK_ERR(ret);
		QMC6309_LOG("status 0x%x count=%d	int=%d\r\n", fifo_status, fifo_level, fifo_status&0x07);
	}

	return (int)fifo_level;
}

int qmc6309_recover(void)
{
	int ret = QMC6309_FAIL;
	unsigned char qmc6309_slave[]={0x7c,0x0c,0x1c,0x2c,0x3c,0x4c,0x5c,0x6c};

	for(int i=0; i<sizeof(qmc6309_slave)/sizeof(qmc6309_slave[0]); i++)
	{
		p_mag.slave_addr = qmc6309_slave[i];		
		ret = qmc6309_get_chipid();
		if(ret) 	// read id OK
		{		
			ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, 0x80);
			qmc6309_delay(3);
			QMC6309_LOG("qmc6308 change slave to 0x7c read id again\n");
			p_mag.slave_addr = QMC6309_IIC_ADDR;
			ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, 0x00);
			qmc6309_delay(1);	
			ret = qmc6309_get_chipid(); // change to 0x7c read id again! 

			break;
		}
	}
	return 1;
}

int qmc6309_init(void)
{
	int ret = QMC6309_FAIL;
	unsigned char qmc6309_slave[]={0x0c,0x1c,0x2c,0x3c,0x4c,0x5c,0x6c,0x7c};

	qmc6309_init_para();
	ret = qmc6309_get_chipid();
	if(ret == 0)
	{
		for(int i=0; i<sizeof(qmc6309_slave)/sizeof(qmc6309_slave[0]); i++)
		{
			p_mag.slave_addr = qmc6309_slave[i];		
			ret = qmc6309_get_chipid();
			if(ret)		// read id OK
			{	
				ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, 0x80);
				qmc6309_delay(3);
				QMC6309_LOG("qmc6309 change slave to 0x7c read id again\n");
				p_mag.slave_addr = QMC6309_IIC_ADDR;
				ret = qmc6309_write_reg(QMC6309_CTL_REG_TWO, 0x00);
				qmc6309_delay(1);	
				ret = qmc6309_get_chipid(); // change to 0x7c read id again! 
				break;
			}
		}
	}
	
	if(!ret)
	{
		return 0;
	}
	qmc6309_soft_reset();	
	qmc6309_check_nvram();

	ret = qmc6309_disable();
	QMC6309_CHECK_ERR(ret);
	ret = qmc6309_enable();
	QMC6309_CHECK_ERR(ret);

	qmc6309_dump_reg();
#if defined(QMC6309_MODE_SWITCH)
	p_mag.set_ctl.mode = 0;
	p_mag.set_ctl.count = 0;
#endif
	if(p_mag.ctrl1.bit.mode == QMC6309_MODE_SINGLE)
	{
		ret = qmc6309_write_reg(QMC6309_CTL_REG_ONE, p_mag.ctrl1.value);
		QMC6309_CHECK_ERR(ret);
		qmc6309_delay(1);
	}

	return 1;
}

int qmc6309_self_test(void)
{
	int selftest_result = 0;
	int selftest_retry = 0;
	signed char  st_data[3];
	//unsigned char rx_buf[6] = {0};
	unsigned char rdy = 0x00;
	int t1 = 0;
	int ret = 0;

	while((selftest_result == 0)&&(selftest_retry<3))
	{
		selftest_retry++;
		qmc6309_write_reg(QMC6309_CTL_REG_ONE, 0x00);
		qmc6309_delay(2);

		qmc6309_write_reg(QMC6309_CTL_REG_TWO, 0x08);
		qmc6309_delay(2);
		qmc6309_write_reg(QMC6309_CTL_REG_ONE, 0x03);
		qmc6309_delay(20);
		qmc6309_write_reg(0x0e, 0x80);
		qmc6309_delay(150);
		rdy = 0x00;
		t1 = 0;
		while(!(rdy & 0x04))
		{
			ret = qmc6309_read_block(QMC6309_STATUS_REG, &rdy, 1);
			qmc6309_delay(1);
			if(t1++ > 50)
			{
				break;
			}
		}

		if(rdy & 0x04)
		{
			ret = qmc6309_read_block(QMC6309_DATA_OUT_ST_X, (unsigned char*)st_data, 3);
			if(ret == 0)
				continue;
		}
		else
		{
			QMC6309_LOG("qmc6309_self_test drdy fail!\r\n");
		}
		QMC6309_LOG("qmc6309_self_test(%d) status:0x%x %d	%d	%d\r\n", selftest_retry,rdy,st_data[0],st_data[1],st_data[2]);

		st_data[0] = QMC6309_ABS(st_data[0]);
		st_data[1] = QMC6309_ABS(st_data[1]);
		st_data[2] = QMC6309_ABS(st_data[2]);

		if(	((st_data[0] < QMC6309_SELFTEST_MAX_X) && (st_data[0] > QMC6309_SELFTEST_MIN_X))
			&& ((st_data[1] < QMC6309_SELFTEST_MAX_Y) && (st_data[1] > QMC6309_SELFTEST_MIN_Y))
			&& ((st_data[2] < QMC6309_SELFTEST_MAX_Z) && (st_data[2] > QMC6309_SELFTEST_MIN_Z)) )
	    {
			QMC6309_LOG("qmc6309_self_test OK!\r\n");
	        selftest_result = 1;
	    }
		else
		{
			selftest_result = 0;
		}
	}

	return selftest_result;
}

static u8 qmc6309_idle_query(void)
{
	//1：允许 0：不允许  
	bool GmEnableFlag = \
		GetSensorGmEnableFlag();

	if(GmEnableFlag == true)
		return 0;

	return 1;
}

REGISTER_LP_TARGET(qmc6309_lp_target) = {
    .name = "qmc6309",
    .is_idle = qmc6309_idle_query,
};


