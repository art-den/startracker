#include "font_simple8_ru.hpp"

namespace mgfxpp {

static const uint8_t SYM_SPACE[] = { 2, 0, 0 };
static const uint8_t SYM_QUEST[] = { 4, 0x04, 0x02, 0x5A, 0x04 };

static const uint8_t SYM_0[] = { 4, 0x3C, 0x42, 0x42, 0x3C };
static const uint8_t SYM_1[] = { 3, 0x42, 0x7E, 0x40 };
static const uint8_t SYM_2[] = { 4, 0x64, 0x52, 0x52, 0x4C };
static const uint8_t SYM_3[] = { 4, 0x24, 0x42, 0x4A, 0x34 };
static const uint8_t SYM_4[] = { 4, 0x1E, 0x10, 0x10, 0x7E };
static const uint8_t SYM_5[] = { 4, 0x2E, 0x4A, 0x4A, 0x32 };
static const uint8_t SYM_6[] = { 4, 0x3C, 0x4A, 0x4A, 0x30 };
static const uint8_t SYM_7[] = { 4, 0x06, 0x72, 0x0A, 0x06 };
static const uint8_t SYM_8[] = { 4, 0x34, 0x4A, 0x4A, 0x34 };
static const uint8_t SYM_9[] = { 4, 0x0C, 0x52, 0x52, 0x3C };

static const uint8_t SYM_A[] = { 4, 0x7C, 0x12, 0x12, 0x7C };
static const uint8_t SYM_a[] = { 4, 0x20, 0x54, 0x54, 0x78 };
static const uint8_t SYM_B[] = { 4, 0x7E, 0x4A, 0x4A, 0x34 };
static const uint8_t SYM_b[] = { 4, 0x7E, 0x48, 0x48, 0x30 };
static const uint8_t SYM_C[] = { 4, 0x3C, 0x42, 0x42, 0x24 };
static const uint8_t SYM_c[] = { 4, 0x38, 0x44, 0x44, 0x44 };
static const uint8_t SYM_D[] = { 4, 0x7E, 0x42, 0x42, 0x3C };
static const uint8_t SYM_d[] = { 4, 0x30, 0x48, 0x48, 0x7E };
static const uint8_t SYM_E[] = { 4, 0x7E, 0x4A, 0x4A, 0x42 };
static const uint8_t SYM_e[] = { 4, 0x38, 0x54, 0x54, 0x58 };
static const uint8_t SYM_F[] = { 4, 0x7E, 0x12, 0x12, 0x02 };
static const uint8_t SYM_f[] = { 4, 0x08, 0x7C, 0x0A, 0x02 };
static const uint8_t SYM_G[] = { 4, 0x3C, 0x42, 0x52, 0x34 };
static const uint8_t SYM_g[] = { 4, 0x18, 0xA4, 0xA4, 0x7C };
static const uint8_t SYM_H[] = { 4, 0x7E, 0x08, 0x08, 0x7E };
static const uint8_t SYM_h[] = { 4, 0x7E, 0x08, 0x08, 0x70 };
static const uint8_t SYM_I[] = { 3, 0x42, 0x7E, 0x42 };
static const uint8_t SYM_i[] = { 3, 0x48, 0x7A, 0x40 };
static const uint8_t SYM_J[] = { 4, 0x22, 0x42, 0x42, 0x3E };
static const uint8_t SYM_j[] = { 3, 0x80, 0x88, 0x7A };
static const uint8_t SYM_K[] = { 4, 0x7E, 0x08, 0x14, 0x62 };
static const uint8_t SYM_k[] = { 4, 0x7E, 0x10, 0x28, 0x44 };
static const uint8_t SYM_L[] = { 4, 0x7E, 0x40, 0x40, 0x40 };
static const uint8_t SYM_l[] = { 3, 0x42, 0x7E, 0x40 };
static const uint8_t SYM_M[] = { 5, 0x7E, 0x18, 0x60, 0x18, 0x7E };
static const uint8_t SYM_m[] = { 5, 0x7C, 0x04, 0x78, 0x04, 0x78 };
static const uint8_t SYM_N[] = { 4, 0x7E, 0x08, 0x10, 0x7E };
static const uint8_t SYM_n[] = { 4, 0x7C, 0x04, 0x04, 0x78 };
static const uint8_t SYM_O[] = { 4, 0x3C, 0x42, 0x42, 0x3C };
static const uint8_t SYM_o[] = { 4, 0x38, 0x44, 0x44, 0x38 };
static const uint8_t SYM_P[] = { 4, 0x7E, 0x12, 0x12, 0x0C };
static const uint8_t SYM_p[] = { 4, 0xFC, 0x24, 0x24, 0x18 };
static const uint8_t SYM_Q[] = { 4, 0x3C, 0x42, 0x62, 0xFC };
static const uint8_t SYM_q[] = { 4, 0x38, 0x44, 0x44, 0xFC };
static const uint8_t SYM_R[] = { 4, 0x7E, 0x12, 0x32, 0x4C };
static const uint8_t SYM_r[] = { 3, 0x78, 0x04, 0x04 };
static const uint8_t SYM_S[] = { 4, 0x44, 0x4A, 0x4A, 0x32 };
static const uint8_t SYM_s[] = { 4, 0x48, 0x54, 0x54, 0x24 };
static const uint8_t SYM_T[] = { 5, 0x02, 0x02, 0x7E, 0x02, 0x02 };
static const uint8_t SYM_t[] = { 3, 0x04, 0x3E, 0x44 };
static const uint8_t SYM_U[] = { 4, 0x3E, 0x40, 0x40, 0x3E };
static const uint8_t SYM_u[] = { 4, 0x3C, 0x40, 0x40, 0x3C };
static const uint8_t SYM_V[] = { 5, 0x1E, 0x20, 0x40, 0x20, 0x1E };
static const uint8_t SYM_v[] = { 5, 0x1C, 0x20, 0x40, 0x20, 0x1C };
static const uint8_t SYM_W[] = { 5, 0x7E, 0x20, 0x10, 0x20, 0x7E };
static const uint8_t SYM_w[] = { 5, 0x3C, 0x40, 0x38, 0x40, 0x3C };
static const uint8_t SYM_X[] = { 5, 0x42, 0x24, 0x18, 0x24, 0x42 };
static const uint8_t SYM_x[] = { 5, 0x44, 0x28, 0x10, 0x28, 0x44 };
static const uint8_t SYM_Y[] = { 5, 0x06, 0x08, 0x70, 0x08, 0x06 };
static const uint8_t SYM_y[] = { 4, 0x1C, 0xA0, 0xA0, 0x7C };
static const uint8_t SYM_Z[] = { 4, 0x62, 0x52, 0x4A, 0x46 };
static const uint8_t SYM_z[] = { 4, 0x64, 0x54, 0x54, 0x4C };

static const uint8_t SYM_BE[]   = { 4, 0x7E, 0x4A, 0x4A, 0x32 };
static const uint8_t SYM_be[]   = { 4, 0x38, 0x54, 0x54, 0x32 };
static const uint8_t SYM_ve[]   = { 4, 0x7C, 0x54, 0x54, 0x28 };
static const uint8_t SYM_GE[]   = { 4, 0x7E, 0x02, 0x02, 0x02 };
static const uint8_t SYM_ge[]   = { 3, 0x7C, 0x04, 0x04 };
static const uint8_t SYM_DE[]   = { 5, 0xC0, 0x7E, 0x42, 0x7E, 0xC0 };
static const uint8_t SYM_de[]   = { 5, 0xC0, 0x7C, 0x44, 0x7C, 0xC0 };
static const uint8_t SYM_YO[]   = { 4, 0x7C, 0x55, 0x54, 0x45 };
static const uint8_t SYM_yo[]   = { 4, 0x38, 0x55, 0x54, 0x19 };
static const uint8_t SYM_JE[]   = { 5, 0x76, 0x08, 0x7E, 0x08, 0x76 };
static const uint8_t SYM_je[]   = { 5, 0x6C, 0x10, 0x7C, 0x10, 0x6C };
static const uint8_t SYM_ze[]   = { 4, 0x44, 0x54, 0x54, 0x28 };
static const uint8_t SYM_YI[]   = { 4, 0x7E, 0x20, 0x10, 0x7E };
static const uint8_t SYM_yi[]   = { 4, 0x7C, 0x20, 0x10, 0x7C };
static const uint8_t SYM_YY[]   = { 4, 0x7C, 0x21, 0x11, 0x7C };
static const uint8_t SYM_ke[]   = { 4, 0x7C, 0x10, 0x28, 0x44 };
static const uint8_t SYM_LE[]   = { 4, 0x40, 0x3E, 0x02, 0x7E };
static const uint8_t SYM_le[]   = { 4, 0x40, 0x38, 0x04, 0x7C };
static const uint8_t SYM_me[]   = { 5, 0x7C, 0x08, 0x10, 0x08, 0x7C };
static const uint8_t SYM_ne[]   = { 4, 0x7C, 0x10, 0x10, 0x7C };
static const uint8_t SYM_PE[]   = { 4, 0x7E, 0x02, 0x02, 0x7E };
static const uint8_t SYM_pe[]   = { 4, 0x7C, 0x04, 0x04, 0x7C };
static const uint8_t SYM_te[]   = { 5, 0x04, 0x04, 0x7C, 0x04, 0x04 };
static const uint8_t SYM_UU[]   = { 4, 0x0E, 0x50, 0x50, 0x3E };
static const uint8_t SYM_FE[]   = { 5, 0x18, 0x24, 0x7E, 0x24, 0x18 };
static const uint8_t SYM_fe[]   = { 5, 0x18, 0x24, 0x7C, 0x24, 0x18 };
static const uint8_t SYM_CE[]   = { 5, 0x7E, 0x40, 0x40, 0x7E, 0xC0 };
static const uint8_t SYM_ce[]   = { 4, 0x7C, 0x40, 0x7C, 0xC0 };
static const uint8_t SYM_CHE[]  = { 4, 0x0E, 0x10, 0x10, 0x7E };
static const uint8_t SYM_che[]  = { 4, 0x0C, 0x10, 0x10, 0x7C };
static const uint8_t SYM_SHE[]  = { 5, 0x7E, 0x40, 0x7C, 0x40, 0x7E };
static const uint8_t SYM_she[]  = { 5, 0x7C, 0x40, 0x78, 0x40, 0x7C };
static const uint8_t SYM_SCHE[] = { 5, 0x7E, 0x40, 0x7C, 0x40, 0xFE };
static const uint8_t SYM_sche[] = { 5, 0x7C, 0x40, 0x78, 0x40, 0xFC };
static const uint8_t SYM_TZ[]   = { 5, 0x02, 0x7E, 0x48, 0x48, 0x30 };
static const uint8_t SYM_tz[]   = { 5, 0x04, 0x7C, 0x48, 0x48, 0x30 };
static const uint8_t SYM_YII[]  = { 5, 0x7E, 0x48, 0x78, 0x00, 0x7E };
static const uint8_t SYM_yii[]  = { 5, 0x7C, 0x48, 0x78, 0x00, 0x7C };
static const uint8_t SYM_mz[]   = { 4, 0x7C, 0x48, 0x48, 0x30 };
static const uint8_t SYM_YEE[]  = { 4, 0x24, 0x42, 0x4A, 0x3C };
static const uint8_t SYM_yee[]  = { 4, 0x44, 0x54, 0x54, 0x38 };
static const uint8_t SYM_YU[]   = { 5, 0x7E, 0x08, 0x7E, 0x42, 0x7E };
static const uint8_t SYM_yu[]   = { 5, 0x7C, 0x10, 0x7C, 0x44, 0x7C };
static const uint8_t SYM_YA[]   = { 4, 0x4C, 0x32, 0x12, 0x7E };
static const uint8_t SYM_ya[]   = { 4, 0x48, 0x34, 0x14, 0x7C };

static const uint8_t SYM_S1[]   = { 1, 0x5F };                         // !
static const uint8_t SYM_S2[]   = { 3, 0x06, 0x00, 0x06 };             // "
static const uint8_t SYM_S3[]   = { 3, 0x06, 0x18, 0x60 };             // \  .
static const uint8_t SYM_S4[]   = { 5, 0x24, 0x7E, 0x24, 0x7E, 0x24 }; // #
static const uint8_t SYM_S5[]   = { 5, 0x24, 0x4A, 0xFF, 0x52, 0x24 }; // $
static const uint8_t SYM_S6[]   = { 4, 0x66, 0x16, 0x68, 0x66 };       // %
static const uint8_t SYM_S7[]   = { 5, 0x34, 0x4A, 0x54, 0x20, 0x50 }; // &
static const uint8_t SYM_S8[]   = { 1, 0x06 };                         // '
static const uint8_t SYM_S9[]   = { 2, 0x3C, 0x42 };                   // (
static const uint8_t SYM_S10[]  = { 2, 0x42, 0x3C };                   // )
static const uint8_t SYM_S11[]  = { 5, 0x10, 0x54, 0x38, 0x54, 0x10 }; // *
static const uint8_t SYM_S12[]  = { 3, 0x10, 0x38, 0x10 };             // +
static const uint8_t SYM_S13[]  = { 2, 0x80, 0x60 };                   // ,
static const uint8_t SYM_S14[]  = { 3, 0x10, 0x10, 0x10 };             // -
static const uint8_t SYM_S15[]  = { 2, 0x60, 0x60 };                   // .
static const uint8_t SYM_S16[]  = { 3, 0x60, 0x18, 0x06 };             // /
static const uint8_t SYM_S17[]  = { 5, 0x3C, 0x42, 0x5A, 0x56, 0x1C }; // @
static const uint8_t SYM_S18[]  = { 1, 0x28 };                         // :
static const uint8_t SYM_S19[]  = { 2, 0x80, 0x68 };                   // ;
static const uint8_t SYM_S20[]  = { 3, 0x10, 0x28, 0x44 };             // <
static const uint8_t SYM_S21[]  = { 3, 0x44, 0x28, 0x10 };             // >
static const uint8_t SYM_S22[]  = { 3, 0x28, 0x28, 0x28 };             // =
static const uint8_t SYM_S23[]  = { 2, 0x7E, 0x42 };                   // [
static const uint8_t SYM_S24[]  = { 2, 0x42, 0x7E };                   // ]
static const uint8_t SYM_S25[]  = { 3, 0x40, 0x40, 0x40 };             // _
static const uint8_t SYM_S26[]  = { 3, 0x04, 0x02, 0x04 };             // ^
static const uint8_t SYM_S27[]  = { 3, 0x08, 0x76, 0x42 };             // {
static const uint8_t SYM_S28[]  = { 3, 0x42, 0x76, 0x08 };             // }
static const uint8_t SYM_S29[]  = { 1, 0x7E };                         // |
static const uint8_t SYM_S30[]  = { 4, 0x10, 0x08, 0x10, 0x08 };       // ~

static const uint8_t SYM_micro[] = { 4, 0xFC, 0x40, 0x40, 0x3C }; // µ
static const uint8_t SYM_sad_smile[] = {6, 0x3C, 0x5A, 0x6E, 0x6E, 0x5A, 0x3C}; // ☹

static const Character font_5x8_data[] =
{
	{L' ', SYM_SPACE },
	{L'?', SYM_QUEST},

	{L'0', SYM_0},
	{L'1', SYM_1},
	{L'2', SYM_2},
	{L'3', SYM_3},
	{L'4', SYM_4},
	{L'5', SYM_5},
	{L'6', SYM_6},
	{L'7', SYM_7},
	{L'8', SYM_8},
	{L'9', SYM_9},

	{L'A', SYM_A},
	{L'a', SYM_a},
	{L'B', SYM_B},
	{L'b', SYM_b},
	{L'C', SYM_C},
	{L'c', SYM_c},
	{L'D', SYM_D},
	{L'd', SYM_d},
	{L'E', SYM_E},
	{L'e', SYM_e},
	{L'F', SYM_F},
	{L'f', SYM_f},
	{L'G', SYM_G},
	{L'g', SYM_g},
	{L'H', SYM_H},
	{L'h', SYM_h},
	{L'I', SYM_I},
	{L'i', SYM_i},
	{L'J', SYM_J},
	{L'j', SYM_j},
	{L'K', SYM_K},
	{L'k', SYM_k},
	{L'L', SYM_L},
	{L'l', SYM_l},
	{L'M', SYM_M},
	{L'm', SYM_m},
	{L'N', SYM_N},
	{L'n', SYM_n},
	{L'O', SYM_O},
	{L'o', SYM_o},
	{L'P', SYM_P},
	{L'p', SYM_p},
	{L'Q', SYM_Q},
	{L'q', SYM_q},
	{L'R', SYM_R},
	{L'r', SYM_r},
	{L'S', SYM_S},
	{L's', SYM_s},
	{L'T', SYM_T},
	{L't', SYM_t},
	{L'U', SYM_U},
	{L'u', SYM_u},
	{L'V', SYM_V},
	{L'v', SYM_v},
	{L'W', SYM_W},
	{L'w', SYM_w},
	{L'X', SYM_X},
	{L'x', SYM_x},
	{L'Y', SYM_Y},
	{L'y', SYM_y},
	{L'Z', SYM_Z},
	{L'z', SYM_z},

	{L'А', SYM_A},
	{L'а', SYM_a},
	{L'Б', SYM_BE},
	{L'б', SYM_be},
	{L'В', SYM_B},
	{L'в', SYM_ve},
	{L'Г', SYM_GE},
	{L'г', SYM_ge},
	{L'Д', SYM_DE},
	{L'д', SYM_de},
	{L'Е', SYM_E},
	{L'е', SYM_e},
	{L'Ё', SYM_YO},
	{L'ё', SYM_yo},
	{L'Ж', SYM_JE},
	{L'ж', SYM_je},
	{L'З', SYM_3},
	{L'з', SYM_ze},
	{L'И', SYM_YI},
	{L'и', SYM_yi},
	{L'Й', SYM_YY},
	{L'й', SYM_YY},
	{L'К', SYM_K},
	{L'к', SYM_ke},
	{L'Л', SYM_LE},
	{L'л', SYM_le},
	{L'М', SYM_M},
	{L'м', SYM_me},
	{L'Н', SYM_H},
	{L'н', SYM_ne},
	{L'О', SYM_O},
	{L'о', SYM_o},
	{L'П', SYM_PE},
	{L'п', SYM_pe},
	{L'Р', SYM_P},
	{L'р', SYM_p},
	{L'С', SYM_C},
	{L'с', SYM_c},
	{L'Т', SYM_T},
	{L'т', SYM_te},
	{L'У', SYM_UU},
	{L'у', SYM_y},
	{L'Ф', SYM_FE},
	{L'ф', SYM_fe},
	{L'Х', SYM_X},
	{L'х', SYM_x},
	{L'Ц', SYM_CE},
	{L'ц', SYM_ce},
	{L'Ч', SYM_CHE},
	{L'ч', SYM_che},
	{L'Ш', SYM_SHE},
	{L'ш', SYM_she},
	{L'Щ', SYM_SCHE},
	{L'щ', SYM_sche},
	{L'Ъ', SYM_TZ},
	{L'ъ', SYM_tz},
	{L'Ы', SYM_YII},
	{L'ы', SYM_yii},
	{L'Ь', SYM_b},
	{L'ь', SYM_mz},
	{L'Э', SYM_YEE},
	{L'э', SYM_yee},
	{L'Ю', SYM_YU},
	{L'ю', SYM_yu},
	{L'Я', SYM_YA},
	{L'я', SYM_ya},

	{L'!', SYM_S1},
	{L'\"', SYM_S2},
	{L'\\', SYM_S3},
	{L'#', SYM_S4},
	{L'$', SYM_S5},
	{L'%', SYM_S6},
	{L'&', SYM_S7},
	{L'\'', SYM_S8},
	{L'(', SYM_S9},
	{L')', SYM_S10},
	{L'*', SYM_S11},
	{L'+', SYM_S12},
	{L',', SYM_S13},
	{L'-', SYM_S14},
	{L'.', SYM_S15},
	{L'/', SYM_S16},
	{L'@', SYM_S17},
	{L':', SYM_S18},
	{L';', SYM_S19},
	{L'<', SYM_S20},
	{L'>', SYM_S21},
	{L'=', SYM_S22},
	{L'[', SYM_S23},
	{L']', SYM_S24},
	{L'_', SYM_S25},
	{L'^', SYM_S26},
	{L'{', SYM_S27},
	{L'}', SYM_S28},
	{L'|', SYM_S29},
	{L'~', SYM_S30},

	{L'µ', SYM_micro},
	{L'☹', SYM_sad_smile},

	{0, nullptr}
};

const Font font_simple8_ru = {
	8,                           // height
	FONT_FLAG_ONLY_BITMAP_WIDTH, // flags
	1,                           // spacing
	0,                           // character count
	font_5x8_data                // characters data
};


} // namespace mgfxpp