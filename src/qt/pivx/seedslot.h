#ifndef SEEDSLOT_H
#define SEEDSLOT_H

#include <QWidget>
#include "qt/pivx/colorlabel.h"
class SeedSlot : public QWidget
{
    Q_OBJECT
public:
    explicit SeedSlot(const QString &number,const QString &word,QWidget *parent = nullptr);
    ColorLabel* t_label;
    ColorLabel* n_label;
    ColorLabel* r_label;
    int getWidth();
    int getHeight();
private:
    int width;
    int height;
};

#endif // SEEDSLOT_H
