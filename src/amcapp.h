#ifndef AMCAPP_H
#define AMCAPP_H

#include <memory>
#include <QApplication>

namespace amc
{
class AMCAppPrivate;

class AMCApp
{
public:
    AMCApp(std::unique_ptr<QApplication> application);
    ~AMCApp();

    int run();

public:
    AMCApp(const AMCApp &)            = delete;
    AMCApp(AMCApp &&)                 = delete;
    AMCApp &operator=(const AMCApp &) = delete;
    AMCApp &operator=(AMCApp &&)      = delete;

private:
    std::unique_ptr<AMCAppPrivate> d;
};
} // namespace amc
#endif // AMCAPP_H
