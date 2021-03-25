#pragma once

#include "hl_device.hpp"

namespace hl {

#ifdef RCC_APB2ENR_AFIOEN
inline void rcc_enable_afio()
{
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}

inline void rcc_disable_afio()
{
	RCC->APB2ENR &= ~RCC_APB2ENR_AFIOEN;
}
#endif


#ifdef RCC_CR_RTCPRE

// RTC/LCD prescaler

enum class RTCLCDPrescaler
{
	_2  = 0b00 << 29, // HSE is divided by 2 for RTC/LCD clock
	_4  = 0b01 << 29, // HSE is divided by 4 for RTC/LCD clock
	_8  = 0b10 << 29, // HSE is divided by 8 for RTC/LCD clock
	_16 = 0b11 << 29, // HSE is divided by 16 for RTC/LCD clock
};

inline void rcc_set_rtc_lcd_prescaler(RTCLCDPrescaler prescaler)
{
	uint32_t cr = RCC->CR;
	cr &= ~RCC_CR_RTCPRE;
	cr |= (uint32_t)prescaler;
	RCC->CR = cr;
}

#endif

// Clock security system

inline void rcc_enable_security_system()
{
	RCC->CR |= RCC_CR_CSSON;
}

inline void rcc_disable_security_system()
{
	RCC->CR &= ~RCC_CR_CSSON;
}

inline bool rcc_is_security_system_enabled()
{
	return (RCC->CR & RCC_CR_CSSON) != 0;
}


// PLL

inline void rcc_enable_pll()
{
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) {}
}

inline void rcc_disable_pll()
{
	RCC->CR &= ~RCC_CR_PLLON;
	while (RCC->CR & RCC_CR_PLLRDY) {}
}

#ifdef RCC_CFGR_PLLDIV

enum class PLLHSEDiv
{
	No = RCC_CFGR_PLLDIV1, // PLL clock output = PLLVCO
	_2 = RCC_CFGR_PLLDIV2, // PLL clock output = PLLVCO / 2
	_3 = RCC_CFGR_PLLDIV3, // PLL clock output = PLLVCO / 3
	_4 = RCC_CFGR_PLLDIV4, // PLL clock output = PLLVCO / 4
};

inline void rcc_set_pll_output_division(PLLHSEDiv value)
{
	uint32_t cfgr = RCC->CFGR;
	cfgr &= ~RCC_CFGR_PLLDIV;
	cfgr |= (uint32_t)value;
	RCC->CFGR = cfgr;
	__DSB();
	__DSB();
}

#endif

#if defined(RCC_PLLCFGR_PLLQ)
enum class PLLUsbDiv
{
	No  = 1 << RCC_PLLCFGR_PLLQ_Pos,
	_2  = 2 << RCC_PLLCFGR_PLLQ_Pos,
	_3  = 3 << RCC_PLLCFGR_PLLQ_Pos,
	_4  = 4 << RCC_PLLCFGR_PLLQ_Pos,
	_5  = 5 << RCC_PLLCFGR_PLLQ_Pos,
	_6  = 6 << RCC_PLLCFGR_PLLQ_Pos,
	_7  = 7 << RCC_PLLCFGR_PLLQ_Pos,
	_8  = 8 << RCC_PLLCFGR_PLLQ_Pos,
	_9  = 9 << RCC_PLLCFGR_PLLQ_Pos,
	_10 = 10 << RCC_PLLCFGR_PLLQ_Pos,
	_11 = 11 << RCC_PLLCFGR_PLLQ_Pos,
	_12 = 12 << RCC_PLLCFGR_PLLQ_Pos,
	_13 = 13 << RCC_PLLCFGR_PLLQ_Pos,
	_14 = 14 << RCC_PLLCFGR_PLLQ_Pos,
	_15 = 15 << RCC_PLLCFGR_PLLQ_Pos,
};

inline void rcc_set_pll_usb_division(PLLUsbDiv value)
{
	set_value_by_mask(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ, (uint32_t)value);
}
#endif

#if defined(RCC_PLLCFGR_PLLP)
enum class PLLSysDiv
{
	_2 = 0 << RCC_PLLCFGR_PLLP_Pos,
	_4 = 1 << RCC_PLLCFGR_PLLP_Pos,
	_6 = 2 << RCC_PLLCFGR_PLLP_Pos,
	_8 = 3 << RCC_PLLCFGR_PLLP_Pos,
};

inline void rcc_set_pll_sys_division(PLLSysDiv value)
{
	set_value_by_mask(RCC->PLLCFGR, RCC_PLLCFGR_PLLP, (uint32_t)value);
}
#endif

#if defined(RCC_PLLCFGR_PLLM)
inline void rcc_set_pll_input_division(uint8_t value)
{
	set_value_by_mask(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, ((uint32_t)value << RCC_PLLCFGR_PLLM_Pos));
}
#endif

#if defined(RCC_PLLCFGR_PLLN)
inline void rcc_set_pll_multiplication(uint16_t value)
{
	set_value_by_mask(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, ((uint32_t)value << RCC_PLLCFGR_PLLN_Pos));
}
#endif

#ifdef RCC_CFGR2_PREDIV

enum class PPLHSEDiv
{
	No = RCC_CFGR2_PREDIV_DIV1,
	_2 = RCC_CFGR2_PREDIV_DIV2,
	_3 = RCC_CFGR2_PREDIV_DIV3,
	_4 = RCC_CFGR2_PREDIV_DIV4,
	_5 = RCC_CFGR2_PREDIV_DIV5,
	_6 = RCC_CFGR2_PREDIV_DIV6,
	_7 = RCC_CFGR2_PREDIV_DIV7,
	_8 = RCC_CFGR2_PREDIV_DIV8,
	_9 = RCC_CFGR2_PREDIV_DIV9,
	_10 = RCC_CFGR2_PREDIV_DIV10,
	_11 = RCC_CFGR2_PREDIV_DIV11,
	_12 = RCC_CFGR2_PREDIV_DIV12,
	_13 = RCC_CFGR2_PREDIV_DIV13,
	_14 = RCC_CFGR2_PREDIV_DIV14,
	_15 = RCC_CFGR2_PREDIV_DIV15,
	_16 = RCC_CFGR2_PREDIV_DIV16,
};

inline void rcc_set_hse_pll_division(PPLHSEDiv value)
{
	uint32_t cfgr2 = RCC->CFGR2;
	cfgr2 &= ~RCC_CFGR2_PREDIV;
	cfgr2 |= (uint32_t)value;
	RCC->CFGR2 = cfgr2;
	__DSB();
	__DSB();
}

#endif

#ifdef RCC_CFGR_PLLMULL
#define RCC_CFGR_PLLMUL RCC_CFGR_PLLMULL
#endif

#ifdef RCC_CFGR_PLLMUL

enum class PLLMult
{
#ifdef RCC_CFGR_PLLMULL2
	_2 = RCC_CFGR_PLLMULL2,
#endif
#ifdef RCC_CFGR_PLLMUL2
	_2 = RCC_CFGR_PLLMUL2,
#endif
#ifdef RCC_CFGR_PLLMULL3
	_3 = RCC_CFGR_PLLMULL3,
#endif
#ifdef RCC_CFGR_PLLMUL3
	_3 = RCC_CFGR_PLLMUL3,
#endif
#ifdef RCC_CFGR_PLLMULL4
	_4 = RCC_CFGR_PLLMULL4,
#endif
#ifdef RCC_CFGR_PLLMUL4
	_4 = RCC_CFGR_PLLMUL4,
#endif
#ifdef RCC_CFGR_PLLMULL5
	_5 = RCC_CFGR_PLLMULL5,
#endif
#ifdef RCC_CFGR_PLLMUL5
	_5 = RCC_CFGR_PLLMUL5,
#endif
#ifdef RCC_CFGR_PLLMULL6
	_6 = RCC_CFGR_PLLMULL6,
#endif
#ifdef RCC_CFGR_PLLMUL6
	_6 = RCC_CFGR_PLLMUL6,
#endif
#ifdef RCC_CFGR_PLLMULL7
	_7 = RCC_CFGR_PLLMULL7,
#endif
#ifdef RCC_CFGR_PLLMUL7
	_7 = RCC_CFGR_PLLMUL7,
#endif
#ifdef RCC_CFGR_PLLMULL8
	_8 = RCC_CFGR_PLLMULL8,
#endif
#ifdef RCC_CFGR_PLLMUL8
	_8 = RCC_CFGR_PLLMUL8,
#endif
#ifdef RCC_CFGR_PLLMULL9
	_9 = RCC_CFGR_PLLMULL9,
#endif
#ifdef RCC_CFGR_PLLMUL9
	_9 = RCC_CFGR_PLLMUL9,
#endif
#ifdef RCC_CFGR_PLLMULL10
	_10 = RCC_CFGR_PLLMULL10,
#endif
#ifdef RCC_CFGR_PLLMUL10
	_10 = RCC_CFGR_PLLMUL10,
#endif
#ifdef RCC_CFGR_PLLMULL11
	_11 = RCC_CFGR_PLLMULL11,
#endif
#ifdef RCC_CFGR_PLLMUL11
	_11 = RCC_CFGR_PLLMUL11,
#endif
#ifdef RCC_CFGR_PLLMULL12
	_12 = RCC_CFGR_PLLMULL12,
#endif
#ifdef RCC_CFGR_PLLMUL12
	_12 = RCC_CFGR_PLLMUL12,
#endif
#ifdef RCC_CFGR_PLLMULL13
	_13 = RCC_CFGR_PLLMULL13,
#endif
#ifdef RCC_CFGR_PLLMUL13
	_13 = RCC_CFGR_PLLMUL13,
#endif
#ifdef RCC_CFGR_PLLMULL14
	_14 = RCC_CFGR_PLLMULL14,
#endif
#ifdef RCC_CFGR_PLLMUL14
	_14 = RCC_CFGR_PLLMUL14,
#endif
#ifdef RCC_CFGR_PLLMULL15
	_15 = RCC_CFGR_PLLMULL15,
#endif
#ifdef RCC_CFGR_PLLMUL15
	_15 = RCC_CFGR_PLLMUL15,
#endif
#ifdef RCC_CFGR_PLLMULL16
	_16 = RCC_CFGR_PLLMULL16,
#endif
#ifdef RCC_CFGR_PLLMUL16
	_16 = RCC_CFGR_PLLMUL16,
#endif
#ifdef RCC_CFGR_PLLMUL24
	_24 = RCC_CFGR_PLLMUL24,
#endif
#ifdef RCC_CFGR_PLLMUL32
	_32 = RCC_CFGR_PLLMUL32,
#endif
#ifdef RCC_CFGR_PLLMUL48
	_48 = RCC_CFGR_PLLMUL48,
#endif
};


inline void rcc_set_pll_mult_factor(PLLMult value)
{
	uint32_t cfgr = RCC->CFGR;
	cfgr &= ~RCC_CFGR_PLLMUL;
	cfgr |= (uint32_t)value;
	RCC->CFGR = cfgr;
	__DSB();
	__DSB();
}

#endif

#ifdef RCC_CFGR_PLLXTPRE_HSE_DIV2

inline void rcc_enable_hse_div_for_pll()
{
	RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE_DIV2;
}

inline void rcc_disable_hse_div2_for_pll()
{
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE_HSE_DIV2;
}

#endif


enum class PLLSource
{
#if defined RCC_CFGR_PLLSRC
	HSI = 0,               // HSI oscillator clock selected as PLL input clock
	HSE = RCC_CFGR_PLLSRC, // HSE oscillator clock selected as PLL input clock
#elif defined RCC_PLLCFGR_PLLSRC
	HSI = RCC_PLLCFGR_PLLSRC_HSI,
	HSE = RCC_PLLCFGR_PLLSRC_HSE,
#endif
};

inline void rcc_set_pll_source(PLLSource source)
{
#if defined RCC_CFGR_PLLSRC
	set_value_by_mask(RCC->CFGR, RCC_CFGR_PLLSRC, (uint32_t)source);
#elif defined RCC_PLLCFGR_PLLSRC
	set_value_by_mask(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, (uint32_t)source);
#endif
}


// HSE

inline void rcc_enable_hse(bool oscillator_bypass)
{
	RCC->CR |= RCC_CR_HSEON | (oscillator_bypass ? RCC_CR_HSEBYP : 0);
	while (!(RCC->CR & RCC_CR_HSERDY)) {}
}

inline void rcc_disable_hse()
{
	RCC->CR &= ~RCC_CR_HSEON;
	while (RCC->CR & RCC_CR_HSERDY) {}
}



// MSI

#ifdef RCC_CR_MSION

inline void rcc_enable_msi()
{
	RCC->CR |= RCC_CR_MSION;
	while (!(RCC->CR & RCC_CR_MSIRDY)) {}
}

inline void rcc_disable_msi()
{
	RCC->CR &= ~RCC_CR_MSION;
	while (RCC->CR & RCC_CR_MSIRDY) {}
}

#endif


#ifdef RCC_ICSCR_MSITRIM

inline void rcc_set_msi_trimming(uint8_t value)
{
	uint32_t icscr = RCC->ICSCR;
	icscr &= ~RCC_ICSCR_MSITRIM;
	icscr |= ((uint32_t)value << 24);
	RCC->ICSCR = icscr;
}

#endif

#ifdef RCC_ICSCR_MSIRANGE

enum class MSIRange
{
	_65_536_kHz  = 0b000 << 13, // range 0 around 65.536 kHz
	_131_072_kHz = 0b001 << 13, // range 1 around 131.072 kHz
	_262_144_kHz = 0b010 << 13, // range 2 around 262.144 kHz
	_524_288_kHz = 0b011 << 13, // range 3 around 524.288 kHz
	_1_048_MHz   = 0b100 << 13, // range 4 around 1.048 MHz
	_2_097_MHz   = 0b101 << 13, // range 5 around 2.097 MHz (reset value)
	_4_194_MHz   = 0b110 << 13, // range 6 around 4.194 MHz
};

inline void rcc_set_msi_range(MSIRange range)
{
	uint32_t icscr = RCC->ICSCR;
	icscr &= ~RCC_ICSCR_MSIRANGE;
	icscr |= (uint32_t)range;
	RCC->ICSCR = icscr;
}

#endif


// HSI

inline void rcc_enable_hsi()
{
	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY)) {}
}

inline void rcc_disable_hsi()
{
	RCC->CR &= ~RCC_CR_HSION;
	while (RCC->CR & RCC_CR_HSIRDY) {}
}

inline bool rcc_is_hsi_enabled()
{
	return (RCC->CR & RCC_CR_HSION);
}

#ifdef RCC_ICSCR_HSITRIM

inline void rcc_set_hsi_trimming(uint8_t value)
{
	uint32_t icscr = RCC->ICSCR;
	icscr &= ~RCC_ICSCR_HSITRIM;
	icscr |= ((uint32_t)value << 8);
	RCC->ICSCR = icscr;
}

#endif


// MCO

#ifdef RCC_CFGR_MCOPRE

enum class MCOPrescaler
{
	_1  = 0b000 << 28, // MCO is divided by 1
	_2  = 0b001 << 28, // MCO is divided by 2
	_4  = 0b010 << 28, // MCO is divided by 4
	_8  = 0b011 << 28, // MCO is divided by 8
	_16 = 0b100 << 28, // MCO is divided by 16
};

inline void rcc_set_mco_prescaler(MCOPrescaler prescaler)
{
	uint32_t cfgr = RCC->CFGR;
	cfgr &= ~RCC_CFGR_MCOPRE;
	cfgr |= (uint32_t)prescaler;
	RCC->CFGR = cfgr;
	__DSB();
	__DSB();
}

#endif

#if defined(RCC_CFGR_MCOSEL) | defined(RCC_CFGR_MCO)

enum class MCOClock
{
	Disabled = RCC_CFGR_MCO_NOCLOCK, // MCO output disabled, no clock on MCO
	SYSCLK   = RCC_CFGR_MCO_SYSCLK, // SYSCLK clock selected
	HSI      = RCC_CFGR_MCO_HSI, // HSI oscillator clock selected
#ifdef RCC_CFGR_MCO_MSI
	MSI      = RCC_CFGR_MCO_MSI, // MSI oscillator clock selected
#endif
	HSE      = RCC_CFGR_MCO_HSE, // HSE oscillator clock selected
#ifdef RCC_CFGR_MCO_PLLCLK_DIV2
	PPL_DIV2 = RCC_CFGR_MCO_PLLCLK_DIV2,
#endif
#ifdef RCC_CFGR_MCO_PLL
	PLL      = RCC_CFGR_MCO_PLL, // PLL clock selected
#endif
#ifdef RCC_CFGR_MCO_LSI
	LSI      = RCC_CFGR_MCO_LSI, // LSI oscillator clock selected
#endif
#ifdef RCC_CFGR_MCO_LSE
	LSE      = RCC_CFGR_MCO_LSE, // LSE oscillator clock selected
#endif
};

inline void rcc_set_mco_clock(MCOClock clock)
{
	uint32_t cfgr = RCC->CFGR;
#ifdef RCC_CFGR_MCOSEL
	cfgr &= ~RCC_CFGR_MCOSEL;
#endif
#ifdef RCC_CFGR_MCO
	cfgr &= ~RCC_CFGR_MCO;
#endif
	cfgr |= (uint32_t)clock;
	RCC->CFGR = cfgr;
	__DSB();
	__DSB();
}

#endif

// LSE

#ifdef RCC_CSR_LSEON

inline void rcc_enable_lse()
{
	RCC->CSR &= ~RCC_CSR_LSEON;
	__DSB();
	__DSB();
	RCC->CSR |= RCC_CSR_LSEON;
	while (!(RCC->CSR & RCC_CSR_LSERDY)) break;
}

inline bool rcc_is_lse_on()
{
	return RCC->CSR & RCC_CSR_LSEON;
}

#endif

#ifdef RCC_CSR_LSERDY
inline bool rcc_is_lse_ready()
{
	return RCC->CSR & RCC_CSR_LSERDY;
}
#endif


// LSI

inline void rcc_enable_lsi()
{
	RCC->CSR |= RCC_CSR_LSION;
	while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
}

inline bool rcc_is_lsi_ready()
{
	return RCC->CSR & RCC_CSR_LSIRDY;
}


// RTC

#ifdef RCC_CSR_RTCRST
inline void rcc_reset_rtc()
{
	auto tmp = (RCC->CSR & ~(RCC_CSR_RTCSEL));
	RCC->CSR |= RCC_CSR_RTCRST;
	__DSB();
	RCC->CSR &= ~RCC_CSR_RTCRST;
	__DSB();

	RCC->CSR = tmp;
}
#endif

#ifdef RCC_CSR_RTCEN
inline void rcc_enable_rtc()
{
	RCC->CSR |= RCC_CSR_RTCEN;
}

inline void rcc_disable_rtc()
{
	RCC->CSR &= ~RCC_CSR_RTCEN;
}

inline bool rcc_is_rtc_enabled()
{
	return RCC->CSR & RCC_CSR_RTCEN;
}

#endif

#ifdef RCC_APB2ENR_SYSCFGEN
inline void rcc_enable_syscfg_clock()
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	__DSB();
	__DSB();
	__NOP();
	__NOP();
}

inline void rcc_disable_syscfg_clock()
{
	RCC->APB2ENR &= ~RCC_APB2ENR_SYSCFGEN;
	__DSB();
	__DSB();
	__NOP();
	__NOP();
}
#endif

enum class RTCClock
{
#if defined(RCC_CSR_RTCSEL_NOCLOCK)
	No = RCC_CSR_RTCSEL_NOCLOCK,
#elif defined(RCC_BDCR_RTCSEL_NOCLOCK)
	No  = RCC_BDCR_RTCSEL_NOCLOCK,
#else
	No  = 0,
#endif

#if defined(RCC_CSR_RTCSEL_LSE)
	LSE = RCC_CSR_RTCSEL_LSE,     // LSE oscillator clock used as RTC/LCD clock
#elif defined(RCC_BDCR_RTCSEL_LSE)
	LSE = RCC_BDCR_RTCSEL_LSE,
#else
	LSE = RCC_BDCR_RTCSEL_0,
#endif

#if defined(RCC_CSR_RTCSEL_LSI)
	LSI = RCC_CSR_RTCSEL_LSI,
#elif defined (RCC_BDCR_RTCSEL_LSI)
	LSI = RCC_BDCR_RTCSEL_LSI,
#else
	LSI = RCC_BDCR_RTCSEL_1,
#endif

#if defined(RCC_CSR_RTCSEL_HSE)
	HSE = RCC_CSR_RTCSEL_HSE,
#elif defined(RCC_BDCR_RTCSEL_HSE)
	HSE = RCC_BDCR_RTCSEL_HSE,     // HSE oscillator clock divided by a programmable prescaler
#else
	HSE = RCC_BDCR_RTCSEL_0|RCC_BDCR_RTCSEL_1
#endif
};

inline void rcc_set_rtc_clock(RTCClock clock)
{
#ifdef RCC_CSR_RTCSEL
	set_value_by_mask(RCC->CSR, RCC_CSR_RTCSEL, (uint32_t)clock);
#endif
#ifdef RCC_BDCR_RTCSEL
	set_value_by_mask(RCC->BDCR, RCC_BDCR_RTCSEL, (uint32_t)clock);
#endif

}



// APB2 prescaler

enum class APB2Prescaler
{
#if defined (RCC_CFGR_PPRE2)
	_1  = RCC_CFGR_PPRE2_DIV1, // HCLK not divided
	_2  = RCC_CFGR_PPRE2_DIV2, // HCLK divided by 2
	_4  = RCC_CFGR_PPRE2_DIV4, // HCLK divided by 4
	_8  = RCC_CFGR_PPRE2_DIV8, // HCLK divided by 8
	_16 = RCC_CFGR_PPRE2_DIV16, // HCLK divided by 16
#elif defined(RCC_CFGR_PPRE)
	_1  = RCC_CFGR_PPRE_DIV1, // HCLK not divided
	_2  = RCC_CFGR_PPRE_DIV2, // HCLK divided by 2
	_4  = RCC_CFGR_PPRE_DIV4, // HCLK divided by 4
	_8  = RCC_CFGR_PPRE_DIV8, // HCLK divided by 8
	_16 = RCC_CFGR_PPRE_DIV16, // HCLK divided by 16
#else
	#error Not implemented
#endif
};

inline void rcc_set_apb2_prescaler(APB2Prescaler prescaler)
{
#if defined (RCC_CFGR_PPRE2)
	set_value_by_mask(RCC->CFGR, RCC_CFGR_PPRE2, (uint32_t)prescaler);
#elif defined(RCC_CFGR_PPRE)
	set_value_by_mask(RCC->CFGR, RCC_CFGR_PPRE, (uint32_t)prescaler);
#else
	#error Not implemented
#endif
	__DSB();
	__DSB();
}


// APB1 prescaler

enum class APB1Prescaler
{
#if defined (RCC_CFGR_PPRE1)
	_1  = RCC_CFGR_PPRE1_DIV1, // HCLK not divided
	_2  = RCC_CFGR_PPRE1_DIV2, // HCLK divided by 2
	_4  = RCC_CFGR_PPRE1_DIV4, // HCLK divided by 4
	_8  = RCC_CFGR_PPRE1_DIV8, // HCLK divided by 8
	_16 = RCC_CFGR_PPRE1_DIV16, // HCLK divided by 16
#elif defined(RCC_CFGR_PPRE)
	_1  = RCC_CFGR_PPRE_DIV1, // HCLK not divided
	_2  = RCC_CFGR_PPRE_DIV2, // HCLK divided by 2
	_4  = RCC_CFGR_PPRE_DIV4, // HCLK divided by 4
	_8  = RCC_CFGR_PPRE_DIV8, // HCLK divided by 8
	_16 = RCC_CFGR_PPRE_DIV16, // HCLK divided by 16
#else
	#error Not implemented
#endif
};

inline void rcc_set_apb1_prescaler(APB1Prescaler prescaler)
{
#if defined (RCC_CFGR_PPRE1)
	set_value_by_mask(RCC->CFGR, RCC_CFGR_PPRE1, (uint32_t)prescaler);
#elif defined(RCC_CFGR_PPRE)
	set_value_by_mask(RCC->CFGR, RCC_CFGR_PPRE, (uint32_t)prescaler);
#else
	#error Not implemented
#endif
	__DSB();
	__DSB();
}


// AHB prescaler

enum class AHBPrescaler
{
	_1   = RCC_CFGR_HPRE_DIV1,   // SYSCLK not divided
	_2   = RCC_CFGR_HPRE_DIV2,   // SYSCLK divided by 2
	_4   = RCC_CFGR_HPRE_DIV4,   // SYSCLK divided by 4
	_8   = RCC_CFGR_HPRE_DIV8,   // SYSCLK divided by 8
	_16  = RCC_CFGR_HPRE_DIV16,  // SYSCLK divided by 16
	_64  = RCC_CFGR_HPRE_DIV64,  // SYSCLK divided by 64
	_128 = RCC_CFGR_HPRE_DIV128, // SYSCLK divided by 128
	_256 = RCC_CFGR_HPRE_DIV256, // SYSCLK divided by 256
	_512 = RCC_CFGR_HPRE_DIV512, // SYSCLK divided by 512
};

inline void rcc_set_ahb_prescaler(AHBPrescaler prescaler)
{
	set_value_by_mask(RCC->CFGR, RCC_CFGR_HPRE, (uint32_t)prescaler);
	__DSB();
	__DSB();
}


// system clock

enum class SystemClock
{
#ifdef RCC_CFGR_SW_MSI
	MSI = RCC_CFGR_SW_MSI, // MSI oscillator used as system clock
#endif
#ifdef RCC_CFGR_SW_HSI
	HSI = RCC_CFGR_SW_HSI, // HSI oscillator used as system clock
#endif
#ifdef RCC_CFGR_SW_HSE
	HSE = RCC_CFGR_SW_HSE, // HSE oscillator used as system clock
#endif
#ifdef RCC_CFGR_SW_PLL
	PLL = RCC_CFGR_SW_PLL, // PLL used as system clock
#endif
};


inline void rcc_set_sys_clock(SystemClock clock)
{
	set_value_by_mask(RCC->CFGR, RCC_CFGR_SW, (uint32_t)clock);
	__DSB();
	__DSB();
	while ((RCC_CFGR_SWS & RCC->CFGR) != ((uint32_t)clock << RCC_CFGR_SWS_Pos)) {}
}

inline SystemClock rcc_get_sys_clock()
{
	return SystemClock(RCC->CFGR & RCC_CFGR_SW);
}

#ifdef HL_STM32L1XX

namespace detailed {

inline void rcc_init_msi_for_other_clock_config()
{
	rcc_enable_msi();
	rcc_set_msi_range(MSIRange::_4_194_MHz);
	rcc_set_sys_clock(SystemClock::MSI);
}

inline void rcc_init_ppl_speed_and_source(PLLMult mult, PLLHSEDiv div, PLLSource source)
{
	rcc_disable_pll();
	rcc_set_pll_source(source);
	rcc_set_pll_output_division(div);
	rcc_set_pll_mult_factor(mult);
	rcc_enable_pll();
}

} // namespace detailed

inline void rcc_init_and_select_hse_clock(PLLMult mult, PLLHSEDiv div, bool oscillator_bypass)
{
	detailed::rcc_init_msi_for_other_clock_config();
	rcc_enable_hse(oscillator_bypass);
	detailed::rcc_init_ppl_speed_and_source(mult, div, PLLSource::HSE);
	rcc_set_sys_clock(SystemClock::PLL);
	rcc_disable_msi();
}

inline void rcc_init_and_select_hsi_clock(PLLMult mult, PLLHSEDiv div)
{
	detailed::rcc_init_msi_for_other_clock_config();
	rcc_enable_hsi();
	detailed::rcc_init_ppl_speed_and_source(mult, div, PLLSource::HSI);
	rcc_set_sys_clock(SystemClock::PLL);
	rcc_disable_msi();
}

#endif

} // namespace hl
