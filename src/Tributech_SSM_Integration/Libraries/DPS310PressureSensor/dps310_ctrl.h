/*
 * 	dps310.h *
 *  Created on: 2020 Aug 13
 *  Author: AEASCHER
 */

#ifndef DSP310_PRESSURESENSOR_DPS310_H_
#define DSP310_PRESSURESENSOR_DPS310_H_

#include "dps310_config.h"
#include "stdbool.h"
#include "stdint.h"

extern dps310_state_t dps310_status;
extern dps310_comp_coeff_t dps_coeff;

int16_t dps310_read_byte_bitfield(reg_mask_t reg_mask, bool comparison_timer);
int16_t dps310_write_byte_bitfield(uint8_t data, reg_mask_t reg_mask, bool comparison_timer);
int16_t dps310_write_byte(uint8_t reg_address, uint8_t data, bool check, bool comparison_timer);
int16_t dps310_read_register_block(reg_block_t reg_block, uint8_t *buffer, bool comparison_timer);
void get_twos_complement(int32_t *raw, uint8_t length);
uint16_t calc_busy_time(uint16_t mr, uint16_t osr);
float calc_temp(int32_t raw);
float calc_preassure(int32_t raw);
int16_t dps310_init();
void dps310_deinit();
int16_t dps310_get_product_id(void);
int16_t dps310_get_revision_id(void);
int16_t dps310_get_calibrated_temp_sensor(void);
int16_t dps310_get_temp_sensor_coeff(void);
int16_t dps310_configure_temp_meas(uint8_t temp_mr, uint8_t temp_osr);
int16_t dps310_configure_pressure_meas(uint8_t prs_mr, uint8_t prs_osr);
int16_t dps310_start_meas_temp_once(uint8_t oversampling_rate);
int16_t dps310_start_meas_pres_once(uint8_t oversampling_rate);
int16_t dps310_measure_temp_once(float *result, uint8_t oversampling_rate);
int16_t dps310_measure_pres_once(float *result, uint8_t oversampling_rate);
int16_t dps310_disable_FIFO(void);
int16_t dps310_enable_FIFO(void);
int16_t dps310_flush_FIFO(void);
int16_t dps310_read_FIFO_state(void);
int16_t get_FIFO_value(int32_t *value);
int16_t dps310_correct_temperature(void);
int16_t dps310_get_single_result(float *result);
int16_t dps310_get_raw_result(int32_t *raw, reg_block_t reg);
int16_t dps310_start_measure_both_cont(uint8_t tempMr,
									   uint8_t tempOsr,
									   uint8_t prsMr,
									   uint8_t prsOsr);
int16_t dps310_get_cont_results();
int16_t dps310_set_operation_mode(uint8_t op_mode);
int16_t dps310_set_standby_mode(void);

#endif /* DSP310_PRESSURESENSOR_DPS310_H_ */
