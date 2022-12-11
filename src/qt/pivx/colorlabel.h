#ifndef COLORLABEL_H
#define COLORLABEL_H
#include <QLabel>


#include <QWidget>
#include <QPushButton>
#include <QPalette>

class ColorLabel : public QLabel
{
    Q_OBJECT
public:

    explicit ColorLabel(const QString &text, QWidget *parent = nullptr);
    void setTextColor(const QColor &color);
    void makeRec(const QColor &color);
};

#endif