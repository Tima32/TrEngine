#include "MainWindow.hpp"

#include <iostream>

using namespace std;
using namespace sf;

template <typename A, typename B>
static void VerticalLayout(const A& relatively, B& trget, float interval)
{
	trget.setPosition(relatively.getPosition().x, relatively.getPosition().y + relatively.getSize().y + interval);
}

MainWindow::MainWindow():
	event_e()
{
	string s("Fonts\\verdana.ttf");
	if (!font.loadFromFile(s))
		cout << "Error load font" << endl;

	//подчинение
	{
		subjugation_cb.setFont(font);
		subjugation_cb.setPosition(options_pos_x, options_pos_y);
		subjugation_cb.setString(L"Subjugation");
		energy_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{energy(le, state); });
	}
	
	//вост жизнь щиты энергия
	{
		hp_cb.setFont(font);
		VerticalLayout(subjugation_cb, hp_cb, options_interval);
		hp_cb.setString(L"HP");
		hp_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{hp(le, state); });

		shields_cb.setFont(font);
		VerticalLayout(hp_cb, shields_cb, options_interval);
		shields_cb.setString(L"Shields");
		shields_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{shields(le, state); });

		energy_cb.setFont(font);
		VerticalLayout(shields_cb, energy_cb, options_interval);
		energy_cb.setString(L"Energy");
		energy_cb.setChangeStateCallback(
			[this](st::CheckBox& le, st::CheckBox::State state)
			{energy(le, state); });
	}

	//добавить жизнь щиты энергия
	{
		add_hp_cb.setFont(font);
		VerticalLayout(energy_cb, add_hp_cb, options_interval);
		add_hp_cb.setString(L"Add HP");
		
		add_hp_le.setFont(font);
		add_hp_le.setPosition(options_colum_2_x, add_hp_cb.getPosition().y);
		add_hp_le.setSize(75);
		add_hp_le.setString(L"0");

		add_shields_cb.setFont(font);
		VerticalLayout(add_hp_cb, add_shields_cb, options_interval);
		add_shields_cb.setString(L"Add Shields");

		add_shields_le.setFont(font);
		add_shields_le.setPosition(options_colum_2_x, add_shields_cb.getPosition().y);
		add_shields_le.setSize(75);
		add_shields_le.setString(L"0");

		add_energy_cb.setFont(font);
		VerticalLayout(add_shields_cb, add_energy_cb, options_interval);
		add_energy_cb.setString(L"Add Energy");

		add_energy_le.setFont(font);
		add_energy_le.setPosition(options_colum_2_x, add_energy_cb.getPosition().y);
		add_energy_le.setSize(75);
		add_energy_le.setString(L"0");
	}


	//Minerals{
	set_minerals_b.setFont(font);
	set_minerals_b.setPosition(energy_cb.getPosition().x, 300);
	set_minerals_b.setText(L"Set minerals");
	set_minerals_b.setSize({ set_minerals_b.getSize().x, 17 });

	set_minerals_le.setFont(font);
	set_minerals_le.setPosition(/*set_minerals_b.getPosition().x + set_minerals_b.getSize().x + options_interval*/options_colum_2_x, set_minerals_b.getPosition().y);
	set_minerals_le.setSize(75);
	set_minerals_le.setString(L"1000000");

	set_minerals_t.setFont(font);
	set_minerals_t.setCharacterSize(11);
	set_minerals_t.setPosition(options_colum_3_x, set_minerals_b.getPosition().y);
	//}Minerals

	//Vespen{
	set_vespen_b.setFont(font);
	set_vespen_b.setPosition(set_minerals_b.getPosition().x, set_minerals_b.getPosition().y + set_minerals_b.getSize().y + options_interval);
	set_vespen_b.setText(L"Set vespen");
	set_vespen_b.setSize({ set_vespen_b.getSize().x, 17 });

	set_vespen_le.setFont(font);
	set_vespen_le.setPosition(set_minerals_le.getPosition().x, set_vespen_b.getPosition().y);
	set_vespen_le.setSize(75);
	set_vespen_le.setString(L"1000000");

	set_vespen_t.setFont(font);
	set_vespen_t.setCharacterSize(11);
	set_vespen_t.setPosition(options_colum_3_x, set_vespen_b.getPosition().y);
	//}Vespen


	process_id_t.setFont(font);
	process_id_t.setPosition(process_id_x, process_id_y);
	process_id_t.setCharacterSize(11);

	window.create(sf::VideoMode(1024, 720), L"SC2 trainer (づ｡◕‿‿◕｡)づ ꧁𝕺𝖓𝖊 𝖔𝖋 𝖆 𝖐𝖎𝖓𝖉꧂");
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

	set_vespen_le.event(event_e, focus);
	if (set_vespen_b.event(event_e, focus))
		setVespen();
	set_minerals_le.event(event_e, focus);
	if (set_minerals_b.event(event_e, focus))
		setMinerals();

	add_energy_le.event(event_e, focus);
	add_energy_cb.event(event_e, focus);
	add_shields_le.event(event_e, focus);
	add_shields_cb.event(event_e, focus);
	add_hp_le.event(event_e, focus);
	add_hp_cb.event(event_e, focus);

	energy_cb.event(event_e, focus);
	shields_cb.event(event_e, focus);
	hp_cb.event(event_e, focus);

	subjugation_cb.event(event_e, focus);

	/*set_vespen_le.event(event_e, focus);
	if (set_vespen_b.event(event_e, focus))
		setVespen();
	set_minerals_le.event(event_e, focus);
	if (set_minerals_b.event(event_e, focus))
		setMinerals();
	energy_cb.event(event_e, focus);
	subjugation_cb.event(event_e, focus);
	hp_cb.event(event_e, focus);
	shields_cb.event(event_e, focus);*/
}
void MainWindow::updateGameInfo()
{
	if (!cheat.isOpen())
	{
		cheat.open();
		process_id_t.setString(L"Process ID: ");
		set_minerals_t.setString(L"??");
		set_vespen_t.setString(L"??");

		if (process_is_open == true)
		{
			cheat.deactiveInfiniteShields();
			shields_cb.setState(st::CheckBox::State::Unchecked);
			cheat.deactiveInfiniteHP();
			hp_cb.setState(st::CheckBox::State::Unchecked);
			cheat.deactiveSubjugation();
			subjugation_cb.setState(st::CheckBox::State::Unchecked);
		}

		process_is_open = false;
	}
	else
	{
		process_id_t.setString(wstring(L"Process ID: ") + to_wstring(cheat.getID()));

		int32_t minerals;
		if (cheat.getMinerals(minerals))
		{
			set_minerals_t.setString(to_wstring(minerals));
		}
		else 
			set_minerals_t.setString(L"??");

		int32_t vespen;
		if (cheat.getVespen(vespen))
		{
			set_vespen_t.setString(to_wstring(vespen));
		}
		else
			set_vespen_t.setString(L"??");

		process_is_open = true;
	}
}
void MainWindow::draw()
{
	window.clear(Color(30, 30, 30));

	window.draw(subjugation_cb);

	window.draw(hp_cb);
	window.draw(shields_cb);
	window.draw(energy_cb);

	window.draw(add_hp_cb);
	window.draw(add_hp_le);
	window.draw(add_shields_cb);
	window.draw(add_shields_le);
	window.draw(add_energy_cb);
	window.draw(add_energy_le);

	window.draw(set_minerals_b);
	window.draw(set_minerals_le);
	window.draw(set_minerals_t);
	window.draw(set_vespen_b);
	window.draw(set_vespen_le);
	window.draw(set_vespen_t);

	window.draw(process_id_t);

	window.display();
}

void MainWindow::shields(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeInfiniteShields())
			cout << "<MainWindow:shields>Info: active" << endl;
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:shields>Error: Failed to activate infinite shields." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveInfiniteShields();
	}
}
void MainWindow::hp(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeInfiniteHP())
			cout << "<MainWindow:hps>Info: active" << endl;
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
void MainWindow::subjugation(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeSubjugation())
			cout << "<MainWindow:subjugation>Info: active" << endl;
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:subjugation>Error: Failed to activate subjugation." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveSubjugation();
	}
}
void MainWindow::energy(st::CheckBox& le, st::CheckBox::State state)
{
	if (state == st::CheckBox::State::Checked)
	{
		if (cheat.activeInfiniteEnergy())
			cout << "<MainWindow:energy>Info: active" << endl;
		else
		{
			le.setState(st::CheckBox::State::Unchecked);
			cout << "<MainWindow:energy>Error: Failed to activate infinity energy." << endl;
		}
	}
	else if (state == st::CheckBox::State::Unchecked)
	{
		cheat.deactiveInfiniteEnergy();
	}
}
void MainWindow::setMinerals()
{
	try
	{
		int32_t minerals = std::stoi(set_minerals_le.getString().toWideString());
		cheat.setMinerals(minerals);
	}
	catch(...)
	{

	}
}
void MainWindow::setVespen()
{
	try
	{
		int32_t vespen = std::stoi(set_vespen_le.getString().toWideString());
		cheat.setVespen(vespen);
	}
	catch (...)
	{

	}
}