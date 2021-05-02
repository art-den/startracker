#pragma once

#include <stdint.h>

#include "stm32_hl/hl_spi.hpp"

namespace mgfxpp {

template <
	typename ResetPin,
	typename CsPin,
	typename DcPin,
	typename MosiPin,
	typename SckPin,
	typename DelayT
>
struct SoftSpi5WireConnection
{
	static void init_pins()
	{
		ResetPin::conf_out_push_pull();
		DcPin::conf_out_push_pull();
		CsPin::conf_out_push_pull();
		MosiPin::conf_out_push_pull();
		SckPin::conf_out_push_pull();
	}

	static void select_device()
	{
		CsPin::off();
	}

	static void release_device()
	{
		CsPin::on();
	}

	static void reset(bool active)
	{
		ResetPin::set_out(active);
	}

	static void send_command(uint8_t cmd)
	{
		DcPin::off();
		transfer<uint8_t, 0x80>(cmd);
	}

	static void send_data(uint8_t data)
	{
		DcPin::on();
		transfer<uint8_t, 0x80>(data);
	}

	static void start_send_data16(const uint16_t *data_ptr, unsigned count, bool increment_data_ptr)
	{
		DcPin::on();
		while (count--)
		{
			transfer<uint16_t, 0x8000>(*data_ptr);
			if (increment_data_ptr) data_ptr++;
		}
	}

	static void finish_send_data() {}

private:
	template<typename T, unsigned Mask>
	static void transfer(T data)
	{
		for (unsigned i = 0; i < 8 * sizeof(T); i++)
		{
			SckPin::off();
			MosiPin::set_out(data & Mask);
			data <<= 1;
			DelayT::wait_bit_transfer();
			SckPin::on();
			DelayT::wait_bit_transfer();
		}
	}
};

template <
	typename ResetPin,
	typename CsPin,
	typename DcPin,
	typename MosiPin,
	typename MisoPin,
	typename SckPin,
	typename DelayT
>
struct SoftSpi6WireConnection
{
	static void init_pins()
	{
		ResetPin::conf_out_push_pull();
		CsPin::conf_out_push_pull();
		DcPin::conf_out_push_pull();

		MosiPin::conf_out_push_pull();
		MisoPin::conf_in();
		SckPin::conf_out_push_pull();
	}

	static void select_device()
	{
		CsPin::off();
	}

	static void release_device()
	{
		CsPin::on();
	}

	static void reset(bool active)
	{
		ResetPin::set_out(active);
	}

	static void send_command(uint8_t cmd)
	{
		DcPin::off();
		transfer<uint8_t, 0x80>(cmd);
	}

	static uint8_t send_data(uint8_t data)
	{
		DcPin::on();
		return transfer<uint8_t, 0x80>(data);
	}

	static void start_send_data16(const uint16_t *data_ptr, unsigned count, bool increment_data_ptr)
	{
		DcPin::on();
		while (count--)
		{
			transfer<uint16_t, 0x8000>(*data_ptr);
			if (increment_data_ptr) data_ptr++;
		}
	}

	static void finish_send_data() {}

private:

	template<typename T, unsigned Mask>
	static T transfer(T data)
	{
		T result = 0;
		for (unsigned i = 0; i < 8 * sizeof(T); i++)
		{
			SckPin::off();
			MosiPin::set_out(data & Mask);
			DelayT::wait_bit_transfer();
			SckPin::on();
			result <<= 1;
			if (MisoPin::get_in()) result |= 1;
			data <<= 1;
			DelayT::wait_bit_transfer();
		}
		return result;
	}
};

template <
	typename ResetPin,
	typename CsPin,
	typename DcPin,
	typename Spi
>
struct HardSpi6WireConnection
{
	static void init_pins()
	{
		ResetPin::conf_out_push_pull();
		CsPin::conf_out_push_pull();
		DcPin::conf_out_push_pull();
	}

	static void select_device()
	{
		CsPin::off();
	}

	static void release_device()
	{
		CsPin::on();
	}

	static void reset(bool active)
	{
		ResetPin::set_out(active);
	}

	static void send_command(uint8_t cmd)
	{
		DcPin::off();
		transfer_and_read8(cmd);
	}

	static uint8_t send_data(uint8_t data)
	{
		DcPin::on();
		return transfer_and_read8(data);
	}

	static void start_send_data16(const uint16_t *data_ptr, unsigned count, bool increment_data_ptr)
	{
		if (count == 0) return;

		DcPin::on();

		if (!mode16_)
		{
			Spi::disable();
			Spi::set_frame_len(hl::SpiFrameLen::_16_Bit);
			Spi::enable();
			mode16_ = true;
		}

		while (count--)
		{
			while (!Spi::get_txe_flag()) {}
			Spi::set_data(*data_ptr);
			if (increment_data_ptr) data_ptr++;
		}

		while (Spi::get_busy_flag() || !Spi::get_rxne_flag()) {}
		Spi::get_data();
	}

	static void finish_send_data() {}

private:
	static uint8_t transfer_and_read8(uint8_t data)
	{
		if (mode16_)
		{
			Spi::disable();
			Spi::set_frame_len(hl::SpiFrameLen::_8_Bit);
			Spi::enable();
			mode16_ = false;
		}

		while (!Spi::get_txe_flag()) {}
		Spi::set_data(data);

		while (Spi::get_busy_flag() || !Spi::get_rxne_flag()) {}

		return Spi::get_data();
	}

	inline static bool mode16_ = false;
};

template <
	typename ResetPin,
	typename CsPin,
	typename DcPin,
	typename Spi,
	typename Dma
>
struct HardSpiDma6WireConnection
{
	static void init_pins()
	{
		ResetPin::conf_out_push_pull();
		CsPin::conf_out_push_pull();
		DcPin::conf_out_push_pull();
	}

	static void select_device()
	{
		CsPin::off();
	}

	static void release_device()
	{
		CsPin::on();
	}

	static void reset(bool active)
	{
		ResetPin::set_out(active);
	}

	static void send_command(uint8_t cmd)
	{
		DcPin::off();
		transfer_and_read8(cmd);
	}

	static uint8_t send_data(uint8_t data)
	{
		DcPin::on();
		return transfer_and_read8(data);
	}

	static void start_send_data16(const uint16_t *data_ptr, unsigned count, bool increment_data_ptr)
	{
		if (count == 0) return;

		DcPin::on();

		if (!mode16_)
		{
			Spi::disable();
			Spi::set_frame_len(hl::SpiFrameLen::_16_Bit);
			Spi::enable();
			mode16_ = true;
		}

		if (count < 32)
		{
			while (count--)
			{
				while (!Spi::get_txe_flag()) {}
				Spi::set_data(*data_ptr);
				if (increment_data_ptr) data_ptr++;
			}

			while (Spi::get_busy_flag() || !Spi::get_rxne_flag()) {}
			Spi::get_data();
		}
		else
		{
			Dma::disable();
			Spi::disable();

			Spi::enable_tx_dma();
			Spi::set_frame_len(hl::SpiFrameLen::_16_Bit);

			Dma::set_memory_address((uintptr_t)data_ptr);

			if (increment_data_ptr)
				Dma::enable_memory_increment();
			else
				Dma::disable_memory_increment();

			Dma::set_number_of_data(count);
			Dma::clear_transfer_complete_flag();

			Spi::enable();
			Dma::enable();

			dma_is_enabled_ = true;
		}
	}

	static void finish_send_data()
	{
		if (!dma_is_enabled_) return;

		while (!Dma::get_transfer_complete_flag()) {}
		while (Spi::get_busy_flag() || !Spi::get_rxne_flag()) {}
		Spi::get_data();

		Dma::disable();

		Spi::disable();
		Spi::disable_tx_dma();
		Spi::enable();

		dma_is_enabled_ = false;
	}

private:
	inline static bool mode16_ = false;
	inline static bool dma_is_enabled_ = false;

	static uint8_t transfer_and_read8(uint8_t data)
	{
		if (mode16_)
		{
			Spi::disable();
			Spi::set_frame_len(hl::SpiFrameLen::_8_Bit);
			Spi::enable();
			mode16_ = false;
		}

		while (!Spi::get_txe_flag()) {}
		Spi::set_data(data);

		while (Spi::get_busy_flag() || !Spi::get_rxne_flag()) {}

		return Spi::get_data();
	}
};


} // namespace mgfxpp

