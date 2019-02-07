#include "../js/bindings_host.hpp"

int main() {
    auto host = aardvark::js::BindingsHost();
	    
    auto src = JSStringCreateWithUTF8CString(
        "let app = new DesktopApp();log('create app');let window = app.createWindow(640, 480);log('create window');log(window.width, window.height);app.run();");
    auto exception = JSValueRef();
    auto result = JSEvaluateScript(host.ctx,   // ctx,
                                   src,        // script
                                   nullptr,       // thisObject,
                                   nullptr,       // sourceURL,
                                   1,          // startingLineNumber,
                                   &exception  // exception
    );
    auto str = JSValueToStringCopy(host.ctx, exception, nullptr);
    std::cout << aardvark::js::jsstring_to_std(str) << std::endl;
    JSStringRelease(str);
};
