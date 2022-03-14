#pragma once
#include <SFML/Graphics.hpp>
#include <SFMLSTGUI/CheckBox/CheckBox.hpp>
#include <SFMLSTGUI/Button/Button.hpp>
#include <SFMLSTGUI/LineEdit/LineEdit.hpp>

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

	static constexpr float options_pos_x{100};
	static constexpr float options_pos_y{20};
	static constexpr float options_interval{4};
	static constexpr float options_colum_2_x{ options_pos_x + 100};
	static constexpr float options_colum_3_x{ options_colum_2_x + 100};

	//----------------
	st::CheckBox subjugation_cb;
	void subjugation(st::CheckBox& le, st::CheckBox::State state);
	

	st::CheckBox hp_cb;
	void hp(st::CheckBox& le, st::CheckBox::State state);

	st::CheckBox shields_cb;
	void shields(st::CheckBox& le, st::CheckBox::State state);

	st::CheckBox energy_cb;
	void energy(st::CheckBox& le, st::CheckBox::State state);
	

	st::CheckBox add_hp_cb;
	st::LineEdit add_hp_le;

	st::CheckBox add_shields_cb;
	st::LineEdit add_shields_le;

	st::CheckBox add_energy_cb;
	st::LineEdit add_energy_le;


	st::CheckBox rollback_skill_units;//умения юнитов
	st::CheckBox rollback_skill_building;//varp
	st::CheckBox instant_units_training;//производство юнитов
	st::CheckBox instant_mercenaries;//мгновенные наёмники


	st::Button set_minerals_b;
	st::LineEdit set_minerals_le;
	sf::Text set_minerals_t;
	void setMinerals();

	st::Button set_vespen_b;
	st::LineEdit set_vespen_le;
	sf::Text set_vespen_t;
	void setVespen();

	st::Button set_money_b;
	st::LineEdit set_money_le;
	sf::Text set_money_t;
	//---------------

	static constexpr float process_id_x{ 20 };
	static constexpr float process_id_y{ 400 };
	sf::Text process_id_t;

	Cheat cheat;
	bool process_is_open{ false };
};