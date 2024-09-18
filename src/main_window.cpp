#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include "main_window.h"
#include "source_viewer.h"

namespace fs = std::filesystem;

main_window::main_window() :
	m_current_test(-1),
	m_in_test(false)
{
	m_web_view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
}

void main_window::init()
{
	add(m_hbox);
	m_hbox.show();

	set_titlebar(m_header);
	m_header.show();
	m_header.set_show_close_button(true);

	m_header.pack_start(m_open_folder_button);
	m_open_folder_button.show();
	m_open_folder_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_open_folder_clicked) );
	m_open_folder_button.set_image_from_icon_name("folder-open-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_open_folder_button.set_label("Open Folder");
	m_open_folder_button.set_image_position(Gtk::POS_TOP);

	m_header.pack_start(m_prev_button);
	m_prev_button.show();
	m_prev_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_prev_button_clicked) );
	m_prev_button.set_image_from_icon_name("go-previous-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_prev_button.set_label("Previous");
	m_prev_button.set_image_position(Gtk::POS_TOP);

	m_header.pack_start(m_next_button);
	m_next_button.show();
	m_next_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_next_button_clicked) );
	m_next_button.set_image_from_icon_name("go-next-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_next_button.set_label("Next");
	m_next_button.set_image_position(Gtk::POS_TOP);

	m_header.pack_start(m_approve_button);
	m_approve_button.show();
	m_approve_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_approve_button_clicked) );
	m_approve_button.set_image_from_icon_name("on-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_approve_button.set_label("Approve");
	m_approve_button.set_image_position(Gtk::POS_TOP);

	m_header.pack_start(m_failed_button);
	m_failed_button.show();
	m_failed_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_failed_button_clicked) );
	m_failed_button.set_image_from_icon_name("computer-fail-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_failed_button.set_label("Failed");
	m_failed_button.set_image_position(Gtk::POS_TOP);

	m_header.pack_start(m_unsupported_button);
	m_unsupported_button.show();
	m_unsupported_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_unsupported_button_clicked) );
	m_unsupported_button.set_image_from_icon_name("off-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_unsupported_button.set_label("Unsupported");
	m_unsupported_button.set_image_position(Gtk::POS_TOP);

	m_header.set_custom_title(m_address_bar);
	m_address_bar.set_hexpand_set(true);
	m_address_bar.set_hexpand();
	m_address_bar.property_primary_icon_name().set_value("insert-link-symbolic");
	m_address_bar.set_margin_start(32);
	m_address_bar.show();
	m_address_bar.add_events(Gdk::KEY_PRESS_MASK);
	m_address_bar.signal_key_press_event().connect( sigc::mem_fun(*this, &main_window::on_address_key_press), false ); // NOLINT(*-unused-return-value)

	m_header.pack_end(m_view_source_button);
	m_view_source_button.show();
	m_view_source_button.set_image_from_icon_name("accessories-text-editor-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_view_source_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::show_source) );
	m_view_source_button.set_label("Source");
	m_view_source_button.set_image_position(Gtk::POS_TOP);

	m_header.pack_end(m_save_button);
	m_save_button.show();
	m_save_button.set_image_from_icon_name("document-save-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_save_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_save_button_clicked) );
	m_save_button.set_label("Save");
	m_save_button.set_image_position(Gtk::POS_TOP);

	m_header.pack_end(m_go_button);
	m_go_button.show();
	m_go_button.set_image_from_icon_name("media-playback-start-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
	m_go_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_go_clicked) );
	m_go_button.set_margin_end(32);
	m_go_button.set_label("Go");

	m_hbox.pack_start(m_scrolled_wnd, Gtk::PACK_EXPAND_WIDGET);
	m_scrolled_wnd.show();
	m_scrolled_wnd.add(m_html);
	m_html.show();
	m_html.signal_set_address().connect( sigc::mem_fun(*this, &main_window::set_address) );
	m_html.signal_update_state().connect( sigc::mem_fun(*this, &main_window::on_update_state) );
	m_html.signal_get_viewport().connect( sigc::mem_fun(*this, &main_window::on_get_viewport) );
	m_html.signal_scroll_to().connect( sigc::mem_fun(*this, &main_window::on_scroll_to) );

	m_hbox.pack_start(m_vseparator, Gtk::PACK_SHRINK);
	m_vseparator.show();
	m_vseparator.set_margin_left(3);
	m_vseparator.set_margin_right(3);

	m_hbox.pack_start(m_webkit_box, Gtk::PACK_EXPAND_WIDGET);
	gtk_container_add (GTK_CONTAINER (m_webkit_box.gobj()), GTK_WIDGET (m_web_view));
	gtk_widget_show(GTK_WIDGET (m_web_view));
	m_webkit_box.show();

	signal_delete_event().connect(sigc::mem_fun(m_html, &html_widget::on_close), false);
	signal_delete_event().connect(sigc::mem_fun(*this, &main_window::on_window_close), false);

	set_default_size(1280, 720);

	load_settings();
	if(!m_path.empty())
	{
		load_folder(m_path, m_last_test);
	}

	update_tests_buttons();
}


void main_window::on_update_state(uint32_t /*state*/)
{
	update_tests_buttons();
}

void main_window::on_scroll_to(int x, int y)
{
	m_scrolled_wnd.get_vadjustment()->set_value(m_scrolled_wnd.get_vadjustment()->get_lower() + y);
	m_scrolled_wnd.get_hadjustment()->set_value(m_scrolled_wnd.get_hadjustment()->get_lower() + x);
}

void main_window::on_next_button_clicked()
{
	load_test(m_current_test + 1);
}

void main_window::on_prev_button_clicked()
{
	load_test(m_current_test - 1);
}

void main_window::on_approve_button_clicked()
{
	if(m_current_test >= 0 && m_current_test < (int) m_tests.size())
	{
		m_tests[m_current_test].status = test_status_approve;
		load_test(m_current_test + 1);
	}
}

void main_window::on_unsupported_button_clicked()
{
	if(m_current_test >= 0 && m_current_test < (int) m_tests.size())
	{
		m_tests[m_current_test].status = test_status_unsupported;
		load_test(m_current_test + 1);
	}
}

void main_window::on_save_button_clicked()
{
	int saved_num = 0;
	for(const auto& test : m_tests)
	{
		auto base_path = fs::path(test.filename).parent_path();
		auto filename = fs::path(test.filename).filename();
		fs::path new_filename;
		if(filename.string().substr(0, 2) == "--")
		{
			if(test.status == test_status_approve)
			{
				filename = filename.string().substr(2);
			} if(test.status == test_status_failed)
			{
				filename = filename.string().substr(1);
			}
		} else
		{
			if(test.status == test_status_approve)
			{
				filename = filename.string().substr(1);
			} if(test.status == test_status_unsupported)
			{
				filename = "--" + filename.string().substr(1);
			}
		}
		new_filename = base_path / filename;
		if(new_filename != test.filename)
		{
			try
			{
				fs::rename(test.filename, new_filename);
				saved_num++;
			} catch (const std::filesystem::filesystem_error& err)
			{
				Gtk::MessageDialog dlg(*this, err.what(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
				dlg.run();
			}
		}
	}

	std::stringstream msg;
	msg << saved_num << " test(s) saved.";

	Gtk::MessageDialog dlg(*this, msg.str(), false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
	dlg.run();

	std::string test_to_open;
	if(m_current_test >= 0 && m_current_test < (int) m_tests.size())
	{
		test_to_open = m_tests[m_current_test].filename;
	}
	load_folder(m_path, test_to_open);
}

litehtml::position main_window::on_get_viewport()
{
	litehtml::position viewport;
	viewport.x = (int) (m_scrolled_wnd.get_hadjustment()->get_value() - m_scrolled_wnd.get_hadjustment()->get_lower());
	viewport.y = (int) (m_scrolled_wnd.get_vadjustment()->get_value() - m_scrolled_wnd.get_vadjustment()->get_lower());
	viewport.width = (int) (m_scrolled_wnd.get_hadjustment()->get_page_size());
	viewport.height = (int) (m_scrolled_wnd.get_vadjustment()->get_page_size());
	return viewport;
}

void main_window::load_folder(const std::string &str_path, const std::string& select_test)
{
	fs::path path{str_path};
	if(!exists(path))
	{
		Gtk::MessageDialog msg("The directory '" + str_path + "' doesn't exist", false, Gtk::MESSAGE_ERROR);
		msg.run();
		return;
	}
	m_path = str_path;
	m_tests.clear();
	for(const auto &entry : fs::directory_iterator(path))
	{
		if(entry.is_regular_file() &&
			(entry.path().extension() == ".html" || entry.path().extension() == ".htm") &&
			entry.path().filename().string().front() == '-')
		{
			test_status status;
			if(entry.path().filename().string().substr(0, 2) == "--")
			{
				status = test_status_unsupported;
			} else
			{
				status = test_status_failed;
			}
			m_tests.emplace_back(entry.path().string(), status);
		}
	}
	if(!m_tests.empty())
	{
		std::sort(m_tests.begin(), m_tests.end(), [this](const auto &a, const auto &b)
			{
				return fs::path(a.filename).filename().string() < fs::path(b.filename).filename().string();
			});
		int index = 0;
		if(!select_test.empty())
		{
			for(int i = 0; i < (int) m_tests.size(); i++)
			{
				if(m_tests[i].filename == select_test)
				{
					index = i;
					break;
				}
			}
		}
		if(m_file_to_open.empty())
		{
			load_test(index);
		} else
		{
			m_current_test = index;
			m_last_test = m_tests[index].filename;
			open_url(m_file_to_open);
			select_file(m_file_to_open);
		}
	}
}

void main_window::load_test(int index)
{
	if(index >= 0 && index < (int) m_tests.size())
	{
		m_current_test = index;
		open_url(m_tests[index].filename);
		m_last_test = m_tests[index].filename;
		m_in_test = true;
		m_file_to_open = "";
	}
	update_tests_buttons();
}

void main_window::update_tests_buttons()
{
	if(m_current_test < 0 || m_current_test >= (int) m_tests.size())
	{
		m_prev_button.set_sensitive(false);
		m_next_button.set_sensitive(false);
		m_approve_button.set_sensitive(false);
		m_unsupported_button.set_sensitive(false);
		m_failed_button.set_sensitive(false);
	} else
	{
		m_prev_button.set_sensitive(m_current_test > 0);
		m_next_button.set_sensitive(m_current_test < (int) m_tests.size() - 1);
		m_approve_button.set_sensitive(m_tests[m_current_test].status != test_status_approve && m_in_test);
		m_unsupported_button.set_sensitive(m_tests[m_current_test].status != test_status_unsupported && m_in_test);
		m_failed_button.set_sensitive(m_tests[m_current_test].status != test_status_failed && m_in_test);
	}

}

void main_window::open_url(const std::string &url)
{
	if (!url.empty())
	{
		auto url_to_open = url;
		if(url_to_open[0] == '/')
		{
			url_to_open = "file://" + url_to_open;
		}
		m_html.open_url(url_to_open);
		webkit_web_view_load_uri(WEBKIT_WEB_VIEW (m_web_view), url_to_open.c_str());
	}
}

void main_window::on_open_folder_clicked()
{
	Gtk::FileChooserDialog dlg(*this, "Select folder with tests", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
	//Add response buttons the dialog:
	dlg.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dlg.add_button("Select", Gtk::RESPONSE_OK);
	int result = dlg.run();
	if (result == Gtk::RESPONSE_OK)
	{
		load_folder(dlg.get_file()->get_path());
	}
}

void main_window::on_go_clicked()
{
	litehtml::string url = m_address_bar.get_text();
	open_url(url);

	if(url.substr(0, 7) == "file://")
	{
		url = url.substr(7);
	}

	select_file(url);
}

void main_window::select_file(const std::string &url)
{
	auto res = std::find_if(m_tests.begin(), m_tests.end(), [this, url](const auto& v) { return v.filename == url; });
	if(res != std::end(m_tests))
	{
		m_current_test = (int) std::distance(m_tests.begin(), res);
		m_last_test = url;
		m_in_test = true;
		m_file_to_open = "";
	} else
	{
		m_in_test = false;
		m_file_to_open = url;
	}
	update_tests_buttons();
}

bool main_window::on_address_key_press(GdkEventKey *event)
{
	if(event->keyval == GDK_KEY_Return)
	{
		m_address_bar.select_region(0, -1);
		on_go_clicked();
		return true;
	}

	return false;
}

void main_window::load_settings()
{
	const char* home_dir = std::getenv("HOME");
	if(home_dir == nullptr)
	{
		Gtk::MessageDialog dlg(*this, "WARNING: Empty HOME environment variable", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
		dlg.run();
		return;
	}
	fs::path cfg_file(fs::path{home_dir} / ".config/litehtml");
	if(!exists(cfg_file))
	{
		try
		{
			fs::create_directories(cfg_file);
		} catch (const fs::filesystem_error& err)
		{
			Gtk::MessageDialog dlg(*this, err.what(), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
			dlg.run();
			return;
		}
	}
	cfg_file = cfg_file / "test-suite.ini";
	if(exists(cfg_file))
	{
		std::ifstream infile(cfg_file);
		if(infile.is_open())
		{
			std::getline(infile, m_path);
			std::getline(infile, m_last_test);
			std::getline(infile, m_file_to_open);
		}
	}
}

void main_window::save_settings()
{
	const char* home_dir = std::getenv("HOME");
	if(home_dir == nullptr)
	{
		Gtk::MessageDialog dlg(*this, "WARNING: Empty HOME environment variable", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
		dlg.run();
		return;
	}
	fs::path cfg_file(fs::path{home_dir} / ".config/litehtml");
	if(!exists(cfg_file))
	{
		try
		{
			fs::create_directories(cfg_file);
		} catch (const fs::filesystem_error& err)
		{
			Gtk::MessageDialog dlg(*this, err.what(), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
			dlg.run();
			return;
		}
	}
	cfg_file = cfg_file / "test-suite.ini";

	std::ofstream outfile(cfg_file, std::ios::out | std::ios::trunc);
	if(outfile.is_open())
	{
		outfile << m_path << std::endl << m_last_test << std::endl << m_file_to_open << std::endl;
	}
}

bool main_window::on_window_close(GdkEventAny* /*event*/)
{
	save_settings();
	return false;
}

void main_window::on_failed_button_clicked()
{
	if(m_current_test >= 0 && m_current_test < (int) m_tests.size())
	{
		m_tests[m_current_test].status = test_status_failed;
		load_test(m_current_test + 1);
	}
}

void main_window::show_source()
{
	source_viewer dlg(*this, m_html.get_html_source());
	dlg.run();
}
