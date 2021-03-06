#pragma once
#include "common\\common.h"
#include "population.h"
#include "concurrency_tools\\ve.h"

class world_state;

namespace population {
	class population_state;

	void init_population_state(world_state& ws);
	void determine_farmer_and_laborer(scenario::scenario_manager& s);
	void reset_state(population_state& s);
	pop_tag allocate_new_pop(world_state& ws);
	pop_tag make_new_pop(
		world_state& ws,
		float initial_size,
		float militancy,
		float consciousness,
		float literacy,
		provinces::province_tag location,
		pop_type_tag t,
		cultures::culture_tag c,
		cultures::religion_tag r);
	pop_tag concurrent_make_new_pop(
		world_state& ws,
		float initial_size,
		float militancy,
		float consciousness,
		float literacy,
		provinces::province_tag location,
		pop_type_tag t,
		cultures::culture_tag c,
		cultures::religion_tag r);
	void default_initialize_issues_and_ideology(world_state& ws, pop_tag this_pop);
	void default_initialize_world_issues_and_ideology(world_state& ws);
	void init_pop_demographics(world_state& ws, pop_tag p, float size);
	bool is_pop_accepted(world_state const& ws, pop_tag p, nations::country_tag n);
	void set_militancy_direct(world_state& ws, pop_tag p, float v);
	void set_literacy_direct(world_state& ws, pop_tag p, float v);
	void set_consciousness_direct(world_state& ws, pop_tag p, float v);
	pop_type_tag fit_type_to_province(world_state const& ws, provinces::province_tag prov, pop_type_tag type);

	bool is_dominant_issue(world_state const& ws, pop_tag id, issues::option_tag opt);
	bool is_dominant_ideology(world_state const& ws, pop_tag id, ideologies::ideology_tag opt);
	void change_pop_type(world_state& ws, pop_tag this_pop, pop_type_tag new_type);
	void change_pop_size(world_state& ws, pop_tag this_pop, int32_t new_size);
	void grow_pop_immediate(world_state& ws, pop_tag this_pop, float size_change);
	void change_pop_location(world_state& ws, pop_tag this_pop, provinces::province_tag new_location);
	void remove_pop_from_province(world_state& ws, pop_tag this_pop);
	void free_slave(world_state& ws, pop_tag this_pop);
	void trigger_rising(world_state& ws, rebel_type_tag rtype, nations::country_tag in_nation);
	float total_size_change(world_state const& ws, pop_tag this_pop);
	pop_tag find_in_province(world_state const& ws, provinces::province_tag prov, pop_type_tag type, cultures::culture_tag c, cultures::religion_tag r);

	void update_literacy(world_state& ws);
	void update_pop_ideology_and_issues(world_state& ws);
	void update_militancy(world_state& ws);
	void update_consciousness(world_state& ws);
	void calculate_promotion_and_demotion_qnty(world_state& ws);
	void calculate_migration_qnty(world_state& ws);
	void calculate_assimilation_qnty(world_state& ws);
	void execute_size_changes(world_state& ws);
	void update_independence_movements(world_state& ws);
	void update_local_rebels_and_movements(world_state& ws);

	struct population_range {
		float min;
		float max;
	};

	population_range population_amount_range(world_state const& ws);
	population_range population_density_range(world_state const& ws);
	population_range internal_migration_range(world_state const& ws);
	population_range external_migration_range(world_state const& ws);

	template<typename T>
	auto desired_needs_spending(world_state const& ws, T p) -> decltype(ve::widen_to<T>(0.0f));
	template<typename T>
	auto get_pop_owner(world_state const& ws, T p) -> decltype(ve::widen_to<T>(nations::country_tag()));
	template<typename T>
	auto get_militancy_direct(world_state const& ws, T p) -> decltype(ve::widen_to<T>(0.0f));
	template<typename T>
	auto get_literacy_direct(world_state const& ws, T p) -> decltype(ve::widen_to<T>(0.0f));
	template<typename T>
	auto get_consciousness_direct(world_state const& ws, T p) -> decltype(ve::widen_to<T>(0.0f));

	float project_assimilation_amount(world_state const& ws, pop_tag pop_v);
	float project_promotion_amount(world_state const& ws, pop_tag pop_v);
	float project_emigration_amount(world_state const& ws, pop_tag pop_v);
	float project_migration_amount(world_state const& ws, pop_tag pop_v);
	float project_literacy_change(world_state const& ws, nations::state_tag state_v);
	float project_militancy_change(world_state const& ws, pop_tag pop_v);
	float project_consciousness_change(world_state const& ws, pop_tag pop_v);
}
