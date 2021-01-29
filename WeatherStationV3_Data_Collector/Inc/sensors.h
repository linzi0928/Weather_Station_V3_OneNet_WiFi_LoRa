#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_spi.h"  
#include "usart.h"
#include "bme680.h"
#include "spi.h"
#include "iwdg.h"
#include <string.h>
extern uint8_t USART1_RX_BUF[];
extern uint8_t USART2_RX_BUF[];
extern uint32_t dust25;
extern uint16_t uvv;
extern float tem;
extern float hum;
extern float press;
extern float gas_r;
extern uint32_t voc_level;
extern uint16_t VEML6070_ReadValue(void);
extern unsigned short tvoc_ppb, co2_eq_ppm;
int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int fflag=0;
struct bme680_dev gas_sensor;
struct bme680_field_data data;
void user_delay_ms(uint32_t period);
void Get_sensors(void);
int Get_dust25(void);
void Get_UV(void);
void Get_680(void);
uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer);
uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer);
uint16_t meas_period;
void bme680_init_user (void);
extern void osDelay(uint16_t time);
extern void SGP_Get(void);
