#pragma once

#include "hl_device.hpp"
#include "hl_flash.hpp"
#include "hl_rcc.hpp"

namespace hl {

#if defined(HL_STM32F4XX)

inline void misc_f4_conf_sysclock_hse_high(
	uint8_t       pll_input_div,
	uint16_t      pll_mul,
	PLLSysDiv     pll_sys_div,
	PLLUsbDiv     pll_usb_div,
	APB2Prescaler apb2_prescaler,
	APB1Prescaler apb1_prescaler,
	AHBPrescaler  ahb_prescaler,
	FlashLatency  flash_latency)
{
	rcc_enable_hsi();
	rcc_set_sys_clock(SystemClock::HSI);

	rcc_set_apb2_prescaler(apb2_prescaler);
	rcc_set_apb1_prescaler(apb1_prescaler);
	rcc_set_ahb_prescaler(ahb_prescaler);

	rcc_enable_hse(false);

	rcc_disable_pll();
	rcc_set_pll_source(PLLSource::HSE);
	rcc_set_pll_input_division(pll_input_div);
	rcc_set_pll_multiplication(pll_mul);
	rcc_set_pll_sys_division(pll_sys_div);
	rcc_set_pll_usb_division(pll_usb_div);
	rcc_enable_pll();

	flash_enable_data_cache();
	flash_enable_instructions_cache();
	flash_enable_prefetch();
	flash_set_latency(flash_latency);

	rcc_set_sys_clock(SystemClock::PLL);

	rcc_disable_hsi();
}

#endif


#if defined(HL_STM32F1XX)

inline void misc_f1_conf_sysclock_hse_high(
	PLLMult       ppl_mul,
	APB2Prescaler apb2_prescaler,
	APB1Prescaler apb1_prescaler,
	AHBPrescaler  ahb_prescaler,
	FlashLatency  flash_latency)
{
	rcc_enable_hsi();
	rcc_set_sys_clock(SystemClock::HSI);

	rcc_enable_hse(false);

	flash_enable_prefetch();
	flash_set_latency(flash_latency);

	rcc_set_apb2_prescaler(apb2_prescaler);
	rcc_set_apb1_prescaler(apb1_prescaler);
	rcc_set_ahb_prescaler(ahb_prescaler);

	rcc_disable_pll();
	rcc_set_pll_source(PLLSource::HSE);
	rcc_disable_hse_div2_for_pll();
	rcc_set_pll_mult_factor(ppl_mul);
	rcc_enable_pll();

	rcc_set_sys_clock(SystemClock::PLL);

	rcc_disable_hsi();
}

#endif

#if defined(HL_STM32F0XX)
inline void misc_f0_conf_sysclock_hsi_high(
	PLLMult       ppl_mul,
	APB2Prescaler apb2_prescaler,
	APB1Prescaler apb1_prescaler,
	AHBPrescaler  ahb_prescaler)
{
	rcc_enable_hsi();
	rcc_set_sys_clock(SystemClock::HSI);

	flash_enable_prefetch();
	flash_set_one_wait_state();

	rcc_set_apb2_prescaler(apb2_prescaler);
	rcc_set_apb1_prescaler(apb1_prescaler);
	rcc_set_ahb_prescaler(ahb_prescaler);

	rcc_disable_pll();
	rcc_set_pll_source(PLLSource::HSI);
	rcc_set_pll_mult_factor(ppl_mul);
	rcc_enable_pll();

	rcc_set_sys_clock(SystemClock::PLL);
}

#endif

} // namespace hl