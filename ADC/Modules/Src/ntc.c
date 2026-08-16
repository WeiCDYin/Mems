#include "ntc.h"
#include "adc.h"
#include "main.h"
#include <math.h>

NTC_Data_t g_ntc;

/**
  * @brief  NTC 初始化：PC9 输出高电平，给 10kΩ + NTC 分压网络供电
  */
void NTC_Init(void)
{
    HAL_GPIO_WritePin(NTC_VDD_GPIO_Port, NTC_VDD_Pin, GPIO_PIN_SET);
}

/**
  * @brief  NTC 100ms 周期任务
  *         ADC5 扫描 2 通道（外部参考 2.048V）：
  *           rank1 = CH1(PA8)  NTC 分压点电压
  *           rank2 = CH2(PA9)  DVDD 供电电压（校准 NTC 电源精度）
  *         计算 Rntc = 10k × Vntc / (Vdd − Vntc)，再用 B 方程换算温度。
  */
void NTC_PeriodicTask(void)
{
    uint32_t ntc_raw  = 0;
    uint32_t dvdd_raw = 0;
    float    vntc, vdd, rntc, inv_t;

    if (HAL_ADC_Start(&hadc5) != HAL_OK) {
        return;
    }
    if (HAL_ADC_PollForConversion(&hadc5, 10) == HAL_OK) {
        ntc_raw = HAL_ADC_GetValue(&hadc5);   // rank1: CH1 PA8
    }
    if (HAL_ADC_PollForConversion(&hadc5, 10) == HAL_OK) {
        dvdd_raw = HAL_ADC_GetValue(&hadc5);  // rank2: CH2 PA9
    }
    HAL_ADC_Stop(&hadc5);

    // 外部参考 2.048V，12bit：V = code × 2.048 / 4096
    vntc = (float)ntc_raw  * NTC_ADC_VREF / (float)(1U << NTC_ADC_BITS);
    vdd  = (float)dvdd_raw * NTC_ADC_VREF / (float)(1U << NTC_ADC_BITS);

    g_ntc.ntc_voltage    = vntc;
    g_ntc.dvdd_voltage   = vdd;

    // 有效性检查：Vdd 必须大于 Vntc（分压点不会高于电源），且大于 0
    if ((vdd <= 0.001f) || (vntc <= 0.0001f) || (vntc >= vdd)) {
        g_ntc.ntc_resistance = 0.0f;
        g_ntc.temperature_c  = -273.15f; // 无效标记
        return;
    }

    // NTC 接 GND 侧：Rntc = Rf × Vntc / (Vdd − Vntc)
    rntc = NTC_R_FIXED_OHM * vntc / (vdd - vntc);
    g_ntc.ntc_resistance = rntc;

    if (rntc <= 0.0f) {
        g_ntc.temperature_c = -273.15f;
        return;
    }

    // B 方程：1/T = 1/T0 + ln(Rntc/R25)/B，T 单位 K，输出 °C
    inv_t = 1.0f / NTC_T0_KELVIN + logf(rntc / NTC_R25_OHM) / NTC_B_VALUE;
    g_ntc.temperature_c = 1.0f / inv_t - 273.15f;
}
