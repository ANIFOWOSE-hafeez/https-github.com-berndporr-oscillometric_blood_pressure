/**
 * @file        InfoDialog.cpp
 * @brief       InfoDialog implementation file.
 * @author      Belinda Kneubühler
 * @date        2020-08-18
 * @copyright   GNU General Public License v2.0
 *
 */

#include "InfoDialog.h"

/**
 * The constructor of the InfoDialog class.
 * @param parent A reference to the parent object.
 */
InfoDialog::InfoDialog(QWidget *parent) :
        QDialog(parent) {
    setupUi(this);
}

/**
 * This method is to be called at the initialisation of the object. It builds the user interface.
 * @param Information A pointer to the object itself.
 */
void InfoDialog::setupUi(QDialog *Information)
{
    if (Information->objectName().isEmpty())
        Information->setObjectName(QString::fromUtf8("Information"));
    Information->resize(700, 700);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(Information->sizePolicy().hasHeightForWidth());
    Information->setSizePolicy(sizePolicy);
    vlMain = new QVBoxLayout(Information);
    vlMain->setObjectName(QString::fromUtf8("vlMain"));
    lTilte = new QLabel(Information);
    lTilte->setObjectName(QString::fromUtf8("lTilte"));
    QFont font;
    font.setPointSize(16);
    lTilte->setFont(font);
    lTilte->setScaledContents(false);
    lTilte->setAlignment(Qt::AlignCenter);

    vlMain->addWidget(lTilte);

    textBrowser = new QTextBrowser(Information);
    textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
    textBrowser->setOpenExternalLinks(true);

    vlMain->addWidget(textBrowser);

    lLicense = new QLabel(Information);
    lLicense->setObjectName(QString::fromUtf8("lLicense"));
    lLicense->setAlignment(Qt::AlignCenter);

    vlMain->addWidget(lLicense);


    retranslateUi(Information);

    QMetaObject::connectSlotsByName(Information);
}

/**
 * Sets all the text in the InfoDialog window.
 * @param Information A pointer to the object itself.
 */
void InfoDialog::retranslateUi(QDialog *Information)
{
    Information->setWindowTitle(QApplication::translate("Information", "Information", nullptr));
    lTilte->setText(QApplication::translate("Information", "Oscillometric Blood Pressure Measurement", nullptr));
    textBrowser->setHtml(QApplication::translate("Information", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                                                                "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                                                                "p, li { white-space: pre-wrap; }\n"
                                                                "</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
                                                                "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br />This application was designed as part of a Master's Thesis by <a href=\"https://github.com/itsBelinda\"><span style=\" text-decoration: underline; color:#0000ff;\">Belinda Kneub\303\274hler</span></a>.<br />The entire source code can be found on <a href=\"https://github.com/itsBelinda/obp\"><span style=\" text-decoration: underline; color:#0000ff;\">GitHub</span></a>. <br /></p>\n"
                                                                "<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Supervisor: <a href=\"https"
                                                                "://github.com/berndporr\"><span style=\" text-decoration: underline; color:#0000ff;\">Bernd Porr</span></a></p>\n"
                                                                "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; text-decoration: underline; color:#0000ff;\"><br /></p>\n"
                                                                "<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">University of Glasgow</p>\n"
                                                                "<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">School of Engineering</p>\n"
                                                                "<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2020</p>\n"
                                                                "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
                                                                "<p align=\"ce"
                                                                "nter\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Version: 1.0</p></body></html>", nullptr));
    lLicense->setText(QApplication::translate("Information", "\u00A9 GNU General Public License v2.0", nullptr));
}