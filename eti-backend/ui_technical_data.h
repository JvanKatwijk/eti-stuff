/********************************************************************************
** Form generated from reading UI file 'technical_data.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TECHNICAL_DATA_H
#define UI_TECHNICAL_DATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_technical_data
{
public:
    QLCDNumber *startAddressDisplay;
    QLCDNumber *lengthDisplay;
    QLabel *language;
    QLCDNumber *protectionlevelDisplay;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *programType;
    QLabel *l6;
    QLCDNumber *bitrateDisplay;
    QLCDNumber *subChIdDisplay;
    QLabel *label_6;
    QLabel *uepField;
    QLabel *ASCTy;
    QLabel *ensemble;
    QLCDNumber *frequency;
    QLabel *frequencyLabel;
    QProgressBar *ficError_display;
    QProgressBar *frameError_display;
    QProgressBar *rsError_display;
    QProgressBar *aacError_display;
    QLabel *cpuLabel;
    QLCDNumber *cpuMonitor;
    QLabel *label_4;
    QLabel *motDisplay;

    void setupUi(QWidget *technical_data)
    {
        if (technical_data->objectName().isEmpty())
            technical_data->setObjectName(QStringLiteral("technical_data"));
        technical_data->resize(259, 572);
        startAddressDisplay = new QLCDNumber(technical_data);
        startAddressDisplay->setObjectName(QStringLiteral("startAddressDisplay"));
        startAddressDisplay->setGeometry(QRect(170, 150, 64, 23));
        startAddressDisplay->setFrameShape(QFrame::NoFrame);
        startAddressDisplay->setSegmentStyle(QLCDNumber::Flat);
        lengthDisplay = new QLCDNumber(technical_data);
        lengthDisplay->setObjectName(QStringLiteral("lengthDisplay"));
        lengthDisplay->setGeometry(QRect(170, 180, 64, 23));
        lengthDisplay->setFrameShape(QFrame::NoFrame);
        lengthDisplay->setSegmentStyle(QLCDNumber::Flat);
        language = new QLabel(technical_data);
        language->setObjectName(QStringLiteral("language"));
        language->setGeometry(QRect(60, 350, 181, 20));
        language->setFrameShape(QFrame::Box);
        protectionlevelDisplay = new QLCDNumber(technical_data);
        protectionlevelDisplay->setObjectName(QStringLiteral("protectionlevelDisplay"));
        protectionlevelDisplay->setGeometry(QRect(170, 240, 64, 23));
        protectionlevelDisplay->setFrameShape(QFrame::NoFrame);
        protectionlevelDisplay->setSegmentStyle(QLCDNumber::Flat);
        protectionlevelDisplay->setProperty("value", QVariant(0));
        label = new QLabel(technical_data);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 150, 111, 20));
        label_2 = new QLabel(technical_data);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 180, 63, 20));
        label_3 = new QLabel(technical_data);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 240, 111, 20));
        programType = new QLabel(technical_data);
        programType->setObjectName(QStringLiteral("programType"));
        programType->setGeometry(QRect(60, 370, 181, 20));
        programType->setFrameShape(QFrame::Box);
        l6 = new QLabel(technical_data);
        l6->setObjectName(QStringLiteral("l6"));
        l6->setGeometry(QRect(30, 270, 63, 20));
        bitrateDisplay = new QLCDNumber(technical_data);
        bitrateDisplay->setObjectName(QStringLiteral("bitrateDisplay"));
        bitrateDisplay->setGeometry(QRect(170, 270, 64, 23));
        bitrateDisplay->setFrameShape(QFrame::NoFrame);
        bitrateDisplay->setSegmentStyle(QLCDNumber::Flat);
        subChIdDisplay = new QLCDNumber(technical_data);
        subChIdDisplay->setObjectName(QStringLiteral("subChIdDisplay"));
        subChIdDisplay->setGeometry(QRect(170, 210, 64, 23));
        subChIdDisplay->setFrameShape(QFrame::NoFrame);
        subChIdDisplay->setSegmentStyle(QLCDNumber::Flat);
        label_6 = new QLabel(technical_data);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 210, 111, 20));
        uepField = new QLabel(technical_data);
        uepField->setObjectName(QStringLiteral("uepField"));
        uepField->setGeometry(QRect(60, 310, 181, 20));
        uepField->setFrameShape(QFrame::Box);
        ASCTy = new QLabel(technical_data);
        ASCTy->setObjectName(QStringLiteral("ASCTy"));
        ASCTy->setGeometry(QRect(60, 330, 181, 20));
        ASCTy->setFrameShape(QFrame::Box);
        ensemble = new QLabel(technical_data);
        ensemble->setObjectName(QStringLiteral("ensemble"));
        ensemble->setGeometry(QRect(20, 10, 201, 20));
        frequency = new QLCDNumber(technical_data);
        frequency->setObjectName(QStringLiteral("frequency"));
        frequency->setGeometry(QRect(30, 50, 91, 31));
        frequency->setFrameShape(QFrame::NoFrame);
        frequency->setSmallDecimalPoint(true);
        frequency->setDigitCount(7);
        frequency->setSegmentStyle(QLCDNumber::Flat);
        frequencyLabel = new QLabel(technical_data);
        frequencyLabel->setObjectName(QStringLiteral("frequencyLabel"));
        frequencyLabel->setGeometry(QRect(150, 50, 63, 31));
        ficError_display = new QProgressBar(technical_data);
        ficError_display->setObjectName(QStringLiteral("ficError_display"));
        ficError_display->setGeometry(QRect(40, 400, 201, 23));
        ficError_display->setValue(24);
        frameError_display = new QProgressBar(technical_data);
        frameError_display->setObjectName(QStringLiteral("frameError_display"));
        frameError_display->setGeometry(QRect(40, 430, 201, 23));
        frameError_display->setValue(24);
        rsError_display = new QProgressBar(technical_data);
        rsError_display->setObjectName(QStringLiteral("rsError_display"));
        rsError_display->setGeometry(QRect(40, 460, 201, 23));
        rsError_display->setValue(24);
        aacError_display = new QProgressBar(technical_data);
        aacError_display->setObjectName(QStringLiteral("aacError_display"));
        aacError_display->setGeometry(QRect(40, 490, 201, 23));
        aacError_display->setValue(24);
        cpuLabel = new QLabel(technical_data);
        cpuLabel->setObjectName(QStringLiteral("cpuLabel"));
        cpuLabel->setGeometry(QRect(30, 100, 81, 20));
        cpuMonitor = new QLCDNumber(technical_data);
        cpuMonitor->setObjectName(QStringLiteral("cpuMonitor"));
        cpuMonitor->setGeometry(QRect(150, 92, 71, 31));
        cpuMonitor->setFrameShape(QFrame::NoFrame);
        cpuMonitor->setSegmentStyle(QLCDNumber::Flat);
        label_4 = new QLabel(technical_data);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 530, 121, 20));
        motDisplay = new QLabel(technical_data);
        motDisplay->setObjectName(QStringLiteral("motDisplay"));
        motDisplay->setGeometry(QRect(182, 530, 41, 20));

        retranslateUi(technical_data);

        QMetaObject::connectSlotsByName(technical_data);
    } // setupUi

    void retranslateUi(QWidget *technical_data)
    {
        technical_data->setWindowTitle(QApplication::translate("technical_data", "Technical Data", nullptr));
        language->setText(QString());
        label->setText(QApplication::translate("technical_data", "startAddress", nullptr));
        label_2->setText(QApplication::translate("technical_data", "Length", nullptr));
        label_3->setText(QApplication::translate("technical_data", "ProtectionLevel", nullptr));
        programType->setText(QString());
        l6->setText(QApplication::translate("technical_data", "bitRate", nullptr));
        label_6->setText(QApplication::translate("technical_data", "Subchannel Id", nullptr));
        uepField->setText(QString());
        ASCTy->setText(QString());
        ensemble->setText(QString());
        frequencyLabel->setText(QApplication::translate("technical_data", "MHz", nullptr));
#ifndef QT_NO_TOOLTIP
        ficError_display->setToolTip(QApplication::translate("technical_data", "<html><head/><body><p>Quality of FIC decoding, 100 is good</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        frameError_display->setToolTip(QApplication::translate("technical_data", "<html><head/><body><p>Frame errors. Indication of the quality of the DAB+ frame detection. 100 is good.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        rsError_display->setToolTip(QApplication::translate("technical_data", "<html><head/><body><p>Quality of the DAB+ frames. Indicator for the amount of times that the frames contain</p><p>more errors than the Reed Solomon correction can correct. 100 is good.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        aacError_display->setToolTip(QApplication::translate("technical_data", "<html><head/><body><p>Indicator of the successrate of handling the aac frames in the DAB+ transmissions.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        cpuLabel->setText(QApplication::translate("technical_data", "cpu usage", nullptr));
        label_4->setText(QApplication::translate("technical_data", "MOT decoding", nullptr));
        motDisplay->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class technical_data: public Ui_technical_data {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TECHNICAL_DATA_H
