#pragma once

#include <gtkmm/drawingarea.h>
#include "html_host.h"
#include "web_page.h"
#include "http_requests_pool.h"
#include "web_history.h"

enum page_state
{
	page_state_has_back = 0x01,
	page_state_has_forward = 0x02,
	page_state_downloading = 0x04,
};

class html_widget :		public Gtk::DrawingArea,
						public litebrowser::html_host_interface
{
	int m_rendered_width;
	std::mutex m_page_mutex;
	std::shared_ptr<litebrowser::web_page> m_current_page;
	std::shared_ptr<litebrowser::web_page> m_next_page;
	web_history m_history;

public:
	explicit html_widget();
	~html_widget() override;

	void open_page(const litehtml::string& url, const litehtml::string& hash) override;
	void update_cursor() override;
	void on_parent_size_allocate(Gtk::Allocation allocation);
    void on_size_allocate(Gtk::Allocation& allocation) override;
	void redraw() override;
	void redraw_rect(int x, int y, int width, int height) override;
	int get_render_width() override;
	void on_page_loaded(uint64_t web_page_id) override;
	void open_url(const std::string& url) override;
	void render() override;
	void queue_action(litebrowser::html_host_interface::q_action action) override;
	void go_forward();
	void go_back();
	uint32_t get_state();
	void stop_download();
	void reload();

	std::string get_html_source();
    long render_measure(int number);
    long draw_measure(int number);
	void show_hash(const std::string& hash);
	bool on_close(GdkEventAny* event);

protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
	void scroll_to(int x, int y) override;

	void get_client_rect(litehtml::position& client) const override;
	void set_caption(const char* caption) override;

	bool on_button_press_event(GdkEventButton* event) override;
	bool on_button_release_event(GdkEventButton* event) override;
	bool on_motion_notify_event(GdkEventMotion* event) override;

	void on_parent_changed(Gtk::Widget* previous_parent) override;

private:
	std::shared_ptr<litebrowser::web_page> current_page()
	{
		std::lock_guard<std::mutex> lock(m_page_mutex);
		return m_current_page;
	}
	Gtk::Allocation get_parent_allocation();
	static int action_update_state(void* data);
	static int action_render(void* data);
	static int action_redraw(void* data);

public:
	// Signals types
	typedef sigc::signal<void, std::string> sig_set_address_t;
	typedef sigc::signal<void, uint32_t> sig_update_state_t;
	typedef sigc::signal<Glib::RefPtr<Gtk::Adjustment>> sig_get_adjustment_t;
	typedef sigc::signal<void, int, int> sig_scroll_to_t;
	typedef sigc::signal<litehtml::position> sig_get_view_port_t;

	sig_set_address_t signal_set_address() { return  m_sig_set_address; }
	sig_update_state_t signal_update_state() { return  m_sig_update_state; }
	sig_scroll_to_t signal_scroll_to() { return  m_sig_scroll_to; }
	sig_get_view_port_t signal_get_viewport() { return m_sig_get_viewport; }

private:
	sig_update_state_t m_sig_update_state;
	sig_set_address_t m_sig_set_address;
	sig_get_adjustment_t m_sig_get_v_adjustment;
	sig_get_adjustment_t m_sig_get_h_adjustment;
	sig_scroll_to_t m_sig_scroll_to;
	sig_get_view_port_t m_sig_get_viewport;
};
