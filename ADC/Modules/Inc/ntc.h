#ifndef __NTC_H__
#define __NTC_H__

#include "main.h"

// ===================== NTC 采样配置 =====================
// NCU18XH103F6SRB：Murata NTC，R25 = 10kΩ，B25/85 = 3435K
#define NTC_R25_OHM      10000.0f   // 25°C 标称阻值 Ω
#define NTC_B_VALUE      3435.0f    // B 常数 (25/85)
#define NTC_T0_KELVIN    298.15f    // 25°C 对应开尔文
#define NTC_R_FIXED_OHM  10000.0f   // 串联固定电阻 Ω
#define NTC_ADC_VREF     2.048f     // ADC 外部参考电压（VREF+ 引脚，2.048V）
#define NTC_ADC_BITS     12U        // ADC5 分辨率

// 分压拓扑：PC9(供电) -> 10kΩ -> PA8(采样点) -> NTC -> GND
// Vntc = Rntc/(Rntc+10k) × Vdd  =>  Rntc = 10k × Vntc/(Vdd - Vntc)
// 温度(Steinhart 简化 B 方程)：1/T = 1/T0 + ln(Rntc/R25)/B

typedef struct {
    float ntc_voltage;     // PA8：NTC 分压点电压 (V)
    float dvdd_voltage;    // PA9：DVDD 供电电压 (V)，用于校准电源精度
    float ntc_resistance;  // 计算出的 NTC 阻值 (Ω)
    float temperature_c;   // 计算出的温度 (°C)，无效时为 -273.15f
} NTC_Data_t;

void NTC_Init(void);         // PC9 拉高给分压网络供电
void NTC_PeriodicTask(void); // 100ms 周期任务：双通道 ADC 采样 + 温度换算

extern NTC_Data_t g_ntc;

#endif
