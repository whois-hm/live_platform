#include "mainwindow.h"
#include "ui_mainwindow.h"
class contentsitem : public QListWidgetItem
{
public:
    const QString _contentsid;
    contentsitem(const QIcon &thumbnail,
                 const QString &name,
                 const QString &contentsid) :
        QListWidgetItem(thumbnail,
                        name), _contentsid(contentsid){}

};

void MainWindow::_event_donext(QEvent *e)
{
    indexed_event *ourevent = dynamic_cast< indexed_event* > (e);
    if(ourevent->i() == custom_event_id_send_pixel)
    {
        _event_handle_sendpixel(dynamic_cast<pixel_event &>(*ourevent));
    }
    else if(ourevent->i() == custom_event_id_send_pcm)
    {
        _event_handle_sendpcm(dynamic_cast<pcm_event &>(*ourevent));
    }
    else if(ourevent->i() == custom_event_id_endof_pixel)
    {
        _event_handle_endofpixel(dynamic_cast<endpixel_event &>(*ourevent));
    }
    else if(ourevent->i() == custom_event_id_endof_pcm)
    {
        _event_handle_endofpcm(dynamic_cast<endpcm_event &>(*ourevent));
    }
    else if(ourevent->i() == custom_event_id_http_received)
    {
        _event_handle_httpreceived(dynamic_cast<http_received_event &>(*ourevent));
    }
}
void MainWindow::_event_handle_httpreceived(http_received_event &e)
{
    http_req &http_res = dynamic_cast<http_req &>(e);
    if(http_res.method() == "list")
    {
        _event_handle_httpreceived_list(dynamic_cast<http_req_list &>(http_res));
    }
    else if(http_res.method() == "thumbnail")
    {
        _event_handle_httpreceived_thumbnail(dynamic_cast<http_req_thumbnail &>(http_res));
    }
    else if(http_res.method() == "information")
    {
        _event_handle_httpreceived_information(dynamic_cast<http_req_information &>(http_res));
    }
    else if(http_res.method() == "play")
    {
        _event_handle_httpreceived_play(dynamic_cast<http_req_play &>(http_res));
    }
}
void MainWindow::_event_handle_sendpixel(pixel_event &e)
{
    _navigator->draw_pixel(e.d());
}

void MainWindow::_event_handle_sendpcm(pcm_event &e)
{
    _navigator->play_pcm(e.d());
}

void MainWindow::_event_handle_endofpixel(endpixel_event &e)
{
    _navigator->draw_pixel(e.d());
}

void MainWindow::_event_handle_endofpcm(endpcm_event &e)
{
    _navigator->play_pcm(e.d());
}
void MainWindow::_event_handle_httpreceived_list(http_req_list &e)
{
    if(!e)
    {
        Time_sleep(50);
        _http->send(new http_req_list());
        return;
    }

    Poco::JSON::Parser parser;
    Poco::Dynamic::Var res = parser.parse(e.d().toLocal8Bit().data());
    Poco::JSON::Object::Ptr object = res.extract<Poco::JSON::Object::Ptr>();

    int number = object->get("number").convert<int>();
    qDebug() << number;
    Poco::JSON::Array::Ptr arr = object->getArray("list");
    for(int i = 0; i < number; i++)
    {
        Poco::JSON::Object::Ptr o = arr->getObject(i);
         _http->send(new http_req_thumbnail(QString(o->get("contentsindex").convert<std::string>().c_str()),
                                            QString(o->get("name").convert<std::string>().c_str()),
                                            QString(o->get("type").convert<std::string>().c_str()),
                                            _thumbnailsize.width(),
                                            _thumbnailsize.height(), 14));
    }
}

void MainWindow::_event_handle_httpreceived_thumbnail(http_req_thumbnail &e)
{
    if(!e)
    {
        return;
    }
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var res = parser.parse(e.d().toLocal8Bit().data());
    Poco::JSON::Object::Ptr object = res.extract<Poco::JSON::Object::Ptr>();

    QString type = QString(object->get("type").convert<std::string>().c_str());
    if(type == "not found")
    {
        QPixmap noimage(e._display_width, e._display_height);
        noimage.fill(QColor(200,200,200));

        QPainter painter( &noimage);
        painter.setFont( QFont("Arial") );
        painter.drawText( QRect(0,0,e._display_width, e._display_height), Qt::AlignCenter, "no image" );
        contentsitem *item = new contentsitem(QIcon(noimage), e._name, e._contentsid);
        _listwidget->addItem(item);
        return;
    }

     int width = object->get("width").convert<int>();
     int height = object->get("height").convert<int>();


    QString base64code =  QString(object->get("data").convert<std::string>().c_str());

    QByteArray image(QByteArray::fromBase64(base64code.toLocal8Bit().data()));


     QImage img((const uchar *)image.constData(),
                  width,
                  height,
                  QImage::Format_RGB888);
     QPixmap map = QPixmap::fromImage(img);
     contentsitem *item = new contentsitem(QIcon(map), e._name, e._contentsid);
    _listwidget->addItem(item);
    _http->send(new http_req_information(item->_contentsid));
}

void MainWindow::_event_handle_httpreceived_information(http_req_information &e)
{
    for(int i = 0; i < _listwidget->count(); i++)
    {
        if(((contentsitem *)_listwidget->item(i))->_contentsid == e._contentsid)
        {
            _listwidget->item(i)->setToolTip(e.d());
            break;
        }
    }
}

void MainWindow::_event_handle_httpreceived_play(http_req_play &e)
{
    _navigator->close();
    qDebug() << e.d();
   Poco::JSON::Parser parser;
   Poco::Dynamic::Var pres = parser.parse(e.d().toLocal8Bit().data());
   Poco::JSON::Object::Ptr object = pres.extract<Poco::JSON::Object::Ptr>();

   QString res = QString(object->get("result").convert<std::string>().c_str());
   QString url = QString(object->get("sessionname").convert<std::string>().c_str());
   QString id = QString(object->get("auth-id").convert<std::string>().c_str());
   QString pwd = QString(object->get("auth-password").convert<std::string>().c_str());
   QString cid = QString(object->get("clientid").convert<std::string>().c_str());

   if(!_navigator->open(e._contentsid,
                       res,
                       url,
                       id,
                       pwd,
                       cid,
                        e._savethumbnail))
   {
       _navigator->close();
       _navigator->error();
   }
}
void MainWindow::start_http()
{
        _listwidget->clear();
    /*first list request*/
    _http->send(new http_req_list());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) ,
    _monitorsize(320,240),/*vga*/
    _thumbnailsize(160, 120),/*qqvga*/
    _filter(new filter(*this)),
    _http(new http("192.168.2.60", 80, this)),
    _listwidget(new QListWidget(this)),
    _loadingblock(new QLabel(this)),
    _navigator(new navigator(this))

{

    resize(_monitorsize.width(),
           _monitorsize.height() + _thumbnailsize.height());

    QPixmap du(width(), height());
    du.fill(QColor(200,200,200,200));
    _loadingblock->setPixmap(du);
    _loadingblock->setGeometry(0,0, width(), height());
    _loadingblock->hide();


    _listwidget->setMouseTracking(true);
    _listwidget->setIconSize(_thumbnailsize);
    _listwidget->setGeometry(0,
                             _monitorsize.height(),
                             width(),
                             _thumbnailsize.height());
    connect(_listwidget,
            SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,
            SLOT(item_doubleclick(QListWidgetItem*)));

    _navigator->create(QRect(0,0,_monitorsize.width(), _monitorsize.height()));
    start_http();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::item_doubleclick(QListWidgetItem*i)
{
    _navigator->close();
    _http->send(new http_req_play(((contentsitem *)i)->_contentsid,
                                  i->icon().pixmap(_navigator->width(), _navigator->height())));
}

