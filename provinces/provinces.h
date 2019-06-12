#pragma once
#include <stdint.h>
#include "common\\common.h"
#include "common\\shared_tags.h"
#include "concurrency_tools\\concurrency_tools.hpp"
#include "modifiers\\modifiers.h"
#include "economy\\economy.h"
#include "concurrency_tools\\variable_layout.h"

namespace modifiers {
	class modifiers_manager;
}

namespace military {
	struct fleet_presence;
}

class world_state;

namespace provinces {
	struct timed_provincial_modifier {
		date_tag expiration;
		modifiers::provincial_modifier_tag mod;

		bool operator<(timed_provincial_modifier const& other)  const noexcept { return mod < other.mod; }
		bool operator==(timed_provincial_modifier const& other) const noexcept { return mod == other.mod && expiration == other.expiration; }
	};
}

namespace state_region {
	struct name;
	struct provinces;
}

namespace province {
	struct is_sea;
	struct is_coastal;
	struct is_lake;
	struct area;
	struct centroid;
	struct centroid_2d;
	struct continent;
	struct climate;
	struct state_id;

	struct demographics;
	struct same_type_adjacency;
	struct coastal_adjacency;

	using container =
		variable_layout_contiguous_tagged_vector<
		provinces::province_tag, 4000,
		is_sea, bitfield_type,
		is_coastal, bitfield_type,
		is_lake, bitfield_type, 
		area, float,
		centroid, Eigen::Vector3f,
		centroid_2d, Eigen::Vector2f,
		continent, modifiers::provincial_modifier_tag,
		climate, modifiers::provincial_modifier_tag,
		state_id, provinces::state_tag
		>;

	void initialize(container& c, provinces::province_tag);
}

namespace province_state {
	struct owner;
	struct controller;
	struct rebel_controller;
	struct state_instance;
	struct orders;
	struct strat_hq;
	struct total_population;
	struct nationalism;
	struct siege_progress;
	struct fort_upgrade_progress;
	struct railroad_upgrade_progress;
	struct naval_base_upgrade_progress;
	struct artisan_production_scale;

	struct net_migration_growth;
	struct net_immigration_growth;
	struct monthly_population;
	struct old_migration_growth;
	struct old_immigration_growth;
	struct old_monthly_population;

	struct last_controller_change;
	struct last_immigration;
	struct rgo_worker_data;
	struct last_produced;

	struct cores;
	struct pops;
	struct fleets;
	struct static_modifiers;
	struct timed_modifiers;

	struct name;
	struct crime;
	struct terrain;
	struct base_life_rating;
	struct dominant_culture;

	struct rgo_production;
	struct artisan_production;
	struct dominant_issue;
	struct dominant_ideology;
	struct dominant_religion;

	struct fort_level;
	struct railroad_level;
	struct naval_base_level;
	struct rgo_size;

	struct is_blockaded;
	struct is_overseas;
	struct has_owner_core;
	struct owner_building_railroad;
	struct is_non_state;

	constexpr int32_t container_size = 4000;
	using container =
		variable_layout_contiguous_tagged_vector <
		provinces::province_tag, container_size,

		is_blockaded, bitfield_type,
		is_overseas, bitfield_type,
		has_owner_core, bitfield_type,
		owner_building_railroad, bitfield_type,
		is_non_state, bitfield_type,

		fort_level, uint8_t,
		railroad_level, uint8_t,
		naval_base_level, uint8_t,
		rgo_size, uint8_t,

		dominant_religion, cultures::religion_tag,
		dominant_ideology, ideologies::ideology_tag,
		dominant_issue, issues::option_tag,

		artisan_production, economy::goods_tag,
		rgo_production, economy::goods_tag,

		dominant_culture, cultures::culture_tag,
		base_life_rating, float,
		terrain, modifiers::provincial_modifier_tag,
		crime, modifiers::provincial_modifier_tag,
		name, text_data::text_tag,

		timed_modifiers, multiset_tag<provinces::timed_provincial_modifier>,
		static_modifiers, set_tag<modifiers::provincial_modifier_tag>,
		fleets, set_tag<military::fleet_presence>,
		pops, array_tag<population::pop_tag, int32_t, false>,
		cores, set_tag<cultures::national_tag>,

		rgo_worker_data, economy::worked_instance,
		total_population, float,
		last_controller_change, date_tag,
		last_immigration, date_tag,

		nationalism, float,
		siege_progress, float,
		fort_upgrade_progress, float,
		railroad_upgrade_progress, float,
		naval_base_upgrade_progress, float,
		artisan_production_scale, float,

		net_migration_growth, float,
		net_immigration_growth, float,
		monthly_population, float,
		old_migration_growth, float,
		old_immigration_growth, float,
		old_monthly_population, float,

		owner, nations::country_tag,
		controller, nations::country_tag,
		rebel_controller, population::rebel_faction_tag,
		state_instance, nations::state_tag,
		orders, military::army_orders_tag,
		strat_hq, military::strategic_hq_tag
		> ;

	 void initialize(container& c, provinces::province_tag);
}

namespace provinces {
	class borders_manager {
	public:
		struct province_border_info {
			int16_t size;
			int16_t offset;
			provinces::province_tag a;
			provinces::province_tag b;
		};

		struct border_block {
			std::vector<province_border_info> province_borders;
			std::vector<float> vertices_data;
			std::vector<int32_t> indices_data;

			uint32_t vertices_handle = 0;
			uint32_t indices_handle = 0;

			int32_t coastal_borders_size = 0;
			int32_t state_borders_size = 0;
			int32_t province_borders_size = 0;
		};

		std::vector<border_block> borders;
		uint32_t border_vbo = 0;
	};

	class state_distances_manager {
	private:
		float* distance_data = nullptr;
		int32_t last_aligned_state_max = 0;
	public:
		void update(world_state const& ws);
		float distance(nations::state_tag a, nations::state_tag b) const;
		tagged_array_view<float,nations::state_tag> get_row(nations::state_tag a) const {
			return tagged_array_view<float, nations::state_tag>(distance_data + (1 + to_index(a)) * last_aligned_state_max + 1, last_aligned_state_max);
		}
		int32_t row_size() const { return last_aligned_state_max; }
		~state_distances_manager();
	};

	class provinces_state {
	public:
		province_state::container province_state_container;
		fixed_vectorizable_2d_array<provinces::province_tag, modifiers::value_type, province_state::container_size, modifiers::provincial_offsets::count> modifier_values;

		tagged_fixed_2dvector<float, province_tag, ideologies::ideology_tag> party_loyalty;
		tagged_fixed_blocked_2dvector<float, province_tag, population::demo_tag, aligned_allocator_32<int32_t>> province_demographics;

		std::vector<uint8_t> is_canal_enabled;

		std::vector<float> province_distance_to;
		std::vector<province_tag> province_path_to;
		state_distances_manager state_distances;

		stable_variable_vector_storage_mk_2<cultures::national_tag, 4, 8192> core_arrays;
		stable_variable_vector_storage_mk_2<modifiers::provincial_modifier_tag, 4, 8192> static_modifier_arrays;
		stable_variable_vector_storage_mk_2<timed_provincial_modifier, 4, 8192> timed_modifier_arrays;

		stable_variable_vector_storage_mk_2<province_tag, 4, 8192> province_arrays;

		modifiers::provincial_modifier_tag europe_modifier;
		modifiers::provincial_modifier_tag asia_modifier;
		modifiers::provincial_modifier_tag north_america_modifier;
		modifiers::provincial_modifier_tag south_america_modifier;
		modifiers::provincial_modifier_tag africa_modifier;
		modifiers::provincial_modifier_tag oceania_modifier;
	};

	class province_manager {
	public:
		province::container province_container;
		tagged_vector<text_data::text_tag, state_tag> state_names;
		std::vector<provinces::province_tag> integer_to_province;
		int32_t first_sea_province = 0;

		v_vector<province_tag, state_tag> states_to_province_index;
		boost::container::flat_map<text_data::text_tag, state_tag> named_states_index;

		v_vector<province_tag, province_tag> same_type_adjacency;
		v_vector<province_tag, province_tag> coastal_adjacency;
		std::vector<std::tuple<province_tag, province_tag, text_data::text_tag, province_tag>> canals; // connection A, connection B, name, connection through

		boost::container::flat_map<modifiers::provincial_modifier_tag, graphics::obj_definition_tag> terrain_graphics;

		std::vector<uint16_t> province_map_data;
		int32_t province_map_width = 0;
		int32_t province_map_height = 0;
		borders_manager borders;

		template<typename F>
		void for_each_province(F const& f) const {
			int32_t const cmax = int32_t(province_container.size());
			for(int32_t i = 1; i < cmax; ++i) {
				f(province_tag(province_tag::value_base_t(i)));
			}
		}
		template<typename F>
		void for_each_land_province(F const& f) const {
			int32_t const cmax = first_sea_province;
			for(int32_t i = 1; i < cmax; ++i) {
				f(province_tag(province_tag::value_base_t(i)));
			}
		}
	};

	struct color_to_terrain_map {
		modifiers::provincial_modifier_tag data[256] = { modifiers::provincial_modifier_tag() };
	};

	constexpr uint32_t rgb_to_prov_index(uint8_t r, uint8_t g, uint8_t b) {
		return static_cast<uint32_t>(r) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(b) << 16);
	}
}
