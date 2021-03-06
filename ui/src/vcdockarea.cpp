/*
  Q Light Controller
  vcdockarea.cpp

  Copyright (c) Heikki Junnila

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <QVBoxLayout>
#include <QString>
#include <QDebug>
#include <QtXml>

#include "qlcfile.h"

#include "grandmasterslider.h"
#include "virtualconsole.h"
#include "vcproperties.h"
#include "vcdockarea.h"
#include "outputmap.h"
#include "inputmap.h"

VCDockArea::VCDockArea(QWidget* parent, OutputMap* outputMap, InputMap* inputMap)
    : QFrame(parent)
{
    Q_ASSERT(outputMap != NULL);
    Q_ASSERT(inputMap != NULL);

    new QHBoxLayout(this);
    layout()->setMargin(0);
    layout()->setSpacing(1);

    m_gm = new GrandMasterSlider(this, outputMap, inputMap);
    layout()->addWidget(m_gm);
}

VCDockArea::~VCDockArea()
{
}

void VCDockArea::setGrandMasterInvertedAppearance(UniverseArray::GMSliderMode mode)
{
    Q_ASSERT(m_gm != NULL);
    if (mode == UniverseArray::GMNormal)
        m_gm->setInvertedAppearance(false);
    else
        m_gm->setInvertedAppearance(true);
}

/*****************************************************************************
 * Event Handlers
 *****************************************************************************/

void VCDockArea::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    emit visibilityChanged(true);
}

void VCDockArea::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event);
    emit visibilityChanged(false);
}
