#include <gtkmm/application.h>
#include "main_window.h"
#include "fonts.h"

#ifdef LIBADWAITA_AVAILABLE
#include <adwaita.h>
#endif

int main (int argc, char *argv[])
{
	// Prepare fonts
	prepare_fonts_for_testing();

#ifdef LIBADWAITA_AVAILABLE
	adw_init();
#endif

	// Open the main window
	auto app = Gtk::Application::create("litehtml.testsuite");
	return app->make_window_and_run<main_window>(argc, argv);
}
