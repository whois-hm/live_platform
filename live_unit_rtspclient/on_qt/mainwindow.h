#pragma once
#include "headers.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
   class filter : public QObject
   {
       MainWindow &_win;
   public:
       filter(MainWindow &win) : QObject(),
           _win(win)
       {
           QApplication::instance()->installEventFilter(this);
       }
       virtual ~filter(){}
       virtual bool eventFilter(QObject *obj, QEvent *e)
       {
           if(e->type() >= QEvent::User)
           {
               _win._event_donext(e);
               return true;
           }
           return QObject::eventFilter(obj, e);
       }
   };
   QSize _monitorsize;
   QSize _thumbnailsize;
   filter *_filter;
   http *_http ;
   QListWidget *_listwidget;
   QLabel *_loadingblock;
   navigator *_navigator;
    void _event_donext(QEvent *e);
    void _event_handle_sendpixel(pixel_event &e);
    void _event_handle_sendpcm(pcm_event &e);
    void _event_handle_endofpixel(endpixel_event &e);
    void _event_handle_endofpcm(endpcm_event &e);
    void _event_handle_httpreceived(http_received_event &e);
    void _event_handle_httpreceived_list(http_req_list &e);
    void _event_handle_httpreceived_thumbnail(http_req_thumbnail &e);
    void _event_handle_httpreceived_information(http_req_information &e);
    void _event_handle_httpreceived_play(http_req_play &e);
    void start_http();
private slots:
   void item_doubleclick(QListWidgetItem*i);
};
