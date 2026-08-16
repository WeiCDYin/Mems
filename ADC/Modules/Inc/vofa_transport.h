#ifndef __VOFA_TRANSPORT_H__
#define __VOFA_TRANSPORT_H__

#include <stdint.h>

// ===================== VOFA 数据发送端口选择（编译期宏） =====================
// ADS131 采样数据按 VOFA+ JustFloat 格式从以下端口之一输出：
#define VOFA_PORT_USB    0   // USB CDC 虚拟串口（全速，默认）
#define VOFA_PORT_UART2  1   // USART2 TX (PB3, 115200-8-N-1)
#define VOFA_PORT_UART3  2   // USART3 TX (PB10, 115200-8-N-1)
#define VOFA_PORT_SPI3   3   // SPI3 主机 MOSI (PC12, PA15=CS 软件控制)

#ifndef VOFA_PORT_SELECT
#define VOFA_PORT_SELECT VOFA_PORT_USB
#endif

#define VOFA_MAX_CH      16U // 单帧最多通道数（16ch x 4B = 64B，加帧尾分两包）
#define VOFA_FRAME_TAIL  4U  // JustFloat 帧尾 AA 55 AA 55

/**
  * @brief  发送一帧 VOFA+ JustFloat 数据
  *         帧格式：[ch0..chN-1 各 4 字节 float][0xAA 0x55 0xAA 0x55]
  * @param  ch : 待发送的 float 数组（电压等物理量）
  * @param  num: 通道个数（最大 VOFA_MAX_CH）
  */
void VOFA_SendFrame(float *ch, uint8_t num);

#endif
