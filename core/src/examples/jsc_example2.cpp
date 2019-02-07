#include "../js/bindings_host.hpp"

int main() {
    auto host = aardvark::js::BindingsHost();
	    
    auto src = JSStringCreateWithUTF8CString(
        "let app = new DesktopApp();log('test');let window = app.createWindow(640, 480);log('Hello');log(window.width);app.run();");
    auto exception = JSValueRef();
    auto result = JSEvaluateScript(host.ctx,        // ctx,
                                   src,        // script
                                   NULL,       // thisObject,
                                   NULL,       // sourceURL,
                                   1,          // startingLineNumber,
                                   &exception  // exception
    );
    auto str = JSValueToStringCopy(host.ctx, exception, nullptr);
    std::cout << aardvark::js::jsstring_to_std(str) << std::endl;
    JSStringRelease(str);
};
