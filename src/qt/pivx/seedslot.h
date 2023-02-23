#ifndef SEEDSLOT_H
#define SEEDSLOT_H

#include <QWidget>
#include <QLineEdit>
#include "qlabel.h"
class SeedSlot : public QWidget
{
    Q_OBJECT
public:
    explicit SeedSlot(bool isInput,const QString &number,const QString &word,QWidget *parent = nullptr);
    QLabel* t_label;
    QLabel* n_label;
    QLabel* r_label;
    QLineEdit* input_line;
    int getWidth();
    int getHeight();
private:
    int width;
    int height;
};

#endif // SEEDSLOT_H
