#include "vofa_transport.h"
#include <string.h>

#if (VOFA_PORT_SELECT == VOFA_PORT_USB)
#include "usbd_cdc_if.h"
#elif (VOFA_PORT_SELECT == VOFA_PORT_UART2)
#include "usart.h"
#elif (VOFA_PORT_SELECT == VOFA_PORT_UART3)
#include "usart.h"
#elif (VOFA_PORT_SELECT == VOFA_PORT_SPI3)
#include "spi.h"
#include "main.h"
#else
#error "VOFA_PORT_SELECT 无效：可选 VOFA_PORT_USB / VOFA_PORT_UART2 / VOFA_PORT_UART3 / VOFA_PORT_SPI3"
#endif

#if (VOFA_PORT_SELECT != VOFA_PORT_USB)
// USB 分支使用 CDC 内部双包状态机，不需要本地组帧缓冲
static uint8_t vofa_frame[VOFA_MAX_CH * 4U + VOFA_FRAME_TAIL];
#endif

/**
  * @brief  发送一帧 VOFA+ JustFloat 数据
  *         USB 端口：走 CDC_SendVofaFrame（数据包 + AA55AA55 帧尾分两包）
  *         UART/SPI 端口：本地组帧后一次性阻塞发送
  * @param  ch : 待发送的 float 数组
  * @param  num: 通道个数（最大 VOFA_MAX_CH）
  */
void VOFA_SendFrame(float *ch, uint8_t num)
{
#if (VOFA_PORT_SELECT == VOFA_PORT_USB)
    CDC_SendVofaFrame(ch, num);
#else
    uint16_t len;

    if (num > VOFA_MAX_CH) {
        num = VOFA_MAX_CH;
    }
    len = (uint16_t)num * 4U;
    memcpy(vofa_frame, ch, len);

    // JustFloat 帧尾
    vofa_frame[len + 0U] = 0xAAU;
    vofa_frame[len + 1U] = 0x55U;
    vofa_frame[len + 2U] = 0xAAU;
    vofa_frame[len + 3U] = 0x55U;

#if (VOFA_PORT_SELECT == VOFA_PORT_UART2)
    HAL_UART_Transmit(&huart2, vofa_frame, len + VOFA_FRAME_TAIL, 10);
#elif (VOFA_PORT_SELECT == VOFA_PORT_UART3)
    HAL_UART_Transmit(&huart3, vofa_frame, len + VOFA_FRAME_TAIL, 10);
#elif (VOFA_PORT_SELECT == VOFA_PORT_SPI3)
    HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, vofa_frame, len + VOFA_FRAME_TAIL, 10);
    HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
#endif
#endif
}
