//============================================================================//
// File:          qcan_socket_dialog.cpp                                      //
// Description:   QCAN classes - CAN socket dialog                            //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'LICENSE'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'LICENSE' file.                   //
//                                                                            //
//============================================================================//


#include <QDebug>

#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>

#include "qcan_socket_dialog.hpp"


enum SocketHost_e {
   eSocketHostLocal = 0,
   eSocketHostRemote
};

class QCanSocketDialogPrivate : public QObject
{
   
public:
   QCanSocketDialogPrivate(void) {};

   
   void           createWidgets(QCanSocketDialog * pclWidgetV);
   void           connectSlots(QCanSocketDialog * pclWidgetV);
   CAN_Channel_e  channel(void);
   
   void           doConnectionWait();
   void           enableConnectionWait(bool btEnableV = true);
   void           enableHost(bool btEnableV = true);
   void           setChannel(const CAN_Channel_e teChannelV);
   void           setHostAddress(const QHostAddress clHostAddressV);
   void           showInfoText(const QString clTextV);
   void           translate(QCanSocketDialog * pclWidgetV);
   

   
private:
   QDialogButtonBox *pclButtonBoxP;
   QGroupBox *       pclGroupBoxP;
   QLabel *          pclLblHostP;
   QComboBox *       pclCbxHostP;
   QLabel *          pclLblHostAddressP;
   QLineEdit *       pclEdtHostAddressP;
   QLabel *          pclLblCanChannelP;
   QComboBox *       pclCbxCanChannelP;
   QLabel *          pclLblCanDetailP;
   QLabel *          pclLblCanInfoP;
   QProgressBar *    pclBarConnectWaitP;
};

//----------------------------------------------------------------------------//
// QCanSocketDialogPrivate::createWidgets()                                   //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialogPrivate::createWidgets(QCanSocketDialog * pclWidgetV)
{
   if (pclWidgetV->objectName().isEmpty())
   {
      pclWidgetV->setObjectName(QStringLiteral("QCanSocketDialog"));
   }
   
   //----------------------------------------------------------------
   // set size
   //
   pclWidgetV->setFixedSize(380, 251);

   //----------------------------------------------------------------
   // set buttons
   //
   pclButtonBoxP = new QDialogButtonBox(pclWidgetV);
   pclButtonBoxP->setObjectName(QStringLiteral("pclButtonBoxP"));
   pclButtonBoxP->setGeometry(QRect(10, 210, 361, 32));
   pclButtonBoxP->setOrientation(Qt::Horizontal);
   pclButtonBoxP->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

   pclGroupBoxP = new QGroupBox(pclWidgetV);
   pclGroupBoxP->setObjectName(QStringLiteral("pclGroupBoxP"));
   pclGroupBoxP->setGeometry(QRect(10, 10, 361, 191));
   
   //----------------------------------------------------------------
   // set label and combo-box for "Connect to: <remote/local>"
   //
   pclLblHostP = new QLabel(pclGroupBoxP);
   pclLblHostP->setObjectName(QStringLiteral("pclLblHostP"));
   pclLblHostP->setGeometry(QRect(10, 10, 140, 20));
   pclLblHostP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
 
   pclCbxHostP = new QComboBox(pclGroupBoxP);
   pclCbxHostP->setObjectName(QStringLiteral("pclCbxHostP"));
   pclCbxHostP->setGeometry(QRect(170, 10, 150, 26));

   //----------------------------------------------------------------
   // set label and line-edit for "IPv4-Address: 000.000.000.000"
   //
   pclLblHostAddressP = new QLabel(pclGroupBoxP);
   pclLblHostAddressP->setObjectName(QStringLiteral("pclLblHostAddressP"));
   pclLblHostAddressP->setGeometry(QRect(10, 40, 140, 20));
   pclLblHostAddressP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
   pclLblHostAddressP->setEnabled(false);
   
   pclEdtHostAddressP = new QLineEdit(pclGroupBoxP);
   pclEdtHostAddressP->setObjectName(QStringLiteral("pclEdtHostAddressP"));
   pclEdtHostAddressP->setGeometry(QRect(170, 42, 145, 21));
   pclEdtHostAddressP->setInputMask("000.000.000.000");
   pclEdtHostAddressP->setEnabled(false);

   QFont clFontT;
   clFontT.setFamily(QStringLiteral("Courier"));
   pclEdtHostAddressP->setFont(clFontT);
   pclEdtHostAddressP->setAlignment(Qt::AlignCenter);
   
   
   //----------------------------------------------------------------
   // set label and combo-box for "CAN channel: <CAN 1/ CAN 8>"
   //
   pclLblCanChannelP = new QLabel(pclGroupBoxP);
   pclLblCanChannelP->setObjectName(QStringLiteral("pclLblCanChannelP"));
   pclLblCanChannelP->setGeometry(QRect(10, 70, 140, 20));
   pclLblCanChannelP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

   pclCbxCanChannelP = new QComboBox(pclGroupBoxP);
   pclCbxCanChannelP->setObjectName(QStringLiteral("pclCbxCanChannelM"));
   pclCbxCanChannelP->setGeometry(QRect(170, 70, 150, 26));
   pclCbxCanChannelP->clear();
   for (uint8_t ubCanChannelT = eCAN_CHANNEL_1; 
        ubCanChannelT <= QCAN_NETWORK_MAX; ubCanChannelT++)
   {
      QString clNameT = "CAN " + QString("%1").arg(ubCanChannelT);
      pclCbxCanChannelP->addItem(clNameT, ubCanChannelT);
   }

   //----------------------------------------------------------------
   // set label and text field for "CAN details: <..>"
   //
   pclLblCanDetailP = new QLabel(pclGroupBoxP);
   pclLblCanDetailP->setObjectName(QStringLiteral("pclLblCanDetailP"));
   pclLblCanDetailP->setGeometry(QRect(10, 100, 140, 20));
   pclLblCanDetailP->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

   pclLblCanInfoP = new QLabel(pclGroupBoxP);
   pclLblCanInfoP->setObjectName(QStringLiteral("pclLblCanInfoP"));
   pclLblCanInfoP->setGeometry(QRect(170, 102, 148, 60));
   pclLblCanInfoP->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
   
   pclBarConnectWaitP = new QProgressBar(pclGroupBoxP);
   pclBarConnectWaitP->setObjectName(QStringLiteral("pclBarConnectWaitP"));
   pclBarConnectWaitP->setGeometry(QRect(170, 120, 141, 23));
   pclBarConnectWaitP->setRange(0, 100);
   pclBarConnectWaitP->setValue(0);
   pclBarConnectWaitP->setTextVisible(false);
   pclBarConnectWaitP->hide();
   
   //----------------------------------------------------------------
   // set the tab order
   //
   QWidget::setTabOrder(pclCbxHostP, pclEdtHostAddressP);
   QWidget::setTabOrder(pclEdtHostAddressP, pclCbxCanChannelP);


   
   //----------------------------------------------------------------
   // configure signal / slots
   //
   QObject::connect(pclButtonBoxP, SIGNAL(accepted()), 
                    pclWidgetV, SLOT(accept()));
   QObject::connect(pclButtonBoxP, SIGNAL(rejected()), 
                    pclWidgetV, SLOT(reject()));

   QMetaObject::connectSlotsByName(pclWidgetV);

}


//----------------------------------------------------------------------------//
// QCanSocketDialogPrivate::channel()                                         //
//                                                                            //
//----------------------------------------------------------------------------//
CAN_Channel_e QCanSocketDialogPrivate::channel(void)
{
   return ((CAN_Channel_e) pclCbxCanChannelP->currentData().toInt());
}

//----------------------------------------------------------------------------//
// QCanSocketDialogPrivate::connectSlots()                                    //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialogPrivate::connectSlots(QCanSocketDialog * pclWidgetV)
{
   connect( pclCbxHostP, SIGNAL(currentIndexChanged(int)),
            pclWidgetV, SLOT(onHostChanged(int)));
   
   connect( pclCbxCanChannelP, SIGNAL(currentIndexChanged(int)),
            pclWidgetV, SLOT(onChannelChanged(int)));
   
}

void QCanSocketDialogPrivate::enableConnectionWait(bool btEnableV)
{
   if (btEnableV == true)
   {
      pclBarConnectWaitP->show();
   }
   else
   {
      pclBarConnectWaitP->hide();
      pclBarConnectWaitP->reset();
   }
}

//----------------------------------------------------------------------------//
// QCanSocketDialogPrivate::enableHost()                                      //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialogPrivate::enableHost(bool btEnableV)
{
   pclLblHostAddressP->setEnabled(btEnableV);
   pclEdtHostAddressP->setEnabled(btEnableV);
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::setHostAddress()                                         //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialogPrivate::doConnectionWait()
{
   if (pclBarConnectWaitP->value() <  pclBarConnectWaitP->maximum())
   {
      pclBarConnectWaitP->setValue(pclBarConnectWaitP->value() + 1);      
   }
   else
   {
      pclBarConnectWaitP->setValue(pclBarConnectWaitP->minimum());
   }

}

void QCanSocketDialogPrivate::setChannel(const CAN_Channel_e teChannelV)
{
   pclCbxCanChannelP->setCurrentIndex(pclCbxCanChannelP->findData(teChannelV));
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::setHostAddress()                                         //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialogPrivate::setHostAddress(const QHostAddress clHostAddressV)
{
   if (clHostAddressV == QHostAddress::LocalHost)
   {
      enableHost(false);
   }
   else
   {
      enableHost(true);
      pclEdtHostAddressP->setText(clHostAddressV.toString());
   }
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::setHostAddress()                                         //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialogPrivate::showInfoText(const QString clTextV)
{
   pclLblCanInfoP->setText(clTextV);
}


//----------------------------------------------------------------------------//
// QCanSocketDialogPrivate::translate()                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialogPrivate::translate(QCanSocketDialog * pclWidgetV)
{
   pclWidgetV->setWindowTitle(tr("Connect to CAN interface"));
   pclLblHostP->setText(tr("Connect to:"));
   pclLblHostAddressP->setText(tr("IPv4-Address:"));
   pclLblCanChannelP->setText(tr("CAN channel:"));
   pclLblCanDetailP->setText(tr("CAN details:"));

   pclCbxHostP->clear();
   pclCbxHostP->addItem(tr("Local host"),    eSocketHostLocal);
   pclCbxHostP->addItem(tr("Remote server"), eSocketHostRemote);
}


//----------------------------------------------------------------------------//
// QCanSocketDialog()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
QCanSocketDialog::QCanSocketDialog(QWidget *parent, Qt::WindowFlags f) :
      QDialog(parent, f)
{
   pclSocketP = new QCanSocket();
   pclTimerP  = new QTimer();
   pclWidgetP = new QCanSocketDialogPrivate();
   pclWidgetP->createWidgets(this);
   pclWidgetP->connectSlots(this);
   pclWidgetP->translate(this);
   setModal(true);
   connectSlots();
}

//----------------------------------------------------------------------------//
// QCanSocketDialog()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
QCanSocketDialog::QCanSocketDialog(QWidget *parent, const QString &caption) :
      QDialog(parent)
{
   pclSocketP = new QCanSocket();
   pclTimerP  = new QTimer();
   pclWidgetP = new QCanSocketDialogPrivate();
   pclWidgetP->createWidgets(this);
   pclWidgetP->connectSlots(this);
   pclWidgetP->translate(this);
   this->setWindowTitle(caption);
   setModal(true);
   connectSlots();
}


//----------------------------------------------------------------------------//
// ~QCanSocketDialog()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
QCanSocketDialog::~QCanSocketDialog()
{
   disconnectSlots();
   delete (pclWidgetP);
   delete (pclTimerP);
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::accept()                                                 //
// This method is called when [OK] is clicked. Is is called before done().    //
//----------------------------------------------------------------------------//
void QCanSocketDialog::accept(void)
{
   QDialog::accept();
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::connect()                                                //
// connect to the server                                                      //
//----------------------------------------------------------------------------//
void QCanSocketDialog::connect()
{
   pclSocketP->disconnectNetwork();
   pclSocketP->connectNetwork(pclWidgetP->channel());
   pclWidgetP->showInfoText(tr("Waiting for connection .."));
   pclWidgetP->enableConnectionWait();
   pclTimerP->start(20);
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::connectSlots()                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::connectSlots(void)
{
   QObject::connect( pclTimerP, SIGNAL(timeout()), 
                     this, SLOT(onConnectionTimer()));

   //----------------------------------------------------------------
   // connect signals for socket operations
   //
   QObject::connect(pclSocketP, SIGNAL(connected()),
                    this, SLOT(onSocketConnected()));

   QObject::connect(pclSocketP, SIGNAL(disconnected()),
                    this, SLOT(onSocketDisconnected()));
   
   QObject::connect(pclSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(onSocketError(QAbstractSocket::SocketError)));
   

}


//----------------------------------------------------------------------------//
// QCanSocketDialog::disconnectSlots()                                        //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::disconnectSlots(void)
{
   //----------------------------------------------------------------
   // disconnect signals for socket operations
   //
   QObject::disconnect(pclSocketP, SIGNAL(connected()),
                       this, SLOT(onSocketConnected()));

   QObject::disconnect(pclSocketP, SIGNAL(disconnected()),
                       this, SLOT(onSocketDisconnected()));
   
   QObject::disconnect(pclSocketP, SIGNAL(error(QAbstractSocket::SocketError)),
                       this, SLOT(onSocketError(QAbstractSocket::SocketError)));

}


//----------------------------------------------------------------------------//
// QCanSocketDialog::done()                                                   //
// This method is called when [OK] or [Cancel] is clicked. Possible values    //
// for slResultV: QDialog::Accepted or QDialog::Rejected                      //
//----------------------------------------------------------------------------//
void QCanSocketDialog::done(int slResultV)
{
   if (slResultV == QDialog::Accepted)
   {
      emit socketSelected(pclSocketP);
   }
   else
   {
      //--------------------------------------------------------
      // Cancel has been pressed:
      // - disconnect from network
      // - delete the QCanSocket instance
      //
      pclSocketP->disconnectNetwork();
      delete (pclSocketP);
   }

   QDialog::done(slResultV);
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::onChannelChanged()                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::onChannelChanged(int slIndexV)
{
   Q_UNUSED(slIndexV);
   this->connect();
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::onConnectionTimer()                                      //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::onConnectionTimer()
{
   pclWidgetP->doConnectionWait();
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::onHostChanged()                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::onHostChanged(int slIndexV)
{
   if (slIndexV == eSocketHostLocal)
   {
      pclWidgetP->enableHost(false);
   }
   else
   {
      pclWidgetP->enableHost(true);
   }
   
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::onSocketConnected()                                      //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::onSocketConnected(void)
{
   QCanData::Type_e  ubFrameTypeT;
   QByteArray        clCanDataT;
   QCanFrameApi      clCanApiT;
   QString           clCanInfoT;
   
   pclTimerP->stop();
   pclWidgetP->enableConnectionWait(false);
   pclWidgetP->showInfoText(tr("Connected."));

   if(pclSocketP->read(clCanDataT, &ubFrameTypeT) == true)
   {
      switch(ubFrameTypeT)
      {
         case QCanData::eTYPE_API:
            if (clCanApiT.fromByteArray(clCanDataT) == true)
            {
               clCanInfoT = clCanApiT.toString();
               qDebug() << clCanInfoT;
            }
            break;
            
         default:

            break;
            
      }
   } 
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::onSocketDisconnected()                                   //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::onSocketDisconnected(void)
{
   pclWidgetP->showInfoText(tr("Disconnected."));

   qDebug() << "QCanSocketDialog::onSocketDisconnected()";
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::onSocketError()                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::onSocketError(QAbstractSocket::SocketError teSocketErrorV)
{
   Q_UNUSED(teSocketErrorV);
   pclTimerP->stop();
   pclWidgetP->enableConnectionWait(false);
   pclWidgetP->showInfoText(tr("Failed to connect."));

   qDebug() << "QCanSocketDialog::onSocketError()";
}


//----------------------------------------------------------------------------//
// peerAddress()                                                              //
//                                                                            //
//----------------------------------------------------------------------------//
QHostAddress QCanSocketDialog::peerAddress() const
{
   
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::show()                                                   //
// Show the dialog and try to connect to the server                           //
//----------------------------------------------------------------------------//
void QCanSocketDialog::show()
{
   QDialog::show();
   this->connect();
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::socket()                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
QCanSocket QCanSocketDialog::socket(void) const
{
   
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::setChannel()                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::setChannel(const CAN_Channel_e teChannelV)
{
   pclWidgetP->setChannel(teChannelV);
}


//----------------------------------------------------------------------------//
// QCanSocketDialog::setHostAddress()                                         //
//                                                                            //
//----------------------------------------------------------------------------//
void QCanSocketDialog::setHostAddress(const QHostAddress clHostAddressV)
{
   pclWidgetP->setHostAddress(clHostAddressV);
}
