#ifndef LITEHTML_TESTSUITE_SOURCE_VIEWER_H
#define LITEHTML_TESTSUITE_SOURCE_VIEWER_H

#include <gtkmm/dialog.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>

class source_viewer : public Gtk::Dialog
{
	Gtk::TextView		m_text_view;
	Gtk::ScrolledWindow m_scrolled_wnd;
public:
	explicit source_viewer(Gtk::Window& parent, const std::string& source);
};

#endif //LITEHTML_TESTSUITE_SOURCE_VIEWER_H
