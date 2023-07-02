#ifndef SIMPLE_NOTES_ISTATUSHANDLER_H
#define SIMPLE_NOTES_ISTATUSHANDLER_H

#include <QString>
#include <memory>

#include "Error.h"

template<typename ...Args>
class IStatusHandler {
public:
    virtual ~IStatusHandler() = default;

    virtual void onSuccess(Args &&...args) = 0;
    virtual void onError(const Error &error) = 0;
};

template<typename ...Args>
using IStatusHandlerPtr = std::shared_ptr<IStatusHandler<Args...>>;

#endif //SIMPLE_NOTES_ISTATUSHANDLER_H
