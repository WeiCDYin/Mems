#include "ads131.h"
#include "spi.h"
#include <string.h>

__IO uint8_t  g_ads1_ready_flag;
__IO uint8_t  g_ads2_ready_flag;
uint8_t       g_ads1_tx_buf[ADS131_FRAME_BYTES];
uint8_t       g_ads1_rx_buf[ADS131_FRAME_BYTES];
uint8_t       g_ads2_tx_buf[ADS131_FRAME_BYTES];
uint8_t       g_ads2_rx_buf[ADS131_FRAME_BYTES];
ADS131_Data_t g_ads1_data, g_ads2_data;

uint8_t ADS131_WriteReg(uint8_t ads_index, uint8_t reg_addr, uint16_t reg_val);
uint8_t ADS131_ReadReg(uint8_t ads_index, uint8_t reg_addr, uint16_t *reg_val);

static inline void ADS131_CS_Select(uint8_t ads_index)
{
    if (ads_index == ADS_INDEX_1)
        ADS131_1_CS_LOW();
    else
        ADS131_2_CS_LOW();
}

static inline void ADS131_CS_Deselect(uint8_t ads_index)
{
    if (ads_index == ADS_INDEX_1)
        ADS131_1_CS_HIGH();
    else
        ADS131_2_CS_HIGH();
}

static uint16_t ADS131_SendShortCmd(uint8_t ads_index, uint16_t cmd_word)
{
    uint8_t            tx_frame[3] = {0};
    uint8_t            rx_frame[3] = {0};
    SPI_HandleTypeDef *hspi;

    tx_frame[0] = (cmd_word >> 8) & 0xFFU;
    tx_frame[1] = cmd_word & 0xFFU;
    tx_frame[2] = 0x00U;

    if (ads_index == ADS_INDEX_1)
        hspi = &hspi1;
    else
        hspi = &hspi4;

    ADS131_CS_Select(ads_index);
    HAL_SPI_TransmitReceive(hspi, tx_frame, rx_frame, 3, 10);
    ADS131_CS_Deselect(ads_index);

    return ((uint16_t)rx_frame[0] << 8) | rx_frame[1];
}

uint8_t ADS131_WriteReg(uint8_t ads_index, uint8_t reg_addr, uint16_t reg_val)
{
    uint8_t  tx_frame[6] = {0};
    uint8_t  rx_frame[6] = {0};
    uint16_t wreg_cmd;

    wreg_cmd = ADS_CMD_WREG | ((uint16_t)reg_addr << 8) | 0x0000U;

    tx_frame[0] = (wreg_cmd >> 8) & 0xFFU;
    tx_frame[1] = wreg_cmd & 0xFFU;
    tx_frame[2] = 0x00U;

    tx_frame[3] = (reg_val >> 8) & 0xFFU;
    tx_frame[4] = reg_val & 0xFFU;
    tx_frame[5] = 0x00U;

    if (ads_index == ADS_INDEX_1) {
        ADS131_1_CS_LOW();
        HAL_SPI_TransmitReceive(&hspi1, tx_frame, rx_frame, 6, 10);
        ADS131_1_CS_HIGH();
    }
    else {
        ADS131_2_CS_LOW();
        HAL_SPI_TransmitReceive(&hspi4, tx_frame, rx_frame, 6, 10);
        ADS131_2_CS_HIGH();
    }

    // Fetch write ack
    uint16_t ack       = ADS131_SendShortCmd(ads_index, ADS_CMD_NULL);
    uint8_t  write_cnt = ack & 0x7FU;
    return (write_cnt == 0) ? 0 : 1;
}

uint8_t ADS131_ReadReg(uint8_t ads_index, uint8_t reg_addr, uint16_t *reg_val)
{
    uint16_t rreg_cmd = ADS_CMD_RREG | ((uint16_t)reg_addr << 8);
    ADS131_SendShortCmd(ads_index, rreg_cmd);
    *reg_val = ADS131_SendShortCmd(ads_index, ADS_CMD_NULL);

    return 0;
}

// 电压换算：手册式(10) LSB = +FSR / 2^23，FSR = VREF / PGA
// V = code * VREF / (2^23 * PGA)；外部参考按 1.25/1.2 比例
static float ADS131_CodeToVoltage(int32_t code)
{
    return ((float)code * ADS131_VREF_VOLTAGE) / (float)((int32_t)1 << 23) / (float)ADS131_PGA_GAIN;
}

// Parse received frame, called in SPI TxRx Complete Callback (ISR context)
static void ADS131_ParseData(uint8_t ads_index, uint8_t *rx_buf)
{
    ADS131_Data_t *data;

    if (ads_index == ADS_INDEX_1)
        data = &g_ads1_data;
    else
        data = &g_ads2_data;

    // Word0 = STATUS register（16bit 数据 MSB 对齐，取高 2 字节即可）
    data->status = ((uint16_t)rx_buf[0] << 8) | rx_buf[1];

#if (ADS131_DATA_BITS == 16)
    // 16bit 字：每通道 2 字节，24bit 数据截断低 8bit，MSB 对齐
    for (uint8_t ch = 0; ch < 8; ch++) {
        int16_t raw16 = (int16_t)(((uint16_t)rx_buf[3 + ch * 2] << 8) | rx_buf[4 + ch * 2]);
        data->ch_data[ch]    = (int32_t)raw16 << 8; // 恢复 24bit 位权，便于电压换算
        data->ch_voltage[ch] = ADS131_CodeToVoltage(data->ch_data[ch]);
    }
#else
    // 24bit 字：每通道 3 字节；32bit 字：每通道 4 字节，ADC 数据在字内高 24bit
    for (uint8_t ch = 0; ch < 8; ch++) {
        uint32_t raw24 = ((uint32_t)rx_buf[3 + ch * 3] << 16) | ((uint32_t)rx_buf[4 + ch * 3] << 8) | rx_buf[5 + ch * 3];
        if (raw24 & 0x00800000U)
            data->ch_data[ch] = (int32_t)(raw24 | 0xFF000000U);
        else
            data->ch_data[ch] = (int32_t)raw24;
        data->ch_voltage[ch] = ADS131_CodeToVoltage(data->ch_data[ch]);
    }
#endif
}

void ADS131_Init(uint8_t ads_index)
{
    uint16_t reg_cfg;
    uint16_t verify_val;

    // 1. reset ads131
    ADS131_SendShortCmd(ads_index, ADS_CMD_RESET);
    HAL_Delay(1); // tREGACQ -> 5us

    // 2. MODE：采样分辨率由宏 ADS131_RESOLUTION 决定
    reg_cfg = ADS131_RESOLUTION | MODE_CRC_CCITT | MODE_RXCRC_DIS | MODE_REGCRC_DIS | MODE_TIMEOUT_EN | DRDY_SEL_ALL_CH |
              MODE_DRDY_PUSHPULL | MODE_DRDY_ACTIVE_LOW | MODE_RESERVED_14_15 | MODE_RESERVED_5_7 | MODE_RST_CLEAR;
    ADS131_WriteReg(ads_index, ADS131_MODE, reg_cfg);
    ADS131_ReadReg(ads_index, ADS131_MODE, &verify_val);
    if (verify_val != reg_cfg) { /* MODE register verify failed */ }

    // 3. CLOCK：时钟源/参考源/OSR/PWR 全部由宏决定
    reg_cfg = CLK_ALL_CH_EN | CLK_RSVD_BIT5 | OSR_1024 | PWR_HR;
#if (ADS131_CLOCK_SOURCE == ADS131_CLK_SRC_EXTERNAL)
    reg_cfg |= XTAL_DISABLE; // 外部时钟 CLKIN
#else
    reg_cfg |= XTAL_EN;      // 内部晶振（默认）
#endif
#if (ADS131_REF_SOURCE == ADS131_REF_EXTERNAL)
    reg_cfg |= EXT_REF_ENABLE; // 外部参考 1.25V
#else
    reg_cfg |= INT_REF;        // 内部参考 1.2V（默认）
#endif
    ADS131_WriteReg(ads_index, ADS131_CLOCK, reg_cfg);
    ADS131_ReadReg(ads_index, ADS131_CLOCK, &verify_val);
    if (verify_val != reg_cfg) { /* CLOCK register verify failed */ }

    // 4. GAIN1 CH0~CH3
    ADS131_WriteReg(ads_index, ADS131_GAIN1, GAIN1_DEFAULT_VAL);
    // 5. GAIN2 CH4~CH7
    ADS131_WriteReg(ads_index, ADS131_GAIN2, GAIN2_DEFAULT_VAL);

    // 6. CFG
    reg_cfg = GC_DLY_3 | GC_ENABLE | CD_DISABLE | CD_ANY_CHANNEL | CD_NUM_1 | CD_LEN_128 | CFG_RSVD_13_15;
    ADS131_WriteReg(ads_index, ADS131_CFG, reg_cfg);

    // 7. CHx_CFG：MUX=00 差分输入 AINxP/AINxN，8 通道全配
    uint16_t ch_cfg = CH_MUX_NORMAL | CH_DCBLK_GLOBAL | CH_PHASE_0;
    ADS131_WriteReg(ads_index, ADS131_CH0_CFG, ch_cfg);
    ADS131_WriteReg(ads_index, ADS131_CH1_CFG, ch_cfg);
    ADS131_WriteReg(ads_index, ADS131_CH2_CFG, ch_cfg);
    ADS131_WriteReg(ads_index, ADS131_CH3_CFG, ch_cfg);
    ADS131_WriteReg(ads_index, ADS131_CH4_CFG, ch_cfg);
    ADS131_WriteReg(ads_index, ADS131_CH5_CFG, ch_cfg);
    ADS131_WriteReg(ads_index, ADS131_CH6_CFG, ch_cfg);
    ADS131_WriteReg(ads_index, ADS131_CH7_CFG, ch_cfg);

    // 8. WAKEUP：进入连续转换模式
    ADS131_SendShortCmd(ads_index, ADS_CMD_WAKEUP);
}

// 进入 STANDBY：停止转换（寄存器保留），软件触发模式下每次采样后调用
void ADS131_EnterStandby(uint8_t ads_index)
{
    ADS131_SendShortCmd(ads_index, ADS_CMD_STANDBY);
}

// 软件触发：STANDBY -> WAKEUP，设备启动一次转换，DRDY 中断负责读取
void ADS131_SoftwareTrigger(uint8_t ads_index)
{
    ADS131_SendShortCmd(ads_index, ADS_CMD_WAKEUP);
}

void ADS131_ReadAllChannel(uint8_t ads_index)
{
    // MOSI host transmit 10 x 24bit words:
    // Word0 = NULL command
    // Word1 = zero-filled dummy word (replaces input CRC when RX_CRC_EN=0)
    // Word2~9 = 8 x zero dummy words

    // MISO device output 10 words simultaneously (full duplex):
    // Word0 = STATUS register (response of last frame)
    // Word1~8 = 8 channel ADC conversion data
    // Word9 = output CRC word (CANNOT be disabled, host can ignore it)

    SPI_HandleTypeDef *hspi;
    uint8_t *tx_buf, *rx_buf;

    if (ads_index == ADS_INDEX_1) {
        hspi   = &hspi1;
        tx_buf = g_ads1_tx_buf;
        rx_buf = g_ads1_rx_buf;
    }
    else {
        hspi   = &hspi4;
        tx_buf = g_ads2_tx_buf;
        rx_buf = g_ads2_rx_buf;
    }

    // 防重入：上一次 SPI 传输未完成则跳过（DRDY 已被读取，数据不丢帧）
    if (HAL_SPI_GetState(hspi) != HAL_SPI_STATE_READY)
        return;

    memset(tx_buf, 0, ADS131_FRAME_BYTES);

    // Clear DRDY ready flag before starting SPI to prevent double-trigger
    if (ads_index == ADS_INDEX_1)
        g_ads1_ready_flag = 0;
    else
        g_ads2_ready_flag = 0;

    ADS131_CS_Select(ads_index);
    HAL_SPI_TransmitReceive_IT(hspi, tx_buf, rx_buf, ADS131_FRAME_BYTES);
}

// SPI Transmit Receive Complete Callback（中断上下文）
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        ADS131_ParseData(ADS_INDEX_1, g_ads1_rx_buf);
        ADS131_CS_Deselect(ADS_INDEX_1);
        g_ads1_ready_flag = 2; // Data ready for consumption
    }
    else if (hspi->Instance == SPI4) {
        ADS131_ParseData(ADS_INDEX_2, g_ads2_rx_buf);
        ADS131_CS_Deselect(ADS_INDEX_2);
        g_ads2_ready_flag = 2;
    }
}

// DRDY 下降沿中断：直接启动非阻塞 SPI 读取（满足"中断触发 MCU 读取 8 通道"）
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == SPI1_Ready_Pin) {
        ADS131_ReadAllChannel(ADS_INDEX_1);
    }
    else if (GPIO_Pin == SPI4_Ready_Pin) {
        ADS131_ReadAllChannel(ADS_INDEX_2);
    }
}
