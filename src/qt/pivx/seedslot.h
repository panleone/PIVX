// Copyright (c) 2023 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

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
    QLineEdit* input_line;
};

#endif // SEEDSLOT_H
