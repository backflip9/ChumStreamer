#include "clickablelist.h"
#include "chumlistitem.h"
//#include "ChumStreamer.h"
#include<QDebug>
#include<QMouseEvent>
#include<QKeyEvent>
//clickableList::clickableList(QWidget *parent) : QListWidget(parent){}

void clickableList::mousePressEvent(QMouseEvent* m)
{
    emit Clicked();
    if(m->button()==Qt::BackButton)
    {
      //emit ClickedWithEvent(m);
      emit Back();
    }
    //we have to call the parent class' method so that we don't totally override what qlistwidget would normally do when a mouse button is pressed
    QListWidget::mousePressEvent(m);
}

void clickableList::keyPressEvent(QKeyEvent* k)
{
  qDebug() << "clickableList::keyPressEvent: "<<k->key();
  //it should be noted that most of these cases are sketchy as fuck when there isn't a current selected item, but none of them actually crash the program except enter, since that calls another function with a bunch of pointers
  switch(k->key())
  {
    case Qt::Key_A:
      emit playlistAppend();
      break;
    case Qt::Key_E:
      emit playlistPrepend();
      break;
    case Qt::Key_J:
    case Qt::Key_Down:
      if(currentRow()<count()-1)
      {
        setCurrentRow(currentRow()+1);
      }
      else
      {//loop around
        setCurrentRow(0);
      }
      break;
    case Qt::Key_K:
    case Qt::Key_Up:
      if(currentRow()>0)
      {
        setCurrentRow(currentRow()-1);
      }
      else if(currentRow()==0 || currentItem()==nullptr)
      {
        setCurrentRow(count()-1);
      }

      break;
    case Qt::Key_L:
    case Qt::Key_Return:
      if(currentItem()!=nullptr)
      {
        emit ReturnKey();
      }
      break;
    case Qt::Key_H:
      emit Back();
      break;
    default:
      qDebug() << "that key is irrelevant";
      break;
  }
  //we're not calling this cause we don't like it
  //QListWidget::keyPressEvent(k);
}
