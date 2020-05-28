#pragma once

#include <functional>
#include <memory>
#include <nod/nod.hpp>
#include <unordered_map>
#include <vector>

#include "base_types.hpp"
#include "element.hpp"

namespace aardvark {

class Element;

template <typename T>
class ElementObserver;

template <typename T>
class ElementObserverConnection : public Connection {
    friend ElementObserver<T>;

  public:
    ElementObserverConnection(
        std::weak_ptr<ElementObserver<T>> observer,
        std::weak_ptr<Element> element,
        nod::connection connection)
        : observer(observer),
          element(std::move(element)),
          connection(std::move(connection)){};

    void disconnect() override {
        if (auto observer_sptr = observer.lock()) {
            observer_sptr->disconnect(this);
        }
    }

  private:
    std::weak_ptr<ElementObserver<T>> observer;
    std::weak_ptr<Element> element;
    nod::connection connection;
};

// Allows to observe changes of some property of elements
template <typename T>
class ElementObserver
    : public std::enable_shared_from_this<ElementObserver<T>> {
    friend ElementObserverConnection<T>;

  public:
    struct ElementObserverEntry {
        ElementObserverEntry(T prev_prop_value)
            : prev_prop_value(prev_prop_value){};

        nod::signal<void(T)> signal = nod::signal<void(T)>();
        T prev_prop_value;
    };

    ElementObserver(std::function<T(std::shared_ptr<Element>)> get_prop_value)
        : get_prop_value(get_prop_value){};

    // Start observing element
    std::shared_ptr<Connection> observe(
        std::shared_ptr<Element> element, std::function<void(T)> handler) {
        auto it = observed_elements.find(element);
        if (it == observed_elements.end()) {
            it = observed_elements.emplace(
                element, ElementObserverEntry(get_prop_value(element))).first;
        }
        return std::make_shared<ElementObserverConnection<T>>(
            this->weak_from_this(),             // observer
            element,                            // element
            it->second.signal.connect(handler)  // connection
        );
    }

    // Stop observing element (called when element is removed from the document)
    void unobserve(std::shared_ptr<Element> element) {
        auto it = observed_elements.find(element);
        if (it != observed_elements.end()) observed_elements.remove(it);
    }

    // Marks that value of the observed property of the element might change
    void trigger_element(std::shared_ptr<Element> element) {
        auto it = observed_elements.find(element);
        if (it != observed_elements.end()) triggered_elements.insert(element);
    }

    // Checks values of the observed properties and call handlers if they are
    // changed
    void check_triggered_elements() {
        for (auto& elem : triggered_elements) check_element(elem);
        triggered_elements.clear();
    }

    // Re-check values of the observed properties of all observed elements
    void check_all_elements() {
        for (auto& it : observed_elements) triggered_elements.insert(it.first);
        check_triggered_elements();
    }

  private:
    std::function<T(std::shared_ptr<Element>)> get_prop_value;
    std::unordered_map<std::shared_ptr<Element>, ElementObserverEntry>
        observed_elements;
    std::unordered_set<std::shared_ptr<Element>> triggered_elements;

    void disconnect(ElementObserverConnection<T>* connection) {
        auto element = connection->element.lock();
        // Element can be destroyed only if it is not observed anymore
        if (element == nullptr) return;
        connection->connection.disconnect();
        auto it = observed_elements.find(element);
        // Remove the element if this was the last slot
        if (it != observed_elements.end() &&
            it->second.signal.slot_count() == 0) {
            observed_elements.erase(it);
        }
    }

    void check_element(std::shared_ptr<Element> element) {
        auto it = observed_elements.find(element);
        // Do not call handlers if the element stopped being observed
        if (it == observed_elements.end()) return;
        auto& entry = it->second;
        auto prop_value = get_prop_value(element);
        if (entry.prev_prop_value != prop_value) {
            entry.prev_prop_value = prop_value;
            entry.signal(prop_value);
        }
    }
};

}  // namespace aardvark
