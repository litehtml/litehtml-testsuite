#include <memory>
#include <gtkmm/eventcontrollerkey.h>
#include "source_viewer.h"

source_viewer::source_viewer(Gtk::Window& parent)
{
	// set_destroy_with_parent(true);
	set_transient_for(parent);
	set_title("Source Viewer");
	set_modal(true);

	auto close_btn = Gtk::make_managed<Gtk::Button>("Close");
	close_btn->signal_clicked().connect([this]() { this->hide(); });

	auto header_bar = Gtk::make_managed<Gtk::HeaderBar>();
	header_bar->pack_end(*close_btn);
	header_bar->set_show_title_buttons(false);

	set_titlebar(*header_bar);

	m_text_view.set_editable(false);
	m_text_view.set_monospace(true);

	auto scroll = Gtk::make_managed<Gtk::ScrolledWindow>();

	scroll->set_child(m_text_view);
	scroll->set_expand(true);

	set_child(*scroll);

	set_default_size(800, 700);

	auto controller = Gtk::EventControllerKey::create();
	controller->signal_key_pressed().connect(
		[this](guint keyval, guint, Gdk::ModifierType) -> bool
		{
			if(keyval == GDK_KEY_Escape)
			{
				this->hide();
				return true;
			}
			return false;
		}, false);
	add_controller(controller);

}

void source_viewer::show_source(Gtk::Window& parent, const std::string &source_text)
{
	auto dlg = std::make_shared<source_viewer>(parent);

	dlg->m_text_view.get_buffer()->set_text(source_text);
	dlg->signal_close_request().connect([dlg]() -> bool {
				return false;
			}, false);
	dlg->show();
}
