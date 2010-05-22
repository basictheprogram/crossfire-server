#ifndef _CRESTRINGLISTPANEL_H
#define	_CRESTRINGLISTPANEL_H

#include <QWidget>

#include "CREMessagePanel.h"

class QListWidget;
class QLineEdit;
class QTextEdit;

class CREStringListPanel : public QWidget
{
    Q_OBJECT

    public:
        CREStringListPanel(bool simpleText, QWidget* parent);
        virtual ~CREStringListPanel();

        void clearData();
        void setData(const QStringList& list);
        QStringList getData() const;

    signals:
        void dataModified();

    private:
        int myCurrentLine;
        QListWidget* myItems;
        QLineEdit* myLineEdit;
        QTextEdit* myTextEdit;

        void commitData();
    protected slots:
        void onAddItem(bool);
        void onDeleteItem(bool);
        void onCurrentItemChanged(int currentRow);
        void onLineEditChanged(const QString& text);
        void onTextEditChanged();
};

#endif	/* _CRESTRINGLISTPANEL_H */
