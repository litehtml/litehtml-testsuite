#include <gtkmm/application.h>
#include "main_window.h"
#include "fonts.h"

int main (int argc, char *argv[])
{
    // Prepare fonts
    prepare_fonts_for_testing();

    // Open the main window
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "litehtml.testsuite");
    main_window win;
    win.init();
    return app->run(win);
}
