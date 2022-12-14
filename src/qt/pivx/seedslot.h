#ifndef SEEDSLOT_H
#define SEEDSLOT_H

#include <QWidget>
#include <QLineEdit>
#include "qt/pivx/colorlabel.h"
class SeedSlot : public QWidget
{
    Q_OBJECT
public:
    explicit SeedSlot(bool isInput,const QString &number,const QString &word,QWidget *parent = nullptr);
    ColorLabel* t_label;
    ColorLabel* n_label;
    ColorLabel* r_label;
    QLineEdit* input_line;
    int getWidth();
    int getHeight();
private:
    int width;
    int height;
};

#endif // SEEDSLOT_H
