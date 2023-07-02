#ifndef SIMPLE_NOTES_IDGENERATOR_H
#define SIMPLE_NOTES_IDGENERATOR_H

#include <QString>

class IdGenerator {
public:
    static QString generate(int len = 16);
};

#endif //SIMPLE_NOTES_IDGENERATOR_H
