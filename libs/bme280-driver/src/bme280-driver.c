#include "bme280-driver.h"
#include "bme280-regs.h"
#include <stdio.h>

static bme280_ctx_t bme280_ctx = {0};


static void bme280_read_calib()
{
    uint8_t buf[24] = {0};
    
    bme280_read_regs(BME280_REG_calib00, buf, 24);

    bme280_ctx.calib.dig_T1 = (uint16_t)(buf[1]  << 8) | buf[0];
    bme280_ctx.calib.dig_T2 = (int16_t) (buf[3]  << 8) | buf[2];
    bme280_ctx.calib.dig_T3 = (int16_t) (buf[5]  << 8) | buf[4];

    bme280_ctx.calib.dig_P1 = (uint16_t)(buf[7]  << 8) | buf[6];
    bme280_ctx.calib.dig_P2 = (int16_t) (buf[9]  << 8) | buf[8];
    bme280_ctx.calib.dig_P3 = (int16_t) (buf[11] << 8) | buf[10];
    bme280_ctx.calib.dig_P4 = (int16_t) (buf[13] << 8) | buf[12];
    bme280_ctx.calib.dig_P5 = (int16_t) (buf[15] << 8) | buf[14];
    bme280_ctx.calib.dig_P6 = (int16_t) (buf[17] << 8) | buf[16];
    bme280_ctx.calib.dig_P7 = (int16_t) (buf[19] << 8) | buf[18];
    bme280_ctx.calib.dig_P8 = (int16_t) (buf[21] << 8) | buf[20];
    bme280_ctx.calib.dig_P9 = (int16_t) (buf[23] << 8) | buf[22];

    
    uint8_t h1[1] = {0};
    bme280_read_regs(BME280_REG_calib25, h1, 1);
    bme280_ctx.calib.dig_H1 = h1[0];


    uint8_t hbuf[7] = {0};
    bme280_read_regs(BME280_REG_calib26, hbuf, 7);
    bme280_ctx.calib.dig_H2 = (int16_t) (hbuf[1] << 8) | hbuf[0];
    bme280_ctx.calib.dig_H3 = hbuf[2];
    bme280_ctx.calib.dig_H4 = (int16_t) ((hbuf[3] << 4) | (hbuf[4] & 0x0F));
    bme280_ctx.calib.dig_H5 = (int16_t) ((hbuf[5] << 4) | (hbuf[4] >> 4));
    bme280_ctx.calib.dig_H6 = (int8_t)  hbuf[6];
}

void bme280_init(bme280_i2c_read i2c_read, bme280_i2c_write i2c_write)
{
    bme280_ctx.i2c_read  = i2c_read;
    bme280_ctx.i2c_write = i2c_write;

   
    uint8_t id_buf[1] = {0};
    bme280_read_regs(BME280_REG_id, id_buf, 1);
    if (id_buf[0] != 0x60)
        printf("bme280 error: unexpected id 0x%X\n", id_buf[0]);

   
    bme280_read_calib();

    
    uint8_t ctrl_hum = 0;
    ctrl_hum |= (0b001 << 0);
    bme280_write_reg(BME280_REG_ctrl_hum, ctrl_hum);

    
    uint8_t config = 0;
    config |= (0b0   << 0);
    config |= (0b000 << 2);
    config |= (0b001 << 5);
    bme280_write_reg(BME280_REG_config, config);

   
    uint8_t ctrl_meas = 0;
    ctrl_meas |= (0b11  << 0); 
    ctrl_meas |= (0b001 << 2); 
    ctrl_meas |= (0b001 << 5); 
    bme280_write_reg(BME280_REG_ctrl_meas, ctrl_meas);
}

void bme280_read_regs(uint8_t start_reg_address, uint8_t* buffer, uint8_t length)
{
    uint8_t data[1] = {start_reg_address};
    bme280_ctx.i2c_write(data, sizeof(data));
    bme280_ctx.i2c_read(buffer, length);
}

void bme280_write_reg(uint8_t reg_address, uint8_t value)
{
    uint8_t data[2] = {reg_address, value};
    bme280_ctx.i2c_write(data, sizeof(data));
}


uint32_t bme280_read_temp_raw()
{
    uint8_t buf[3] = {0};
    bme280_read_regs(BME280_REG_temp_msb, buf, 3);
    return (uint32_t)((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));
}

uint32_t bme280_read_pres_raw()
{
    uint8_t buf[3] = {0};
    bme280_read_regs(BME280_REG_press_msb, buf, 3);
    return (uint32_t)((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));
}

uint32_t bme280_read_hum_raw()
{
    uint8_t buf[2] = {0};
    bme280_read_regs(BME280_REG_hum_msb, buf, 2);
    return (uint32_t)((buf[0] << 8) | buf[1]);
}


int32_t bme280_read_temp()
{
    int32_t adc_T = (int32_t)bme280_read_temp_raw();
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)bme280_ctx.calib.dig_T1 << 1)))
                    * ((int32_t)bme280_ctx.calib.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)bme280_ctx.calib.dig_T1))
                    * ((adc_T >> 4) - ((int32_t)bme280_ctx.calib.dig_T1))) >> 12)
                    * ((int32_t)bme280_ctx.calib.dig_T3)) >> 14;
    bme280_ctx.t_fine = var1 + var2;
    return (bme280_ctx.t_fine * 5 + 128) >> 8;  // °C * 100
}


uint32_t bme280_read_pres()
{
    bme280_read_temp(); // обновляем t_fine
    int32_t adc_P = (int32_t)bme280_read_pres_raw();
    int64_t var1 = ((int64_t)bme280_ctx.t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)bme280_ctx.calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)bme280_ctx.calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)bme280_ctx.calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)bme280_ctx.calib.dig_P3) >> 8)
         + ((var1 * (int64_t)bme280_ctx.calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bme280_ctx.calib.dig_P1) >> 33;
    if (var1 == 0) return 0;
    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)bme280_ctx.calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bme280_ctx.calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)bme280_ctx.calib.dig_P7) << 4);
    return (uint32_t)(p >> 8);  // Па
}


uint32_t bme280_read_hum()
{
    bme280_read_temp(); 
    int32_t adc_H = (int32_t)bme280_read_hum_raw();
    int32_t v = bme280_ctx.t_fine - (int32_t)76800;
    v = (((((adc_H << 14) - (((int32_t)bme280_ctx.calib.dig_H4) << 20)
        - (((int32_t)bme280_ctx.calib.dig_H5) * v)) + 16384) >> 15)
        * (((((((v * ((int32_t)bme280_ctx.calib.dig_H6)) >> 10)
        * (((v * ((int32_t)bme280_ctx.calib.dig_H3)) >> 11) + 32768)) >> 10) + 2097152)
        * ((int32_t)bme280_ctx.calib.dig_H2) + 8192) >> 14));
    v = v - (((((v >> 15) * (v >> 15)) >> 7) * ((int32_t)bme280_ctx.calib.dig_H1)) >> 4);
    v = (v < 0) ? 0 : v;
    v = (v > 419430400) ? 419430400 : v;
    return (uint32_t)(v >> 12);  
}