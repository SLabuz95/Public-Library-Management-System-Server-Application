#ifndef BOOKCOMMENT_HPP
#define BOOKCOMMENT_HPP

#include<QString>

struct BookComment{
    QString content;
    unsigned long long userId = 0;
};

#endif // BOOKCOMMENT_HPP
