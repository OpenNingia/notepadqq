#ifndef NQQ_SCRIPT_H
#define NQQ_SCRIPT_H

#include <QScriptable>
#include <QStringList>
#include <QScriptValue>
#include <QScriptValueList>

class MainWindow;

namespace api {
    class NqqQtScript : public QObject, protected QScriptable {
        Q_OBJECT
    public:
        NqqQtScript(MainWindow* ptr);
    private:
        MainWindow* mptr;

    public slots:

        void             addMenuItem        (const QString & text, QScriptValue callback );
        void             activateFile       (const QString & fileName);
        void             activateIndex      (int index);
        void             close              (int index = -1);
        void             closeAll           ();
        void             closeAllButCurrent ();
        int              currentDocIndex    ();
        QString          currentFileName    ();
        QScriptValue     getEditor          (int index = -1);
        QStringList      getFiles           ();
        QString          getVersion         ();
        QScriptValue     create             ();
        QScriptValue     open               (QString fileName);
        QString          prompt             (const QString & prompt, const QString & title, const QString & defValue);
        // ??? TODO messagebox
        void             reload             (int index = -1);
        void             save               (int index = -1);
        void             saveAll            ();
        void             saveAs             (const QString & fileName, int index = -1);
        void             saveAsCopy         (const QString & fileName, int index = -1);
        // TODO saveCurrentSession, saveSession

        void             debug              (const QString & text);
    };
}

#endif // NQQ_SCRIPT_H
