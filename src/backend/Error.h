#ifndef SIMPLE_NOTES_ERROR_H
#define SIMPLE_NOTES_ERROR_H

#include <QString>
#include <utility>

#include "ErrorCode.h"

class Error {
public:
    enum class Type {
        Unknown,
        Internal,
        Firebase
    };

public:
    Error() = default;

    Error(int code, Type type, QString message)
        : m_code(code), m_type(type), m_message(std::move(message)) {}

    int code() const {
        return m_code;
    }

    Type type() const {
        return m_type;
    }

    const QString& message() const {
        return m_message;
    }

    static Error internal(ErrorCode code, QString message) {
        return {static_cast<int>(code), Type::Internal, std::move(message)};
    }

    static Error firebase(int code, QString message) {
        return {code, Type::Firebase, std::move(message)};
    }

private:
    int m_code {0};
    Type m_type {Type::Unknown};
    QString m_message;
};

#endif //SIMPLE_NOTES_ERROR_H
