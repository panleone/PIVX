#include "qt/pivx/seedslot.h"
#include "qlabel.h"
#include <QColor>
#include <QGridLayout>

SeedSlot::SeedSlot(bool isInput, const QString& number, const QString& word, QWidget* parent)
    : QWidget{parent}
{
    QGridLayout *seedGrid = new QGridLayout;
    n_label = new QLabel(number, this);
    n_label->setProperty("cssClass", "text-main-number-grey");
    seedGrid->addWidget(n_label, 0, 0, 1, 1);
    QFont t_Font("Monospace", 15, QFont::Bold);
    if (!isInput) {
        t_label = new QLabel(word,this);
        seedGrid->addWidget(t_label, 0, 1, 1, 1);
        t_label->setProperty("cssClass", "text-main-seed-phrase-white");
    } else {
        input_line = new QLineEdit(this);
        input_line->setProperty("cssClass", "text-main-seed-phrase-black");
        seedGrid->addWidget(input_line, 0, 2, 1, 1);
    }
    r_label = new QLabel("", this);
    r_label->setAutoFillBackground(true);
    QPalette palette = r_label->palette();
    palette.setColor(this->backgroundRole(), Qt::gray);
    r_label->setPalette(palette);
    r_label->setGeometry(0, 35, 105, 2);
    parent->setLayout(seedGrid);
    adjustSize();
}