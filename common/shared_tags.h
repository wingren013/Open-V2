#pragma once
#include "common.h"

namespace graphics {
	using obj_definition_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 174634>>;
	using texture_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 74896>>;
	using font_tag = tag_type<uint8_t, std::true_type, std::integral_constant<size_t, 860367>>;
}

namespace ui {
	using button_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 15356>>;
	using icon_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 15357>>;
	using text_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 15358>>;
	using position_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 15359>>;
	using overlapping_region_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 15360>>;
	using listbox_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 15361>>;
	using scrollbar_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 15362>>;
	using window_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 15363>>;
}

namespace text_data {
	using text_tag = tag_type<uint16_t, std::true_type, std::integral_constant<size_t, 342678>>;
}