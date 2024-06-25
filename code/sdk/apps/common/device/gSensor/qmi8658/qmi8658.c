#include "qmi8658.h"
#include "../../../ui/lv_watch/lv_watch.h"

#if TCFG_QMI8658_EN

//#define QMI8658_UINT_MG_DPS
#define M_PI			(3.14159265358979323846f)
#define ONE_G			(9.807f)
#define QFABS(x)		(((x)<0.0f)?(-1.0f*(x)):(x))

static qmi8658_state g_imu;
#if defined(QMI8658_USE_CALI)
static qmi8658_cali g_cali;
#endif

static const int interrupt_io_num = \
	IO_PORTB_03;

int qmi8658_write_reg(unsigned char reg, unsigned char value)
{
	u8 retry = 5;
    bool succ_ret = false;

	u8 slave_addr = g_imu.slave;

    succ_ret = sensor_iic_u8addr_tx_byte(slave_addr, reg, value);

    while(retry && !succ_ret)
    {
        succ_ret = sensor_iic_u8addr_tx_byte(slave_addr, reg, value);

        retry--;

        printf("%s:retry = %d\n", __func__, retry);
    }

    return succ_ret;
}

int qmi8658_write_regs(unsigned char reg, unsigned char *value, unsigned char len)
{
	if(!value || len == 0)
        return false;

    u8 retry = 5;
    bool succ_ret = false;

	u8 slave_addr = g_imu.slave;

    succ_ret = sensor_iic_u8addr_tx_buf(slave_addr, reg, value, len);

    while(retry && !succ_ret)
    {
        succ_ret = sensor_iic_u8addr_tx_buf(slave_addr, reg, value, len);

        retry--;

        printf("%s:retry = %d\n", __func__, retry);
    }

    return succ_ret;
}

int qmi8658_read_reg(unsigned char reg, unsigned char* buf, unsigned short len)
{
	if(!buf || len == 0)
        return false;

    u8 retry = 5;
    bool succ_ret = false;

	u8 slave_addr = g_imu.slave;

    succ_ret = sensor_iic_u8addr_rx_buf(slave_addr, reg, buf, len);

    while(retry && !succ_ret)
    {
        succ_ret = sensor_iic_u8addr_rx_buf(slave_addr, reg, buf, len);

        retry--;

        printf("%s:retry = %d\n", __func__, retry);
    }

    return succ_ret;
}

void qmi8658_delay(unsigned int ms)
{
	mdelay(ms);

    return;
}

void qmi8658_delay_us(unsigned int us)
{
	udelay(us);

    return;
}

u16 GetGsACCSsvt(void)
{
	return g_imu.ssvt_a;
}

void qmi8658_axis_convert(float data_a[3], float data_g[3], int layout)
{
	float raw[3],raw_g[3];

	raw[0] = data_a[0];
	raw[1] = data_a[1];
	//raw[2] = data[2];
	raw_g[0] = data_g[0];
	raw_g[1] = data_g[1];
	//raw_g[2] = data_g[2];

	if(layout >=4 && layout <= 7)
	{
		data_a[2] = -data_a[2];
		data_g[2] = -data_g[2];
	}

	if(layout%2)
	{
		data_a[0] = raw[1];
		data_a[1] = raw[0];
		
		data_g[0] = raw_g[1];
		data_g[1] = raw_g[0];
	}
	else
	{
		data_a[0] = raw[0];
		data_a[1] = raw[1];

		data_g[0] = raw_g[0];
		data_g[1] = raw_g[1];
	}

	if((layout==1)||(layout==2)||(layout==4)||(layout==7))
	{
		data_a[0] = -data_a[0];
		data_g[0] = -data_g[0];
	}
	if((layout==2)||(layout==3)||(layout==6)||(layout==7))
	{
		data_a[1] = -data_a[1];
		data_g[1] = -data_g[1];
	}
}

#if defined(QMI8658_USE_CALI)
void qmi8658_data_cali(unsigned char sensor, float data[3])
{
	float data_diff[3];

	if(sensor == 1) 	// accel
	{
		data_diff[0] = QFABS((data[0]-g_cali.acc_last[0]));
		data_diff[1] = QFABS((data[1]-g_cali.acc_last[1]));
		data_diff[2] = QFABS((data[2]-g_cali.acc_last[2]));
		g_cali.acc_last[0] = data[0];
		g_cali.acc_last[1] = data[1];
		g_cali.acc_last[2] = data[2];

//		qmi8658_log("acc diff : %f	", (data_diff[0]+data_diff[1]+data_diff[2]));
		if((data_diff[0]+data_diff[1]+data_diff[2]) < 0.5f)
		{
			if(g_cali.acc_cali_num == 0)
			{
				g_cali.acc_sum[0] = 0.0f;
				g_cali.acc_sum[1] = 0.0f;
				g_cali.acc_sum[2] = 0.0f;
			}
			if(g_cali.acc_cali_num < QMI8658_CALI_DATA_NUM)
			{
				g_cali.acc_cali_num++;
				g_cali.acc_sum[0] += data[0];
				g_cali.acc_sum[1] += data[1];
				g_cali.acc_sum[2] += data[2];
				if(g_cali.acc_cali_num == QMI8658_CALI_DATA_NUM)
				{
					if((g_cali.acc_cali_flag == 0)&&(data[2]<11.8f)&&(data[2]>7.8f))
					{
						g_cali.acc_sum[0] = g_cali.acc_sum[0]/QMI8658_CALI_DATA_NUM;
						g_cali.acc_sum[1] = g_cali.acc_sum[1]/QMI8658_CALI_DATA_NUM;
						g_cali.acc_sum[2] = g_cali.acc_sum[2]/QMI8658_CALI_DATA_NUM;

						g_cali.acc_bias[0] = 0.0f - g_cali.acc_sum[0];
						g_cali.acc_bias[1] = 0.0f - g_cali.acc_sum[1];
						g_cali.acc_bias[2] = 9.807f - g_cali.acc_sum[2];
						g_cali.acc_cali_flag = 1;
					}
					g_cali.imu_static_flag = 1;
					qmi8658_log("qmi8658 acc static!!!\n");
				}
			}

			if(g_cali.imu_static_flag)
			{
				if(g_cali.acc_fix_flag == 0)
				{
					g_cali.acc_fix_flag = 1;
					g_cali.acc_fix[0] = data[0];
					g_cali.acc_fix[1] = data[1];
					g_cali.acc_fix[2] = data[2];
				}
			}
			else
			{
				g_cali.acc_fix_flag = 0;
				g_cali.gyr_fix_flag = 0;
			}
		}
		else
		{
			g_cali.acc_cali_num = 0;
			g_cali.acc_sum[0] = 0.0f;
			g_cali.acc_sum[1] = 0.0f;
			g_cali.acc_sum[2] = 0.0f;

			g_cali.imu_static_flag = 0;
			g_cali.acc_fix_flag = 0;
			g_cali.gyr_fix_flag = 0;
		}

		if(g_cali.acc_fix_flag)
		{
			if(g_cali.acc_fix_index != 0)
				g_cali.acc_fix_index = 0;
			else
				g_cali.acc_fix_index = 1;

			data[0] = g_cali.acc_fix[0] + g_cali.acc_fix_index*0.01f;
			data[1] = g_cali.acc_fix[1] + g_cali.acc_fix_index*0.01f;
			data[2] = g_cali.acc_fix[2] + g_cali.acc_fix_index*0.01f;
		}
		if(g_cali.acc_cali_flag)
		{
			g_cali.acc[0] = data[0] + g_cali.acc_bias[0];
			g_cali.acc[1] = data[1] + g_cali.acc_bias[1];
			g_cali.acc[2] = data[2] + g_cali.acc_bias[2];
			data[0] = g_cali.acc[0];
			data[1] = g_cali.acc[1];
			data[2] = g_cali.acc[2];
		}
		else
		{
			g_cali.acc[0] = data[0];
			g_cali.acc[1] = data[1];
			g_cali.acc[2] = data[2];
		}
	}
	else if(sensor == 2)			// gyroscope
	{
		data_diff[0] = QFABS((data[0]-g_cali.gyr_last[0]));
		data_diff[1] = QFABS((data[1]-g_cali.gyr_last[1]));
		data_diff[2] = QFABS((data[2]-g_cali.gyr_last[2]));
		g_cali.gyr_last[0] = data[0];
		g_cali.gyr_last[1] = data[1];
		g_cali.gyr_last[2] = data[2];
		
//		qmi8658_log("gyr diff : %f	\n", (data_diff[0]+data_diff[1]+data_diff[2]));
		if(((data_diff[0]+data_diff[1]+data_diff[2]) < 0.03f)
			&& ((data[0]>-1.0f)&&(data[0]<1.0f))
			&& ((data[1]>-1.0f)&&(data[1]<1.0f))
			&& ((data[2]>-1.0f)&&(data[2]<1.0f))
			)
		{
			if(g_cali.gyr_cali_num == 0)
			{
				g_cali.gyr_sum[0] = 0.0f;
				g_cali.gyr_sum[1] = 0.0f;
				g_cali.gyr_sum[2] = 0.0f;
			}
			if(g_cali.gyr_cali_num < QMI8658_CALI_DATA_NUM)
			{
				g_cali.gyr_cali_num++;
				g_cali.gyr_sum[0] += data[0];
				g_cali.gyr_sum[1] += data[1];
				g_cali.gyr_sum[2] += data[2];
				if(g_cali.gyr_cali_num == QMI8658_CALI_DATA_NUM)
				{
					if(g_cali.gyr_cali_flag == 0)
					{
						g_cali.gyr_sum[0] = g_cali.gyr_sum[0]/QMI8658_CALI_DATA_NUM;
						g_cali.gyr_sum[1] = g_cali.gyr_sum[1]/QMI8658_CALI_DATA_NUM;
						g_cali.gyr_sum[2] = g_cali.gyr_sum[2]/QMI8658_CALI_DATA_NUM;
			
						g_cali.gyr_bias[0] = 0.0f - g_cali.gyr_sum[0];
						g_cali.gyr_bias[1] = 0.0f - g_cali.gyr_sum[1];
						g_cali.gyr_bias[2] = 0.0f - g_cali.gyr_sum[2];
						g_cali.gyr_cali_flag = 1;
					}
					g_cali.imu_static_flag = 1;
					qmi8658_log("qmi8658 gyro static!!!\n");
				}
			}
			
			if(g_cali.imu_static_flag)
			{
				if(g_cali.gyr_fix_flag == 0)
				{
					g_cali.gyr_fix_flag = 1;
					g_cali.gyr_fix[0] = data[0];
					g_cali.gyr_fix[1] = data[1];
					g_cali.gyr_fix[2] = data[2];
				}
			}
			else
			{
				g_cali.gyr_fix_flag = 0;
				g_cali.acc_fix_flag = 0;
			}
		}
		else
		{
			g_cali.gyr_cali_num = 0;
			g_cali.gyr_sum[0] = 0.0f;
			g_cali.gyr_sum[1] = 0.0f;
			g_cali.gyr_sum[2] = 0.0f;
			
			g_cali.imu_static_flag = 0;
			g_cali.gyr_fix_flag = 0;
			g_cali.acc_fix_flag = 0;
		}

		if(g_cali.gyr_fix_flag)
		{		
			if(g_cali.gyr_fix_index != 0)
				g_cali.gyr_fix_index = 0;
			else
				g_cali.gyr_fix_index = 1;

			data[0] = g_cali.gyr_fix[0] + g_cali.gyr_fix_index*0.00005f;
			data[1] = g_cali.gyr_fix[1] + g_cali.gyr_fix_index*0.00005f;
			data[2] = g_cali.gyr_fix[2] + g_cali.gyr_fix_index*0.00005f;
		}

		if(g_cali.gyr_cali_flag)
		{
			g_cali.gyr[0] = data[0] + g_cali.gyr_bias[0];
			g_cali.gyr[1] = data[1] + g_cali.gyr_bias[1];
			g_cali.gyr[2] = data[2] + g_cali.gyr_bias[2];
			data[0] = g_cali.gyr[0];
			data[1] = g_cali.gyr[1];
			data[2] = g_cali.gyr[2];
		}
		else
		{		
			g_cali.gyr[0] = data[0];
			g_cali.gyr[1] = data[1];
			g_cali.gyr[2] = data[2];
		}
	}
}

#endif

void qmi8658_config_acc(enum qmi8658_AccRange range, enum qmi8658_AccOdr odr, enum qmi8658_LpfConfig lpfEnable, enum qmi8658_StConfig stEnable)
{
	unsigned char ctl_dada;

	switch(range)
	{
		case Qmi8658AccRange_2g:
			g_imu.ssvt_a = (1<<14);
			break;
		case Qmi8658AccRange_4g:
			g_imu.ssvt_a = (1<<13);
			break;
		case Qmi8658AccRange_8g:
			g_imu.ssvt_a = (1<<12);
			break;
		case Qmi8658AccRange_16g:
			g_imu.ssvt_a = (1<<11);
			break;
		default: 
			range = Qmi8658AccRange_8g;
			g_imu.ssvt_a = (1<<12);
	}
	if(stEnable == Qmi8658St_Enable)
		ctl_dada = (unsigned char)range|(unsigned char)odr|0x80;
	else
		ctl_dada = (unsigned char)range|(unsigned char)odr;
		
	qmi8658_write_reg(Qmi8658Register_Ctrl2, ctl_dada);
// set LPF & HPF
	qmi8658_read_reg(Qmi8658Register_Ctrl5, &ctl_dada, 1);
	ctl_dada &= 0xf0;
	if(lpfEnable == Qmi8658Lpf_Enable)
	{
		ctl_dada |= A_LSP_MODE_3;
		ctl_dada |= 0x01;
	}
	else
	{
		ctl_dada &= ~0x01;
	}
	//ctl_dada = 0x00;
	qmi8658_write_reg(Qmi8658Register_Ctrl5,ctl_dada);
// set LPF & HPF
}

void qmi8658_config_gyro(enum qmi8658_GyrRange range, enum qmi8658_GyrOdr odr, enum qmi8658_LpfConfig lpfEnable, enum qmi8658_StConfig stEnable)
{
	// Set the CTRL3 register to configure dynamic range and ODR
	unsigned char ctl_dada; 

	// Store the scale factor for use when processing raw data
	switch (range)
	{
		case Qmi8658GyrRange_16dps:
			g_imu.ssvt_g = 2048;
			break;			
		case Qmi8658GyrRange_32dps:
			g_imu.ssvt_g = 1024;
			break;
		case Qmi8658GyrRange_64dps:
			g_imu.ssvt_g = 512;
			break;
		case Qmi8658GyrRange_128dps:
			g_imu.ssvt_g = 256;
			break;
		case Qmi8658GyrRange_256dps:
			g_imu.ssvt_g = 128;
			break;
		case Qmi8658GyrRange_512dps:
			g_imu.ssvt_g = 64;
			break;
		case Qmi8658GyrRange_1024dps:
			g_imu.ssvt_g = 32;
			break;
		case Qmi8658GyrRange_2048dps:
			g_imu.ssvt_g = 16;
			break;
//		case Qmi8658GyrRange_4096dps:
//			g_imu.ssvt_g = 8;
//			break;
		default: 
			range = Qmi8658GyrRange_512dps;
			g_imu.ssvt_g = 64;
			break;
	}

	if(stEnable == Qmi8658St_Enable)
		ctl_dada = (unsigned char)range|(unsigned char)odr|0x80;
	else
		ctl_dada = (unsigned char)range | (unsigned char)odr;
	qmi8658_write_reg(Qmi8658Register_Ctrl3, ctl_dada);

// Conversion from degrees/s to rad/s if necessary
// set LPF & HPF
	qmi8658_read_reg(Qmi8658Register_Ctrl5, &ctl_dada,1);
	ctl_dada &= 0x0f;
	if(lpfEnable == Qmi8658Lpf_Enable)
	{
		ctl_dada |= G_LSP_MODE_3;
		ctl_dada |= 0x10;
	}
	else
	{
		ctl_dada &= ~0x10;
	}
	//ctl_dada = 0x00;
	qmi8658_write_reg(Qmi8658Register_Ctrl5,ctl_dada);
// set LPF & HPF
}


int qmi8658_send_ctl9cmd(enum qmi8658_Ctrl9Command cmd)
{
	unsigned char status1 = 0x00;
	unsigned short count=0;
	unsigned char status_reg = Qmi8658Register_StatusInt;	
	unsigned char cmd_done = 0x80;
	unsigned char retry = 0;
	int ret1 = 0;
	int ret2 = 0;

#if defined(QMI8658_SYNC_SAMPLE_MODE)
	if(g_imu.cfg.syncSample == 1)
	{
		status_reg = Qmi8658Register_Status1;
		cmd_done = 0x01;
	}
#endif
	while(retry++ < 3)
	{
		qmi8658_write_reg(Qmi8658Register_Ctrl9, (unsigned char)cmd);	// write commond to ctrl9

		qmi8658_read_reg(status_reg, &status1, 1);
		while(((status1&cmd_done)!=cmd_done)&&(count++<100))		// read statusINT until bit7 is 1
		{
			qmi8658_delay(1);
			qmi8658_read_reg(status_reg, &status1, 1);
		}
		//qmi8658_log("ctrl9 cmd (%d) done1 count=%d\n", cmd, count);
		if(count < 100)
		{
			ret1 = 1;
		}
		else
		{
			ret1 = 0;
		}

		qmi8658_write_reg(Qmi8658Register_Ctrl9, qmi8658_Ctrl9_Cmd_Ack);	// write commond  0x00 to ctrl9
		count = 0;
		qmi8658_read_reg(status_reg, &status1, 1);
		while(((status1&cmd_done)==cmd_done)&&(count++<100))		// read statusINT until bit7 is 0
		{
			qmi8658_delay(1);	// 1 ms
			qmi8658_read_reg(status_reg, &status1, 1);
		}
		//qmi8658_log("ctrl9 cmd (%d) done2 count=%d\n", qmi8658_Ctrl9_Cmd_Ack, count);
		if(count < 100)
		{
			ret2 = 1;
		}
		else
		{
			ret2 = 0;
		}

		if((ret1==0) || (ret2==0))
		{
			continue;
		}
		else
		{
			break;
		}
	}

	if(ret1 && ret2)
	{
		return 1;
	}
	else
	{
		qmi8658_log("qmi8658_send_ctl9cmd fail cmd=%d\n", cmd);
		return 0;
	}
}

unsigned char qmi8658_readStatusInt(void)
{
	unsigned char status_int;

	qmi8658_read_reg(Qmi8658Register_StatusInt, &status_int, 1);

	return status_int;
}

unsigned char qmi8658_readStatus0(void)
{
	unsigned char status0;

	qmi8658_read_reg(Qmi8658Register_Status0, &status0, 1);

	return status0;
}

unsigned char qmi8658_readStatus1(void)
{
	unsigned char status1;
	
	qmi8658_read_reg(Qmi8658Register_Status1, &status1, 1);

	return status1;
}

float qmi8658_readTemp(void)
{
	unsigned char buf[2];
	short temp = 0;
	float temp_f = 0;

	qmi8658_read_reg(Qmi8658Register_Tempearture_L, buf, 2);
	temp = ((short)buf[1]<<8)|buf[0];
	temp_f = (float)temp/256.0f;

	return temp_f;
}

void qmi8658_read_timestamp(unsigned int *tim_count)
{
	unsigned char	buf[3];
	unsigned int timestamp;

	if(tim_count)
	{
		qmi8658_read_reg(Qmi8658Register_Timestamp_L, buf, 3);
		timestamp = (unsigned int)(((unsigned int)buf[2]<<16)|((unsigned int)buf[1]<<8)|buf[0]);
		if(timestamp > g_imu.timestamp)
			g_imu.timestamp = timestamp;
		else
			g_imu.timestamp = (timestamp+0x1000000-g_imu.timestamp);

		*tim_count = g_imu.timestamp;		
	}
}

void qmi8658_read_sensor_data(float acc[3], float gyro[3])
{
	unsigned char	buf_reg[12];
	short 			raw_acc_xyz[3];
	short 			raw_gyro_xyz[3];

	qmi8658_read_reg(Qmi8658Register_Ax_L, buf_reg, 12);
	raw_acc_xyz[0] = (short)((unsigned short)(buf_reg[1]<<8) |( buf_reg[0]));
	raw_acc_xyz[1] = (short)((unsigned short)(buf_reg[3]<<8) |( buf_reg[2]));
	raw_acc_xyz[2] = (short)((unsigned short)(buf_reg[5]<<8) |( buf_reg[4]));

	raw_gyro_xyz[0] = (short)((unsigned short)(buf_reg[7]<<8) |( buf_reg[6]));
	raw_gyro_xyz[1] = (short)((unsigned short)(buf_reg[9]<<8) |( buf_reg[8]));
	raw_gyro_xyz[2] = (short)((unsigned short)(buf_reg[11]<<8) |( buf_reg[10]));

#if defined(QMI8658_UINT_MG_DPS)
	// mg
	acc[0] = (float)(raw_acc_xyz[0]*1000.0f)/g_imu.ssvt_a;
	acc[1] = (float)(raw_acc_xyz[1]*1000.0f)/g_imu.ssvt_a;
	acc[2] = (float)(raw_acc_xyz[2]*1000.0f)/g_imu.ssvt_a;
#else
	// m/s2
	acc[0] = (float)(raw_acc_xyz[0]*ONE_G)/g_imu.ssvt_a;
	acc[1] = (float)(raw_acc_xyz[1]*ONE_G)/g_imu.ssvt_a;
	acc[2] = (float)(raw_acc_xyz[2]*ONE_G)/g_imu.ssvt_a;
#endif

#if defined(QMI8658_UINT_MG_DPS)
	// dps
	gyro[0] = (float)(raw_gyro_xyz[0]*1.0f)/g_imu.ssvt_g;
	gyro[1] = (float)(raw_gyro_xyz[1]*1.0f)/g_imu.ssvt_g;
	gyro[2] = (float)(raw_gyro_xyz[2]*1.0f)/g_imu.ssvt_g;
#else
	// rad/s
	gyro[0] = (float)(raw_gyro_xyz[0]*M_PI)/(g_imu.ssvt_g*180);		// *pi/180
	gyro[1] = (float)(raw_gyro_xyz[1]*M_PI)/(g_imu.ssvt_g*180);
	gyro[2] = (float)(raw_gyro_xyz[2]*M_PI)/(g_imu.ssvt_g*180);
#endif
}

void qmi8658_read_xyz(float acc[3], float gyro[3])
{
	unsigned char	status = 0;
	unsigned char data_ready = 0;
	int	retry = 0;

	while(retry++ < 3)
	{
#if defined(QMI8658_SYNC_SAMPLE_MODE)
		qmi8658_read_reg(Qmi8658Register_StatusInt, &status, 1);
		if((status&0x01)&&(status&0x02))
		{
			data_ready = 1;
			qmi8658_delay_us(12);	// delay 12us <=500Hz�� 12us 1000Hz, 4us 2000Hz 2us > 2000Hz
			break;
		}
#else
		qmi8658_read_reg(Qmi8658Register_Status0, &status, 1);
		//qmi8658_log("status0 0x%x\n", status);
		if(status&0x03)
		{
			data_ready = 1;
			break;
		}
#endif
	}

	if(data_ready)
	{
		qmi8658_read_sensor_data(acc, gyro);
		qmi8658_axis_convert(acc, gyro, 0);
#if defined(QMI8658_USE_CALI)
		qmi8658_data_cali(1, acc);
		qmi8658_data_cali(2, gyro);
#endif
		g_imu.imu[0] = acc[0];
		g_imu.imu[1] = acc[1];
		g_imu.imu[2] = acc[2];
		g_imu.imu[3] = gyro[0];
		g_imu.imu[4] = gyro[1];
		g_imu.imu[5] = gyro[2];
	}
	else
	{
		acc[0] = g_imu.imu[0];
		acc[1] = g_imu.imu[1];
		acc[2] = g_imu.imu[2];
		gyro[0] = g_imu.imu[3];
		gyro[1] = g_imu.imu[4];
		gyro[2] = g_imu.imu[5];
		qmi8658_log("data ready fail!\n");
	}
}

#if defined(QMI8658_SYNC_SAMPLE_MODE)
void qmi8658_enable_AHB_clock(int enable)
{
	if(enable)
	{
		qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);
		qmi8658_write_reg(Qmi8658Register_Cal1_L, 0x00);
		qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_AHB_Clock_Gating);
	}
	else
	{
		qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);
		qmi8658_write_reg(Qmi8658Register_Cal1_L, 0x01);
		qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_AHB_Clock_Gating);
	}
}
#endif

void qmi8658_enableSensors(unsigned char enableFlags)
{
#if defined(QMI8658_SYNC_SAMPLE_MODE)
	qmi8658_write_reg(Qmi8658Register_Ctrl7, enableFlags | 0x80);
#else
	qmi8658_write_reg(Qmi8658Register_Ctrl7, enableFlags);		// QMI8658_DRDY_DISABLE
#endif
	g_imu.cfg.enSensors = enableFlags&0x03;

	qmi8658_delay(2);
}

void qmi8658_dump_reg(void)
{
#if 1
    unsigned char read_data[8];
 
    qmi8658_read_reg(Qmi8658Register_Ctrl1, read_data, 8);
    qmi8658_log("%s:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
    	__func__, 
			read_data[0], read_data[1], read_data[2], read_data[3], \
				read_data[4], read_data[5], read_data[6], read_data[7]);
#else
	unsigned char read_data[8];

	for(int i=0; i<=Qmi8658Register_Reset; i++)
	{
		qmi8658_read_reg(i, read_data, 1);
		qmi8658_log("reg %02d = 0x%x\n", i, read_data[0]);
	}
#endif
}

void qmi8658_get_chip_info(void)
{
	unsigned char revision_id = 0x00;
	unsigned char firmware_id[3];
	unsigned char uuid[6];
	unsigned int uuid_low, uuid_high;

	qmi8658_read_reg(Qmi8658Register_Revision, &revision_id, 1);
	qmi8658_read_reg(Qmi8658Register_firmware_id, firmware_id, 3);
	qmi8658_read_reg(Qmi8658Register_uuid, uuid, 6);
	uuid_low = (unsigned int)((unsigned int)(uuid[2]<<16)|(unsigned int)(uuid[1]<<8)|(uuid[0]));
	uuid_high = (unsigned int)((unsigned int)(uuid[5]<<16)|(unsigned int)(uuid[4]<<8)|(uuid[3]));
	//qmi8658_log("VS ID[0x%x]\n", revision_id);
	qmi8658_log("**FW ID[%d %d %d] Revision;0x%x\n", \
		firmware_id[2], firmware_id[1],firmware_id[0],revision_id);
	qmi8658_log("**UUID[0x%x %x]\n", uuid_high ,uuid_low);
}

void qmi8658_soft_reset(void)
{
	unsigned char reset_done = 0x00;
	int retry = 0;

	qmi8658_log("qmi8658_soft_reset \n");
	qmi8658_write_reg(Qmi8658Register_Reset, 0xb0);
	qmi8658_delay(10);	// delay	
	while(reset_done != 0x80)
	{
		qmi8658_delay(1);
		qmi8658_read_reg(Qmi8658Register_Reset_done, &reset_done, 1);
		if(retry++ > 500)
		{
			break;
		}
	}
	qmi8658_log("qmi8658_soft_reset done retry=%d\n", retry);
}

void qmi8658_get_gyro_gain(unsigned char cod_data[6])
{
	qmi8658_read_reg(Qmi8658Register_Dvx_L, &cod_data[0], 6);
	qmi8658_log("cod data[0x%x 0x%x 0x%x 0x%x 0x%x 0x%x]\n", \
		cod_data[0], cod_data[1], cod_data[2], cod_data[3], \
			cod_data[4], cod_data[5]);
}

#if 0
void qmi8658_apply_gyr_gain(unsigned char cod_data[6])
{
	qmi8658_enableSensors(QMI8658_DISABLE_ALL);
	qmi8658_write_reg(Qmi8658Register_Cal1_L, cod_data[0]);
	qmi8658_write_reg(Qmi8658Register_Cal1_H, cod_data[1]);
	qmi8658_write_reg(Qmi8658Register_Cal2_L, cod_data[2]);
	qmi8658_write_reg(Qmi8658Register_Cal2_H, cod_data[3]);
	qmi8658_write_reg(Qmi8658Register_Cal3_L, cod_data[4]);
	qmi8658_write_reg(Qmi8658Register_Cal3_H, cod_data[5]);

	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_Apply_Gyro_Gain);
}
#endif

void qmi8658_on_demand_cali(void)
{
	unsigned char cod_status = 0x00;

	qmi8658_log("qmi8658_on_demand_cali start\n");
	qmi8658_write_reg(Qmi8658Register_Ctrl9, (unsigned char)qmi8658_Ctrl9_Cmd_On_Demand_Cali);
	qmi8658_delay(2200);	// delay 2000ms above
	qmi8658_write_reg(Qmi8658Register_Ctrl9, (unsigned char)qmi8658_Ctrl9_Cmd_Ack);
	qmi8658_delay(10);		// delay
	qmi8658_read_reg(Qmi8658Register_Cod_Status, &cod_status, 1);
	if(cod_status)
	{
		qmi8658_log("qmi8658_on_demand_cali fail! status=0x%x\n", cod_status);
	}
	else
	{
		qmi8658_get_gyro_gain(g_imu.cod_data);
		qmi8658_log("qmi8658_on_demand_cali done! cod[%d %d %d]\n", 
			(unsigned short)(g_imu.cod_data[1]<<8|g_imu.cod_data[0]),
				(unsigned short)(g_imu.cod_data[3]<<8|g_imu.cod_data[2]),
					(unsigned short)(g_imu.cod_data[5]<<8|g_imu.cod_data[4]));
	}
}

void qmi8658_config_reg(unsigned char low_power)
{
	qmi8658_enableSensors(QMI8658_DISABLE_ALL);
	if(low_power)
	{
		g_imu.cfg.enSensors = QMI8658_ACC_ENABLE;
		g_imu.cfg.accRange = Qmi8658AccRange_8g;
		g_imu.cfg.accOdr = Qmi8658AccOdr_LowPower_21Hz;
		g_imu.cfg.gyrRange = Qmi8658GyrRange_1024dps;
		g_imu.cfg.gyrOdr = Qmi8658GyrOdr_125Hz;
	}else
	{
		g_imu.cfg.enSensors = QMI8658_ACCGYR_ENABLE;
		g_imu.cfg.accRange = Qmi8658AccRange_8g;
		g_imu.cfg.accOdr = Qmi8658AccOdr_62_5Hz;
		g_imu.cfg.gyrRange = Qmi8658GyrRange_1024dps;
		g_imu.cfg.gyrOdr = Qmi8658GyrOdr_62_5Hz;
	}
	
	if(g_imu.cfg.enSensors & QMI8658_ACC_ENABLE)
	{
		qmi8658_config_acc(g_imu.cfg.accRange, g_imu.cfg.accOdr, \
			Qmi8658Lpf_Disable, Qmi8658St_Disable);
	}

	if(g_imu.cfg.enSensors & QMI8658_GYR_ENABLE)
	{
		qmi8658_config_gyro(g_imu.cfg.gyrRange, g_imu.cfg.gyrOdr, \
			Qmi8658Lpf_Disable, Qmi8658St_Disable);
	}
}


unsigned char qmi8658_get_id(void)
{
	unsigned char qmi8658_chip_id = 0x00;
	unsigned char qmi8658_slave[2] = {QMI8658_SLAVE_ADDR_H, QMI8658_SLAVE_ADDR_L};
	int retry = 0;
	unsigned char iCount = 0;

	while(iCount<2)
	{
		g_imu.slave = qmi8658_slave[iCount];
		retry = 0;
		while((qmi8658_chip_id != 0x05)&&(retry++ < 5))
		{
			qmi8658_read_reg(Qmi8658Register_WhoAmI, &qmi8658_chip_id, 1);
		}
		qmi8658_log("qmi8658 slave = 0x%x WhoAmI = 0x%x\n", g_imu.slave, qmi8658_chip_id);
		if(qmi8658_chip_id == 0x05)
		{
			g_imu.cfg.syncSample = 0;
			g_imu.cfg.ctrl8_value = 0xc0;
			qmi8658_soft_reset();
			qmi8658_write_reg(Qmi8658Register_Ctrl1, \
				0x60|QMI8658_INT2_ENABLE|QMI8658_INT1_ENABLE);
			// qmi8658_write_reg(Qmi8658Register_Ctrl1, \
			// 	0x64|QMI8658_INT1_ENABLE);
			qmi8658_get_chip_info();
#if defined(QMI8658_USE_GYRO_STARTUP_TEST)	// check 0x45
			unsigned char opt_status = 0x00;
			qmi8658_write_reg(Qmi8658Register_Ctrl2, 0x25);
			qmi8658_write_reg(Qmi8658Register_Ctrl3, 0x66);
			qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x03);
			qmi8658_delay(300);
			qmi8658_read_reg(0x45, &opt_status, 1);
			//qmi8658_log("opt_status = 0x%x\n", opt_status);
			if(opt_status != 0x80)
			{
				qmi8658_log("**ERROR[0x45=0x%x]\n", opt_status);
				return 0;
			}
			else
			{
				qmi8658_log("**SUCCESS[0x45=0x%x]\n", opt_status);
			}
			qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);
#endif

#if defined(QMI8658_USE_HW_SELFTEST)
			qmi8658_do_hw_selftest(QMI8658_ACCGYR_ENABLE);
			//qmi8658_soft_reset();
			//qmi8658_write_reg(Qmi8658Register_Ctrl1, 0x60|QMI8658_INT2_ENABLE|QMI8658_INT1_ENABLE);
#endif
			//qmi8658_on_demand_cali();
			qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);
			qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
			break;
		}
		iCount++;
	}
	if(qmi8658_chip_id != 0x05)
	{		
		qmi8658_log("**ERROR1[id=0x%x]\n", qmi8658_chip_id);
	}

	return qmi8658_chip_id;
}

#if defined(QMI8658_USE_WOM)
void qmi8658_enable_wom(int enable, enum qmi8658_Interrupt int_map)
{
	unsigned char ctrl1 = 0x00;
	unsigned char wom_int = 0x00;

	if(enable)
	{
		wom_int |= 0x0a;	// Blanking Time
		if(int_map == qmi8658_Int2)
		{
			wom_int |= (0x01<<6);		// map to int2
		}
		//int_map |= (0x01<<7); 	// int iniial state is high
		//qmi8658_soft_reset();
		qmi8658_enableSensors(QMI8658_DISABLE_ALL);
		qmi8658_delay(2);
		qmi8658_config_acc(Qmi8658AccRange_8g, Qmi8658AccOdr_31_25Hz, \
			Qmi8658Lpf_Disable, Qmi8658St_Disable);
		qmi8658_write_reg(Qmi8658Register_Cal1_L, 255);		// threshold  mg
		qmi8658_write_reg(Qmi8658Register_Cal1_H, wom_int);
		qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_WoM_Setting);
		qmi8658_enableSensors(QMI8658_ACC_ENABLE);

		qmi8658_read_reg(Qmi8658Register_Ctrl1, &ctrl1, 1);
		if(int_map == qmi8658_Int1)
		{
			ctrl1 |= QMI8658_INT1_ENABLE;
		}
		else if(int_map == qmi8658_Int2)
		{
			ctrl1 |= QMI8658_INT2_ENABLE;
		}
		qmi8658_write_reg(Qmi8658Register_Ctrl1, ctrl1);// enable int for dev-E
	}
	else
	{
		qmi8658_enableSensors(QMI8658_DISABLE_ALL);
		qmi8658_write_reg(Qmi8658Register_Cal1_L, 0);
		qmi8658_write_reg(Qmi8658Register_Cal1_H, 0);
		qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_WoM_Setting);
	}
}
#endif

#if defined(QMI8658_USE_AMD)||defined(QMI8658_USE_NO_MOTION)||defined(QMI8658_USE_SIG_MOTION)
void qmi8658_config_motion(void)
{
	g_imu.cfg.ctrl8_value = 0xc0;	// &= (~QMI8658_CTRL8_ANYMOTION_EN);
	qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
	qmi8658_delay(2);
	qmi8658_enableSensors(QMI8658_DISABLE_ALL);

	qmi8658_write_reg(Qmi8658Register_Cal1_L, 0x03);		// any motion X threshold(uint 1/32 g)
	qmi8658_write_reg(Qmi8658Register_Cal1_H, 0x03);		// any motion Y threshold(uint 1/32 g)
	qmi8658_write_reg(Qmi8658Register_Cal2_L, 0x03);		// any motion Z threshold(uint 1/32 g)
	qmi8658_write_reg(Qmi8658Register_Cal2_H, 0x02);		// no motion X threshold(uint 1/32 g)
	qmi8658_write_reg(Qmi8658Register_Cal3_L, 0x02);		// no motion X threshold(uint 1/32 g)
	qmi8658_write_reg(Qmi8658Register_Cal3_H, 0x02);		// no motion X threshold(uint 1/32 g)

	qmi8658_write_reg(Qmi8658Register_Cal4_L, 0xf7);		// MOTION_MODE_CTRL
	qmi8658_write_reg(Qmi8658Register_Cal4_H, 0x01);		// value 0x01

	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_Motion);

	qmi8658_write_reg(Qmi8658Register_Cal1_L, 0x03);		// AnyMotionWindow. 
	qmi8658_write_reg(Qmi8658Register_Cal1_H, 0xff);		// NoMotionWindow 
	qmi8658_write_reg(Qmi8658Register_Cal2_L, 0x2c);		// SigMotionWaitWindow[7:0]
	qmi8658_write_reg(Qmi8658Register_Cal2_H, 0x01);		// SigMotionWaitWindow [15:8]
	qmi8658_write_reg(Qmi8658Register_Cal3_L, 0x64);		// SigMotionConfirmWindow[7:0]
	qmi8658_write_reg(Qmi8658Register_Cal3_H, 0x00);		// SigMotionConfirmWindow[15:8]
	//qmi8658_write_reg(Qmi8658Register_Cal4_L, 0xf7);
	qmi8658_write_reg(Qmi8658Register_Cal4_H, 0x02);		// value 0x02

	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_Motion);
}

#if defined(QMI8658_USE_AMD)
void qmi8658_enable_amd(unsigned char enable, enum qmi8658_Interrupt int_map, unsigned char low_power)
{
	if(enable)
	{
		unsigned char ctrl1;

		qmi8658_write_reg(Qmi8658Register_Ctrl8, 0xc0);
		qmi8658_enableSensors(QMI8658_DISABLE_ALL);
		qmi8658_config_reg(low_power);

		qmi8658_read_reg(Qmi8658Register_Ctrl1, &ctrl1, 1);
		if(int_map == qmi8658_Int1)
		{
			ctrl1 |= QMI8658_INT1_ENABLE;
			g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_INT_SEL;
		}
		else if(int_map == qmi8658_Int2)
		{
			ctrl1 |= QMI8658_INT2_ENABLE;
			g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_INT_SEL);
		}
		qmi8658_write_reg(Qmi8658Register_Ctrl1, ctrl1);// enable int for dev-E

		g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_ANYMOTION_EN;
		qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
		qmi8658_delay(1);
		qmi8658_enableSensors(g_imu.cfg.enSensors);
	}
	else
	{
		g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_ANYMOTION_EN);
		qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
	}
}
#endif

#if defined(QMI8658_USE_NO_MOTION)
void qmi8658_enable_no_motion(unsigned char enable, enum qmi8658_Interrupt int_map)
{
	if(enable)
	{
		unsigned char ctrl1;

		qmi8658_enableSensors(QMI8658_DISABLE_ALL);

		qmi8658_read_reg(Qmi8658Register_Ctrl1, &ctrl1, 1);
		if(int_map == qmi8658_Int1)
		{
			ctrl1 |= QMI8658_INT1_ENABLE;
			g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_INT_SEL;
		}
		else if(int_map == qmi8658_Int2)
		{
			ctrl1 |= QMI8658_INT2_ENABLE;
			g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_INT_SEL);
		}
		qmi8658_write_reg(Qmi8658Register_Ctrl1, ctrl1);// enable int for dev-E

		// recommend odr		
		qmi8658_config_acc(Qmi8658AccRange_8g, Qmi8658AccOdr_62_5Hz, Qmi8658Lpf_Disable, Qmi8658St_Disable);
		qmi8658_config_gyro(Qmi8658GyrRange_1024dps, Qmi8658GyrOdr_62_5Hz, Qmi8658Lpf_Disable, Qmi8658St_Disable);

		g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_NOMOTION_EN;
		qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
		qmi8658_delay(1);

		qmi8658_enableSensors(QMI8658_ACCGYR_ENABLE);
	}
	else
	{
		g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_NOMOTION_EN);
		qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
	}
}
#endif

#if defined(QMI8658_USE_SIG_MOTION)
void qmi8658_enable_sig_motion(unsigned char enable, enum qmi8658_Interrupt int_map)
{
	if(enable)
	{
		unsigned char ctrl1;

		qmi8658_enableSensors(QMI8658_DISABLE_ALL);

		qmi8658_read_reg(Qmi8658Register_Ctrl1, &ctrl1, 1);
		if(int_map == qmi8658_Int1)
		{
			ctrl1 |= QMI8658_INT1_ENABLE;
			g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_INT_SEL;
		}
		else if(int_map == qmi8658_Int2)
		{
			ctrl1 |= QMI8658_INT2_ENABLE;
			g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_INT_SEL);
		}		
		qmi8658_write_reg(Qmi8658Register_Ctrl1, ctrl1);// enable int for dev-E

		g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_SIGMOTION_EN|QMI8658_CTRL8_ANYMOTION_EN|QMI8658_CTRL8_NOMOTION_EN;
		qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
		qmi8658_delay(1);

		qmi8658_enableSensors(QMI8658_ACCGYR_ENABLE);
	}
	else
	{
		g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_SIGMOTION_EN);
		qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
	}
}
#endif

#endif

#if defined(QMI8658_USE_TAP)
unsigned char qmi8658_readTapStatus(void)
{
	unsigned char status;
	
	qmi8658_read_reg(Qmi8658Register_Tap_Status, &status, 1);

	return status;
}

void qmi8658_config_tap(void)
{
	unsigned char peakWindow = 0x1e;	//0x1e;
	unsigned char priority = 0x05;
	unsigned short TapWindow = 100;
	unsigned short DTapWindow = 500;

	unsigned char alpha = 0x08;
	unsigned char gamma = 0x20;
	unsigned short peakMagThr = 0x0599;
	unsigned short UDMThr = 0x0199;

	qmi8658_write_reg(Qmi8658Register_Cal1_L, peakWindow & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal1_H, priority & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal2_L, TapWindow & 0x00FF);
	qmi8658_write_reg(Qmi8658Register_Cal2_H, (TapWindow >> 8) & 0x00FF);
	qmi8658_write_reg(Qmi8658Register_Cal3_L, DTapWindow & 0x00FF);
	qmi8658_write_reg(Qmi8658Register_Cal3_H, (DTapWindow >> 8) & 0x00FF);
	qmi8658_write_reg(Qmi8658Register_Cal4_H, 0x01);
	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_EnableTap);

	qmi8658_write_reg(Qmi8658Register_Cal1_L, alpha & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal1_H, gamma & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal2_L, peakMagThr & 0x00FF);
	qmi8658_write_reg(Qmi8658Register_Cal2_H, (peakMagThr>>8) & 0x00FF);
	qmi8658_write_reg(Qmi8658Register_Cal3_L, UDMThr & 0x00FF);
	qmi8658_write_reg(Qmi8658Register_Cal3_H, (UDMThr>>8) & 0x00FF);
	qmi8658_write_reg(Qmi8658Register_Cal4_H, 0x02);
	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_EnableTap);
}

void qmi8658_enable_tap(unsigned char enable, enum qmi8658_Interrupt int_map)
{
	if(enable)
	{
		unsigned char ctrl1;

		qmi8658_enableSensors(QMI8658_DISABLE_ALL);

		qmi8658_read_reg(Qmi8658Register_Ctrl1, &ctrl1, 1);
		if(int_map == qmi8658_Int1)
		{
			ctrl1 |= QMI8658_INT1_ENABLE;
			g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_INT_SEL;
		}
		else if(int_map == qmi8658_Int2)
		{
			ctrl1 |= QMI8658_INT2_ENABLE;
			g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_INT_SEL);
		}
		qmi8658_write_reg(Qmi8658Register_Ctrl1, ctrl1);// enable int for dev-E

		g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_TAP_EN;
		qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
		qmi8658_delay(1);

		qmi8658_enableSensors(QMI8658_ACCGYR_ENABLE);
	}
	else
	{
		g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_TAP_EN);
		qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
	}

}
#endif

#if defined(QMI8658_USE_PEDOMETER)
void qmi8658_config_pedometer(unsigned short odr)
{
	float Rate = (float)(1000.0f/odr);
	unsigned short ped_sample_cnt = (unsigned short)(0x0032);//6;//(unsigned short)(0x0032 / finalRate) ;
	unsigned short ped_fix_peak2peak = 100;	//0x00AC;//0x0006;//0x00CC;
	unsigned short ped_fix_peak = 116;	//0x00AC;//0x0006;//0x00CC;
	unsigned short ped_time_up = (unsigned short)(2000 / Rate);
	unsigned char ped_time_low = (unsigned char) (300 / Rate);
	unsigned char ped_time_cnt_entry = 8;
	unsigned char ped_fix_precision = 0;
	unsigned char ped_sig_count = 1;//�Ʋ�����1

	qmi8658_write_reg(Qmi8658Register_Cal1_L, ped_sample_cnt & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal1_H, (ped_sample_cnt >> 8) & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal2_L, ped_fix_peak2peak & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal2_H, (ped_fix_peak2peak >> 8) & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal3_L, ped_fix_peak & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal3_H, (ped_fix_peak >> 8) & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal4_H, 0x01);
	qmi8658_write_reg(Qmi8658Register_Cal4_L, 0x02);
	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_EnablePedometer);

	qmi8658_write_reg(Qmi8658Register_Cal1_L, ped_time_up & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal1_H, (ped_time_up >> 8) & 0xFF);
	qmi8658_write_reg(Qmi8658Register_Cal2_L, ped_time_low);
	qmi8658_write_reg(Qmi8658Register_Cal2_H, ped_time_cnt_entry);
	qmi8658_write_reg(Qmi8658Register_Cal3_L, ped_fix_precision);
	qmi8658_write_reg(Qmi8658Register_Cal3_H, ped_sig_count);
	qmi8658_write_reg(Qmi8658Register_Cal4_H, 0x02);
	qmi8658_write_reg(Qmi8658Register_Cal4_L, 0x02);
	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_EnablePedometer);
}

void qmi8658_enable_pedometer(unsigned char enable)
{
	if(enable)
	{
		g_imu.cfg.ctrl8_value |= QMI8658_CTRL8_PEDOMETER_EN;
	}
	else
	{
		g_imu.cfg.ctrl8_value &= (~QMI8658_CTRL8_PEDOMETER_EN);
	}
	qmi8658_write_reg(Qmi8658Register_Ctrl8, g_imu.cfg.ctrl8_value);
}

unsigned int qmi8658_read_pedometer(void)
{
	unsigned char buf[3];

    qmi8658_read_reg(Qmi8658Register_Pedo_L, buf, 3);	// 0x5a
	g_imu.step = (unsigned int)((buf[2]<<16)|(buf[1]<<8)|(buf[0]));

	return g_imu.step;
}

void qmi8658_reset_pedometer(void)
{
	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_ResetPedometer);
}
#endif

#if defined(QMI8658_USE_FIFO)
void qmi8658_config_fifo(unsigned char watermark,enum qmi8658_FifoSize size, \
	enum qmi8658_FifoMode mode, enum qmi8658_Interrupt int_map, uint8_t sensor)
{
	unsigned char ctrl1;

	qmi8658_write_reg(Qmi8658Register_FifoCtrl, 0x00);
	qmi8658_enableSensors(QMI8658_DISABLE_ALL);
	qmi8658_delay(2);
	qmi8658_read_reg(Qmi8658Register_Ctrl1, &ctrl1, 1);
	if(int_map == qmi8658_Int1)
	{
		ctrl1 |= QMI8658_FIFO_MAP_INT1;
	}
	else if(int_map == qmi8658_Int2)
	{
		ctrl1 &= QMI8658_FIFO_MAP_INT2;
	}
	qmi8658_write_reg(Qmi8658Register_Ctrl1, ctrl1);

	g_imu.cfg.fifo_ctrl = (unsigned char)(size | mode);
	qmi8658_write_reg(Qmi8658Register_FifoCtrl, g_imu.cfg.fifo_ctrl);
	qmi8658_write_reg(Qmi8658Register_FifoWmkTh, watermark);

	qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_Rst_Fifo);
	qmi8658_enableSensors(sensor);
}

unsigned short qmi8658_read_fifo(unsigned char* data)
{
	unsigned char fifo_status[2] = {0,0};
	unsigned char fifo_sensors = 1;
	unsigned short fifo_bytes = 0;
	unsigned short fifo_level = 0;
	
	if((g_imu.cfg.fifo_ctrl&0x03)!=qmi8658_Fifo_Bypass)
	{
		//qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_Req_Fifo);
		qmi8658_read_reg(Qmi8658Register_FifoCount, fifo_status, 2);
		fifo_bytes = (unsigned short)(((fifo_status[1]&0x03)<<8)|fifo_status[0]);
		if((g_imu.cfg.enSensors == QMI8658_ACC_ENABLE)||(g_imu.cfg.enSensors == QMI8658_GYR_ENABLE))
		{
			fifo_sensors = 1;
		}
		else if(g_imu.cfg.enSensors == QMI8658_ACCGYR_ENABLE)
		{
			fifo_sensors = 2;
		}
		fifo_level = fifo_bytes/(3*fifo_sensors);
		fifo_bytes = fifo_level*(6*fifo_sensors);
		//qmi8658_log("fifo-level : %d\n", fifo_level);
		if(fifo_level > 0)
		{	
			qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_Req_Fifo);
#if 0
			for(int i=0; i<fifo_level; i++)
			{
				qmi8658_read_reg(Qmi8658Register_FifoData, &data[i*fifo_sensors*6], fifo_sensors*6);
			}
#else
			qmi8658_read_reg(Qmi8658Register_FifoData, data, fifo_bytes);
#endif
			qmi8658_write_reg(Qmi8658Register_FifoCtrl, g_imu.cfg.fifo_ctrl);
		}
		//qmi8658_send_ctl9cmd(qmi8658_Ctrl9_Cmd_Rst_Fifo);
	}

	return fifo_level;
}
#endif

#if defined(QMI8658_USE_HW_SELFTEST)
void qmi8658_do_hw_selftest(int enSensor)
{
	unsigned char	status_int = 0x00;
	unsigned int	retry = 0;
	unsigned char	reg[6];
	short	raw[3];
	float	st_out[3];

	if(enSensor & QMI8658_ACC_ENABLE)
	{
		qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);
		qmi8658_delay(2);
		qmi8658_write_reg(Qmi8658Register_Ctrl2, \
			Qmi8658AccRange_8g|Qmi8658AccOdr_250Hz|0x80);
		status_int = 0;
		retry = 0;
		while(!(status_int & 0x01))
		{
			qmi8658_read_reg(Qmi8658Register_StatusInt, &status_int, 1);
			qmi8658_delay(1);
			//qmi8658_log("count=%d ",retry);
			if(retry++ > 5000)
			{
				qmi8658_log("wati int high timeout\n");
				break;
			}
		}
		qmi8658_write_reg(Qmi8658Register_Ctrl2, Qmi8658AccRange_8g|Qmi8658AccOdr_250Hz);
		retry = 0;
		status_int = 0x01;
		while((status_int & 0x01))
		{
			qmi8658_read_reg(Qmi8658Register_StatusInt, &status_int, 1);
			qmi8658_delay(1);
			//qmi8658_log("count=%d ",retry);
			if(retry++ > 5000)
			{
				qmi8658_log("wati int low timeout\n");
				break;
			}
		}
		qmi8658_read_reg(Qmi8658Register_Dvx_L, reg, 6);
		raw[0] = (short)((unsigned short)(reg[1]<<8) |( reg[0]));
		raw[1] = (short)((unsigned short)(reg[3]<<8) |( reg[2]));
		raw[2] = (short)((unsigned short)(reg[5]<<8) |( reg[4]));
		st_out[0] = (float)(raw[0]*1000.0f/2048);	// mg
		st_out[1] = (float)(raw[1]*1000.0f/2048);
		st_out[2] = (float)(raw[2]*1000.0f/2048);
		g_imu.st_out[0] = st_out[0];
		g_imu.st_out[1] = st_out[1];
		g_imu.st_out[2] = st_out[2];
		if((QFABS(st_out[0]) > 200) && (QFABS(st_out[1]) > 200) && (QFABS(st_out[2]) > 200))
		{
			qmi8658_log("acc-selftest raw[%d %d	%d] out[%f %f %f] Pass!\n", \
				raw[0],raw[1],raw[2],st_out[0],st_out[1],st_out[2]);
		}
		else
		{
			qmi8658_log("acc-selftest raw[%d	%d	%d] out[%f	%f	%f] Fail!\n", \
				raw[0],raw[1],raw[2],st_out[0],st_out[1],st_out[2]);
		}
	}

	if(enSensor & QMI8658_GYR_ENABLE)
	{	
		qmi8658_write_reg(Qmi8658Register_Ctrl7, 0x00);
		qmi8658_delay(2);
		qmi8658_write_reg(Qmi8658Register_Ctrl3, Qmi8658GyrRange_1024dps|Qmi8658GyrOdr_250Hz|0x80);
		status_int = 0;
		retry = 0;
		while(!(status_int & 0x01))
		{
			qmi8658_read_reg(Qmi8658Register_StatusInt, &status_int, 1);
			qmi8658_delay(1);
			if(retry++ > 5000)
			{
				qmi8658_log("wati int high timeout\n");
				break;
			}
		}
		qmi8658_write_reg(Qmi8658Register_Ctrl3, Qmi8658GyrRange_1024dps|Qmi8658GyrOdr_250Hz);
		retry = 0;
		status_int = 0x01;
		while((status_int & 0x01))
		{
			qmi8658_read_reg(Qmi8658Register_StatusInt, &status_int, 1);
			qmi8658_delay(1);
			if(retry++ > 5000)
			{
				qmi8658_log("wati int low timeout\n");
				break;
			}
		}
		qmi8658_read_reg(Qmi8658Register_Dvx_L, reg, 6);
		raw[0] = (short)((unsigned short)(reg[1]<<8) |( reg[0]));
		raw[1] = (short)((unsigned short)(reg[3]<<8) |( reg[2]));
		raw[2] = (short)((unsigned short)(reg[5]<<8) |( reg[4]));
		st_out[0] = (float)(raw[0]/16.0f);	// dps
		st_out[1] = (float)(raw[1]/16.0f);
		st_out[2] = (float)(raw[2]/16.0f);	
		g_imu.st_out[3] = st_out[0];
		g_imu.st_out[4] = st_out[1];
		g_imu.st_out[5] = st_out[2];
		if((QFABS(st_out[0]) > 300) && (QFABS(st_out[1]) > 300) && (QFABS(st_out[2]) > 300))
		{
			qmi8658_log("gyr-selftest raw[%d %d	%d] out[%f %f %f] Pass!\n", \
				raw[0],raw[1],raw[2],st_out[0],st_out[1],st_out[2]);
		}
		else
		{
			qmi8658_log("gyr-selftest raw[%d %d	%d] out[%f %f %f] Fail!\n", \
				raw[0],raw[1],raw[2],st_out[0],st_out[1],st_out[2]);
		}
	}
}

#if 0
void qmi8658_get_hw_selftest_data(float out[6])
{
	out[0] = g_imu.st_out[0];
	out[1] = g_imu.st_out[1];
	out[2] = g_imu.st_out[2];
	out[3] = g_imu.st_out[3];
	out[4] = g_imu.st_out[4];
	out[5] = g_imu.st_out[5];
}
#endif
#endif

static void Qmi8658_Fifo_WM_int_handle(u8 index, u8 gpio)
{
	int SensorGsMsg[1];
	SensorGsMsg[0] = \
		SensorGsMsgProcess;
	PostSensorGsTaskMsg(SensorGsMsg, 1);

	return;
}

unsigned char qmi8658_init(void)
{
	if(qmi8658_get_id() == 0x05)
	{
#if 1
		/*配置中断*/ 
		gpio_set_pull_up(interrupt_io_num, 0);
		gpio_set_pull_down(interrupt_io_num, 1);
		gpio_set_direction(interrupt_io_num, 1);
		gpio_set_die(interrupt_io_num, 1);
		port_edge_wkup_set_callback_by_index(4, \
			Qmi8658_Fifo_WM_int_handle);
#endif

#if 1
#if defined(QMI8658_SYNC_SAMPLE_MODE)
		qmi8658_enable_AHB_clock(0);
		g_imu.cfg.syncSample = 1;
#endif
#if defined(QMI8658_USE_AMD)||defined(QMI8658_USE_NO_MOTION)||defined(QMI8658_USE_SIG_MOTION)
		qmi8658_config_motion();
#endif
#if defined(QMI8658_USE_TAP)
		qmi8658_config_tap();
#endif
#if defined(QMI8658_USE_PEDOMETER)
		qmi8658_config_pedometer(125);
		qmi8658_enable_pedometer(1);
#endif
		qmi8658_config_reg(0);

#if defined(QMI8658_USE_FIFO)
		qmi8658_config_fifo(Qmi8658_Fifo_WM, qmi8658_Fifo_128, \
			qmi8658_Fifo_Fifo, qmi8658_Int1, QMI8658_DISABLE_ALL);
#else
		qmi8658_enableSensors(g_imu.cfg.enSensors);
#endif

		qmi8658_dump_reg();
#if defined(QMI8658_USE_CALI)
		memset(&g_cali, 0, sizeof(g_cali));
#endif
		//qmi8658_delay(300);
#endif

		return 1;
	}
	else
	{
		qmi8658_log("qmi8658_init fail\n");
		return 0;
	}
}
#endif
