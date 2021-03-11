#ifndef DPSREGISTER_H_
#define DPSREGISTER_H_

#include "stdbool.h"
#include "stdint.h"

typedef struct
{
    uint8_t register_address;
    uint8_t mask;
    uint8_t shift;
} reg_mask_t;

typedef struct
{
    uint8_t register_address;
    uint8_t length;
} reg_block_t;

typedef struct
{
	//temperature measure rate (value from 0 to 7)
	//2^temp_mr temperature measurement results per second
	int16_t temp_mr;
	//temperature oversampling rate (value from 0 to 7)
	//2^temp_osr internal temperature measurements per result
	//A higher value increases precision
	int16_t temp_osr;
	//pressure measure rate (value from 0 to 7)
	//2^prs_mr pressure measurement results per second
	int16_t prs_mr;
	//pressure oversampling rate (value from 0 to 7)
	//2^prs_osr internal pressure measurements per result
	//A higher value increases precision
	int16_t prs_osr;

}dps310_configuration_t;

typedef struct
{
	bool init_flag;
	int8_t mode_flag;
	uint8_t temp_Sensor;

	//settings
	uint8_t temp_mr;
	uint8_t temp_osr;
	uint8_t prs_mr;
	uint8_t prs_osr;

	//last measurements
	float temp_meas;
	float pres_meas;
}dps310_state_t;

typedef struct
{
	// compensation coefficients for both dps310 and dps422
	int32_t c0_half;
	int32_t c1;
	int32_t c00;
	int32_t c10;
	int32_t c01;
	int32_t c11;
	int32_t c20;
	int32_t c21;
	int32_t c30;


	// last measured scaled temperature (necessary for pressure compensation)
	float last_temp_scal;
}dps310_comp_coeff_t;

#endif	/*DPSREGISTER_H_*/
