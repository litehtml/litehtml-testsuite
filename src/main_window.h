#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm/window.h>
#include "html_widget.h"
#include <webkit2/webkit2.h>

enum test_status
{
	test_status_approve,
	test_status_failed,
	test_status_unsupported,
};

struct test_file
{
	std::string filename;
	test_status status;

	test_file(std::string _filename, test_status _status) : filename(_filename), status(_status) {}
};

class main_window : public Gtk::Window
{
	std::vector<test_file> m_tests;
	int m_current_test;
	std::string m_path;
	std::string m_last_test;
	bool m_in_test;
	std::string m_file_to_open;
public:
	main_window();

	void init();
	void set_address(const std::string& text)
	{
		m_address_bar.set_text(text);
	}

	Gtk::ScrolledWindow* get_scrolled() { return &m_scrolled_wnd; }

private:
	void open_url(const std::string& url);
	void on_open_folder_clicked();
	void on_go_clicked();
	bool on_address_key_press(GdkEventKey* event);
	void on_update_state(uint32_t state);
	void on_scroll_to(int x, int y);
	void on_next_button_clicked();
	void on_prev_button_clicked();
	void on_approve_button_clicked();
	void on_unsupported_button_clicked();
	void on_failed_button_clicked();
	void on_save_button_clicked();
	bool on_window_close(GdkEventAny* event);

	litehtml::position on_get_viewport();
	void load_folder(const std::string& path, const std::string& select_test = "");
	void load_test(int index);
	void load_settings();
	void save_settings();
	void update_tests_buttons();
	void show_source();
protected:
	html_widget			m_html;
	Gtk::Entry			m_address_bar;
	Gtk::HBox			m_hbox;
	Gtk::HBox			m_webkit_box;
	Gtk::VSeparator		m_vseparator;
	Gtk::HeaderBar		m_header;
	Gtk::ScrolledWindow m_scrolled_wnd;
	WebKitWebView*		m_web_view;

	Gtk::Button			m_open_folder_button;
	Gtk::Button			m_go_button;
	Gtk::Button			m_next_button;
	Gtk::Button			m_prev_button;
	Gtk::Button			m_approve_button;
	Gtk::Button			m_unsupported_button;
	Gtk::Button			m_failed_button;
	Gtk::Button			m_save_button;
	Gtk::Button			m_view_source_button;

	void select_file(const std::string &url);
};

#endif //MAIN_WINDOW_H
