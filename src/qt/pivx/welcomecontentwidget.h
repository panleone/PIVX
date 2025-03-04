// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef WELCOMECONTENTWIDGET_H
#define WELCOMECONTENTWIDGET_H

#include "qt/pivx/seedslot.h"
#include <QDialog>
#include <QPushButton>
#include <QWidget>
class OptionsModel;

namespace Ui {
class WelcomeContentWidget;
}

class WelcomeContentWidget : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomeContentWidget(QWidget *parent = nullptr);
    ~WelcomeContentWidget();
    int pos = 0;
    bool isOk = false;

    void setModel(OptionsModel *model);

Q_SIGNALS:
    void onLanguageSelected();

public Q_SLOTS:
    void onNextClicked();
    void onBackClicked();
    void onSkipClicked();
    void onImportClicked();
    void onGenerateClicked();
    void onImportSeedPhraseClicked();
    void checkLanguage();

private:
    Ui::WelcomeContentWidget *ui;
    QPushButton *icConfirm1;
    QPushButton *icConfirm2;
    QPushButton *icConfirm3;
    QPushButton *icConfirm4;
    QPushButton* icConfirm5;
    QPushButton *backButton;
    QPushButton *nextButton;

    OptionsModel *model;
    SeedSlot* input_slots[24];
    SeedSlot* output_slots[24];

    void initLanguages();
    void setSeedPhrase(const std::string& lang, bool firstLoad);
};

#endif // WELCOMECONTENTWIDGET_H
