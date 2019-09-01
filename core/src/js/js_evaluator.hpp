#pragma once

class JsEvaluator {
  public:
    Evaluator();

    JSValueRef eval_script(const std::string& src,
                           const std::string& source_url = "");

    JSValueRef eval_file(const std::string& filepath);

    // eval_url(const std::string& url);

  private:
    std::weak_ptr<JSGlobalContextWrapper> ctx_wptr;
    std::string relative_path;
    std::unordered_map<std::string, std::string> source_maps;
};

JSValueRef JSEvaluator::eval_script(const std::string& source,
                                    const std::string& source_url = "") {
    auto ctx_sptr = ctx_wptr.lock();
    if (!ctx_sptr) return;

    auto js_source = JSStringCreateWithUTF8CString(source.c_str());
    auto js_source_url =
        source_url.empty() ? nullptr
                           : JSStringCreateWithUTF8CString(source_url.c_str());
    auto exception = JSValueRef();
    auto result = JSEvaluateScript(ctx_sptr->ctx,  // ctx,
                                   js_source,      // script
                                   nullptr,        // thisObject,
                                   js_source_url,  // sourceURL,
                                   1,              // startingLineNumber,
                                   &exception      // exception
    );
    JSStringRelease(js_source);
    if (js_source_url != nullptr) JSStringRelease(js_source_url);
    if (exception != nullptr) handle_exception(ex);
    return result;
}

JSValueRef JSEvaluator::eval_file(const std::string& filepath) {
    auto abs_filepath = is_relative_path(filepath)
                            ? fs::current_path().append(filepath)
                            : filepath;

    // check if file exist

    auto source = aardvark::utils::read_text_file(abs_filepath);
    source_maps[source_url] = get_source_map(source, abs_filepath);
    return eval_script(source, source_url);
}
