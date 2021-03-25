#pragma once

#include <stdint.h>
#include "hl_device.hpp"

namespace hl {

// fwd.
namespace detailed {
	template <unsigned N> struct TimerHelper;
	template <unsigned N> struct TimerChanHelper;
}

///////////////////////////////////////////////////////////////////////////////

enum class TimMasterMode
{
	Reset  = 0 << 4,
	Enable = 1 << 4,
	Update = 2 << 4
};

template <unsigned N>
class BasicTimer : public Device<detailed::TimerHelper<N>>
{
public:
	typedef detailed::TimerHelper<N> Helper;

	static constexpr IRQn_Type IRQn = Helper::IRQn;


	static void enable_auto_reload_preload()
	{
		set_periph_reg_bit<TIM_CR1, TIM_CR1_ARPE>();
	}

	static void disable_auto_reload_preload()
	{
		clear_periph_reg_bit<TIM_CR1, TIM_CR1_ARPE>();
	}

	static void enable_one_pulse_mode()
	{
		set_periph_reg_bit<TIM_CR1, TIM_CR1_OPM>();
	}

	static void disable_one_pulse_mode()
	{
		clear_periph_reg_bit<TIM_CR1, TIM_CR1_OPM>();
	}

	static void generate_update()
	{
		HL_UI32REG(TIM_EGR) = TIM_EGR_UG;
	}

	// CR1_URS ???
	// CR1_UDIS ???

	static void start()
	{
		set_periph_reg_bit<TIM_CR1, TIM_CR1_CEN>();
	}

	static bool is_started()
	{
		return get_periph_reg_bit<TIM_CR1, TIM_CR1_CEN>();
	}

	static void stop()
	{
		clear_periph_reg_bit<TIM_CR1, TIM_CR1_CEN>();
		__DSB();
		__NOP();
	}

	static void set_master_mode(TimMasterMode mode)
	{
		set_value_by_mask<uint32_t>(TIM_CR2, TIM_CR2_MMS, (uint32_t)mode);
	}

	static void enable_update_dma_request()
	{
		set_periph_reg_bit<TIM_DIER, TIM_DIER_UDE>();
	}

	static void disable_update_dma_request()
	{
		clear_periph_reg_bit<TIM_DIER, TIM_DIER_UDE>();
	}

	static void enable_update_interrupt()
	{
		set_periph_reg_bit<TIM_DIER, TIM_DIER_UIE>();
	}

	static void disable_update_interrupt()
	{
		clear_periph_reg_bit<TIM_DIER, TIM_DIER_UIE>();
	}

	static void clear_all_flags()
	{
		HL_UI32REG(TIM_SR) = 0;
		HL_UI32REG(TIM_SR);
	}

	static bool get_update_interrupt_flag()
	{
		return get_periph_reg_bit<TIM_SR, TIM_SR_UIF>();
	}

	static void clear_update_interrupt_flag()
	{
		HL_UI32REG(TIM_SR) = ~TIM_SR_UIF;
		HL_UI32REG(TIM_SR); // <- HACK to avoid the problem with race condition of twice ticks:
	}

	static void set_counter(uint16_t value)
	{
		HL_UI32REG(TIM_CNT) = value;
	}

	static uint16_t get_counter()
	{
		return HL_UI32REG(TIM_CNT);
	}

	static void set_prescaler(uint16_t value)
	{
		HL_UI32REG(TIM_PSC) = value;
	}

	static uint16_t get_prescaler()
	{
		return HL_UI32REG(TIM_PSC);
	}

	static void set_auto_reload_value(uint16_t value)
	{
		HL_UI32REG(TIM_ARR) = value;
	}

	static uint16_t get_auto_reload_value()
	{
		return HL_UI32REG(TIM_ARR);
	}

protected:
	static constexpr uintptr_t tim = Helper::tim;

	static constexpr uintptr_t TIM_CR1  = tim + offsetof(TIM_TypeDef, CR1);
	static constexpr uintptr_t TIM_CR2  = tim + offsetof(TIM_TypeDef, CR2);
	static constexpr uintptr_t TIM_DIER = tim + offsetof(TIM_TypeDef, DIER);
	static constexpr uintptr_t TIM_SR   = tim + offsetof(TIM_TypeDef, SR);
	static constexpr uintptr_t TIM_SMCR = tim + offsetof(TIM_TypeDef, SMCR);
	static constexpr uintptr_t TIM_CCER = tim + offsetof(TIM_TypeDef, CCER);
	static constexpr uintptr_t TIM_CNT  = tim + offsetof(TIM_TypeDef, CNT);
	static constexpr uintptr_t TIM_PSC  = tim + offsetof(TIM_TypeDef, PSC);
	static constexpr uintptr_t TIM_ARR  = tim + offsetof(TIM_TypeDef, ARR);
	static constexpr uintptr_t TIM_EGR  = tim + offsetof(TIM_TypeDef, EGR);
};


///////////////////////////////////////////////////////////////////////////////

enum class TimerClockDivision
{
	_1 = 0 << 8,
	_2 = 1 << 8,
	_4 = 2 << 8,
};

enum class TimerAlign
{
	Edge    = 0 << 5,
	Center1 = 1 << 5,
	Center2 = 2 << 5,
	Center3 = 3 << 5,
};

enum class TimerDirection
{
	Up   = 0,
	Down = TIM_CR1_DIR
};

enum class PWMMode
{
	_1 = 6 << 4,
	_2 = 7 << 4
};

enum class PWMPolarity
{
	High = 0 << 1,
	Low  = 1 << 1
};

enum class PWMPreload
{
	Disable = 0,
	Enable = TIM_CCMR1_OC1PE,
};

enum class ICPolarity
{
	Rising  = 0b000 << 1,
	Falling = 0b001 << 1,
	Both    = 0b101 << 1
};

enum class ICPrescaler
{
	No = 0b00 << 2,
	_2 = 0b01 << 2,
	_4 = 0b10 << 2,
	_8 = 0b11 << 2,
};

enum class ICFilter
{
	No    = 0b0000 << 4, // No filter, sampling is done at f DTS
	_1_2  = 0b0001 << 4, // f SAMPLING =f CK_INT , N=2
	_1_4  = 0b0010 << 4, //f SAMPLING =f CK_INT , N=4
	_1_8  = 0b0011 << 4, //f SAMPLING =f CK_INT , N=8
	_2_6  = 0b0100 << 4, //f SAMPLING =f DTS /2, N=6
	_2_8  = 0b0101 << 4, //f SAMPLING =f DTS /2, N=8
	_4_6  = 0b0110 << 4, //f SAMPLING =f DTS /4, N=6
	_4_8  = 0b0111 << 4, //f SAMPLING =f DTS /4, N=8
	_8_6  = 0b1000 << 4, //f SAMPLING =f DTS /8, N=6
	_8_8  = 0b1001 << 4, //f SAMPLING =f DTS /8, N=8
	_16_5 = 0b1010 << 4, //f SAMPLING =f DTS /16, N=5
	_16_6 = 0b1011 << 4, //f SAMPLING =f DTS /16, N=6
	_16_8 = 0b1100 << 4, //f SAMPLING =f DTS /16, N=8
	_32_5 = 0b1101 << 4, //f SAMPLING =f DTS /32, N=5
	_32_6 = 0b1110 << 4, //f SAMPLING =f DTS /32, N=6
	_32_8 = 0b1111 << 4, //f SAMPLING =f DTS /32, N=8
};

enum class ICChanMapping
{
	Same = 0b01 << 0,
	Near = 0b10 << 0,
	TRC  = 0b11 << 0,
};

// Slave modes

enum class SlaveMode
{
	Disable       = 0b000 << 0, // Slave mode disabled - if CEN = ‘1 then the prescaler is clocked directly by the internal clock.
	Encoder1      = 0b001 << 0, // Encoder mode 1 - Counter counts up/down on TI2FP2 edge depending on TI1FP1 level.
	Encoder2      = 0b010 << 0, // Encoder mode 2 - Counter counts up/down on TI1FP1 edge depending on TI2FP2 level.
	Encoder3      = 0b011 << 0, // Encoder mode 3 - Counter counts up/down on both TI1FP1 and TI2FP2 edges depending on the level of the other input.
	Reset         = 0b100 << 0, // Reset Mode - Rising edge of the selected trigger input (TRGI) reinitializes the counter and generates an update of the registers.
	Gated         = 0b101 << 0, // Gated Mode - The counter clock is enabled when the trigger input (TRGI) is high. The counter stops (but is not reset) as soon as the trigger becomes low. Both start and stop of the counter are controlled.
	Trigger       = 0b110 << 0, // Trigger Mode - The counter starts at a rising edge of the trigger TRGI (but it is not reset). Only the start of the counter is controlled.
	ExternalClock = 0b111 << 0, // External Clock Mode 1 - Rising edges of the selected trigger (TRGI) clock the counter.
};

// Slave mode triggers
enum class SlaveModeTrigger
{
	InternTrigger0      = 0b000 << 4, // Internal Trigger 0 (ITR0)
	InternTrigger1      = 0b001 << 4, // Internal Trigger 1 (ITR1)
	InternTrigger2      = 0b010 << 4, // Internal Trigger 2 (ITR2)
	InternTrigger3      = 0b011 << 4, // Internal Trigger 3 (ITR3)
	TI1EdgeDetector     = 0b100 << 4, // TI1 Edge Detector (TI1F_ED)
	FilteredTimerInput1 = 0b101 << 4, // Filtered Timer Input 1 (TI1FP1)
	FilteredTimerInput2 = 0b110 << 4, // Filtered Timer Input 2 (TI2FP2)
	ExternalTrigger     = 0b111 << 4, // External Trigger input (ETRF)
};

///////////////////////////////////////////////////////////////////////////////

template <unsigned N>
class GeneralPurposeTimer : public BasicTimer<N>
{
public:
	static void set_clock_division(TimerClockDivision value)
	{
		set_value_by_mask<uint32_t>(TIM_CR1, TIM_CR1_CKD, (uint32_t)value);
	}

	static void set_align(TimerAlign value)
	{
		set_value_by_mask<uint32_t>(TIM_CR1, TIM_CR1_CMS, (uint32_t)value);
	}

	static void set_direction(TimerDirection value)
	{
		set_value_by_mask<uint32_t>(TIM_CR1, TIM_CR1_DIR, (uint32_t)value);
	}

	static void connect_ch1_to_ti1_input()
	{
		clear_periph_reg_bit<TIM_CR2, TIM_CR2_TI1S>();
	}

	static void connect_ch1_ch2_ch3_to_ti1_input()
	{
		set_periph_reg_bit<TIM_CR2, TIM_CR2_TI1S>();
	}


	// DMA

	static void enable_trigger_dma_request()
	{
		set_periph_reg_bit<TIM_DIER, TIM_DIER_TDE>();
	}

	static void disable_trigger_dma_request()
	{
		clear_periph_reg_bit<TIM_DIER, TIM_DIER_TDE>();
	}

	static bool is_trigger_dma_request_enabled()
	{
		return get_periph_reg_bit<TIM_DIER, TIM_DIER_TDE>();
	}

	// interrupts

	static void enable_trigger_interrupt()
	{
		set_periph_reg_bit<TIM_DIER, TIM_DIER_TIE>();
	}

	static void disable_trigger_interrupt()
	{
		clear_periph_reg_bit<TIM_DIER, TIM_DIER_TIE>();
	}

	static bool is_trigger_interrupt_enabled()
	{
		return get_periph_reg_bit<TIM_DIER, TIM_DIER_TIE>();
	}

	template <unsigned Chan>
	static void enable_cc_interrupt()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		set_periph_reg_bit<TIM_DIER, 1 << Chan>();
		__DSB();
		__NOP();
	}

	template <unsigned Chan>
	static void disable_cc_interrupt()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		clear_periph_reg_bit<TIM_DIER, 1 << Chan>();
		__DSB();
		__NOP();
	}

	template <unsigned Chan>
	static bool is_cc_interrupt_enabled()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		return get_periph_reg_bit<TIM_DIER, 1 << Chan>();
	}

	// DMA

	template <unsigned Chan>
	static void enable_cc_dma_request()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		set_periph_reg_bit<TIM_DIER, detailed::TimerChanHelper<Chan>::cc_dma>();
	}

	template <unsigned Chan>
	static void disable_cc_dma_request()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		clear_periph_reg_bit<TIM_DIER, detailed::TimerChanHelper<Chan>::cc_dma>();
	}

	template <unsigned Chan>
	static bool is_cc_dma_request_enabled()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		return get_periph_reg_bit<TIM_DIER, detailed::TimerChanHelper<Chan>::cc_dma>();
	}


	// slave mode

	static void set_slave_mode(SlaveMode mode)
	{
		set_value_by_mask<uint32_t>(TIM_SMCR, TIM_SMCR_SMS, (uint32_t)mode);
	}

	static void set_slave_mode_trigger(SlaveModeTrigger trigger)
	{
		set_value_by_mask<uint32_t>(TIM_SMCR, TIM_SMCR_TS, (uint32_t)trigger);
	}

	static void enable_slave_mode()
	{
		set_periph_reg_bit<TIM_SMCR, TIM_SMCR_MSM>();
	}

	static void disable_slave_mode()
	{
		clear_periph_reg_bit<TIM_SMCR, TIM_SMCR_MSM>();
	}


	// status

	template <unsigned Chan>
	static bool get_overcapture_flag()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		constexpr unsigned mask = 1 << (Chan + 8);
		return get_periph_reg_bit<TIM_SR, mask>();
	}

	template <unsigned Chan>
	static void clear_overcapture_flag()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		HL_UI32REG(TIM_SR) = ~(1 << (Chan + 8));
		HL_UI32REG(TIM_SR);
	}

	static bool get_trigger_interrupt_flag()
	{
		return get_periph_reg_bit<TIM_SR, TIM_SR_TIF>() != 0;
	}

	static void reset_trigger_interrupt_flag()
	{
		HL_UI32REG(TIM_SR) = ~TIM_SR_TIF;
		HL_UI32REG(TIM_SR);
	}

	template <unsigned Chan>
	static bool get_cc_flag()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		constexpr unsigned mask = 1 << Chan;
		return get_periph_reg_bit<TIM_SR, mask>();
	}

	template <unsigned Chan>
	static void clear_cc_flag()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		HL_UI32REG(TIM_SR) = ~(1 << Chan);
		HL_UI32REG(TIM_SR);
	}

	//////

	template <unsigned Chan>
	static void disable_output_compare()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");

		typedef detailed::TimerChanHelper<Chan> ChanHelper;
		uint16_t tmp = HL_UI32REG(tim + ChanHelper::ccmr_offset);
		tmp &= ~(TIM_CCMR1_OC1CE << ChanHelper::ccmr_shift);
		HL_UI32REG(tim + ChanHelper::ccmr_offset) = tmp;
	}

	template <unsigned Chan>
	static void conf_pwm(PWMMode mode, PWMPolarity polarity, PWMPreload preload)
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");

		typedef detailed::TimerChanHelper<Chan> ChanHelper;
		constexpr unsigned ccer_shift = (Chan-1) * 4;

		HL_UI32REG(TIM_CCER) &= ~(TIM_CCER_CC1E << ccer_shift); // disable channel before configure

		uint16_t ccmr = HL_UI32REG(tim + ChanHelper::ccmr_offset) & ~(0xFF << ChanHelper::ccmr_shift);
		uint16_t ccer = HL_UI32REG(TIM_CCER) & ~(0xF << ccer_shift);

		ccmr |= ((uint16_t)mode << ChanHelper::ccmr_shift);
		ccmr |= ((uint16_t)preload << ChanHelper::ccmr_shift);

		ccer |= ((uint16_t)polarity << ccer_shift);
		ccer |= (TIM_CCER_CC1E << ccer_shift); // signal is output on the corresponding output pin

		HL_UI32REG(TIM_CCER) = ccer;
		HL_UI32REG(tim + ChanHelper::ccmr_offset) = ccmr;
	}

	template <unsigned Chan>
	static void set_pwm_value(uint16_t value)
	{
		get_ccr<Chan>() = value;
	}

	template <unsigned Chan>
	static uint16_t get_pwm_value()
	{
		return get_ccr<Chan>();
	}


	template <unsigned Chan>
	static void conf_ic(
		ICChanMapping mapping,
		ICPolarity    polarity,
		ICPrescaler   prescaler,
		ICFilter      filter)
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");

		typedef detailed::TimerChanHelper<Chan> ChanHelper;
		constexpr unsigned ccer_shift = (Chan-1) * 4;

		uint16_t ccer = HL_UI32REG(TIM_CCER);
		ccer &= ~(0xF << ccer_shift);
		uint16_t ccmr = HL_UI32REG(tim + ChanHelper::ccmr_offset);
		ccmr &= ~(0xFF << ChanHelper::ccmr_shift);

		ccmr |= ((uint16_t)mapping << ChanHelper::ccmr_shift); // channel mapping
		ccmr |= ((uint16_t)prescaler << ChanHelper::ccmr_shift); // prescaler
		ccmr |= ((uint16_t)filter << ChanHelper::ccmr_shift); // filter

		ccer |= ((uint16_t)polarity << ccer_shift); // polarity

		HL_UI32REG(tim + ChanHelper::ccmr_offset) = ccmr;
		HL_UI32REG(TIM_CCER) = ccer;
	}

	template <unsigned Chan>
	static void disable_capture()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		constexpr unsigned mask = TIM_CCER_CC1E << (Chan-1) * 4;
		clear_periph_reg_bit<TIM_CCER, mask>();
	}

	template <unsigned Chan>
	static void enable_capture()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		constexpr unsigned mask = TIM_CCER_CC1E << (Chan-1) * 4;
		set_periph_reg_bit<TIM_CCER, mask>();
	}

	template <unsigned Chan>
	static uint16_t get_cc_result()
	{
		return get_ccr<Chan>();
	}

	template <unsigned Chan>
	static uintptr_t get_cc_addr()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		typedef detailed::TimerChanHelper<Chan> ChanHelper;
		return tim + ChanHelper::ccr_offset;
	}

protected:
	using BasicTimer<N>::tim;
	using BasicTimer<N>::TIM_CR1;
	using BasicTimer<N>::TIM_CR2;
	using BasicTimer<N>::TIM_DIER;
	using BasicTimer<N>::TIM_SR;
	using BasicTimer<N>::TIM_SMCR;
	using BasicTimer<N>::TIM_CCER;

private:
	template <unsigned Chan>
	static volatile uint32_t& get_ccr()
	{
		static_assert(Chan >= 1 && Chan <= 4, "Wrong channel value");
		typedef detailed::TimerChanHelper<Chan> ChanHelper;
		return HL_UI32REG(tim + ChanHelper::ccr_offset);
	}
};

///////////////////////////////////////////////////////////////////////////////

template <unsigned N>
class AdvancedControlTimer : public GeneralPurposeTimer<N>
{
public:
	static void enable_pwm_outputs()
	{
		set_periph_reg_bit<TIM_BDTR, TIM_BDTR_MOE>();
	}

	static void disable_pwm_outputs()
	{
		clear_periph_reg_bit<TIM_BDTR, TIM_BDTR_MOE>();
	}

private:
	using GeneralPurposeTimer<N>::tim;

	static constexpr uintptr_t TIM_BDTR = tim + offsetof(TIM_TypeDef, BDTR);
};


///////////////////////////////////////////////////////////////////////////////

#ifdef TIM1
template <> struct detailed::TimerHelper<1>
{
	static constexpr uintptr_t tim = TIM1_BASE;
#ifdef HL_STM32F0XX
	static constexpr IRQn_Type IRQn = TIM1_BRK_UP_TRG_COM_IRQn;
#else
	static constexpr IRQn_Type IRQn = TIM1_UP_TIM10_IRQn;
#endif
#ifdef RCC_APB2ENR_TIM1EN
	static constexpr uint32_t RccBit = RCC_APB2ENR_TIM1EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
#endif
};
typedef AdvancedControlTimer<1> Timer1;
#endif // TIM1

#ifdef TIM2
template <> struct detailed::TimerHelper<2>
{
	static constexpr uintptr_t tim = TIM2_BASE;
	static constexpr IRQn_Type IRQn = TIM2_IRQn;
#ifdef RCC_APB1ENR_TIM2EN
	static constexpr uint32_t RccBit = RCC_APB1ENR_TIM2EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
#endif
};
typedef GeneralPurposeTimer<2> Timer2;
#endif // TIM2

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM3
template <> struct detailed::TimerHelper<3>
{
	static constexpr uintptr_t tim = TIM3_BASE;
	static constexpr IRQn_Type IRQn = TIM3_IRQn;
#ifdef RCC_APB1ENR_TIM3EN
	static constexpr uint32_t RccBit = RCC_APB1ENR_TIM3EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
#endif
};
typedef GeneralPurposeTimer<3> Timer3;
#endif // TIM3

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM4
template <> struct detailed::TimerHelper<4>
{
	static constexpr uintptr_t tim = TIM4_BASE;
	static constexpr IRQn_Type IRQn = TIM4_IRQn;
#ifdef RCC_APB1ENR_TIM4EN
	static constexpr uint32_t RccBit = RCC_APB1ENR_TIM4EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
#endif
};
typedef GeneralPurposeTimer<4> Timer4;
#endif // TIM4

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM5
template <> struct detailed::TimerHelper<5>
{
	static constexpr uintptr_t tim = TIM5_BASE;
	static constexpr IRQn_Type IRQn = TIM5_IRQn;
#ifdef RCC_APB1ENR_TIM5EN
	static constexpr uint32_t RccBit = RCC_APB1ENR_TIM5EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
#endif
};
typedef GeneralPurposeTimer<5> Timer5;
#endif // TIM5

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM6
template <> struct detailed::TimerHelper<6>
{
	static constexpr uintptr_t tim = TIM6_BASE;
#if defined(HL_STM32L1XX)
	static constexpr IRQn_Type IRQn = TIM6_IRQn;
#elif defined(HL_STM32F3XX) || defined(HL_STM32F4XX)
	static constexpr IRQn_Type IRQn = TIM6_DAC_IRQn;
#endif
#ifdef RCC_APB1ENR_TIM6EN
	static constexpr uint32_t RccBit = RCC_APB1ENR_TIM6EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
#endif
};
typedef BasicTimer<6> Timer6;
#endif // TIM6

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM7
template <> struct detailed::TimerHelper<7>
{
	static constexpr uintptr_t tim = TIM7_BASE;
	static constexpr IRQn_Type IRQn = TIM7_IRQn;
#ifdef RCC_APB1ENR_TIM7EN
	static constexpr uint32_t RccBit = RCC_APB1ENR_TIM7EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
#endif
};
typedef BasicTimer<7> Timer7;
#endif // TIM7

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM9
template <> struct detailed::TimerHelper<9>
{
	static constexpr uintptr_t tim = TIM9_BASE;
#ifdef HL_STM32F4XX
	static constexpr IRQn_Type IRQn = TIM1_BRK_TIM9_IRQn;
#else
	static constexpr IRQn_Type IRQn = TIM9_IRQn;
#endif
#ifdef RCC_APB2ENR_TIM9EN
	static constexpr uint32_t RccBit = RCC_APB2ENR_TIM9EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
#endif
};
typedef GeneralPurposeTimer<9> Timer9;
#endif // TIM9

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM10
template <> struct detailed::TimerHelper<10>
{
	static constexpr uintptr_t tim = TIM10_BASE;
#ifdef RCC_APB2ENR_TIM10EN
	static constexpr uint32_t RccBit = RCC_APB2ENR_TIM10EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
#endif
#ifdef HL_STM32F4XX
	static constexpr IRQn_Type IRQn = TIM1_UP_TIM10_IRQn;
#else
	static constexpr IRQn_Type IRQn = TIM10_IRQn;
#endif
};
typedef GeneralPurposeTimer<10> Timer10;
#endif // TIM10

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM11
template <> struct detailed::TimerHelper<11>
{
	static constexpr uintptr_t tim = TIM11_BASE;
#ifdef HL_STM32F4XX
	static constexpr IRQn_Type IRQn = TIM1_TRG_COM_TIM11_IRQn;
#else
	static constexpr IRQn_Type IRQn = TIM11_IRQn;
#endif
#ifdef RCC_APB2ENR_TIM11EN
	static constexpr uint32_t RccBit = RCC_APB2ENR_TIM11EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
#endif
};
typedef GeneralPurposeTimer<11> Timer11;
#endif // TIM11

///////////////////////////////////////////////////////////////////////////////

#if defined(TIM14) && defined(RCC_APB1ENR_TIM14EN)
template <> struct detailed::TimerHelper<14>
{
	static constexpr uintptr_t tim = TIM14_BASE;
	static constexpr uint32_t RccBit = RCC_APB1ENR_TIM14EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
	static constexpr IRQn_Type IRQn = TIM14_IRQn;
};
typedef GeneralPurposeTimer<14> Timer14;
#endif // TIM14


#if defined(TIM15) && defined(RCC_APB2ENR_TIM15EN)
template <> struct detailed::TimerHelper<15>
{
	static constexpr uintptr_t tim = TIM15_BASE;
	static constexpr uint32_t RccBit = RCC_APB2ENR_TIM15EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
#if defined(HL_STM32F1XX)
	static constexpr IRQn_Type IRQn = TIM1_BRK_TIM15_IRQn;
#else
	static constexpr IRQn_Type IRQn = TIM15_IRQn;
#endif
};
typedef GeneralPurposeTimer<15> Timer15;
#endif // TIM15

///////////////////////////////////////////////////////////////////////////////

#if defined(TIM16) && defined(RCC_APB2ENR_TIM16EN)
template <> struct detailed::TimerHelper<16>
{
	static constexpr uintptr_t tim = TIM16_BASE;
	static constexpr uint32_t RccBit = RCC_APB2ENR_TIM16EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
#if defined(HL_STM32F1XX)
	static constexpr IRQn_Type IRQn = TIM1_UP_TIM16_IRQn;
#else
	static constexpr IRQn_Type IRQn = TIM16_IRQn;
#endif
};
typedef GeneralPurposeTimer<16> Timer16;
#endif // TIM16

///////////////////////////////////////////////////////////////////////////////

#ifdef TIM17
template <> struct detailed::TimerHelper<17>
{
	static constexpr uintptr_t tim = TIM17_BASE;
#if defined(HL_STM32F1XX)
	static constexpr IRQn_Type IRQn = TIM1_TRG_COM_TIM17_IRQn;
#else
	static constexpr IRQn_Type IRQn = TIM17_IRQn;
#endif
#ifdef RCC_APB2ENR_TIM17EN
	static constexpr uint32_t RccBit = RCC_APB2ENR_TIM17EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
#endif
};
typedef GeneralPurposeTimer<17> Timer17;
#endif // TIM17

///////////////////////////////////////////////////////////////////////////////

template <>
struct detailed::TimerChanHelper<1>
{
	static constexpr uint32_t ccmr_offset = offsetof(TIM_TypeDef, CCMR1);
	static constexpr uint32_t ccr_offset = offsetof(TIM_TypeDef, CCR1);
	static constexpr unsigned ccmr_shift = 0;
	static constexpr uint32_t cc_dma = TIM_DIER_CC1DE;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct detailed::TimerChanHelper<2>
{
	static constexpr uint32_t ccmr_offset = offsetof(TIM_TypeDef, CCMR1);
	static constexpr uint32_t ccr_offset = offsetof(TIM_TypeDef, CCR2);
	static constexpr unsigned ccmr_shift = 8;
	static constexpr uint32_t cc_dma = TIM_DIER_CC2DE;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct detailed::TimerChanHelper<3>
{
	static constexpr uint32_t ccmr_offset = offsetof(TIM_TypeDef, CCMR2);
	static constexpr uint32_t ccr_offset = offsetof(TIM_TypeDef, CCR3);
	static constexpr unsigned ccmr_shift = 0;
	static constexpr uint32_t cc_dma = TIM_DIER_CC3DE;
};

///////////////////////////////////////////////////////////////////////////////

template <>
struct detailed::TimerChanHelper<4>
{
	static constexpr uint32_t ccmr_offset = offsetof(TIM_TypeDef, CCMR2);
	static constexpr uint32_t ccr_offset = offsetof(TIM_TypeDef, CCR4);
	static constexpr unsigned ccmr_shift = 8;
	static constexpr uint32_t cc_dma = TIM_DIER_CC4DE;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace hl
