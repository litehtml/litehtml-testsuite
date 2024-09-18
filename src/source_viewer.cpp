#include "source_viewer.h"

source_viewer::source_viewer(Gtk::Window& parent, const std::string &source) :
		Gtk::Dialog("Source Viewer", parent, Gtk::DIALOG_USE_HEADER_BAR | Gtk::DIALOG_MODAL)
{
	add_button("Close", Gtk::RESPONSE_CLOSE);
	get_vbox()->pack_start(m_scrolled_wnd);
	m_scrolled_wnd.add(m_text_view);
	m_scrolled_wnd.show();
	m_text_view.set_editable(false);
	m_text_view.set_monospace(true);
	m_text_view.get_buffer()->set_text(source);
	m_text_view.show();

	set_default_size(800, 700);
}
