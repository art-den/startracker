#pragma once

#include "hl_device.hpp"
#include "hl_exti.hpp"

#if defined (HL_STM32L1XX)
	#define HL_GPIO_VER2
#elif defined (HL_STM32F1XX)
	#define HL_GPIO_VER1
#elif defined (HL_STM32F3XX)
	#define HL_GPIO_VER2
#elif defined (HL_STM32F4XX)
	#define HL_GPIO_VER2
#elif defined (HL_STM32F0XX)
	#define HL_GPIO_VER2
#else
	#error "You must define HL_STM32L1XX or HL_STM32F3XX"
#endif

namespace hl {

enum class AltFun
{
	System        = 0,
#if defined(HL_STM32L1XX)
	Timer2        = 1,
	Timers3_4_5   = 2,
	Timers9_10_11 = 3,
	I2C           = 4,
	SPI_1_2       = 5,
	SPI_3         = 6,
	USART         = 7,
	USB_          = 10,
	LCD_          = 11,
	RI_           = 14,
	EventOut      = 15
#elif defined(HL_STM32F3XX)
#endif
	_0 = 0,
	_1 = 1,
	_2 = 2,
	_3 = 3,
	_4 = 4,
	_5 = 5,
	_6 = 6,
	_7 = 7,
	_8 = 8,
	_9 = 9,
	_10 = 10,
	_11 = 11,
	_12 = 12,
	_13 = 13,
	_14 = 14,
	_15 = 15,
};


template<class PortHelper>
class Port : public Device<PortHelper>
{
public:
	typedef PortHelper Helper;

	static void HL_ALWAYS_INLINE set(uint16_t value)
	{
		HL_UI32REG(ODR_ADDR) = value;
	}

	static void HL_ALWAYS_INLINE set(uint16_t value, uint16_t mask)
	{
		HL_UI32REG(BSRR_ADDR) = value | ((uint32_t)mask << 16);
	}

	static uint16_t HL_ALWAYS_INLINE get_in()
	{
		return HL_UI32REG(IDR_ADDR);
	}

	static uint16_t HL_ALWAYS_INLINE get_out()
	{
		return HL_UI32REG(ODR_ADDR);
	}

public:
	constexpr static uintptr_t BSRR_ADDR = Helper::gpio + offsetof(GPIO_TypeDef, BSRR);
	constexpr static uintptr_t ODR_ADDR = Helper::gpio + offsetof(GPIO_TypeDef, ODR);
	constexpr static uintptr_t IDR_ADDR = Helper::gpio + offsetof(GPIO_TypeDef, IDR);
};

enum class PinSpeed
{
#if defined (HL_GPIO_VER1)
	Low     = 0b10,
	Medium  = 0b01,
	High    = 0b11,
#elif defined (HL_GPIO_VER2)
	VeryLow = 0b00,
	Low     = 0b01,
	Medium  = 0b10,
	High    = 0b11,
#endif
};

template <class PortType, unsigned PinIndex>
class Pin
{
public:
	static constexpr unsigned Index = PinIndex;
	typedef PortType Port;

	// digital input

	static HL_ALWAYS_INLINE void conf_in()
	{
#if defined (HL_GPIO_VER1)
		set_value_by_mask<uint32_t>(CR, InvMask4, 0b01/*float in*/ << (Shift4+2));
#elif defined (HL_GPIO_VER2)
		set_value_by_mask(MODER_ADDR, InvMask2, ModerInput << PinIndex*2);
		set_value_by_mask(PUPDR_ADDR, InvMask2, PupdrNone);
#endif
	}

	static void HL_ALWAYS_INLINE conf_in_pull_up()
	{
#if defined (HL_GPIO_VER1)
		set_value_by_mask<uint32_t>(CR, InvMask4, 0b10/*pulled in*/ << (Shift4+2));
		on();
#elif defined (HL_GPIO_VER2)
		set_value_by_mask(MODER_ADDR, InvMask2, ModerInput << PinIndex*2);
		set_value_by_mask(PUPDR_ADDR, InvMask2, PupdrPullUp << PinIndex*2);
#endif
	}

	static void HL_ALWAYS_INLINE conf_in_pull_down()
	{
#if defined (HL_GPIO_VER1)
		set_value_by_mask<uint32_t>(CR, InvMask4, 0b10/*pulled in*/ << (Shift4+2));
		off();
#elif defined (HL_GPIO_VER2)

		set_value_by_mask(MODER_ADDR, InvMask2, ModerInput << PinIndex*2);
		set_value_by_mask(PUPDR_ADDR, InvMask2, PupdrPullDown << PinIndex*2);
#endif
	}

	static bool HL_ALWAYS_INLINE get_in()
	{
		return get_periph_reg_bit<IDR, BitMask>();
	}

	static void HL_ALWAYS_INLINE configure_exti(bool rising, bool falling, bool enable_event, bool enable_interrupt)
	{
#if defined (HL_GPIO_VER1)
#elif defined (HL_GPIO_VER2)
		constexpr uint32_t Shift = (PinIndex & 0x3) * 4;
		if (rising || falling)
		{
			RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
			__DSB(); __DSB(); __NOP(); __NOP();
		}
		set_value_by_mask(SYSCFG->EXTICR[PinIndex >> 2], 0xF << Shift, PortType::Helper::EXTIx << Shift);
		exti_configure_channel(PinIndex, enable_event, enable_interrupt, rising, falling);
#endif
	}

	static bool HL_ALWAYS_INLINE get_interrupt_bit()
	{
		return exti_get_pending_bit(PinIndex);
	}

	static void HL_ALWAYS_INLINE clear_interrupt_bit()
	{
		exti_clear_pending_bit(PinIndex);
	}


	// digital output

	static void HL_ALWAYS_INLINE conf_out_push_pull(PinSpeed speed = PinSpeed::High)
	{
#if defined (HL_GPIO_VER1)
		set_value_by_mask<uint32_t>(CR, InvMask4, ((uint32_t)speed << Shift4) | (0b00/*pp*/ << (Shift4+2)));
#elif defined (HL_GPIO_VER2)
		set_value_by_mask(MODER_ADDR, InvMask2, ModerOutput << PinIndex*2);
		set_value_by_mask(OTYPER_ADDR, InvMask1, OyperPP << PinIndex);
		set_value_by_mask(OSPEEDR_ADDR, InvMask2, ((uint32_t)speed) << PinIndex*2);
#endif
	}

	static void HL_ALWAYS_INLINE conf_out_open_drain(PinSpeed speed = PinSpeed::High)
	{
#if defined (HL_GPIO_VER1)
		set_value_by_mask<uint32_t>(CR, InvMask4, ((uint32_t)speed << Shift4) | (0b01/*od*/ << (Shift4+2)));
#elif defined (HL_GPIO_VER2)
		set_value_by_mask(MODER_ADDR, InvMask2, ModerOutput << PinIndex*2);
		set_value_by_mask(OTYPER_ADDR, InvMask1, OyperOD << PinIndex);
		set_value_by_mask(OSPEEDR_ADDR, InvMask2, ((uint32_t)speed) << PinIndex*2);
#endif
	}

	static void HL_ALWAYS_INLINE on()
	{
		HL_UI32REG(Port::BSRR_ADDR) = BitMask;
	}

	static void HL_ALWAYS_INLINE toggle()
	{
		set_out(!get_out());
	}

	static void HL_ALWAYS_INLINE off()
	{
		HL_UI32REG(Port::BSRR_ADDR) = (BitMask << 16);
	}

	static void HL_ALWAYS_INLINE set_out(bool const value)
	{
		if (value) on();
		else off();
	}

	static bool HL_ALWAYS_INLINE get_out()
	{
		return get_periph_reg_bit<ODR, BitMask>();
	}

	// alternate function

	static void HL_ALWAYS_INLINE conf_alt_push_pull(PinSpeed speed = PinSpeed::High)
	{
#if defined (HL_GPIO_VER1)
		set_value_by_mask<uint32_t>(CR, InvMask4, ((uint32_t)speed << Shift4) | (0b10/*alt pp*/ << (Shift4+2)));
#elif defined (HL_GPIO_VER2)
		set_value_by_mask(MODER_ADDR, InvMask2, ModerAF << PinIndex*2);
		set_value_by_mask(OTYPER_ADDR, InvMask1, OyperPP << PinIndex);
		set_value_by_mask(OSPEEDR_ADDR, InvMask2, ((uint32_t)speed) << PinIndex*2);
#endif
	}

	static void HL_ALWAYS_INLINE conf_alt_open_drain(PinSpeed speed = PinSpeed::High)
	{
#if defined (HL_GPIO_VER1)
		set_value_by_mask<uint32_t>(CR, InvMask4, ((uint32_t)speed << Shift4) | (0b11/*alt od*/ << (Shift4+2)));
#elif defined (HL_GPIO_VER2)
		set_value_by_mask(MODER_ADDR, InvMask2, ModerAF << PinIndex*2);
		set_value_by_mask(OTYPER_ADDR, InvMask1, OyperOD << PinIndex);
		set_value_by_mask(OSPEEDR_ADDR, InvMask2, ((uint32_t)speed) << PinIndex*2);
#endif
	}

#if defined (HL_GPIO_VER2)
	static void HL_ALWAYS_INLINE set_alt_function(AltFun func)
	{
		constexpr uint32_t AfrIndex = PinIndex >> 3;
		constexpr uint32_t Shift = (PinIndex & 0x7) * 4;
		constexpr uint32_t Mask = 0xF << Shift;
		set_value_by_mask(AFR_ADDR + AfrIndex*4, Mask, (uint32_t)func << Shift);
	}
#endif

	// analog

	static void HL_ALWAYS_INLINE conf_analog()
	{
#if defined (HL_GPIO_VER1)
		set_value_by_mask<uint32_t>(CR, InvMask4, (0b00/*analog in*/ << (Shift4+2)));
#elif defined (HL_GPIO_VER2)
		set_value_by_mask(MODER_ADDR, InvMask2, ModerAnalog << PinIndex*2);
		set_value_by_mask(PUPDR_ADDR, InvMask2, PupdrNone);
#endif
	}

private:
	constexpr static uint32_t BitMask = (1U << PinIndex);
	constexpr static uintptr_t GPIO_ADDR = PortType::Helper::gpio;

#if defined (HL_GPIO_VER1)

	constexpr static uintptr_t CR = GPIO_ADDR + ((PinIndex < 8) ? offsetof(GPIO_TypeDef, CRL) : offsetof(GPIO_TypeDef, CRH));

	constexpr static uint32_t Shift4 = (PinIndex % 8)*4;
	constexpr static uint32_t InvMask4 = (uint32_t)0b1111 << Shift4;

#elif defined (HL_GPIO_VER2)
	constexpr static uint32_t InvMask2 = (3U << PinIndex*2);
	constexpr static uint32_t InvMask1 = BitMask;
	constexpr static uint32_t ModerInput  = 0;
	constexpr static uint32_t ModerOutput = 1;
	constexpr static uint32_t ModerAF     = 2;
	constexpr static uint32_t ModerAnalog = 3;
	constexpr static uint32_t PupdrNone = 0;
	constexpr static uint32_t PupdrPullUp = 1;
	constexpr static uint32_t PupdrPullDown = 2;
	constexpr static uint32_t OyperPP = 0;
	constexpr static uint32_t OyperOD = 1;

	constexpr static uintptr_t OSPEEDR_ADDR = GPIO_ADDR + offsetof(GPIO_TypeDef, OSPEEDR);
	constexpr static uintptr_t MODER_ADDR = GPIO_ADDR + offsetof(GPIO_TypeDef, MODER);
	constexpr static uintptr_t PUPDR_ADDR = GPIO_ADDR + offsetof(GPIO_TypeDef, PUPDR);
	constexpr static uintptr_t AFR_ADDR = GPIO_ADDR + offsetof(GPIO_TypeDef, AFR);
	constexpr static uintptr_t OTYPER_ADDR = GPIO_ADDR + offsetof(GPIO_TypeDef, OTYPER);
#endif
	constexpr static uintptr_t IDR = GPIO_ADDR + offsetof(GPIO_TypeDef, IDR);
	constexpr static uintptr_t ODR = GPIO_ADDR + offsetof(GPIO_TypeDef, ODR);
};

#define HL_DECL_PINS(PORT) \
typedef Pin<PORT,  0> PORT##0; \
typedef Pin<PORT,  1> PORT##1; \
typedef Pin<PORT,  2> PORT##2; \
typedef Pin<PORT,  3> PORT##3; \
typedef Pin<PORT,  4> PORT##4; \
typedef Pin<PORT,  5> PORT##5; \
typedef Pin<PORT,  6> PORT##6; \
typedef Pin<PORT,  7> PORT##7; \
typedef Pin<PORT,  8> PORT##8; \
typedef Pin<PORT,  9> PORT##9; \
typedef Pin<PORT, 10> PORT##10; \
typedef Pin<PORT, 11> PORT##11; \
typedef Pin<PORT, 12> PORT##12; \
typedef Pin<PORT, 13> PORT##13; \
typedef Pin<PORT, 14> PORT##14; \
typedef Pin<PORT, 15> PORT##15;

#ifdef GPIOA_BASE
namespace detailed { struct PortHelperA
{
#if defined (RCC_APB2ENR_IOPAEN)
	constexpr static uint32_t RccBit = RCC_APB2ENR_IOPAEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
#elif defined(RCC_AHBENR_GPIOAEN)
	constexpr static uint32_t RccBit = RCC_AHBENR_GPIOAEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBENR);
#elif defined(RCC_AHB1ENR_GPIOAEN)
	constexpr static uint32_t RccBit = RCC_AHB1ENR_GPIOAEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR);
#endif
	constexpr static uintptr_t gpio = GPIOA_BASE;
	constexpr static uint32_t EXTIx = 0;
}; }
typedef Port<detailed::PortHelperA> PA;
HL_DECL_PINS(PA)
#endif

#ifdef GPIOB_BASE
namespace detailed { struct PortHelperB
{
#if defined (RCC_APB2ENR_IOPBEN)
	constexpr static uint32_t RccBit = RCC_APB2ENR_IOPBEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
#elif defined(RCC_AHBENR_GPIOBEN)
	constexpr static uint32_t RccBit = RCC_AHBENR_GPIOBEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR);
#elif defined(RCC_AHB1ENR_GPIOBEN)
	constexpr static uint32_t RccBit = RCC_AHB1ENR_GPIOBEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR);
#endif
	constexpr static uintptr_t gpio = GPIOB_BASE;
	constexpr static uint32_t EXTIx = 1;
}; }
typedef Port<detailed::PortHelperB> PB;
HL_DECL_PINS(PB)
#endif

#if defined(GPIOC_BASE)
namespace detailed { struct PortHelperC
{
#if defined (RCC_APB2ENR_IOPCEN)
	constexpr static uint32_t RccBit = RCC_APB2ENR_IOPCEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
#elif defined (RCC_AHBENR_GPIOCEN)
	constexpr static uint32_t RccBit = RCC_AHBENR_GPIOCEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR);
#elif defined(RCC_AHB1ENR_GPIOCEN)
	constexpr static uint32_t RccBit = RCC_AHB1ENR_GPIOCEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR);
#endif
	constexpr static uintptr_t gpio = GPIOC_BASE;
	constexpr static uint32_t EXTIx = 2;
}; }
typedef Port<detailed::PortHelperC> PC;
HL_DECL_PINS(PC)
#endif

#if defined(GPIOD_BASE)
namespace detailed { struct PortHelperD
{
#if defined (RCC_APB2ENR_IOPDEN)
	constexpr static uint32_t RccBit = RCC_APB2ENR_IOPDEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
#elif defined (RCC_AHBENR_GPIODEN)
	constexpr static uint32_t RccBit = RCC_AHBENR_GPIODEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR);
#elif defined(RCC_AHB1ENR_GPIODEN)
	constexpr static uint32_t RccBit = RCC_AHB1ENR_GPIODEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR);
#endif
	constexpr static uintptr_t gpio = GPIOD_BASE;
	constexpr static uint32_t EXTIx = 3;
}; }
typedef Port<detailed::PortHelperD> PD;
HL_DECL_PINS(PD)
#endif

#if defined(GPIOE_BASE)
namespace detailed { struct PortHelperE
{
#if defined (RCC_APB2ENR_IOPEEN)
	constexpr static uint32_t RccBit = RCC_APB2ENR_IOPEEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
#elif defined (RCC_AHBENR_GPIOEEN)
	constexpr static uint32_t RccBit = RCC_AHBENR_GPIOEEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR);
#elif defined(RCC_AHB1ENR_GPIOEEN)
	constexpr static uint32_t RccBit = RCC_AHB1ENR_GPIOEEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR);
#endif
	constexpr static uintptr_t gpio = GPIOE_BASE;
	constexpr static uint32_t EXTIx = 4;
}; }
typedef Port<detailed::PortHelperE> PE;
HL_DECL_PINS(PE)
#endif

#if defined(GPIOF_BASE)
namespace detailed { struct PortHelperF
{
#if defined (RCC_APB2ENR_IOPFEN)
	constexpr static uint32_t RccBit = RCC_APB2ENR_IOPFEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
#elif defined (RCC_AHBENR_GPIOFEN)
	constexpr static uint32_t RccBit = RCC_AHBENR_GPIOFEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR);
#elif defined(RCC_AHB1ENR_GPIOFEN)
	constexpr static uint32_t RccBit = RCC_AHB1ENR_GPIOFEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR);
#endif
	constexpr static uintptr_t gpio = GPIOF_BASE;
	constexpr static uint32_t EXTIx = 5;
}; }
typedef Port<detailed::PortHelperF> PF;
HL_DECL_PINS(PF)
#endif

#if defined(GPIOG_BASE)
namespace detailed { struct PortHelperG
{
#if defined (RCC_APB2ENR_IOPGEN)
	constexpr static uint32_t RccBit = RCC_APB2ENR_IOPGEN;
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR);
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, APB2ENR);
#elif defined (RCC_AHBENR_GPIOGEN)
	constexpr static uint32_t RccBit = RCC_AHBENR_GPIOGEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHBRSTR);
#elif defined(RCC_AHB1ENR_GPIOGEN)
	constexpr static uint32_t RccBit = RCC_AHB1ENR_GPIOGEN;
	constexpr static uintptr_t ClockRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR);
	constexpr static uintptr_t ResetRegister = RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR);
#endif
	constexpr static uintptr_t gpio = GPIOG_BASE;
	constexpr static uint32_t EXTIx = 6;
}; }
typedef Port<detailed::PortHelperG> PG;
HL_DECL_PINS(PG)
#endif
#undef HL_DECL_PINS

// remap function for STM32F1 devices
#if defined (HL_GPIO_VER1)

inline void gpio_disable_jtag()
{
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

inline void gpio_enable_jtag()
{
	AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

#endif

} // namespace hl

