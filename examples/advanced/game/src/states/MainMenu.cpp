#include <states/MainMenu.hpp>
#include <states/Playing.hpp>

#include <Resources.hpp>

std::shared_ptr<BaseState> MainMenu::update()
{
	if (_start_requested.load())
	{
		return std::make_shared<Playing>();
	}
	else
	{
		return nullptr;
	}
}

void MainMenu::on_entry()
{
	blue::Context::logger().info("Entering main menu.");

	Button::init();

	auto& res = Resources::instance();

	{
		PlacingRules rules;
		rules.width_in_screen_percent = 20;
		rules.height_in_screen_percent = 7;
		rules.x_in_screen_percent_from_left = 50;
		rules.y_in_screen_percent_from_up = 35;
		controls.start.update_placing_rules(rules);
		controls.start.create(res.gui_environment.environment,
			res.textures.start_clicked,
			res.textures.start);
	}
	{
		PlacingRules rules;
		rules.width_in_screen_percent = 20;
		rules.height_in_screen_percent = 7;
		rules.x_in_screen_percent_from_left = 50;
		rules.y_in_screen_percent_from_up = 50;
		controls.options.update_placing_rules(rules);
		controls.options.create(res.gui_environment.environment,
			res.textures.options_clicked,
			res.textures.options);
	}
	{
		PlacingRules rules;
		rules.width_in_screen_percent = 20;
		rules.height_in_screen_percent = 7;
		rules.x_in_screen_percent_from_left = 50;
		rules.y_in_screen_percent_from_up = 65;
		controls.exit.update_placing_rules(rules);
		controls.exit.create(res.gui_environment.environment,
			res.textures.exit_clicked,
			res.textures.exit);
	}

	MouseKeyCallback down_callback;
	down_callback.key_type = SDL_BUTTON_LEFT;
	down_callback.action = SDL_MOUSEBUTTONDOWN;
	down_callback.callback = [this](double x, double y)
	{

		Button* start = &controls.start;
		Button* end = &controls.exit;

		for (Button* current = start; current <= end; current++)
		{
			bool clicked = current->collision(x, y);
			if (clicked) current->set_clicked(true);
		}
	};

	MouseKeyCallback release_callback;
	release_callback.key_type = SDL_BUTTON_LEFT;
	release_callback.action = SDL_MOUSEBUTTONUP;
	release_callback.callback = [this](double x, double y)
	{

		Button* start = &controls.start;
		Button* end = &controls.exit;

		for (Button* current = start; current <= end; current++)
		{
			if (current->is_clicked())
			{
				if (current == &controls.start)
				{
					blue::Context::logger().info("Start requested");
					_start_requested.store(true);
				}
				if (current == &controls.options)
				{
					blue::Context::logger().info("Options requested");
					_options_requested.store(true);
				}
				if (current == &controls.exit)
				{
					blue::Context::logger().info("Exit requested");
					_exit_requested.store(true);
				}
			}
			current->set_clicked(false);
		}
	};

	blue::Context::input().registerMouseKeyCallback({ down_callback });
	blue::Context::input().registerMouseKeyCallback({ release_callback });
}

MainMenu::~MainMenu()
{
	Button* start = &controls.start;
	Button* end = &controls.exit;

	for (Button* current = start; current <= end; current++)
	{
		current->dispose();
	}

	blue::Context::input().mouseButtonCommands.clear();
}
