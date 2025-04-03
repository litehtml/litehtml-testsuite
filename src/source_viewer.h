#ifndef LITEHTML_TESTSUITE_SOURCE_VIEWER_H
#define LITEHTML_TESTSUITE_SOURCE_VIEWER_H

#include <gtkmm/dialog.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>

class source_viewer : public Gtk::Window
{
	Gtk::TextView		m_text_view;
public:
	explicit source_viewer(Gtk::Window& parent);

	static void show_source(Gtk::Window& parent, const std::string& source_text);
};

#endif //LITEHTML_TESTSUITE_SOURCE_VIEWER_H
