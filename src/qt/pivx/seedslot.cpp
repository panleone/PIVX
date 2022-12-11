#include "qt/pivx/seedslot.h"
#include <QColor>
SeedSlot::SeedSlot(const QString &number,const QString &word,QWidget *parent)
    : QWidget{parent}
{
    width=100;
    height=40;

    setFixedSize(width, height);
    n_label = new ColorLabel(number,this);
    n_label->setTextColor(Qt::gray);
    n_label->setGeometry(0,0,15,20);
    t_label = new ColorLabel(word,this);

    QFont t_Font( "Arial", 9,QFont::Bold);
    t_label->setFont(t_Font);
    t_label->setTextColor(Qt::white);
    t_label ->setGeometry(20,0,60,20);

    r_label = new ColorLabel("",this);
    r_label->makeRec(Qt::gray);
    r_label->setGeometry(0,22,70,2);
}
int SeedSlot::getWidth(){
    return width;
}
int SeedSlot::getHeight(){
    return height;
}