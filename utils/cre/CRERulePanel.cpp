#include "CRERulePanel.h"
#include "MessageFile.h"
#include <QtGui>
#include "CREStringListPanel.h"
#include "CREPrePostPanel.h"

CRERulePanel::CRERulePanel(QWidget* parent) : QTabWidget(parent)
{
    myMatches = new CREStringListPanel(true, this);
    connect(myMatches, SIGNAL(dataModified()), this, SLOT(onMatchModified()));
    addTab(myMatches, tr("matches"));
    QStringList pre;
    pre << "age" << "item" << "level" << "quest" << "token";
    myPre = new CREPrePostPanel(pre, this);
    connect(myPre, SIGNAL(dataModified()), this, SLOT(onPreModified()));
    addTab(myPre, tr("pre"));
    myMessages = new CREStringListPanel(false, this);
    connect(myMessages, SIGNAL(dataModified()), this, SLOT(onMessageModified()));
    addTab(myMessages, tr("message"));
    QStringList post;
    post << "connection" << "givecontents" << "giveitem" << "marktime" << "quest" << "settoken" << "takeitem";
    myPost = new CREPrePostPanel(post, this);
    connect(myPost, SIGNAL(dataModified()), this, SLOT(onPostModified()));
    addTab(myPost, tr("post"));
    addTab(new QLabel(tr("replies")), tr("replies"));

    QWidget* w = new QWidget(this);
    QHBoxLayout* l = new QHBoxLayout(w);
    l->addWidget(new QLabel(tr("Includes:"), this));
    myInclude = new QLineEdit(this);
    connect(myInclude, SIGNAL(textChanged(const QString&)), this, SLOT(onIncludeModified(const QString&)));
    l->addWidget(myInclude);
    addTab(w, tr("includes"));

    myRule = NULL;
}

CRERulePanel::~CRERulePanel()
{
}

void CRERulePanel::setMessageRule(MessageRule* rule)
{
    myRule = NULL;

    myMatches->clearData();
    myMessages->clearData();
    myInclude->clear();

    myRule = rule;

    if (rule != NULL)
    {
        myMatches->setData(rule->match());
        myPre->setData(rule->preconditions());
        myMessages->setData(rule->messages());
        myPost->setData(rule->postconditions());
        myInclude->setText(rule->include());
    }
}

void CRERulePanel::onMatchModified()
{
    if (myRule == NULL)
        return;
    myRule->setMatch(myMatches->getData());
    emit currentRuleModified();
}

void CRERulePanel::onPreModified()
{
    if (myRule == NULL)
        return;
    myRule->setPreconditions(myPre->getData());
    emit currentRuleModified();
}

void CRERulePanel::onMessageModified()
{
    if (myRule == NULL)
        return;
    myRule->setMessages(myMessages->getData());
    emit currentRuleModified();
}

void CRERulePanel::onPostModified()
{
    if (myRule == NULL)
        return;
    myRule->setPostconditions(myPost->getData());
    emit currentRuleModified();
}

void CRERulePanel::onIncludeModified(const QString& text)
{
    if (myRule != NULL)
    {
        myRule->setInclude(text);
        emit currentRuleModified();
    }
}
