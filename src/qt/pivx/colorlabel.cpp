#include "qt/pivx/colorlabel.h"


ColorLabel::ColorLabel(const QString &text, QWidget *parent):QLabel(text,parent)

    {
    
    }

    void ColorLabel::setTextColor(const QColor &color)
    {
        QPalette palette = this->palette();
        palette.setColor(this->foregroundRole(), color);
        this->setPalette(palette);
    }

    void ColorLabel::makeRec(const QColor &color)
    {
        setAutoFillBackground(true);
        QPalette palette = this->palette();
        palette.setColor(this->backgroundRole(), color);
        this->setPalette(palette);
    }

