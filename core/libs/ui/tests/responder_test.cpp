#include <Catch2/catch.hpp>
#include <aardvark/pointer_events/responder.hpp>
#include <vector>

using namespace aardvark;

TEST_CASE("ResponderReconciler", "[responder]" ) {
    class TestResponder : public Responder {
      public:
        enum class Call { Start, Update, End, Terminate };
        std::vector<Call> calls = {};
        void start() override { calls.push_back(Call::Start); };
        void update() override { calls.push_back(Call::Update); };
        void end(bool is_terminated) override {
            calls.push_back(is_terminated ? Call::Terminate : Call::End);
        };
    };

    class TestElement : public Element {
      public:
        TestElement(ResponderMode mode)
            : Element(false, false), mode(mode) {
            responder = TestResponder();
        };
        ResponderMode mode;
        TestResponder responder;
        ResponderMode get_responder_mode() override { return mode; };
        Responder* get_responder() override { return &responder; };
    };

    SECTION("PassThrough") {
        auto reconciler = ResponderReconciler();
        auto a = std::make_shared<TestElement>(ResponderMode::PassThrough);
        auto b = std::make_shared<TestElement>(ResponderMode::PassThrough);

        auto hit_elements = std::vector<std::shared_ptr<Element>>{b, a};
        reconciler.reconcile(hit_elements, a.get());
        REQUIRE(a->responder.calls[0] == TestResponder::Call::Start);
        REQUIRE(b->responder.calls[0] == TestResponder::Call::Start);
        a->responder.calls.clear();
        b->responder.calls.clear();

        hit_elements = std::vector<std::shared_ptr<Element>>{a};
        reconciler.reconcile(hit_elements, a.get());
        REQUIRE(a->responder.calls[0] == TestResponder::Call::Update);
        REQUIRE(b->responder.calls[0] == TestResponder::Call::End);
        a->responder.calls.clear();
        b->responder.calls.clear();

        hit_elements = std::vector<std::shared_ptr<Element>>{};
        reconciler.reconcile(hit_elements, a.get());
        REQUIRE(a->responder.calls[0] == TestResponder::Call::End);
        REQUIRE(b->responder.calls.size() == 0);
    }

    SECTION("Absorb") {
        auto reconciler = ResponderReconciler();
        auto a = std::make_shared<TestElement>(ResponderMode::Absorb);
        auto b = std::make_shared<TestElement>(ResponderMode::PassThrough);

        auto hit_elements = std::vector<std::shared_ptr<Element>>{b, a};
        reconciler.reconcile(hit_elements, a.get());
        REQUIRE(a->responder.calls[0] == TestResponder::Call::Start);
        REQUIRE(b->responder.calls.size() == 0);
    }

    SECTION("PassToParent") {
        auto reconciler = ResponderReconciler();
        auto a = std::make_shared<TestElement>(ResponderMode::PassToParent);
        auto b = std::make_shared<TestElement>(ResponderMode::PassThrough);
        auto a_p = std::make_shared<TestElement>(ResponderMode::PassThrough);
        a->parent = a_p.get();

        auto hit_elements = std::vector<std::shared_ptr<Element>>{a_p, b, a};
        reconciler.reconcile(hit_elements, a.get());
        REQUIRE(a->responder.calls[0] == TestResponder::Call::Start);
        REQUIRE(b->responder.calls.size() == 0);
        REQUIRE(a_p->responder.calls[0] == TestResponder::Call::Start);
    }

    SECTION("Capture") {
        auto reconciler = ResponderReconciler();
        auto a = std::make_shared<TestElement>(ResponderMode::PassThrough);
        auto b = std::make_shared<TestElement>(ResponderMode::PassThrough);
        auto root = std::make_shared<TestElement>(ResponderMode::PassThrough);
        a->parent = root.get();
        b->parent = root.get();

        // no capture
        auto hit_elements = std::vector<std::shared_ptr<Element>>{b, a};
        reconciler.reconcile(hit_elements, root.get());
        REQUIRE(a->responder.calls[0] == TestResponder::Call::Start);
        REQUIRE(b->responder.calls[0] == TestResponder::Call::Start);
        a->responder.calls.clear();
        b->responder.calls.clear();

        // "b" captures and terminates "a"
        b->mode = ResponderMode::Capture;
        reconciler.reconcile(hit_elements, root.get());
        REQUIRE(a->responder.calls[0] == TestResponder::Call::Terminate);
        REQUIRE(b->responder.calls[0] == TestResponder::Call::Update);
        a->responder.calls.clear();
        b->responder.calls.clear();
  
        // "b" continues to capture while not being hit
        hit_elements = std::vector<std::shared_ptr<Element>>{a};
        reconciler.reconcile(hit_elements, root.get());
        REQUIRE(a->responder.calls.size() == 0);
        REQUIRE(b->responder.calls[0] == TestResponder::Call::Update);
        a->responder.calls.clear();
        b->responder.calls.clear();

        // "b" is removed from document
        b->parent = nullptr;
        reconciler.reconcile(hit_elements, root.get());
        REQUIRE(b->responder.calls[0] == TestResponder::Call::End);
        REQUIRE(a->responder.calls[0] == TestResponder::Call::Start);
    }
}
