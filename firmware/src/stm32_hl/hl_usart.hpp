#pragma once

#include <stdint.h>
#include "hl_gpio.hpp"

namespace hl {

#ifdef USART_CR1_OVER8
enum class UsartOversampling
{
	_16 = 0,
	_8  = USART_CR1_OVER8,
};
#endif

enum class UsartWordLen
{
	_8 = 0,
	_9 = USART_CR1_M,
};

enum class UsartWakeup
{
	IdleLine    = 0,
	AddressMark = USART_CR1_WAKE,
};

enum class UsartReceiverMode
{
	Active = 0,
	Mute   = USART_CR1_RWU,
};

enum class StopBits
{
	_1   = 0b00 << USART_CR2_STOP_Pos,
	_0_5 = 0b01 << USART_CR2_STOP_Pos,
	_2   = 0b10 << USART_CR2_STOP_Pos,
	_1_5 = 0b11 << USART_CR2_STOP_Pos,
};

enum class UsartClockPolarity
{
	Low  = 0,
	High = USART_CR2_CPOL,
};

enum class UsartClockPhase
{
	First  = 0,
	Second = USART_CR2_CPHA,
};

enum class LinBreakLen
{
	_10_bit = 0,
	_11_bit = USART_CR2_LBDL,
};

#ifdef USART_CR3_ONEBIT
enum class SampleBitMethod
{
	Three = 0,
	One   = USART_CR3_ONEBIT,
};
#endif


enum class IrDAPowerMode
{
	Normal   = 0,
	LowPower = USART_CR3_IRLP,
};

namespace detailed {
	template<int N> struct UsartDevice;
}

template<int N>
class Usart :
	public Device<detailed::UsartDevice<N>>
{
public:
	static constexpr IRQn_Type IRQn = detailed::UsartDevice<N>::IRQn;


	static bool get_cts_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_CTS>();
	}

	static void clear_cts_flag()
	{
		HL_UI32REG(sr_addr) = ~USART_SR_CTS;
	}

	static bool get_lin_break_detection_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_LBD>();
	}

	static void clear_lin_break_detection_flag()
	{
		HL_UI32REG(sr_addr) = ~USART_SR_LBD;
	}

	static bool get_txe_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_TXE>();
	}

	static bool get_tc_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_TC>();
	}

	static void clear_tc_flag()
	{
		HL_UI32REG(sr_addr) = ~USART_SR_TC;
	}

	static bool get_rxne_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_RXNE>();
	}

	static void clear_rxne_flag()
	{
		HL_UI32REG(sr_addr) = ~USART_SR_RXNE;
	}

	static bool get_idle_line_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_IDLE>();
	}

	static bool get_overrun_error_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_ORE>();
	}

	static bool get_noise_error_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_NE>();
	}

	static bool get_framing_error_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_FE>();
	}

	static bool get_parity_error_flag()
	{
		return get_periph_reg_bit<sr_addr, USART_SR_PE>();
	}

	///

	static void send_data(uint16_t value)
	{
		HL_UI32REG(dr_addr) = value;
	}

	static void send_data_and_wait(uint16_t value)
	{
		HL_UI32REG(dr_addr) = value;
		while (!get_txe_flag()) {}
	}

	static uint16_t get_data()
	{
		return HL_UI32REG(dr_addr);
	}

	///

	static void set_baud_rate(uint16_t mantissa, uint8_t fraction)
	{
		HL_UI32REG(brr_addr) = (uint32_t)(mantissa << 4) | (uint32_t)fraction;
	}

	static void set_baud_rate(uint32_t value)
	{
		HL_UI32REG(brr_addr) = value;
	}

#ifdef USART_CR1_OVER8
	static void set_oversampling(UsartOversampling value)
	{
		set_value_by_mask<uint32_t>(cr1_addr, USART_CR1_OVER8, (uint32_t)value);
	}
#endif

	static void enable()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_UE>();
	}

	static void disable()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_UE>();
	}

	static void set_word_len(UsartWordLen len)
	{
		set_value_by_mask<uint32_t>(cr1_addr, USART_CR1_M, (uint32_t)len);
	}

	static void set_wakeup_method(UsartWakeup method)
	{
		set_value_by_mask<uint32_t>(cr1_addr, USART_CR1_WAKE, (uint32_t)method);
	}

	static void enable_parity_control()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_PCE>();
	}

	static void disable_parity_control()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_PCE>();
	}

	static void enable_pe_interrupt()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_PEIE>();
	}

	static void disable_pe_interrupt()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_PEIE>();
	}

	static void enable_txe_interrupt()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_TXEIE>();
	}

	static void disable_txe_interrupt()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_TXEIE>();
	}

	static void enable_tc_interrupt()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_TCIE>();
	}

	static void disable_tc_interrupt()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_TCIE>();
	}

	static void enable_rxne_interrupt()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_RXNEIE>();
	}

	static void disable_rxne_interrupt()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_RXNEIE>();
	}

	static void enable_idle_interrupt()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_IDLEIE>();
	}

	static void disable_idle_interrupt()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_IDLEIE>();
	}

	static void enable_transmitter()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_TE>();
	}

	static void disable_transmitter()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_TE>();
	}

	static void enable_receiver()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_RE>();
	}

	static void disable_receiver()
	{
		clear_periph_reg_bit<cr1_addr, USART_CR1_RE>();
	}

	static void set_receiver_mode(UsartReceiverMode mode)
	{
		set_value_by_mask<uint32_t>(cr1_addr, USART_CR1_RWU, (uint32_t)mode);
	}

	static void send_break()
	{
		set_periph_reg_bit<cr1_addr, USART_CR1_SBK>();
	}

	static void enable_lin_mode()
	{
		set_periph_reg_bit<cr2_addr, USART_CR2_LINEN>();
	}

	static void disable_lin_mode()
	{
		clear_periph_reg_bit<cr2_addr, USART_CR2_LINEN>();
	}

	static void set_stop_bits(StopBits value)
	{
		set_value_by_mask<uint32_t>(cr2_addr, USART_CR2_STOP, (uint32_t)value);
	}

	static void enable_sclk_pin()
	{
		set_periph_reg_bit<cr2_addr, USART_CR2_CLKEN>();
	}

	static void disable_sclk_pin()
	{
		clear_periph_reg_bit<cr2_addr, USART_CR2_CLKEN>();
	}

	static void set_clock_polarity(UsartClockPolarity value)
	{
		set_value_by_mask<uint32_t>(cr2_addr, USART_CR2_CPOL, (uint32_t)value);
	}

	static void set_clock_phase(UsartClockPhase value)
	{
		set_value_by_mask<uint32_t>(cr2_addr, USART_CR2_CPHA, (uint32_t)value);
	}

	static void enable_last_bit_clock_pulse()
	{
		set_periph_reg_bit<cr2_addr, USART_CR2_LBCL>();
	}

	static void disable_last_bit_clock_pulse()
	{
		clear_periph_reg_bit<cr2_addr, USART_CR2_LBCL>();
	}

	static void enable_lin_break_detection_interrupt()
	{
		set_periph_reg_bit<cr2_addr, USART_CR2_LBDIE>();
	}

	static void disable_lin_break_detection_interrupt()
	{
		clear_periph_reg_bit<cr2_addr, USART_CR2_LBDIE>();
	}

	static void set_lin_break_detection_len(LinBreakLen value)
	{
		set_value_by_mask<uint32_t>(cr2_addr, USART_CR2_CPHA, (uint32_t)value);
	}

	static void set_node_address(uint8_t address)
	{
		set_value_by_mask<uint32_t>(cr2_addr, USART_CR2_ADD, (uint32_t)address);
	}

#ifdef USART_CR3_ONEBIT
	static void set_sample_bit_method(SampleBitMethod method)
	{
		set_value_by_mask<uint32_t>(cr3_addr, USART_CR3_ONEBIT, (uint32_t)method);
	}
#endif

	static void enable_cts_interrupt()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_CTSIE>();
	}

	static void disable_cts_interrupt()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_CTSIE>();
	}

	static void enable_cts()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_CTSE>();
	}

	static void disable_cts()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_CTSE>();
	}

	static void enable_rts()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_RTSE>();
	}

	static void disable_rts()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_RTSE>();
	}

	static void enable_transmitter_dma()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_DMAT>();
	}

	static void disable_transmitter_dma()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_DMAT>();
	}

	static void enable_receiver_dma()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_DMAR>();
	}

	static void disable_receiver_dma()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_DMAR>();
	}

	static void enable_smartcard_mode()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_SCEN>();
	}

	static void disable_smartcard_mode()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_SCEN>();
	}

	static void enable_smartcard_nack()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_NACK>();
	}

	static void disable_smartcard_nack()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_NACK>();
	}

	static void enable_half_duplex_mode()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_HDSEL>();
	}

	static void disable_half_duplex_mode()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_HDSEL>();
	}

	static void set_irda_power_mode(IrDAPowerMode mode)
	{
		set_value_by_mask<uint32_t>(cr3_addr, USART_CR3_IRLP, (uint32_t)mode);
	}

	static void enable_irda()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_IREN>();
	}

	static void disable_irda()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_IREN>();
	}

	static void enable_error_interrupt()
	{
		set_periph_reg_bit<cr3_addr, USART_CR3_EIE>();
	}

	static void disable_error_interrupt()
	{
		clear_periph_reg_bit<cr3_addr, USART_CR3_EIE>();
	}

private:
	static constexpr uintptr_t usart_addr = detailed::UsartDevice<N>::usart_addr;

	static constexpr uintptr_t sr_addr  = usart_addr + offsetof(USART_TypeDef, SR);
	static constexpr uintptr_t dr_addr  = usart_addr + offsetof(USART_TypeDef, DR);
	static constexpr uintptr_t brr_addr = usart_addr + offsetof(USART_TypeDef, BRR);
	static constexpr uintptr_t cr1_addr = usart_addr + offsetof(USART_TypeDef, CR1);
	static constexpr uintptr_t cr2_addr = usart_addr + offsetof(USART_TypeDef, CR2);
	static constexpr uintptr_t cr3_addr = usart_addr + offsetof(USART_TypeDef, CR3);
};


#ifdef USART1

namespace detailed { template<> struct UsartDevice<1>
{
	static constexpr uint32_t RccBit = RCC_APB2ENR_USART1EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
	static constexpr uintptr_t usart_addr = USART1_BASE;
	static constexpr IRQn_Type IRQn = USART1_IRQn;
};}

typedef Usart<1> Usart1;

#endif

#ifdef USART2

namespace detailed { template<> struct UsartDevice<2>
{
	static constexpr uint32_t RccBit = RCC_APB1ENR_USART2EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
	static constexpr uintptr_t usart_addr = USART2_BASE;
	static constexpr IRQn_Type IRQn = USART2_IRQn;
};}

typedef Usart<2> Usart2;

#endif

#ifdef USART3

typedef Usart<3> Usart3;

namespace detailed { template<> struct UsartDevice<3>
{
	static constexpr uint32_t RccBit = RCC_APB1ENR_USART3EN;
	static constexpr uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1ENR);
	static constexpr uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR);
	static constexpr uintptr_t usart_addr = USART3_BASE;
	static constexpr IRQn_Type IRQn = USART3_IRQn;
};}

#endif

} // namespace hl
