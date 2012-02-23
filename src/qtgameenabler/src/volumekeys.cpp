/**
 * Copyright (c) 2011 Nokia Corporation.
 * All rights reserved.
 *
 * Part of the Qt GameEnabler.
 *
 * For the applicable distribution terms see the license text file included in
 * the distribution.
 */

#include "volumekeys.h"
#include "trace.h"

using namespace GE;

/*!
  \class VolumeKeys
  \brief Monitors the state of volume up and down buttons on Symbian devices.
*/


/*!
  Constructor.
*/
VolumeKeys::VolumeKeys(QObject *parent, IVolumeKeyObserver *observer) :
    QObject(parent),
    m_interfaceSelector(NULL),
    m_coreTarget(NULL),
    m_observer(observer)
{
    DEBUG_INFO(this);

    QT_TRAP_THROWING(
        m_interfaceSelector = CRemConInterfaceSelector::NewL();
        m_coreTarget = CRemConCoreApiTarget::NewL(*m_interfaceSelector, *this);
        m_interfaceSelector->OpenTargetL();
    );
}

/*!
  Destructor.
*/
VolumeKeys::~VolumeKeys()
{
    DEBUG_POINT;

    delete m_interfaceSelector;
    m_interfaceSelector = NULL;
    m_coreTarget = NULL; // owned by interfaceselector
}

/*!
  From MRemConCoreApiTargetObserver.

  Checks if volume up/down keys are pressed and calls observer's (GameWindow's)
  callbacks respectively.
*/
void VolumeKeys::MrccatoCommand(TRemConCoreApiOperationId operationId,
                                TRemConCoreApiButtonAction buttonAct)
{
    DEBUG_INFO("operation:" << operationId << " action:" << buttonAct);

    if (buttonAct == ERemConCoreApiButtonClick) {
        if (operationId == ERemConCoreApiVolumeUp)
            m_observer->volumeUp();
        else if (operationId == ERemConCoreApiVolumeDown)
            m_observer->volumeDown();
    }
}
