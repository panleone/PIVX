// Copyright (c) 2023 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

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
    parent->setLayout(seedGrid);
    adjustSize();
}