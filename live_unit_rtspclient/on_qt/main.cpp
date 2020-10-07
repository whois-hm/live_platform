#include "mainwindow.h"
#include "headers.h"
const QEvent::Type custom_base_event_id = static_cast<QEvent::Type>(QEvent::registerEventType(QEvent::User + 100));
int main(int argc, char *argv[])
{

    livemedia_pp::ref();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.exec();
    livemedia_pp::ref(false);
    return 0;
}
