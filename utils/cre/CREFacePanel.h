#ifndef CLASS_CRE_FACE_PANEL_H
#define CLASS_CRE_FACE_PANEL_H

#include <QObject>
#include <QtWidgets>
#include "CREPanel.h"

extern "C" {
#include "global.h"
}

class CREFacePanel : public CRETPanel<const New_Face>
{
    Q_OBJECT

    public:
        CREFacePanel();
        virtual void setItem(const New_Face* face);

    protected:
        const New_Face* myFace;

        QTreeWidget* myUsing;
        QComboBox* myColor;
        QLineEdit* myFile;
        QPushButton* mySave;

    private slots:
        void saveClicked(bool);
        void makeSmooth(bool);
};

#endif // CLASS_CRE_FACE_PANEL_H
