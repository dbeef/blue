#include <states/Playing.hpp>
#include <Game.hpp>

std::shared_ptr <BaseState> Playing::update()
{
    return nullptr;
}

void Playing::on_entry()
{
	Game::instance().get_map().import_from_file("resources/map.bin");
	Game::instance().get_map().upload_decoration();
	Game::instance().get_map().upload_clickable();
	Game::instance().get_flora().import_from_file("resources/flora.bin");
    Game::instance().get_water().import_from_file("resources/water.bin");
	Game::instance().get_water().create_water(Game::instance().get_map());

    _intersection_job.start();
}

Playing::~Playing()
{
    _intersection_job.shutdown();
}
