#ifndef SIMPLE_NOTES_STATUSHANDLER_H
#define SIMPLE_NOTES_STATUSHANDLER_H

#include "IStatusHandler.h"

#include <functional>

template<typename ...Args>
class StatusHandler : public IStatusHandler<Args...> {
    using OnSuccess_t = std::function<void(Args...)>;
    using OnError_t = std::function<void(const Error &error)>;

public:
    StatusHandler(OnSuccess_t onSuccess, OnError_t onError)
        : m_onSuccess(std::move(onSuccess)), m_onError(std::move(onError)) {}

    void onSuccess(Args &&...args) override {
        if (m_onSuccess) {
            m_onSuccess(std::forward<Args>(args)...);
        }
    }

    void onError(const Error &error) override {
        if (m_onError) {
            m_onError(error);
        }
    }

    static IStatusHandlerPtr<Args...> success(OnSuccess_t onSuccess) {
        return std::make_shared<StatusHandler>(std::move(onSuccess), OnError_t {});
    }

    static IStatusHandlerPtr<Args...> error(OnError_t onError) {
        return std::make_shared<StatusHandler>(OnSuccess_t {}, std::move(onError));
    }

    static IStatusHandlerPtr<Args...> both(OnSuccess_t onSuccess, OnError_t onError) {
        return std::make_shared<StatusHandler>(std::move(onSuccess), std::move(onError));
    }

private:
    OnSuccess_t m_onSuccess;
    OnError_t m_onError;
};

#endif //SIMPLE_NOTES_STATUSHANDLER_H
