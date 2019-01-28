/********************************************************************************
** Form generated from reading UI file 'eti-backend.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ETI_2D_BACKEND_H
#define UI_ETI_2D_BACKEND_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_eti_backend
{
public:
    QWidget *centralwidget;
    QComboBox *streamoutSelector;
    QPushButton *audioDumpButton;
    QLabel *timeDisplay;
    QLabel *dynamicLabel;
    QPushButton *startButton;
    QPushButton *quitButton;
    QListView *ensembleDisplay;
    QLabel *versionName;
    QLCDNumber *ensembleId;
    QLabel *ensembleName;
    QLCDNumber *rateDisplay;
    QLabel *dabModus;
    QPushButton *showProgramData;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *eti_backend)
    {
        if (eti_backend->objectName().isEmpty())
            eti_backend->setObjectName(QStringLiteral("eti_backend"));
        eti_backend->resize(285, 445);
        centralwidget = new QWidget(eti_backend);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        streamoutSelector = new QComboBox(centralwidget);
        streamoutSelector->addItem(QString());
        streamoutSelector->setObjectName(QStringLiteral("streamoutSelector"));
        streamoutSelector->setGeometry(QRect(20, 150, 261, 29));
        audioDumpButton = new QPushButton(centralwidget);
        audioDumpButton->setObjectName(QStringLiteral("audioDumpButton"));
        audioDumpButton->setGeometry(QRect(20, 120, 261, 31));
        timeDisplay = new QLabel(centralwidget);
        timeDisplay->setObjectName(QStringLiteral("timeDisplay"));
        timeDisplay->setGeometry(QRect(20, 90, 201, 21));
        dynamicLabel = new QLabel(centralwidget);
        dynamicLabel->setObjectName(QStringLiteral("dynamicLabel"));
        dynamicLabel->setGeometry(QRect(20, 380, 231, 41));
        startButton = new QPushButton(centralwidget);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setGeometry(QRect(190, 180, 91, 61));
        quitButton = new QPushButton(centralwidget);
        quitButton->setObjectName(QStringLiteral("quitButton"));
        quitButton->setGeometry(QRect(190, 290, 91, 51));
        ensembleDisplay = new QListView(centralwidget);
        ensembleDisplay->setObjectName(QStringLiteral("ensembleDisplay"));
        ensembleDisplay->setGeometry(QRect(20, 180, 171, 161));
        versionName = new QLabel(centralwidget);
        versionName->setObjectName(QStringLiteral("versionName"));
        versionName->setGeometry(QRect(20, 60, 251, 21));
        ensembleId = new QLCDNumber(centralwidget);
        ensembleId->setObjectName(QStringLiteral("ensembleId"));
        ensembleId->setGeometry(QRect(0, 10, 61, 23));
        ensembleId->setFrameShape(QFrame::NoFrame);
        ensembleId->setDigitCount(4);
        ensembleId->setMode(QLCDNumber::Hex);
        ensembleId->setSegmentStyle(QLCDNumber::Flat);
        ensembleName = new QLabel(centralwidget);
        ensembleName->setObjectName(QStringLiteral("ensembleName"));
        ensembleName->setGeometry(QRect(10, 340, 171, 21));
        rateDisplay = new QLCDNumber(centralwidget);
        rateDisplay->setObjectName(QStringLiteral("rateDisplay"));
        rateDisplay->setGeometry(QRect(120, 10, 71, 21));
        rateDisplay->setFrameShape(QFrame::NoFrame);
        rateDisplay->setDigitCount(3);
        rateDisplay->setSegmentStyle(QLCDNumber::Flat);
        dabModus = new QLabel(centralwidget);
        dabModus->setObjectName(QStringLiteral("dabModus"));
        dabModus->setGeometry(QRect(50, 10, 121, 31));
        showProgramData = new QPushButton(centralwidget);
        showProgramData->setObjectName(QStringLiteral("showProgramData"));
        showProgramData->setGeometry(QRect(220, 10, 51, 36));
        eti_backend->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(eti_backend);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        eti_backend->setStatusBar(statusbar);

        retranslateUi(eti_backend);

        QMetaObject::connectSlotsByName(eti_backend);
    } // setupUi

    void retranslateUi(QMainWindow *eti_backend)
    {
        eti_backend->setWindowTitle(QApplication::translate("eti_backend", "eti-backend", nullptr));
        streamoutSelector->setItemText(0, QApplication::translate("eti_backend", "select device", nullptr));

#ifndef QT_NO_TOOLTIP
        streamoutSelector->setToolTip(QApplication::translate("eti_backend", "<html><head/><body><p>Select a device (channel) for the audio output. On program start up a default is chosen.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        audioDumpButton->setToolTip(QApplication::translate("eti_backend", "<html><head/><body><p>Push this button for saving the audio output into a file. First push will show a menu for file selection. Once saving, push again for closing the file.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        audioDumpButton->setText(QApplication::translate("eti_backend", "dump audio", nullptr));
        timeDisplay->setText(QString());
        dynamicLabel->setText(QApplication::translate("eti_backend", "TextLabel", nullptr));
        startButton->setText(QApplication::translate("eti_backend", "START", nullptr));
        quitButton->setText(QApplication::translate("eti_backend", "QUIT", nullptr));
#ifndef QT_NO_TOOLTIP
        versionName->setToolTip(QApplication::translate("eti_backend", "<html><head/><body><p>Copyright (C)  2016  Jan van Katwijk (J.vanKatwijk@gmail.com),  Lazy Chair Programming</p><p><br/></p><p>DAB-rpi is free software; you can redistribute it and/or modify  it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.</p><p><br/></p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        versionName->setText(QString());
#ifndef QT_NO_TOOLTIP
        ensembleId->setToolTip(QApplication::translate("eti_backend", "<html><head/><body><p>The hecadecimal number indicated the ensemble being received.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        ensembleName->setText(QString());
#ifndef QT_NO_TOOLTIP
        rateDisplay->setToolTip(QApplication::translate("eti_backend", "<html><head/><body><p>The rate with which the encoded audio is coming. In general: higher is better.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        dabModus->setText(QString());
        showProgramData->setText(QApplication::translate("eti_backend", "PD", nullptr));
    } // retranslateUi

};

namespace Ui {
    class eti_backend: public Ui_eti_backend {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ETI_2D_BACKEND_H
