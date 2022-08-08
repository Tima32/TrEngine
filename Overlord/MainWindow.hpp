#pragma once
#include <SFML/Graphics.hpp>
#include <SFMLSTGUI/CheckBox/CheckBox.hpp>
#include <SFMLSTGUI/Button/Button.hpp>
#include <SFMLSTGUI/LineEdit/LineEdit.hpp>
#include <SFMLSTGUI/Slider/Slider.hpp>

#include "Cheat.hpp"

class MainWindow
{
public:
	MainWindow();
	~MainWindow();

	void autoStep();
private:
	void step();
	void event();
	void updateGameInfo();
	void draw();

	static constexpr float time_tick{ 1.f / 60 };
	sf::RenderWindow window;
	sf::Event event_e;
	sf::Font font;

	static constexpr float options_pos_x{ 100 };
	static constexpr float options_pos_y{ 20 };
	static constexpr float options_interval{ 4 };
	static constexpr float options_colum_2_x{ options_pos_x + 200 };
	static constexpr float options_colum_3_x{ options_colum_2_x + 100 };

	//----------------
	st::CheckBox hp_cb;
	void hp(st::CheckBox& le, st::CheckBox::State state);

	st::CheckBox min_hp_cb;
	void min_hp(st::CheckBox& le, st::CheckBox::State state);

	st::CheckBox overlord_set_hp_cb;
	void overlord_set_hp(st::CheckBox& le, st::CheckBox::State state);
	st::SliderEditHorizontal overlord_set_hp_se;
	void overlord_set_hp_value(st::SliderEditHorizontal& seh, double value);

	st::CheckBox minions_set_hp_cb;
	void minions_set_hp(st::CheckBox& le, st::CheckBox::State state);
	st::SliderEditHorizontal minions_set_hp_se;
	void minions_set_hp_value(st::SliderEditHorizontal& seh, double value);

	st::CheckBox infinite_mp_cb;
	void infinite_mp(st::CheckBox& le, st::CheckBox::State state);

	st::CheckBox mp_multipler_cb;
	void mp_multipler(st::CheckBox& le, st::CheckBox::State state);
	st::SliderEditHorizontal mp_multipler_se;
	void mp_multipler_value(st::SliderEditHorizontal& seh, double value);

	//One hit kill
	st::CheckBox one_hit_kill_cb;
	void one_hit_kill(st::CheckBox& le, st::CheckBox::State state);
	//---------------

	static constexpr float process_id_x{ 20 };
	static constexpr float process_id_y{ 400 };
	sf::Text process_id_t;

	Cheat cheat;
	bool process_is_open{ false };
};