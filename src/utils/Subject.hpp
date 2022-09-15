#pragma once

#include <vector>
#include <functional>

template<typename... Args>
class Subject {
public:
    void AddObserver(std::function<void(Args...)> observer);
//    void RemoveObserver(std::function<void(Args...)> observer);
    void Notify(Args... input);

private:
    std::vector<std::function<void(Args...)>> m_Observers;
};

template<typename... Args>
void Subject<Args...>::AddObserver(std::function<void(Args...)> observer) {
    m_Observers.push_back(std::move(observer));
}

template<typename... Args>
void Subject<Args...>::Notify(Args... input) {
    for (auto& observer : m_Observers)
        observer(input...);
}
