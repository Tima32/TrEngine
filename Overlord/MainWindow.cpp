#include "MainWindow.hpp"

#include <iostream>

using namespace std;
using namespace sf;

template <typename A, typename B>
static void VerticalLayout(const A& relatively, B& trget, float interval)
{
	trget.setPosition(relatively.getPosition().x, relatively.getPosition().y + relatively.getSize().y + interval);
}

MainWindow::MainWindow() :
	event_e()
{
	string s("Fonts\\verdana.ttf");
	if (!font.loadFromFile(s))
		cout << "Error load font" << endl;

	// Immortal
	{
		hp_cb.setFont(font);
		hp_cb.setPosition(options_pos_x, options_pos_y);
		hp_cb.setString(L"HP");
		hp_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{hp(le, state); });

		min_hp_cb.setFont(font);
		VerticalLayout(hp_cb, min_hp_cb, options_interval);
		min_hp_cb.setString(L"Minions HP");
		min_hp_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{min_hp(le, state); });
	}

	// Set HP
	{
		overlord_set_hp_cb.setFont(font);
		VerticalLayout(min_hp_cb, overlord_set_hp_cb, options_interval);
		overlord_set_hp_cb.setString(L"Set Overlord max HP");
		overlord_set_hp_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{overlord_set_hp(le, state); });

		overlord_set_hp_se.setFont(font);
		overlord_set_hp_se.setPosition(options_colum_2_x, overlord_set_hp_cb.getPosition().y);
		overlord_set_hp_se.setMin(150);
		overlord_set_hp_se.setMax(500);
		overlord_set_hp_se.setValue(150);
		overlord_set_hp_se.setTextMode(st::SliderEditHorizontal::TextMode::Value);
		overlord_set_hp_se.setTextPrecision(2);
		overlord_set_hp_se.setChangeValueEndCallback(
			[this](st::SliderEditHorizontal& seh, double value)
			{overlord_set_hp_value(seh, value); });

		minions_set_hp_cb.setFont(font);
		VerticalLayout(overlord_set_hp_cb, minions_set_hp_cb, options_interval);
		minions_set_hp_cb.setString(L"Set minions max HP");
		minions_set_hp_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{minions_set_hp(le, state); });

		minions_set_hp_se.setFont(font);
		minions_set_hp_se.setPosition(options_colum_2_x, minions_set_hp_cb.getPosition().y);
		minions_set_hp_se.setMin(150);
		minions_set_hp_se.setMax(500);
		minions_set_hp_se.setValue(150);
		minions_set_hp_se.setTextMode(st::SliderEditHorizontal::TextMode::Value);
		minions_set_hp_se.setTextPrecision(2);
		minions_set_hp_se.setChangeValueEndCallback(
			[this](st::SliderEditHorizontal& seh, double value)
			{minions_set_hp_value(seh, value); });
	}

	// MP
	{
		infinite_mp_cb.setFont(font);
		VerticalLayout(minions_set_hp_cb, infinite_mp_cb, options_interval);
		infinite_mp_cb.setString(L"Infinitie MP");
		infinite_mp_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{infinite_mp(le, state); });

		mp_multipler_cb.setFont(font);
		VerticalLayout(infinite_mp_cb, mp_multipler_cb, options_interval);
		mp_multipler_cb.setString(L"Magic spending multipler");
		mp_multipler_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{mp_multipler(le, state); });

		mp_multipler_se.setFont(font);
		mp_multipler_se.setPosition(options_colum_2_x, mp_multipler_cb.getPosition().y);
		mp_multipler_se.setMin(0);
		mp_multipler_se.setMax(1);
		mp_multipler_se.setValue(0.1);
		mp_multipler_se.setTextMode(st::SliderEditHorizontal::TextMode::Value);
		mp_multipler_se.setTextPrecision(2);
		mp_multipler_se.setChangeValueEndCallback(
			[this](st::SliderEditHorizontal& seh, double value)
			{mp_multipler_value(seh, value); });
	}

	// One hit kill
	{
		one_hit_kill_cb.setFont(font);
		VerticalLayout(mp_multipler_cb, one_hit_kill_cb, options_interval);
		one_hit_kill_cb.setString(L"One hit kill");
		one_hit_kill_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{one_hit_kill(le, state); });
	}

	process_id_t.setFont(font);
	process_id_t.setPosition(process_id_x, process_id_y);
	process_id_t.setCharacterSize(11);

	window.create(sf::VideoMode(720, 420), L"Overlord trainer");
}
MainWindow::~MainWindow()
{

}

void MainWindow::autoStep()
{
	while (window.isOpen())
		step();
}

void MainWindow::step()
{
	while (window.pollEvent(event_e))
		event();
	updateGameInfo();
	draw();

	sf::sleep(sf::seconds(time_tick));
}
void MainWindow::event()
{
	if (event_e.type == Event::Resized)
	{
		auto view = window.getView();
		view.reset(FloatRect(0, 0, float(event_e.size.width), float(event_e.size.height)));
		window.setView(view);
	}
	if (event_e.type == Event::Closed)
	{
		window.close();
	}

	bool focus = false;
	
	// Immortal
	hp_cb.event(event_e, focus);
	min_hp_cb.event(event_e, focus);

	// Set HP
	overlord_set_hp_cb.event(event_e, focus);
	overlord_set_hp_se.event(event_e, focus);
	minions_set_hp_cb.event(event_e, focus);
	minions_set_hp_se.event(event_e, focus);

	// MP
	infinite_mp_cb.event(event_e, focus);
	mp_multipler_cb.event(event_e, focus);
	mp_multipler_se.event(event_e, focus);

	// One hit kill
	one_hit_kill_cb.event(event_e, focus);
}
void MainWindow::updateGameInfo()
{
	if (!cheat.isOpen())
	{
		cheat.open();
		process_id_t.setString(L"Process ID: ");

		if (process_is_open == true)
		{
			hp_cb.setState(st::CheckBox::State::Unchecked);
			min_hp_cb.setState(st::CheckBox::State::Unchecked);

			overlord_set_hp_cb.setState(st::CheckBox::State::Unchecked);
			minions_set_hp_cb.setState(st::CheckBox::State::Unchecked);

			infinite_mp_cb.setState(st::CheckBox::State::Unchecked);
			mp_multipler_cb.setState(st::CheckBox::State::Unchecked);

			one_hit_kill_cb.setState(st::CheckBox::State::Unchecked);
		}

		process_is_open = false;
	}
	else
	{
		process_id_t.setString(wstring(L"Process ID: ") + to_wstring(cheat.getID()));

		process_is_open = true;
	}
}
void MainWindow::draw()
{
	window.clear(Color(30, 30, 30));

	// Immmortal
	window.draw(hp_cb);
	window.draw(min_hp_cb);

	// Set HP
	window.draw(overlord_set_hp_cb);
	window.draw(overlord_set_hp_se);
	window.draw(minions_set_hp_cb);
	window.draw(minions_set_hp_se);

	// MP
	window.draw(infinite_mp_cb);
	window.draw(mp_multipler_cb);
	window.draw(mp_multipler_se);

	// One hit kill
	window.draw(one_hit_kill_cb);

	window.draw(process_id_t);

	window.display();
}

void MainWindow::hp(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeInfiniteHP())
			cout << "<MainWindow:hp>Info: active" << endl;
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:hp>Error: Failed to activate infinite hp." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveInfiniteHP();
	}
}
void MainWindow::min_hp(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeInfiniteMinionsHP())
			cout << "<MainWindow:min_hp>Info: active" << endl;
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:min_hp>Error: Failed to activate infinite minions hp." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveInfiniteMinionsHP();
	}
}

void MainWindow::overlord_set_hp(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeOverlordSetMaxHP())
		{
			cout << "<MainWindow:set_hp>Info: active" << endl;
			if (cheat.setOverlordSetMaxHP((float)overlord_set_hp_se.getValue()))
				cout << "<MainWindow:set_hp>Info: Set max HP " << overlord_set_hp_se.getValue() << "." << endl;
		}
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:set_hp>Error: Failed to activate infinite minions hp." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveOverlordSetMaxHP();
	}
}
void MainWindow::overlord_set_hp_value(st::SliderEditHorizontal& seh, double value)
{
	if (cheat.setOverlordSetMaxHP((float)value))
		cout << "<MainWindow:set_hp_valuep>Info: Set max HP " << value << "." << endl;
	else
	{
		cout << "<MainWindow:set_hp_valuep>Error: Failed to set max hp." << endl;
	}
}

void MainWindow::minions_set_hp(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeMinionsSetMaxHP())
		{
			cout << "<MainWindow:set_hp>Info: active" << endl;
			if (cheat.setMinionsSetMaxHP((float)minions_set_hp_se.getValue()))
				cout << "<MainWindow:set_hp>Info: Set max HP " << minions_set_hp_se.getValue() << "." << endl;
		}
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:set_hp>Error: Failed to activate infinite minions hp." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveMinionsSetMaxHP();
	}
}
void MainWindow::minions_set_hp_value(st::SliderEditHorizontal& seh, double value)
{
	if (cheat.setMinionsSetMaxHP((float)value))
		cout << "<MainWindow:set_hp_valuep>Info: Set max HP " << value << "." << endl;
	else
	{
		cout << "<MainWindow:set_hp_valuep>Error: Failed to set max hp." << endl;
	}
}

// MP
void MainWindow::infinite_mp(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeInfiniteMP())
			cout << "<MainWindow:infinite_mp>Info: active" << endl;
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:infinite_mp>Error: Failed to activate infinite mp." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveInfiniteMP();
	}
}

void MainWindow::mp_multipler(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeMPMultipler())
		{
			cout << "<MainWindow:mp_multipler>Info: active" << endl;
			if (cheat.setMPMultipler((float)mp_multipler_se.getValue()))
				cout << "<MainWindow:mp_multipler>Info: Set MP spending multipler: " << mp_multipler_se.getValue() << "." << endl;
		}
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:set_hp>Error: Failed to activate mp multipler." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveMPMultipler();
	}
}
void MainWindow::mp_multipler_value(st::SliderEditHorizontal& seh, double value)
{
	if (cheat.setMPMultipler((float)value))
		cout << "<MainWindow:set_hp_valuep>Info: Set MP spending multipler: " << value << "." << endl;
	else
	{
		cout << "<MainWindow:set_hp_valuep>Error: Failed to MP spending multipler." << endl;
	}
}

// One hit kill
void MainWindow::one_hit_kill(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeOneHitKill())
			cout << "<MainWindow:one_hit_kill>Info: active" << endl;
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:one_hit_kill>Error: Failed to activate one hit kill." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveOneHitKill();
	}
}