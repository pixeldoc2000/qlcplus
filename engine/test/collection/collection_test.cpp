/*
  Q Light Controller - Unit test
  collection_test.cpp

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

#include <QtTest>
#include <QtXml>

#define protected public
#include "mastertimer_stub.h"
#include "collection_test.h"
#include "universearray.h"
#include "collection.h"
#include "qlcchannel.h"
#include "function.h"
#include "fixture.h"
#include "qlcfile.h"
#include "scene.h"
#include "doc.h"
#undef protected

void Collection_Test::initTestCase()
{
    m_doc = new Doc(this);
}

void Collection_Test::cleanupTestCase()
{
    delete m_doc;
}

void Collection_Test::init()
{
}

void Collection_Test::cleanup()
{
    m_doc->clearContents();
}

void Collection_Test::initial()
{
    Collection c(m_doc);
    QVERIFY(c.type() == Function::Collection);
    QVERIFY(c.name() == "New Collection");
    QVERIFY(c.functions().size() == 0);
    QVERIFY(c.id() == Function::invalidId());
}

void Collection_Test::functions()
{
    Collection c(m_doc);
    c.setID(50);
    QVERIFY(c.functions().size() == 0);

    /* A collection should not be allowed to be its own member */
    QVERIFY(c.addFunction(50) == false);
    QVERIFY(c.functions().size() == 0);

    /* Add a function with id "12" to the Collection */
    QVERIFY(c.addFunction(12) == true);
    QVERIFY(c.functions().size() == 1);
    QVERIFY(c.functions().at(0) == 12);

    /* Add another function in the middle */
    QVERIFY(c.addFunction(34) == true);
    QVERIFY(c.functions().size() == 2);
    QVERIFY(c.functions().at(0) == 12);
    QVERIFY(c.functions().at(1) == 34);

    /* Must not be able to add the same function multiple times */
    QVERIFY(c.addFunction(12) == false);
    QVERIFY(c.functions().size() == 2);
    QVERIFY(c.functions().at(0) == 12);
    QVERIFY(c.functions().at(1) == 34);

    /* Removing a non-existent function should make no modifications */
    QVERIFY(c.removeFunction(999) == false);
    QVERIFY(c.functions().size() == 2);
    QVERIFY(c.functions().at(0) == 12);
    QVERIFY(c.functions().at(1) == 34);

    /* Removing the last step should succeed */
    QVERIFY(c.removeFunction(34) == true);
    QVERIFY(c.functions().size() == 1);
    QVERIFY(c.functions().at(0) == 12);

    /* Removing the only step should succeed */
    QVERIFY(c.removeFunction(12) == true);
    QVERIFY(c.functions().size() == 0);
}

void Collection_Test::functionRemoval()
{
    Collection c(m_doc);
    c.setID(42);
    QVERIFY(c.functions().size() == 0);

    QVERIFY(c.addFunction(0) == true);
    QVERIFY(c.addFunction(1) == true);
    QVERIFY(c.addFunction(2) == true);
    QVERIFY(c.addFunction(3) == true);
    QVERIFY(c.functions().size() == 4);

    /* Simulate function removal signal with an uninteresting function id */
    c.slotFunctionRemoved(6);
    QVERIFY(c.functions().size() == 4);

    /* Simulate function removal signal with a function in the Collection */
    c.slotFunctionRemoved(1);
    QVERIFY(c.functions().size() == 3);
    QVERIFY(c.functions().at(0) == 0);
    QVERIFY(c.functions().at(1) == 2);
    QVERIFY(c.functions().at(2) == 3);

    /* Simulate function removal signal with an invalid function id */
    c.slotFunctionRemoved(Function::invalidId());
    QVERIFY(c.functions().size() == 3);
    QVERIFY(c.functions().at(0) == 0);
    QVERIFY(c.functions().at(1) == 2);
    QVERIFY(c.functions().at(2) == 3);

    /* Simulate function removal signal with a function in the Collection */
    c.slotFunctionRemoved(0);
    QVERIFY(c.functions().size() == 2);
    QVERIFY(c.functions().at(0) == 2);
    QVERIFY(c.functions().at(1) == 3);
}

void Collection_Test::loadSuccess()
{
    QDomDocument doc;

    QDomElement root = doc.createElement("Function");
    root.setAttribute("Type", "Collection");

    QDomElement s1 = doc.createElement("Step");
    QDomText s1Text = doc.createTextNode("50");
    s1.appendChild(s1Text);
    root.appendChild(s1);

    QDomElement s2 = doc.createElement("Step");
    QDomText s2Text = doc.createTextNode("12");
    s2.appendChild(s2Text);
    root.appendChild(s2);

    QDomElement s3 = doc.createElement("Step");
    QDomText s3Text = doc.createTextNode("87");
    s3.appendChild(s3Text);
    root.appendChild(s3);

    Collection c(m_doc);
    QVERIFY(c.loadXML(root) == true);
    QVERIFY(c.functions().size() == 3);
    QVERIFY(c.functions().contains(50) == true);
    QVERIFY(c.functions().contains(12) == true);
    QVERIFY(c.functions().contains(87) == true);
}

void Collection_Test::loadWrongType()
{
    QDomDocument doc;

    QDomElement root = doc.createElement("Function");
    root.setAttribute("Type", "Chaser");

    QDomElement s1 = doc.createElement("Step");
    QDomText s1Text = doc.createTextNode("50");
    s1.appendChild(s1Text);
    root.appendChild(s1);

    QDomElement s2 = doc.createElement("Step");
    QDomText s2Text = doc.createTextNode("12");
    s2.appendChild(s2Text);
    root.appendChild(s2);

    QDomElement s3 = doc.createElement("Step");
    QDomText s3Text = doc.createTextNode("87");
    s3.appendChild(s3Text);
    root.appendChild(s3);

    Collection c(m_doc);
    QVERIFY(c.loadXML(root) == false);
}

void Collection_Test::loadWrongRoot()
{
    QDomDocument doc;

    QDomElement root = doc.createElement("Collection");
    root.setAttribute("Type", "Collection");

    QDomElement s1 = doc.createElement("Step");
    QDomText s1Text = doc.createTextNode("50");
    s1.appendChild(s1Text);
    root.appendChild(s1);

    QDomElement s2 = doc.createElement("Step");
    QDomText s2Text = doc.createTextNode("12");
    s2.appendChild(s2Text);
    root.appendChild(s2);

    QDomElement s3 = doc.createElement("Step");
    QDomText s3Text = doc.createTextNode("87");
    s3.appendChild(s3Text);
    root.appendChild(s3);

    Collection c(m_doc);
    QVERIFY(c.loadXML(root) == false);
}

void Collection_Test::loadWrongMemberTag()
{
    QDomDocument doc;

    QDomElement root = doc.createElement("Function");
    root.setAttribute("Type", "Collection");

    QDomElement s1 = doc.createElement("Foo");
    QDomText s1Text = doc.createTextNode("50");
    s1.appendChild(s1Text);
    root.appendChild(s1);

    QDomElement s2 = doc.createElement("Step");
    QDomText s2Text = doc.createTextNode("12");
    s2.appendChild(s2Text);
    root.appendChild(s2);

    QDomElement s3 = doc.createElement("Bar");
    QDomText s3Text = doc.createTextNode("87");
    s3.appendChild(s3Text);
    root.appendChild(s3);

    Collection c(m_doc);
    QVERIFY(c.loadXML(root) == true);
    QCOMPARE(c.functions().size(), 1);
}

void Collection_Test::loadPostLoad()
{
    QDomDocument doc;

    QDomElement root = doc.createElement("Function");
    root.setAttribute("Type", "Collection");

    QDomElement s2 = doc.createElement("Step");
    QDomText s2Text = doc.createTextNode("12");
    s2.appendChild(s2Text);
    root.appendChild(s2);

    Collection c(m_doc);
    QVERIFY(c.loadXML(root) == true);
    QCOMPARE(c.functions().size(), 1);
    c.postLoad();
    QCOMPARE(c.functions().size(), 0);
}

void Collection_Test::save()
{
    Collection c(m_doc);
    c.addFunction(3);
    c.addFunction(1);
    c.addFunction(0);
    c.addFunction(2);

    QDomDocument doc;
    QDomElement root = doc.createElement("TestRoot");

    QVERIFY(c.saveXML(&doc, &root) == true);
    QVERIFY(root.firstChild().toElement().tagName() == "Function");
    QVERIFY(root.firstChild().toElement().attribute("Type") == "Collection");

    QDomNode node = root.firstChild().firstChild();
    int fids = 0;
    while (node.isNull() == false)
    {
        QDomElement tag = node.toElement();
        if (tag.tagName() == "Step")
        {
            quint32 fid = tag.text().toUInt();
            QVERIFY(fid == 0 || fid == 1 || fid == 2 || fid == 3);
            fids++;
        }
        else
        {
            QFAIL("Unhandled XML tag.");
        }

        node = node.nextSibling();
    }

    QVERIFY(fids == 4);
}

void Collection_Test::copyFrom()
{
    Collection c1(m_doc);
    c1.setName("First");
    c1.addFunction(2);
    c1.addFunction(0);
    c1.addFunction(1);
    c1.addFunction(25);

    /* Verify that Collection contents are copied */
    Collection c2(m_doc);
    QSignalSpy spy(&c2, SIGNAL(changed(quint32)));
    QVERIFY(c2.copyFrom(&c1) == true);
    QCOMPARE(spy.size(), 1);
    QVERIFY(c2.name() == c1.name());
    QVERIFY(c2.functions().size() == 4);
    QVERIFY(c2.functions().at(0) == 2);
    QVERIFY(c2.functions().at(1) == 0);
    QVERIFY(c2.functions().at(2) == 1);
    QVERIFY(c2.functions().at(3) == 25);

    /* Verify that a Collection gets a copy only from another Collection */
    Scene s(m_doc);
    QVERIFY(c2.copyFrom(&s) == false);

    /* Make a third Collection */
    Collection c3(m_doc);
    c3.setName("Third");
    c3.addFunction(15);
    c3.addFunction(94);
    c3.addFunction(3);

    /* Verify that copying TO the same Collection a second time succeeds and
       that steps are not appended but replaced completely. */
    QVERIFY(c2.copyFrom(&c3) == true);
    QVERIFY(c2.name() == c3.name());
    QVERIFY(c2.functions().size() == 3);
    QVERIFY(c2.functions().at(0) == 15);
    QVERIFY(c2.functions().at(1) == 94);
    QVERIFY(c2.functions().at(2) == 3);
}

void Collection_Test::createCopy()
{
    Doc doc(this);

    Collection* c1 = new Collection(m_doc);
    c1->setName("First");
    c1->addFunction(20);
    c1->addFunction(30);
    c1->addFunction(40);

    doc.addFunction(c1);
    QVERIFY(c1->id() != Function::invalidId());

    Function* f = c1->createCopy(&doc);
    QVERIFY(f != NULL);
    QVERIFY(f != c1);
    QVERIFY(f->id() != c1->id());

    Collection* copy = qobject_cast<Collection*> (f);
    QVERIFY(copy != NULL);
    QVERIFY(copy->functions().size() == 3);
    QVERIFY(copy->functions().at(0) == 20);
    QVERIFY(copy->functions().at(1) == 30);
    QVERIFY(copy->functions().at(2) == 40);
}

void Collection_Test::write()
{
    Doc* doc = new Doc(this);

    Fixture* fxi = new Fixture(doc);
    fxi->setAddress(0);
    fxi->setUniverse(0);
    fxi->setChannels(4);
    doc->addFixture(fxi);

    Scene* s1 = new Scene(doc);
    s1->setName("Scene1");
    s1->setValue(fxi->id(), 0, UCHAR_MAX);
    s1->setValue(fxi->id(), 1, UCHAR_MAX);
    doc->addFunction(s1);

    Scene* s2 = new Scene(doc);
    s2->setName("Scene2");
    s2->setValue(fxi->id(), 2, UCHAR_MAX);
    s2->setValue(fxi->id(), 3, UCHAR_MAX);
    doc->addFunction(s2);

    Collection* c = new Collection(doc);
    c->setName("Collection");
    c->addFunction(s1->id());
    c->addFunction(s2->id());

    UniverseArray uni(1);
    MasterTimerStub* mts = new MasterTimerStub(m_doc, uni);

    /* Collection starts all of its members immediately when it is started
       itself. */
    QVERIFY(c->stopped() == true);
    c->start(mts);
    QVERIFY(c->stopped() == false);

    c->write(mts, &uni);
    QVERIFY(c->stopped() == false);
    QVERIFY(mts->m_functionList.size() == 3);
    QVERIFY(mts->m_functionList[0] == c);
    QVERIFY(mts->m_functionList[1] == s1);
    QVERIFY(mts->m_functionList[2] == s2);

    /* All write calls to the collection "succeed" as long as there are
       members running. */
    c->write(mts, &uni);
    QVERIFY(c->stopped() == false);
    QVERIFY(mts->m_functionList.size() == 3);
    QVERIFY(mts->m_functionList[0] == c);
    QVERIFY(mts->m_functionList[1] == s1);
    QVERIFY(mts->m_functionList[2] == s2);

    c->write(mts, &uni);
    QVERIFY(c->stopped() == false);
    QVERIFY(mts->m_functionList.size() == 3);
    QVERIFY(mts->m_functionList[0] == c);
    QVERIFY(mts->m_functionList[1] == s1);
    QVERIFY(mts->m_functionList[2] == s2);

    /* S2 is still running after this so the collection is also running */
    mts->stopFunction(s1);
    QVERIFY(s1->stopped() == true);

    c->write(mts, &uni);
    QVERIFY(c->stopped() == false);
    QVERIFY(mts->m_functionList.size() == 2);
    QVERIFY(mts->m_functionList[0] == c);
    QVERIFY(mts->m_functionList[1] == s2);

    /* Now the collection must also tell it's ready to be stopped */
    mts->stopFunction(s2);
    c->write(mts, &uni);
    QVERIFY(s2->stopped() == true);
    QVERIFY(c->stopped() == true);
    mts->stopFunction(c);

    delete mts;
    delete doc;
}

void Collection_Test::stopNotOwnChildren()
{
    Doc* doc = new Doc(this);

    Fixture* fxi = new Fixture(doc);
    fxi->setAddress(0);
    fxi->setUniverse(0);
    fxi->setChannels(4);
    doc->addFixture(fxi);

    Scene* s1 = new Scene(doc);
    s1->setName("Scene1");
    s1->setValue(fxi->id(), 0, UCHAR_MAX);
    s1->setValue(fxi->id(), 1, UCHAR_MAX);
    doc->addFunction(s1);

    Scene* s2 = new Scene(doc);
    s2->setName("Scene2");
    s2->setValue(fxi->id(), 2, UCHAR_MAX);
    s2->setValue(fxi->id(), 3, UCHAR_MAX);
    doc->addFunction(s2);

    Collection* c = new Collection(doc);
    c->setName("Collection");
    c->addFunction(s1->id());
    c->addFunction(s2->id());
    doc->addFunction(c);

    UniverseArray uni(512);
    MasterTimerStub* mts = new MasterTimerStub(m_doc, uni);

    QVERIFY(c->stopped() == true);
    c->start(mts);
    QVERIFY(c->stopped() == false);

    c->preRun(mts);
    QVERIFY(c->m_runningChildren.isEmpty() == true);

    c->write(mts, &uni);
    QVERIFY(s1->stopped() == false);
    QVERIFY(s2->stopped() == false);

    // Collection controls s1 & s2
    QVERIFY(c->m_runningChildren.contains(s1->id()) == true);
    QVERIFY(c->m_runningChildren.contains(s2->id()) == true);

    // Manually stop and re-start s1
    s1->stop();
    s1->write(mts, &uni);
    s1->postRun(mts, &uni);
    s1->start(mts);
    QVERIFY(s1->stopped() == false);

    // Collection should no longer be controlling s1
    QVERIFY(c->m_runningChildren.contains(s1->id()) == false);
    QVERIFY(c->m_runningChildren.contains(s2->id()) == true);

    c->stop();
    c->write(mts, &uni);
    c->postRun(mts, &uni);

    QVERIFY(c->stopped() == true);
    QVERIFY(s1->stopped() == false); // No longer controlled by collection
    QVERIFY(s2->stopped() == true);
}

QTEST_APPLESS_MAIN(Collection_Test)
