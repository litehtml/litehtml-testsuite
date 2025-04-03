#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include "main_window.h"
#include "source_viewer.h"

namespace fs = std::filesystem;

main_window::main_window()
{
	init();
}

static inline void mk_button(Gtk::Button& btn, const std::string& label_text, const std::string& icon_name)
{
	auto icon = Gtk::make_managed<Gtk::Image>();
	icon->set_from_icon_name(icon_name);
	icon->set_icon_size(Gtk::IconSize::NORMAL);
	icon->set_expand(true);

	auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
	hbox->append(*icon);
	hbox->set_expand(false);

	btn.set_child(*hbox);
	btn.set_tooltip_text(label_text);
	btn.set_expand(false);
}

void main_window::init()
{
	m_header.set_show_title_buttons(false);

	auto title_box	= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
	auto left_box	= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 5);
	auto right_box	= Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 5);

	left_box->append(m_open_folder_button);
	m_open_folder_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_open_folder_clicked) );
	mk_button(m_open_folder_button, "Open Folder", "folder-open-symbolic");

	left_box->append(m_prev_button);
	m_prev_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_prev_button_clicked) );
	mk_button(m_prev_button, "Previous", "go-previous-symbolic");

	left_box->append(m_next_button);
	m_next_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_next_button_clicked) );
	mk_button(m_next_button, "Next", "go-next-symbolic");

	left_box->append(m_approve_button);
	m_approve_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_approve_button_clicked) );
	mk_button(m_approve_button, "Approve", "on-symbolic");

	left_box->append(m_failed_button);
	m_failed_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_failed_button_clicked) );
	mk_button(m_failed_button, "Failed", "computer-fail-symbolic");

	left_box->append(m_unsupported_button);
	m_unsupported_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_unsupported_button_clicked) );
	mk_button(m_unsupported_button, "Unsupported", "off-symbolic");

	left_box->set_hexpand(false);
	title_box->append(*left_box);

	title_box->append(m_address_bar);
	m_address_bar.set_hexpand(true);
	m_address_bar.set_halign(Gtk::Align::FILL);
	m_address_bar.set_margin_start(20);
	m_address_bar.set_margin_end(3);
	m_address_bar.signal_activate().connect(sigc::mem_fun(*this, &main_window::on_go_clicked));
	m_address_bar.property_primary_icon_name().set_value("insert-link-symbolic");

	right_box->append(m_go_button);
	m_go_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_go_clicked) );
	m_go_button.set_margin_end(32);
	mk_button(m_go_button, "Go", "media-playback-start-symbolic");

	right_box->append(m_save_button);
	m_save_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_save_button_clicked) );
	mk_button(m_save_button, "Save", "document-save-symbolic");

	right_box->append(m_view_source_button);
	m_view_source_button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::show_source) );
	mk_button(m_view_source_button, "Source", "accessories-text-editor-symbolic");

	auto win_ctls = Gtk::make_managed<Gtk::WindowControls>(Gtk::PackType::END);
	right_box->append(*win_ctls);

	title_box->append(*right_box);

	title_box->set_halign(Gtk::Align::FILL);
	title_box->set_hexpand(true);
	m_header.set_title_widget(*title_box);
	set_titlebar(m_header);

	auto main_box = Gtk::make_managed<Gtk::CenterBox>();

	main_box->set_start_widget(m_html);
	m_html.set_expand(true);
	m_html.signal_set_address().connect( sigc::mem_fun(*this, &main_window::set_address) );
	m_html.signal_update_state().connect( sigc::mem_fun(*this, &main_window::on_update_state) );
	signal_close_request().connect(sigc::mem_fun(m_html, &html_widget::on_close), false);

	auto vseparator = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::VERTICAL);

	main_box->set_center_widget(*vseparator);
	vseparator->set_hexpand(false);
	vseparator->set_vexpand(true);
	vseparator->set_margin_start(3);
	vseparator->set_margin_end(3);

	main_box->set_end_widget(*Glib::wrap(GTK_WIDGET(m_web_view)));
	gtk_widget_set_hexpand(GTK_WIDGET (m_web_view), true);
	gtk_widget_set_vexpand(GTK_WIDGET (m_web_view), true);
	gtk_widget_set_visible(GTK_WIDGET (m_web_view), true);

	set_child(*main_box);

	signal_close_request().connect(sigc::mem_fun(*this, &main_window::on_window_close), false);

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
				auto dialog = Gtk::AlertDialog::create();
				dialog->set_message(err.what());
				dialog->set_detail("An error occurred.");
				dialog->set_buttons({"OK"});
				dialog->set_modal(true);
				dialog->show();
			}
		}
	}

	std::stringstream msg;
	msg << saved_num << " test(s) saved.";

	auto dialog = Gtk::AlertDialog::create();
	dialog->set_message(msg.str());
	dialog->set_buttons({"OK"});
	dialog->set_modal(true);
	dialog->show();

	std::string test_to_open;
	if(m_current_test >= 0 && m_current_test < (int) m_tests.size())
	{
		test_to_open = m_tests[m_current_test].filename;
	}
	load_folder(m_path, test_to_open);
}

void main_window::load_folder(const std::string &str_path, const std::string& select_test)
{
	fs::path path{str_path};
	if(!exists(path))
	{
		auto dialog = Gtk::AlertDialog::create();
		dialog->set_message("The directory '" + str_path + "' doesn't exist");
		dialog->set_buttons({"OK"});
		dialog->set_modal(true);
		dialog->show();
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
	auto file_dialog = Gtk::FileDialog::create();
	file_dialog->set_title("Select folder with tests");

	// Отображение диалога выбора папки
	file_dialog->select_folder(*this, [this, file_dialog](const Glib::RefPtr<Gio::AsyncResult>& result) {
		try {
			auto folder = file_dialog->select_folder_finish(result);
			if (folder) {
				load_folder(folder->get_path());
			}
		} catch (const std::exception& ex) {
			auto dialog = Gtk::AlertDialog::create();
			dialog->set_message(ex.what());
			dialog->set_buttons({"OK"});
			dialog->set_modal(true);
			dialog->show();
		}
	});
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

void main_window::load_settings()
{
	const char* home_dir = std::getenv("HOME");
	if(home_dir == nullptr)
	{
		auto dialog = Gtk::AlertDialog::create();
		dialog->set_message("WARNING: Empty HOME environment variable");
		dialog->set_buttons({"OK"});
		dialog->set_modal(true);
		dialog->show();
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
			auto dialog = Gtk::AlertDialog::create();
			dialog->set_message(err.what());
			dialog->set_buttons({"OK"});
			dialog->set_modal(true);
			dialog->show();
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
		auto dialog = Gtk::AlertDialog::create();
		dialog->set_message("WARNING: Empty HOME environment variable");
		dialog->set_buttons({"OK"});
		dialog->set_modal(true);
		dialog->show();
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
			auto dialog = Gtk::AlertDialog::create();
			dialog->set_message(err.what());
			dialog->set_buttons({"OK"});
			dialog->set_modal(true);
			dialog->show();
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

bool main_window::on_window_close()
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
	source_viewer::show_source(*this, m_html.get_html_source());
}
