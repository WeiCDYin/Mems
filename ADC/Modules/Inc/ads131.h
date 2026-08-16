#ifndef __ADS131_H__
#define __ADS131_H__

#include "main.h"

#define ADS131_1_CS_LOW() HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define ADS131_1_CS_HIGH() HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)
#define ADS131_2_CS_LOW() HAL_GPIO_WritePin(SPI4_CS_GPIO_Port, SPI4_CS_Pin, GPIO_PIN_RESET)
#define ADS131_2_CS_HIGH() HAL_GPIO_WritePin(SPI4_CS_GPIO_Port, SPI4_CS_Pin, GPIO_PIN_SET)

// ===================== 用户配置宏（编译期开关） =====================
// 1. 时钟源选择：内部晶振(8.192MHz) / 外部时钟 CLKIN（由 MCU 提供）
#define ADS131_CLK_SRC_INTERNAL 0
#define ADS131_CLK_SRC_EXTERNAL 1
#define ADS131_CLOCK_SOURCE ADS131_CLK_SRC_INTERNAL

// 2. 参考电压源：内部 1.2V / 外部 1.25V（手册 8.3.2 节）
#define ADS131_REF_INTERNAL 0
#define ADS131_REF_EXTERNAL 1
#define ADS131_REF_SOURCE ADS131_REF_INTERNAL
#if (ADS131_REF_SOURCE == ADS131_REF_EXTERNAL)
#define ADS131_VREF_VOLTAGE 1.25f
#else
#define ADS131_VREF_VOLTAGE 1.2f
#endif

// 3. 采样分辨率（MODE.WLENGTH）：16 / 24 / 32 bit
#define ADS131_RES_16BIT MODE_WLENGTH_16BIT
#define ADS131_RES_24BIT MODE_WLENGTH_24BIT
#define ADS131_RES_32BIT MODE_WLENGTH_32BIT
#define ADS131_RESOLUTION ADS131_RES_24BIT
#if (ADS131_RESOLUTION == ADS131_RES_16BIT)
#define ADS131_FRAME_BYTES 20 // 10字 x 16bit：STATUS + 8通道 + CRC
#define ADS131_DATA_BITS 16
#elif (ADS131_RESOLUTION == ADS131_RES_32BIT)
#define ADS131_FRAME_BYTES 40 // 10字 x 32bit
#define ADS131_DATA_BITS 24   // 32bit 字内高 24bit 为 ADC 数据
#else
#define ADS131_FRAME_BYTES 30 // 10字 x 24bit
#define ADS131_DATA_BITS 24
#endif

// 4. 采样模式：连续转换 / 软件触发（WAKEUP->DRDY->读取->STANDBY，一次触发一帧）
#define ADS131_SAMPLE_CONTINUOUS 0
#define ADS131_SAMPLE_SW_TRIGGER 1
#define ADS131_SAMPLE_MODE ADS131_SAMPLE_CONTINUOUS
#define ADS131_SW_TRIGGER_MS 100 // 软件触发周期

// 5. 其它
#define ADS131_PGA_GAIN 1 // PGA 增益（与 GAIN1/GAIN2 寄存器一致），电压换算用

// ===================== 指令字 16bit =====================
#define ADS_CMD_NULL 0x0000U  // NULL,read adc data
#define ADS_CMD_RESET 0x0011U // software reset
#define ADS_CMD_STANDBY 0x0022U
#define ADS_CMD_WAKEUP 0x0033U
#define ADS_CMD_LOCK 0x0555U
#define ADS_CMD_UNLOCK 0x0655U
#define ADS_CMD_RREG 0xA000U // read register 101a aaaa annn nnnn
#define ADS_CMD_WREG 0x6000U // write register 011a aaaa annn nnnn

// ADS131M08
enum {
    ADS131_ID          = 0x00,
    ADS131_STATUS      = 0x01,
    ADS131_MODE        = 0x02,
    ADS131_CLOCK       = 0x03,
    ADS131_GAIN1       = 0x04,
    ADS131_GAIN2       = 0x05,
    ADS131_CFG         = 0x06,
    ADS131_THRSHLD_MSB = 0x07,
    ADS131_THRSHLD_LSB = 0x08,

    // CH0
    ADS131_CH0_CFG      = 0x09,
    ADS131_CH0_OCAL_MSB = 0x0A,
    ADS131_CH0_OCAL_LSB = 0x0B,
    ADS131_CH0_GCAL_MSB = 0x0C,
    ADS131_CH0_GCAL_LSB = 0x0D,

    // CH1
    ADS131_CH1_CFG      = 0x0E,
    ADS131_CH1_OCAL_MSB = 0x0F,
    ADS131_CH1_OCAL_LSB = 0x10,
    ADS131_CH1_GCAL_MSB = 0x11,
    ADS131_CH1_GCAL_LSB = 0x12,

    // CH2
    ADS131_CH2_CFG      = 0x13,
    ADS131_CH2_OCAL_MSB = 0x14,
    ADS131_CH2_OCAL_LSB = 0x15,
    ADS131_CH2_GCAL_MSB = 0x16,
    ADS131_CH2_GCAL_LSB = 0x17,

    // CH3
    ADS131_CH3_CFG      = 0x18,
    ADS131_CH3_OCAL_MSB = 0x19,
    ADS131_CH3_OCAL_LSB = 0x1A,
    ADS131_CH3_GCAL_MSB = 0x1B,
    ADS131_CH3_GCAL_LSB = 0x1C,

    // CH4
    ADS131_CH4_CFG      = 0x1D,
    ADS131_CH4_OCAL_MSB = 0x1E,
    ADS131_CH4_OCAL_LSB = 0x1F,
    ADS131_CH4_GCAL_MSB = 0x20,
    ADS131_CH4_GCAL_LSB = 0x21,

    // CH5
    ADS131_CH5_CFG      = 0x22,
    ADS131_CH5_OCAL_MSB = 0x23,
    ADS131_CH5_OCAL_LSB = 0x24,
    ADS131_CH5_GCAL_MSB = 0x25,
    ADS131_CH5_GCAL_LSB = 0x26,

    // CH6
    ADS131_CH6_CFG      = 0x27,
    ADS131_CH6_OCAL_MSB = 0x28,
    ADS131_CH6_OCAL_LSB = 0x29,
    ADS131_CH6_GCAL_MSB = 0x2A,
    ADS131_CH6_GCAL_LSB = 0x2B,

    // CH7
    ADS131_CH7_CFG      = 0x2C,
    ADS131_CH7_OCAL_MSB = 0x2D,
    ADS131_CH7_OCAL_LSB = 0x2E,
    ADS131_CH7_GCAL_MSB = 0x2F,
    ADS131_CH7_GCAL_LSB = 0x30,

    ADS131_REGMAP_CRC = 0x3E,
    ADS131_RESERVED   = 0x3F
};

//==================== 1.MODE 0x02 defaule 0x0510 ====================
// BIT15~14 reserved
#define MODE_RESERVED_14_15 (0x00U << 14)
// BIT13 REG_CRC_EN
#define MODE_REGCRC_DIS (0x00U << 13) // default
#define MODE_REGCRC_EN (0x01U << 13)
// BIT12 RX_CRC_EN
#define MODE_RXCRC_DIS (0x00U << 12) // default
#define MODE_RXCRC_EN (0x01U << 12)
// BIT11 CRC_TYPE
#define MODE_CRC_CCITT (0x00U << 11)
#define MODE_CRC_ANSI (0x01U << 11)
// BIT10 RESET
#define MODE_RST_CLEAR (0x00U << 10)
#define MODE_RST_FLAG (0x01U << 10)
// BIT9~8 WLENGTH
#define MODE_WLENGTH_MASK (0x03U << 8)
#define MODE_WLENGTH_16BIT (0x00U << 8)
#define MODE_WLENGTH_24BIT (0x01U << 8) // default
#define MODE_WLENGTH_32BIT (0x02U << 8)
// BIT7~5 reserved
#define MODE_RESERVED_5_7 (0x00U << 5)
// BIT4 TIMEOUT
#define MODE_TIMEOUT_DIS (0x00U << 4)
#define MODE_TIMEOUT_EN (0x01U << 4) // default
// BIT3~2 DRDY_SEL
#define DRDY_SEL_MASK (0x03U << 2)
#define DRDY_SEL_ALL_CH (0x00U << 2) // 00 default
#define DRDY_SEL_ANY_CH (0x01U << 2) // 01
#define DRDY_SEL_RSVD1 (0x02U << 2)
#define DRDY_SEL_RSVD2 (0x03U << 2)
// BIT1 DRDY_HiZ
#define MODE_DRDY_PUSHPULL (0x00U << 1)  // default
#define MODE_DRDY_OPENDRAIN (0x01U << 1) //
// BIT0 DRDY_FMT
#define MODE_DRDY_ACTIVE_LOW (0x00U << 0) // default
#define MODE_DRDY_ACTIVE_HIGH (0x01U << 0)
// MODE default 0x0510
#define MODE_DEFAULT_VAL                                                                                                                             \
    (MODE_RESERVED_14_15 | MODE_REGCRC_DIS | MODE_RXCRC_DIS | MODE_CRC_CCITT | MODE_RST_CLEAR | MODE_WLENGTH_24BIT | MODE_RESERVED_5_7 |             \
     MODE_TIMEOUT_EN | DRDY_SEL_ALL_CH | MODE_DRDY_PUSHPULL | MODE_DRDY_ACTIVE_LOW)

//==================== 2.CLOCK 0x03 default 0xFF0E ====================
// BIT15 CH7_EN
#define CLK_CH7_EN (0x01U << 15)
// BIT14 CH6_EN
#define CLK_CH6_EN (0x01U << 14)
// BIT13 CH5_EN
#define CLK_CH5_EN (0x01U << 13)
// BIT12 CH4_EN
#define CLK_CH4_EN (0x01U << 12)
// BIT11 CH3_EN
#define CLK_CH3_EN (0x01U << 11)
// BIT10 CH2_EN
#define CLK_CH2_EN (0x01U << 10)
// BIT9 CH1_EN
#define CLK_CH1_EN (0x01U << 9)
// BIT8 CH0_EN
#define CLK_CH0_EN (0x01U << 8)
#define CLK_ALL_CH_EN (CLK_CH0_EN | CLK_CH1_EN | CLK_CH2_EN | CLK_CH3_EN | CLK_CH4_EN | CLK_CH5_EN | CLK_CH6_EN | CLK_CH7_EN)
// BIT7 XTAL_DIS
#define XTAL_EN (0x00U << 7)      // internal default
#define XTAL_DISABLE (0x01U << 7) // external
// BIT6 EXTREF_EN
#define INT_REF (0x00U << 6)        // internal reference default
#define EXT_REF_ENABLE (0x01U << 6) // external reference
// BIT5 保留
#define CLK_RSVD_BIT5 (0x00U << 5)
// BIT4~2 OSR
#define OSR_MASK (0x07U << 2)
#define OSR_128 (0x00U << 2)
#define OSR_256 (0x01U << 2)
#define OSR_512 (0x02U << 2)
#define OSR_1024 (0x03U << 2) // 4ksps
#define OSR_2048 (0x04U << 2)
#define OSR_4096 (0x05U << 2)
#define OSR_8192 (0x06U << 2)
#define OSR_16384 (0x07U << 2)
// BIT1~0 PWR
#define PWR_MASK (0x03U << 0)
#define PWR_VLP (0x00U << 0) //
#define PWR_LP (0x01U << 0)  //
#define PWR_HR (0x02U << 0)  // high resolution default
#define PWR_HR2 (0x03U << 0) //
// CLOCK default 0xFF0E
#define CLOCK_DEFAULT_VAL (CLK_ALL_CH_EN | XTAL_EN | INT_REF | CLK_RSVD_BIT5 | OSR_1024 | PWR_HR)

//==================== 3.GAIN1 0x04 default 0x0000 CH0~CH3 ====================
// 通用PGA增益值
#define PGA_1 0x00U // default
#define PGA_2 0x01U
#define PGA_4 0x02U
#define PGA_8 0x03U
#define PGA_16 0x04U
#define PGA_32 0x05U
#define PGA_64 0x06U
#define PGA_128 0x07U
// BIT15
#define GAIN1_RSVD15 (0x00U << 15)
// BIT14~12 CH3 PGAGAIN
#define PGA_CH3_MASK (0x07U << 12)
// BIT11
#define GAIN1_RSVD11 (0x00U << 11)
// BIT10~8 CH2 PGAGAIN
#define PGA_CH2_MASK (0x07U << 8)
// BIT7
#define GAIN1_RSVD7 (0x00U << 7)
// BIT6~4 CH1 PGAGAIN
#define PGA_CH1_MASK (0x07U << 4)
// BIT3
#define GAIN1_RSVD3 (0x00U << 3)
// BIT2~0 CH0 PGAGAIN
#define PGA_CH0_MASK (0x07U << 0)
// GAIN1
#define GAIN1_DEFAULT_VAL (GAIN1_RSVD15 | (PGA_1 << 12) | GAIN1_RSVD11 | (PGA_1 << 8) | GAIN1_RSVD7 | (PGA_1 << 4) | GAIN1_RSVD3 | PGA_1)

//==================== 4.GAIN2 0x05 default 0x0000 CH4~CH7 ====================
// BIT15
#define GAIN2_RSVD15 (0x00U << 15)
// BIT14~12 CH7 PGAGAIN
#define PGA_CH7_MASK (0x07U << 12)
// BIT11
#define GAIN2_RSVD11 (0x00U << 11)
// BIT10~8 CH6 PGAGAIN
#define PGA_CH6_MASK (0x07U << 8)
// BIT7
#define GAIN2_RSVD7 (0x00U << 7)
// BIT6~4 CH5 PGAGAIN
#define PGA_CH5_MASK (0x07U << 4)
// BIT3
#define GAIN2_RSVD3 (0x00U << 3)
// BIT2~0 CH4 PGAGAIN
#define PGA_CH4_MASK (0x07U << 0)
// GAIN2
#define GAIN2_DEFAULT_VAL (GAIN2_RSVD15 | (PGA_1 << 12) | GAIN2_RSVD11 | (PGA_1 << 8) | GAIN2_RSVD7 | (PGA_1 << 4) | GAIN2_RSVD3 | PGA_1)

//==================== 5.CFG 0x06 default 0x0600 ====================
// BIT15~13
#define CFG_RSVD_13_15 (0x00U << 13)
// BIT12~9 GC_DLY
#define GC_DLY_MASK (0x0FU << 9)
#define GC_DLY_1 (0x01U << 9)
#define GC_DLY_2 (0x02U << 9)
#define GC_DLY_3 (0x03U << 9) // default
#define GC_DLY_MAX (0x0FU << 9)
// BIT8 GC_EN
#define GC_DISABLE (0x00U << 8) // default
#define GC_ENABLE (0x01U << 8)
// BIT7 CD_ALLCH
#define CD_ANY_CHANNEL (0x00U << 7) // default
#define CD_ALL_CHANNEL (0x01U << 7)
// BIT6~4 CD_NUM
#define CD_NUM_MASK (0x07U << 4)
#define CD_NUM_1 (0x00U << 4) // default
#define CD_NUM_2 (0x01U << 4)
#define CD_NUM_8 (0x07U << 4)
// BIT3~1 CD_LEN
#define CD_LEN_MASK (0x07U << 1)
#define CD_LEN_128 (0x00U << 1) // default
#define CD_LEN_256 (0x01U << 1)
#define CD_LEN_3584 (0x07U << 1)
// BIT0 CD_EN
#define CD_DISABLE (0x00U << 0) // default
#define CD_ENABLE (0x01U << 0)
// cfg default
#define CFG_DEFAULT_VAL (CFG_RSVD_13_15 | GC_DLY_3 | GC_DISABLE | CD_ANY_CHANNEL | CD_NUM_1 | CD_LEN_128 | CD_DISABLE)

// ==================== CHx_CFG ====================
// BIT15 ~ BIT6  PHASE[9:0] 10bit
#define CH_PHASE_SHIFT 6U
#define CH_PHASE_MASK (0x3FFU << CH_PHASE_SHIFT)
#define CH_PHASE_0 (0x000U << CH_PHASE_SHIFT) // default
// BIT5 ~ BIT3
#define CH_CFG_RSVD (0x07U << 3)
// BIT2 DCBLKx_DIS
#define CH_DCBLK_GLOBAL (0x00U << 2)    // 0：跟随全局DCBLOCK配置（默认）
#define CH_DCBLK_FORCE_OFF (0x01U << 2) // 1：强制关闭本通道DC滤波
// BIT1 ~ BIT0 MUXx 输入多路选择
#define CH_MUX_NORMAL 0x00U // 00 AINxP/AINxN diff input
#define CH_MUX_SHORT 0x01U  // 01 通道短接（零点校准）
#define CH_MUX_DC_POS 0x02U // 10 内部正直流测试源
#define CH_MUX_DC_NEG 0x03U // 11 内部负直流测试源
// CHx_CFG default
#define CH_CFG_DEFAULT (CH_PHASE_0 | CH_CFG_RSVD | CH_DCBLK_GLOBAL | CH_MUX_NORMAL)

typedef enum {
    ADS_INDEX_1,
    ADS_INDEX_2,

    ADS_INDEX_MAX
} ADS_INDEX_t;

typedef struct {
    __IO uint16_t status;
    __IO int32_t  ch_data[8];    // 24bit有符号采样值
    __IO float    ch_voltage[8]; // 换算后的电压值(V)，SPI完成中断内计算
} ADS131_Data_t;

void ADS131_Init(uint8_t ads_index);
void ADS131_ReadAllChannel(uint8_t ads_index);
void ADS131_EnterStandby(uint8_t ads_index);    // 进入 STANDBY（停止转换，寄存器保留）
void ADS131_SoftwareTrigger(uint8_t ads_index); // 软件触发一次采样（STANDBY->WAKEUP）

extern ADS131_Data_t g_ads1_data, g_ads2_data;
extern __IO uint8_t  g_ads1_ready_flag;
extern __IO uint8_t  g_ads2_ready_flag;

#endif