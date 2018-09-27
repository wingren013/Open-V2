#pragma once
#include "common\\common.h"
#include "technologies_gui.h"
#include "gui\\gui.hpp"
#include "modifiers\\modifiers_gui.h"
#include "technologies_functions.h"
#include "triggers\\trigger_functions.h"

namespace technologies {
	template<typename W>
	void tech_window_base::on_create(W& w, world_state& ws) {
		ui::hide(*associated_object);

		associated_object->size = ui::xy_pair{ 1017i16, 636i16 };
		ui::for_each_child(ws.w.gui_m, ui::tagged_gui_object{ *associated_object, ui::gui_object_tag() }, [](ui::tagged_gui_object obj) {
			obj.object.position += ui::xy_pair{ -3i16, 3i16 };
		});
		w.template get<CT_STRING("administration_type")>().associated_object->position.x += 10i16;
		
		for(uint32_t i = 0; i < std::extent_v<decltype(top_tabs)>; ++i) {
			top_tabs[i].category = tech_category_tag(static_cast<tech_category_tag::value_base_t>(i));

			auto o = ui::create_static_element(
				ws,
				std::get<ui::window_tag>(ws.s.gui_m.ui_definitions.name_to_element_map["folder_window"]),
				ui::tagged_gui_object{ *associated_object, w.window_object },
				top_tabs[i]);
			top_tabs[i].associated_object->position = ui::xy_pair{ int16_t(26 + 194 * i), 58i16};
			top_tabs[i].associated_object->size.y = 44i16;
			ui::move_to_front(ws.w.gui_m, o);
		}

		for(uint32_t i = 0; i < std::extent_v<decltype(subtabs)>; ++i) {
			subtabs[i].nth_sub_category = i;

			auto o = ui::create_static_element(
				ws,
				std::get<ui::window_tag>(ws.s.gui_m.ui_definitions.name_to_element_map["tech_group"]),
				ui::tagged_gui_object{ *associated_object, w.window_object },
				subtabs[i]);
			subtabs[i].associated_object->position = ui::xy_pair{ int16_t(26 + 194 * i), 112i16 };
			ui::move_to_front(ws.w.gui_m, o);

			for(uint32_t j = 0; j < number_of_member_techs; ++j) {
				individual_techs[i * number_of_member_techs + j].nth_sub_category = i;
				individual_techs[i * number_of_member_techs + j].nth_member = j;

				auto p = ui::create_static_element(
					ws,
					std::get<ui::window_tag>(ws.s.gui_m.ui_definitions.name_to_element_map["tech_window"]),
					ui::tagged_gui_object{ *associated_object, w.window_object },
					individual_techs[i * number_of_member_techs + j]);
				individual_techs[i * number_of_member_techs + j].associated_object->position = ui::xy_pair{ int16_t(26 + 194 * i), int16_t(126 + j * 39) };
				ui::move_to_front(ws.w.gui_m, p);
			}
		}
	}

	template<typename window_type>
	void plusminus_icon::windowed_update(ui::dynamic_icon<plusminus_icon>& self, window_type & win, world_state & ws) {
		if(win.modifier < 0.0f)
			self.set_frame(ws.w.gui_m, 0ui32);
		else
			self.set_frame(ws.w.gui_m, 1ui32);
	}
	template<typename window_type>
	void school_item_icon::windowed_update(ui::dynamic_icon<school_item_icon>& self, window_type &win, world_state & ws) {
		self.set_frame(ws.w.gui_m, win.index);
	}

	template<typename W>
	void school_item_base::on_create(W & w, world_state &) {
		associated_object->size = ui::xy_pair{ 36i16, 32i16 };
	}

	template<typename lb_type>
	void school_modifiers_lb::populate_list(lb_type & lb, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			if(auto ts = player->tech_school; is_valid_index(ts)) {
				auto milmod = ws.s.modifiers_m.national_modifier_definitions[ts][modifiers::national_offsets::army_tech_research_bonus];
				auto navmod = ws.s.modifiers_m.national_modifier_definitions[ts][modifiers::national_offsets::navy_tech_research_bonus];
				auto commod = ws.s.modifiers_m.national_modifier_definitions[ts][modifiers::national_offsets::commerce_tech_research_bonus];
				auto culmod = ws.s.modifiers_m.national_modifier_definitions[ts][modifiers::national_offsets::culture_tech_research_bonus];
				auto indmod = ws.s.modifiers_m.national_modifier_definitions[ts][modifiers::national_offsets::industry_tech_research_bonus];

				if(milmod != 0.0f)
					lb.add_item(ws, std::tuple<float, uint32_t, text_data::text_tag>(
						milmod, 0ui32, ws.s.modifiers_m.national_offset_names[modifiers::national_offsets::army_tech_research_bonus]));
				if(navmod != 0.0f)
					lb.add_item(ws, std::tuple<float, uint32_t, text_data::text_tag>(
						navmod, 1ui32, ws.s.modifiers_m.national_offset_names[modifiers::national_offsets::navy_tech_research_bonus]));
				if(commod != 0.0f)
					lb.add_item(ws, std::tuple<float, uint32_t, text_data::text_tag>(
						commod, 2ui32, ws.s.modifiers_m.national_offset_names[modifiers::national_offsets::commerce_tech_research_bonus]));
				if(culmod != 0.0f)
					lb.add_item(ws, std::tuple<float, uint32_t, text_data::text_tag>(
						culmod, 3ui32, ws.s.modifiers_m.national_offset_names[modifiers::national_offsets::culture_tech_research_bonus]));
				if(indmod != 0.0f)
					lb.add_item(ws, std::tuple<float, uint32_t, text_data::text_tag>(
						indmod, 4ui32, ws.s.modifiers_m.national_offset_names[modifiers::national_offsets::industry_tech_research_bonus]));
			}
		}
	}

	template<typename window_type>
	void invention_category_icon::windowed_update(ui::dynamic_icon<invention_category_icon>& self, window_type& win, world_state & ws) {
		if(is_valid_index(win.invention)) {
			auto invention_cat = ws.s.technology_m.technologies_container[win.invention].category;
			if(invention_cat == tech_category_type::army)
				self.set_frame(ws.w.gui_m, 0ui32);
			if(invention_cat == tech_category_type::navy)
				self.set_frame(ws.w.gui_m, 1ui32);
			if(invention_cat == tech_category_type::commerce)
				self.set_frame(ws.w.gui_m, 2ui32);
			if(invention_cat == tech_category_type::culture)
				self.set_frame(ws.w.gui_m, 3ui32);
			if(invention_cat == tech_category_type::industry)
				self.set_frame(ws.w.gui_m, 4ui32);
		}
	}

	template<typename window_type>
	void invention_item_name::windowed_update(window_type & win, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		invention = win.invention;
		if(is_valid_index(win.invention)) {
			ui::add_linear_text(ui::xy_pair{ 0,0 }, ws.s.technology_m.technologies_container[win.invention].name, fmt, ws.s.gui_m, ws.w.gui_m, box, lm);
			lm.finish_current_line();
		}
	}

	template<typename window_type>
	void invention_item_percent::windowed_update(window_type & win, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		invention = win.invention;
		if(auto player = ws.w.local_player_nation; bool(player) && is_valid_index(invention)) {
			auto chance = get_invention_chance(invention, ws, *player);
			char16_t local_buf[16];
			put_value_in_buffer(local_buf, display_type::percent, chance);
			ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buf), box, ui::xy_pair{ 0,0 }, fmt, lm);
			lm.finish_current_line();
		}
	}

	template<typename lb_type>
	void invention_lb::populate_list(lb_type & lb, world_state & ws) {
		boost::container::small_vector<std::pair<tech_tag, float>, 64, concurrent_allocator<std::pair<tech_tag, float>>> data;

		if(auto player = ws.w.local_player_nation; player) {
			if(auto pid = player->id; ws.w.nation_s.nations.is_valid_index(pid)) {
				for(auto i : ws.s.technology_m.inventions) {
					if(bit_vector_test(ws.w.nation_s.active_technologies.get_row(pid), to_index(i)) == false &&
						(!is_valid_index(ws.s.technology_m.technologies_container[i].allow) ||
							triggers::test_trigger(ws.s.trigger_m.trigger_data.data() + to_index(ws.s.technology_m.technologies_container[i].allow), ws, player, player, nullptr, nullptr))) {
						data.emplace_back(i, get_invention_chance(i, ws, *player));
					}
				}
			}
		}

		switch(ws.w.techui_invention_sort) {
			case current_state::invention_sort::none: break;
			case current_state::invention_sort::chance:
				std::sort(data.begin(), data.end(), [&ws](std::pair<tech_tag, float> a, std::pair<tech_tag, float> b) {
					return a.second > b.second;
				});
				break;
			case current_state::invention_sort::name:
			{
				vector_backed_string_lex_less<char16_t> lss(ws.s.gui_m.text_data_sequences.text_data);
				std::sort(data.begin(), data.end(), [&ws, &lss](std::pair<tech_tag, float> a, std::pair<tech_tag, float> b) {
					auto a_name = ws.s.technology_m.technologies_container[a.first].name;
					auto b_name = ws.s.technology_m.technologies_container[b.first].name;
					return lss(
						text_data::text_tag_to_backing(ws.s.gui_m.text_data_sequences, a_name),
						text_data::text_tag_to_backing(ws.s.gui_m.text_data_sequences, b_name));
				});
				break;
			}
			case current_state::invention_sort::type:
				std::sort(data.begin(), data.end(), [&ws](std::pair<tech_tag, float> a, std::pair<tech_tag, float> b) {
					return int32_t(ws.s.technology_m.technologies_container[a.first].category) < int32_t(ws.s.technology_m.technologies_container[b.first].category);
				});
				break;
			default: break;
		}

		lb.new_list(data.begin().get_ptr(), data.end().get_ptr());
	}

	template<typename window_type>
	void selected_tech_invention_item_icon::windowed_update(ui::dynamic_icon<selected_tech_invention_item_icon>& self, window_type & win, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			if(auto id = player->id; ws.w.nation_s.nations.is_valid_index(id)) {
				if(bit_vector_test(ws.w.nation_s.active_technologies.get_row(id), to_index(win.invention))) {
					self.set_frame(ws.w.gui_m, 1ui32);
					return;
				}
			}
		}
		self.set_frame(ws.w.gui_m, 0ui32);
	}

	template<typename lb_type>
	void selected_tech_invention_lb::populate_list(lb_type & lb, world_state & ws) {
		if(is_valid_index(ws.w.selected_technology)) {
			boost::container::small_vector<technologies::tech_tag, 32, concurrent_allocator<technologies::tech_tag>> data;

			for(auto i : ws.s.technology_m.inventions) {
				if(is_valid_index(ws.s.technology_m.technologies_container[i].allow) &&
					triggers::refers_to_technology(ws.s.technology_m.technologies_container[i].allow, ws.w.selected_technology, ws)) {
					data.push_back(i);
				}
			}

			lb.new_list(data.begin().get_ptr(), data.end().get_ptr());
		}
	}

	template<typename window_type>
	void folder_tab_button::windowed_update(ui::simple_button<folder_tab_button>& self, window_type & win, world_state & ws) {
		category = win.category;
		if(category == ws.w.selected_tech_category)
			self.set_frame(ws.w.gui_m, 1ui32);
		else
			self.set_frame(ws.w.gui_m, 0ui32);
	}

	template<typename window_type>
	void folder_tab_icon::windowed_update(ui::dynamic_icon<folder_tab_icon>& self, window_type & win, world_state & ws) {
		if(is_valid_index(win.category)) {
			auto cat_type = ws.s.technology_m.technology_categories[win.category].type;
			if(cat_type == tech_category_type::army)
				self.set_frame(ws.w.gui_m, 0ui32);
			else if(cat_type == tech_category_type::navy)
				self.set_frame(ws.w.gui_m, 1ui32);
			else if(cat_type == tech_category_type::commerce)
				self.set_frame(ws.w.gui_m, 2ui32);
			else if(cat_type == tech_category_type::culture)
				self.set_frame(ws.w.gui_m, 3ui32);
			else if(cat_type == tech_category_type::industry)
				self.set_frame(ws.w.gui_m, 4ui32);
		}
	}

	template<typename window_type>
	void folder_tab_name::windowed_update(window_type & win, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(is_valid_index(win.category)) {
			auto cat_name = ws.s.technology_m.technology_categories[win.category].name;
			ui::add_linear_text(ui::xy_pair{ 0,0 }, cat_name, fmt, ws.s.gui_m, ws.w.gui_m, box, lm);
			lm.finish_current_line();
		}
	}

	template<typename window_type>
	void folder_tab_count::windowed_update(window_type & win, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			if(auto id = player->id; ws.w.nation_s.nations.is_valid_index(id)) {
				auto cat_type = ws.s.technology_m.technology_categories[win.category].type;
				int32_t total_count = 0;
				int32_t has_count = 0;
				auto techs = ws.w.nation_s.active_technologies.get_row(id);

				for(auto& t : ws.s.technology_m.technologies_container) {
					if(t.category == cat_type && t.cost != 0) {
						++total_count;
						if(bit_vector_test(techs, to_index(t.id)))
							++has_count;
					}
				}

				char16_t local_buffer[32];
				auto e1 = put_value_in_buffer(local_buffer, display_type::integer, has_count);
				*e1 = u'/';
				put_value_in_buffer(e1 + 1, display_type::integer, total_count);

				ui::text_chunk_to_instances(ws.s.gui_m, ws.w.gui_m, vector_backed_string<char16_t>(local_buffer), box, ui::xy_pair{ 0,0 }, fmt, lm);
				lm.finish_current_line();
			}
		}
	}

	template<typename window_type>
	void foldet_tab_progress_bar::windowed_update(ui::progress_bar<foldet_tab_progress_bar>& bar, window_type & win, world_state & ws) {
		if(auto player = ws.w.local_player_nation; player) {
			if(auto id = player->id; ws.w.nation_s.nations.is_valid_index(id)) {
				auto cat_type = ws.s.technology_m.technology_categories[win.category].type;
				int32_t total_count = 0;
				int32_t has_count = 0;
				auto techs = ws.w.nation_s.active_technologies.get_row(id);

				for(auto& t : ws.s.technology_m.technologies_container) {
					if(t.category == cat_type && t.cost != 0) {
						++total_count;
						if(bit_vector_test(techs, to_index(t.id)))
							++has_count;
					}
				}

				if(total_count != 0)
					bar.set_fraction(float(has_count) / float(total_count));
				else
					bar.set_fraction(1.0f);
			}
		}
	}

	template<typename window_type>
	void subcategory_name::windowed_update(window_type & win, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(is_valid_index(ws.w.selected_tech_category)) {
			technologies::technology_category& cat = ws.s.technology_m.technology_categories[ws.w.selected_tech_category];
			auto subcat = cat.subcategories[win.nth_sub_category];
			if(is_valid_index(subcat)) {
				ui::add_linear_text(ui::xy_pair{ 0,0 }, ws.s.technology_m.technology_subcategories[subcat].name, fmt, ws.s.gui_m, ws.w.gui_m, box, lm);
				lm.finish_current_line();
			}
		}
	}

	template<typename window_type>
	void individual_tech_button::windowed_update(ui::simple_button<individual_tech_button>& self, window_type & win, world_state & ws) {
		if(is_valid_index(ws.w.selected_tech_category)) {
			auto subcat = ws.s.technology_m.technology_categories[ws.w.selected_tech_category].subcategories[win.nth_sub_category];
			tech = ws.s.technology_m.technology_subcategories[subcat].member_techs[win.nth_member];
		}
		if(auto player = ws.w.local_player_nation; is_valid_index(tech) && bool(player)) {
			if(player->current_research == tech) {
				self.set_frame(ws.w.gui_m, 0ui32);
				return;
			}
			auto pid = player->id;
			if(!ws.w.nation_s.nations.is_valid_index(pid))
				return;
			if(bit_vector_test(ws.w.nation_s.active_technologies.get_row(pid), to_index(tech))) {
				self.set_frame(ws.w.gui_m, 1ui32);
				return;
			}
			if(technologies::can_research(tech, ws, *player)) {
				self.set_frame(ws.w.gui_m, 2ui32);
				return;
			}
			self.set_frame(ws.w.gui_m, 3ui32);
		}
	}

	template<typename window_type>
	void individual_tech_name::windowed_update(window_type & win, ui::tagged_gui_object box, ui::text_box_line_manager & lm, ui::text_format & fmt, world_state & ws) {
		if(is_valid_index(ws.w.selected_tech_category)) {
			auto subcat = ws.s.technology_m.technology_categories[ws.w.selected_tech_category].subcategories[win.nth_sub_category];
			auto tech = ws.s.technology_m.technology_subcategories[subcat].member_techs[win.nth_member];
			if(is_valid_index(tech)) {
				ui::add_linear_text(ui::xy_pair{ 0,0 }, ws.s.technology_m.technologies_container[tech].name, fmt, ws.s.gui_m, ws.w.gui_m, box, lm);
				lm.finish_current_line();
			}
		}
	}
}
