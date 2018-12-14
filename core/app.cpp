

void handler(aardvark::App& app, aardvark::Command cmd) {
	AppState* state = (AppState*)app.user_data;
}

int main() {
  aardvark::App app;
  AppState state;
  app.user_data = &state;
  app.handle_event = handler;
  auto window = app.create_window(Size{550, 550});
  while (true) {
    app.handle_events();
    window.make_current();
    document.repaint();
    window.swap_now();
  }
}
