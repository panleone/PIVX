#include "qt/pivx/seedslot.h"
#include <QColor>
SeedSlot::SeedSlot(bool isInput,const QString &number,const QString &word,QWidget *parent)
    : QWidget{parent}
{
    width=100;
    height=40;

    setFixedSize(width, height);
    n_label = new ColorLabel(number,this);
    n_label->setTextColor(Qt::gray);
    n_label->setGeometry(0,1,15,20);
    QFont t_Font( "Arial", 8,QFont::Bold);
    if(!isInput){
        t_label = new ColorLabel(word,this);
        
        t_label->setFont(t_Font);
        t_label->setTextColor(Qt::white);
        t_label ->setGeometry(20,0,55,20);
    }else{
        input_line = new QLineEdit(this);
        input_line ->setGeometry(20,2,50,16);
        input_line -> setFont(t_Font);
    }
    r_label = new ColorLabel("",this);
    r_label->makeRec(Qt::gray);
    r_label->setGeometry(0,22,75,2);
}
int SeedSlot::getWidth(){
    return width;
}
int SeedSlot::getHeight(){
    return height;
}