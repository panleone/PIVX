// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/pivx-config.h"
#endif

#include "guiutil.h"
#include "qt/pivx/forms/ui_welcomecontentwidget.h"
#include "qt/pivx/welcomecontentwidget.h"
#include "qwidget.h"
#include "wallet/bip39.h"

#include <QDir>
#include <QFile>
#include <QListView>
#include <QScreen>
#include <QSettings>
#include <array>
#include <iostream>

WelcomeContentWidget::WelcomeContentWidget(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
    ui(new Ui::WelcomeContentWidget),
    icConfirm1(new QPushButton()),
    icConfirm2(new QPushButton()),
    icConfirm3(new QPushButton()),
    icConfirm4(new QPushButton()),
    backButton(new QPushButton()),
    nextButton(new QPushButton())
{
    ui->setupUi(this);

    this->setStyleSheet(GUIUtil::loadStyleSheet());

    ui->frame->setProperty("cssClass", "container-welcome-stack");
#ifdef Q_OS_MAC
    ui->frame_2->load("://bg-welcome");
    ui->frame_2->setProperty("cssClass", "container-welcome-no-image");
#else
    ui->frame_2->setProperty("cssClass", "container-welcome");
#endif

    backButton = new QPushButton(ui->container);
    nextButton = new QPushButton(ui->container);

    backButton->show();
    backButton->raise();
    nextButton->show();
    nextButton->raise();

    backButton->setProperty("cssClass", "btn-welcome-back");
    nextButton->setProperty("cssClass", "btn-welcome-next");

    QSize BUTTON_SIZE = QSize(60, 60);
    backButton->setMinimumSize(BUTTON_SIZE);
    backButton->setMaximumSize(BUTTON_SIZE);

    nextButton->setMinimumSize(BUTTON_SIZE);
    nextButton->setMaximumSize(BUTTON_SIZE);

    int backX = 0;
    int backY = 240;
    int nextX = 820;
    int nextY = 240;

    // position
    backButton->move(backX, backY);
    backButton->setStyleSheet("background: url(://ic-arrow-white-left); background-repeat:no-repeat;background-position:center;border:  0;background-color:#5c4b7d;color: #5c4b7d; border-radius:2px;");
    nextButton->move(nextX, nextY);
    nextButton->setStyleSheet("background: url(://ic-arrow-white-right);background-repeat:no-repeat;background-position:center;border:  0;background-color:#5c4b7d;color: #5c4b7d; border-radius:2px;");

    if (pos == 0) {
        backButton->setVisible(false);
    }

    ui->labelLine1->setProperty("cssClass", "line-welcome");
    ui->labelLine2->setProperty("cssClass", "line-welcome");
    ui->labelLine3->setProperty("cssClass", "line-welcome");
    ui->labelLine4->setProperty("cssClass", "line-welcome");

    ui->groupBoxName->setProperty("cssClass", "container-welcome-box");
    ui->groupContainer->setProperty("cssClass", "container-welcome-box");

    ui->pushNumber1->setProperty("cssClass", "btn-welcome-number-check");
    ui->pushNumber1->setEnabled(false);
    ui->pushNumber2->setProperty("cssClass", "btn-welcome-number-check");
    ui->pushNumber2->setEnabled(false);
    ui->pushNumber3->setProperty("cssClass", "btn-welcome-number-check");
    ui->pushNumber3->setEnabled(false);
    ui->pushNumber4->setProperty("cssClass", "btn-welcome-number-check");
    ui->pushNumber4->setEnabled(false);
    ui->pushNumber5->setProperty("cssClass", "btn-welcome-number-check");
    ui->pushNumber5->setEnabled(false);

    ui->pushName1->setProperty("cssClass", "btn-welcome-name-check");
    ui->pushName1->setEnabled(false);
    ui->pushName2->setProperty("cssClass", "btn-welcome-name-check");
    ui->pushName2->setEnabled(false);
    ui->pushName3->setProperty("cssClass", "btn-welcome-name-check");
    ui->pushName3->setEnabled(false);
    ui->pushName4->setProperty("cssClass", "btn-welcome-name-check");
    ui->pushName4->setEnabled(false);
    ui->pushName5->setProperty("cssClass", "btn-welcome-name-check");
    ui->pushName5->setEnabled(false);

    ui->error_seed_phrase->setProperty("cssClass", "text-warning");
    ui->error_seed_phrase->setVisible(false);

    ui->stackedWidget->setCurrentIndex(0);

    setSeedPhrase("en", true);
    // Frame 1
    ui->page_1->setProperty("cssClass", "container-welcome-step1");
    ui->labelTitle1->setProperty("cssClass", "text-title-welcome");
    ui->comboBoxLanguage->setProperty("cssClass", "btn-combo-welcome");
    ui->comboBoxLanguage->setView(new QListView());

    // Frame 2
    ui->page_2->setProperty("cssClass", "container-welcome-step2");
    ui->labelTitle2->setProperty("cssClass", "text-title-welcome");
    ui->labelTitle2->setText(ui->labelTitle2->text().arg(PACKAGE_NAME));
    ui->labelMessage2->setProperty("cssClass", "text-main-white");

    // Frame 3
    ui->page_3->setProperty("cssClass", "container-welcome-step3");
    ui->labelTitle3->setProperty("cssClass", "text-title-welcome");
    ui->labelMessage3->setProperty("cssClass", "text-main-white");

    // Frame 4
    ui->page_4->setProperty("cssClass", "container-welcome-step4");
    ui->labelTitle4->setProperty("cssClass", "text-title-welcome");
    ui->labelMessage4->setProperty("cssClass", "text-main-white");

    // Frame 5
    ui->page_5->setProperty("cssClass", "container-welcome-step4");
    ui->labelTitle5->setProperty("cssClass", "text-title-welcome");
    ui->yesButton->setProperty("cssClass", "btn-primary");
    ui->importBtn->setProperty("cssClass", "btn-primary");
    // ui->labelMessage5->setProperty("cssClass", "text-main-white");

    // Frame 6
    ui->page_6->setProperty("cssClass", "container-welcome-step4");
    ui->labelTitle6->setProperty("cssClass", "text-title-welcome");
    ui->yesButton_2->setProperty("cssClass", "btn-primary");
    ui->generatetBtn->setProperty("cssClass", "btn-primary");

    // Confirm icons
    icConfirm1 = new QPushButton(ui->layoutIcon1_2);
    icConfirm2 = new QPushButton(ui->layoutIcon2_2);
    icConfirm3 = new QPushButton(ui->layoutIcon3_2);
    icConfirm4 = new QPushButton(ui->layoutIcon4_2);
    icConfirm5 = new QPushButton(ui->layoutIcon5_2);

    QSize BUTTON_CONFIRM_SIZE = QSize(22, 22);
    int posX = 0;
    int posY = 0;

    icConfirm1->setProperty("cssClass", "ic-step-confirm-welcome");
    icConfirm1->setMinimumSize(BUTTON_CONFIRM_SIZE);
    icConfirm1->setMaximumSize(BUTTON_CONFIRM_SIZE);
    icConfirm1->show();
    icConfirm1->raise();
    icConfirm1->setVisible(false);
    icConfirm1->move(posX, posY);
    icConfirm2->setProperty("cssClass", "ic-step-confirm-welcome");
    icConfirm2->setMinimumSize(BUTTON_CONFIRM_SIZE);
    icConfirm2->setMaximumSize(BUTTON_CONFIRM_SIZE);
    icConfirm2->move(posX, posY);
    icConfirm2->show();
    icConfirm2->raise();
    icConfirm2->setVisible(false);
    icConfirm3->setProperty("cssClass", "ic-step-confirm-welcome");
    icConfirm3->setMinimumSize(BUTTON_CONFIRM_SIZE);
    icConfirm3->setMaximumSize(BUTTON_CONFIRM_SIZE);
    icConfirm3->move(posX, posY);
    icConfirm3->show();
    icConfirm3->raise();
    icConfirm3->setVisible(false);
    icConfirm4->setProperty("cssClass", "ic-step-confirm-welcome");
    icConfirm4->setMinimumSize(BUTTON_CONFIRM_SIZE);
    icConfirm4->setMaximumSize(BUTTON_CONFIRM_SIZE);
    icConfirm4->move(posX, posY);
    icConfirm4->show();
    icConfirm4->raise();
    icConfirm4->setVisible(false);
    icConfirm5->setProperty("cssClass", "ic-step-confirm-welcome");
    icConfirm5->setMinimumSize(BUTTON_CONFIRM_SIZE);
    icConfirm5->setMaximumSize(BUTTON_CONFIRM_SIZE);
    icConfirm5->move(posX, posY);
    icConfirm5->show();
    icConfirm5->raise();
    icConfirm5->setVisible(false);

    ui->pushButtonSkip->setProperty("cssClass", "btn-close-white");
    onNextClicked();

    connect(ui->importBtn, &QPushButton::clicked, this, &WelcomeContentWidget::onImportClicked);
    connect(ui->generatetBtn, &QPushButton::clicked, this, &WelcomeContentWidget::onGenerateClicked);
    connect(ui->yesButton, &QPushButton::clicked, this, &WelcomeContentWidget::onSkipClicked);
    connect(ui->yesButton_2, &QPushButton::clicked, this, &WelcomeContentWidget::onImportSeedPhraseClicked);
    connect(ui->pushButtonSkip, &QPushButton::clicked, this, &WelcomeContentWidget::close);
    connect(nextButton, &QPushButton::clicked, this, &WelcomeContentWidget::onNextClicked);
    connect(backButton, &QPushButton::clicked, this, &WelcomeContentWidget::onBackClicked);
    connect(ui->comboBoxLanguage, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &WelcomeContentWidget::checkLanguage);
    initLanguages();

    // Resize window and move to center of desktop, disallow resizing
    QRect r(QPoint(), size());
    resize(r.size());
    setFixedSize(r.size());
    move(QGuiApplication::primaryScreen()->geometry().center() - r.center());
}

void WelcomeContentWidget::initLanguages()
{
    /* Language selector */
    QDir translations(":translations");
    ui->comboBoxLanguage->addItem(QString("(") + tr("default") + QString(")"), QVariant(""));
    for (const QString& langStr : translations.entryList()) {
        QLocale locale(langStr);

        /** check if the locale name consists of 2 parts (language_country) */
        if (langStr.contains("_")) {
            /** display language strings as "native language - native country (locale name)", e.g. "Deutsch - Deutschland (de)" */
            ui->comboBoxLanguage->addItem(locale.nativeLanguageName() + QString(" - ") + locale.nativeCountryName() + QString(" (") + langStr + QString(")"), QVariant(langStr));
        } else {
            /** display language strings as "native language (locale name)", e.g. "Deutsch (de)" */
            ui->comboBoxLanguage->addItem(locale.nativeLanguageName() + QString(" (") + langStr + QString(")"), QVariant(langStr));
        }
    }
}

void WelcomeContentWidget::setModel(OptionsModel *model)
{
    this->model = model;
}

void WelcomeContentWidget::checkLanguage()
{
    QString sel = ui->comboBoxLanguage->currentData().toString();
    QSettings settings;
    if (settings.value("language") != sel){
        settings.setValue("language", sel);
        settings.sync();
        Q_EMIT onLanguageSelected();
        ui->retranslateUi(this);
        ui->labelTitle2->setText(ui->labelTitle2->text().arg(PACKAGE_NAME));
        setSeedPhrase(ui->comboBoxLanguage->currentData().toString().toStdString(), false);
    }
}

void WelcomeContentWidget::onNextClicked()
{
    switch(pos) {
        case 0:{
            ui->stackedWidget->setCurrentIndex(1);
            break;
        }
        case 1: {
            backButton->setVisible(true);
            ui->stackedWidget->setCurrentIndex(2);
            ui->pushNumber2->setChecked(true);
            ui->pushName5->setChecked(false);
            ui->pushName4->setChecked(false);
            ui->pushName3->setChecked(false);
            ui->pushName2->setChecked(true);
            ui->pushName1->setChecked(true);
            icConfirm1->setVisible(true);
            break;
        }
        case 2: {
            ui->stackedWidget->setCurrentIndex(3);
            ui->pushNumber3->setChecked(true);
            ui->pushName5->setChecked(false);
            ui->pushName4->setChecked(false);
            ui->pushName3->setChecked(true);
            ui->pushName2->setChecked(true);
            ui->pushName1->setChecked(true);
            icConfirm2->setVisible(true);
            break;
        }
        case 3: {
            ui->stackedWidget->setCurrentIndex(4);
            ui->pushNumber4->setChecked(true);
            ui->pushName5->setChecked(false);
            ui->pushName4->setChecked(true);
            ui->pushName3->setChecked(true);
            ui->pushName2->setChecked(true);
            ui->pushName1->setChecked(true);
            icConfirm3->setVisible(true);
            break;
        }
        case 4: {
            ui->stackedWidget->setCurrentIndex(5);
            ui->pushNumber5->setChecked(true);
            ui->pushName5->setChecked(true);
            ui->pushName4->setChecked(true);
            ui->pushName3->setChecked(true);
            ui->pushName2->setChecked(true);
            ui->pushName1->setChecked(true);
            icConfirm4->setVisible(true);
            nextButton->hide();
            nextButton->setDisabled(true);
            break;
        }
        case 5:{
            isOk = true;
            accept();
            break;
        }
    }
    pos++;

}

void WelcomeContentWidget::onBackClicked()
{
    if (pos == 0) return;
    pos--;
    switch(pos) {
        case 0:{
            ui->stackedWidget->setCurrentIndex(0);
            break;
        }
        case 1: {
            ui->stackedWidget->setCurrentIndex(1);
            ui->pushNumber1->setChecked(true);
            ui->pushNumber4->setChecked(false);
            ui->pushNumber3->setChecked(false);
            ui->pushNumber2->setChecked(false);
            ui->pushName5->setChecked(false);
            ui->pushName4->setChecked(false);
            ui->pushName3->setChecked(false);
            ui->pushName2->setChecked(false);
            ui->pushName1->setChecked(true);
            icConfirm1->setVisible(false);
            backButton->setVisible(false);

            break;
        }
        case 2: {
            ui->stackedWidget->setCurrentIndex(2);
            ui->pushNumber2->setChecked(true);
            ui->pushNumber4->setChecked(false);
            ui->pushNumber3->setChecked(false);
            ui->pushName5->setChecked(false);
            ui->pushName4->setChecked(false);
            ui->pushName3->setChecked(false);
            ui->pushName2->setChecked(true);
            ui->pushName1->setChecked(true);
            icConfirm2->setVisible(false);
            break;
        }
        case 3:{
            ui->stackedWidget->setCurrentIndex(3);
            ui->pushNumber3->setChecked(true);
            ui->pushNumber4->setChecked(false);
            ui->pushName5->setChecked(false);
            ui->pushName4->setChecked(false);
            ui->pushName3->setChecked(true);
            ui->pushName2->setChecked(true);
            ui->pushName1->setChecked(true);
            icConfirm3->setVisible(false);
            break;
        }
        case 4: {
            ui->stackedWidget->setCurrentIndex(4);
            ui->pushNumber4->setChecked(true);
            ui->pushNumber5->setChecked(false);
            ui->pushName5->setChecked(false);
            ui->pushName4->setChecked(true);
            ui->pushName3->setChecked(true);
            ui->pushName2->setChecked(true);
            ui->pushName1->setChecked(true);
            icConfirm4->setVisible(false);
            nextButton->show();
            nextButton->setDisabled(false);
        }
    }

    if (pos == 0) {
        backButton->setVisible(false);
    }
}

void WelcomeContentWidget::onImportClicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    ui->error_seed_phrase->setVisible(false);
}
void WelcomeContentWidget::onGenerateClicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void WelcomeContentWidget::onSkipClicked()
{
    isOk = true;
    accept();
}

void WelcomeContentWidget::onImportSeedPhraseClicked()
{
    std::string input = "";
    for (int i = 0; i < 24; i++) {
        input += input_slots[i]->input_line->text().trimmed().toLower().toStdString();
        if (i < 23) {
            input += " ";
        }
    }
    int result = CheckValidityOfSeedPhrase(input, true);
    if (result == BIP39_ERRORS::BIP39_OK) {
        isOk = true;
        accept();
    } else if (result == BIP39_ERRORS::WRONG_CHECKSUM) {
        ui->error_seed_phrase->setText("Error: seed phrase is not valid!");
    } else if (result == BIP39_ERRORS::WRONG_ENTROPY) {
        ui->error_seed_phrase->setText("Error: seed phrase has not enough words");
        ui->error_seed_phrase->setVisible(true);
    } else {
        if (this->input_slots[result]->input_line->text().length() == 0) {
            ui->error_seed_phrase->setText("Error: there is one or more empty words");
        } else {
            ui->error_seed_phrase->setText("Error: word " + this->input_slots[result]->input_line->text() + " not in wordlist");
        }
        ui->error_seed_phrase->setVisible(true);
    }
}

void WelcomeContentWidget::setSeedPhrase(const std::string& lang, bool firstLoad)
{
    std::vector<std::string> seed_split;
    std::string recovery_phrase = CreateRandomSeedPhrase(true, lang);
    split(recovery_phrase, seed_split, ' ');
    std::array<QWidget*, 24> seeds = {ui->seed1, ui->seed2, ui->seed3, ui->seed4, ui->seed5, ui->seed6, ui->seed7, ui->seed8, ui->seed9, ui->seed10, ui->seed11, ui->seed12, ui->seed13, ui->seed14, ui->seed15, ui->seed16, ui->seed17, ui->seed18, ui->seed19, ui->seed20, ui->seed21, ui->seed22, ui->seed23, ui->seed24};
    std::array<QWidget*, 24> input_seeds = {ui->seed1_2, ui->seed2_2, ui->seed3_2, ui->seed4_2, ui->seed5_2, ui->seed6_2, ui->seed7_2, ui->seed8_2, ui->seed9_2, ui->seed10_2, ui->seed11_2, ui->seed12_2, ui->seed13_2, ui->seed14_2, ui->seed15_2, ui->seed16_2, ui->seed17_2, ui->seed18_2, ui->seed19_2, ui->seed20_2, ui->seed21_2, ui->seed22_2, ui->seed23_2, ui->seed24_2};
    if (firstLoad) {
        for (int i = 0; i < 24; i++) {
            SeedSlot* seed1 = new SeedSlot(false, QString::number(i + 1), QString::fromStdString(seed_split.at(i)), seeds[i]);
            SeedSlot* input_seed1 = new SeedSlot(true, QString::number(i + 1), QString::fromStdString(seed_split.at(i)), input_seeds[i]);
            seeds[i]->setStyleSheet("border-bottom:2px solid gray;");
            output_slots[i] = seed1;
            output_slots[i]->show();
            output_slots[i]->setStyleSheet("border-bottom:2px solid gray;");
            input_slots[i] = input_seed1;
            input_slots[i]->show();
            input_seeds[i]->setStyleSheet("border-bottom:2px solid gray;");
        }
    } else {
        for (int i = 0; i < 24; i++) {
            output_slots[i]->t_label->setText(QString::fromStdString(seed_split.at(i)));
        }
    }
}

WelcomeContentWidget::~WelcomeContentWidget()
{
    delete ui;
}