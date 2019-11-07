#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace aardvark {

class Connection {
    virtual void disconnect(){};
}

class ElementObserver;

class ElementObserverConnection : public Connection {
  public:
    ElementObserverConnection(std::weak_ptr<BoundingBoxObserver> observer,
                              std::weak_ptr<Element> element,
                              nod::connection connection)
        : document(document), element(element), connection(connection){};

    void disconnect override() {
        if (auto observer_sptr = observer.lock()) {
            observer_sptr->disconnect(this);
        }
    }

  private:
    std::weak_ptr<ElementObserver> observer;
    std::weak_ptr<Element> element;
    nod::connection connection;
};

// Allows to observe changes of some property of elements
template <typename T>
class ElementObserver : public std::enable_shared_from_this<ElementObserver> {
  public:
    struct ElementObserverEntry {
        nod::signal<void(T)> signal;
        T prev_prop_value;
    };

    ElementObserver(std::function<T(std::shared_ptr<Element>)> get_prop_value)
        : get_prop_value(get_prop_value){};

    ElementObserverConnection observe(std::shared_ptr<Element> element,
                                      std::function<void(T)> handler) {
        if (!map_contains(observed_elements, element)) {
            observed_elements[element] = ElementObserverEntry{
                nod::signal<void(T)>(),  // signal
                get_prop_value(element)  // prev_prop_value
            };
        }
        return ElementObserverConnection(
            weak_from_this(),                                   // observer
            element,                                            // element
            observed_elements[element].signal.connect(handler)  // connection
        );
    }

    void disconnect(const BoundingBoxObserverConnection& connection) {
        auto element = connection->element.lock();
        // If element is destroyed, then observing is already stopped
        if (element != nullptr) return;
        connection->connection.disconnect();
        auto it = observed_elements.find(element);
        // If last slot disconnects, then remove the entry
        if (it != observed_elements.end() && it->second.slot_count() == 0) {
            observed_elements.remove(it);
        }
    }

    void unobserve_element(std::shared_ptr<Element> element) {
        auto it = observed_elements.find(element);
        if (it != observed_elements.end()) {
            observed_elements.remove(it);
        }
    }

    void trigger_element(std::shared_ptr<Element> element) {
        if (map_contains(observed_elements, element)) {
            triggered_elements.push_back(element);
        }
    }

    void call_triggered_handlers() {
        for (auto elem : triggered_elements) call_element_handlers(elem);
        triggered_elements.clear();
    }

    void call_all_handlers() {
        for (auto it : observed_elements) {
            triggered_elements.push_back(it->first);
        }
        call_handlers();
    }

  private:
    std::function<T(std::shared_ptr<Element>)> get_prop_value;

    std::unordered_map<std::shared_ptr<Element>, ElementObserverEntry>
        observed_elements;

    std::unordered_set<std::weak_ptr<Element>> triggered_elements;

    void call_element_handlers(std::weak_ptr<Element> element_wptr) {
        auto element = element.lock();
        // Do not call handlers if the element is destroyed
        if (element == nullptr) return;
        auto it = observed_elements.find(element);
        // Do not call handlers if the element stopped being observed
        if (it = observed_elements.end()) return;
        auto entry = it->second;
        auto prop_value = get_prop_value(element);
        if (entry->prev_prop_value != prop_value) {
            entry->prev_prop_value = prop_value;
            entry->signal(prop_value);
        }
    }
};

}  // namespace aardvark
