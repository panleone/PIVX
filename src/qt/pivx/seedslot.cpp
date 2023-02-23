#include "qt/pivx/seedslot.h"
#include "qlabel.h"
#include <QColor>
SeedSlot::SeedSlot(bool isInput, const QString& number, const QString& word, QWidget* parent)
    : QWidget{parent}
{
    width = 100;
    height = 40;

    setFixedSize(width, height);
    n_label = new QLabel(number, this);
    n_label->setProperty("cssClass", "text-main-number-grey");
    n_label->move(0,1);
    QFont t_Font("Arial", 12, QFont::Bold);
    if (!isInput) {
        t_label = new QLabel(word,this);
        t_label->setProperty("cssClass","text-main-seed-phrase-white");
        t_label->move(20,-4);
    } else {
        input_line = new QLineEdit(this);
        input_line->setGeometry(20, 2, 80, 18);
        input_line->setProperty("cssClass","text-main-seed-phrase-black");
    }
    r_label = new QLabel("", this);
    r_label->setAutoFillBackground(true);
    QPalette palette = r_label->palette();
    palette.setColor(this->backgroundRole(), Qt::gray);
    r_label->setPalette(palette);
    r_label->setGeometry(0, 22, 100, 2);
}
int SeedSlot::getWidth()
{
    return width;
}
int SeedSlot::getHeight()
{
    return height;
}