#include "nqqscript.h"
#include "../mainwindow.h"
#include "../constants.h"
#include <QDebug>
#include <QInputDialog>

#define GET_FOCUSED_VIEW_OR_ERROR(x) \
    QTabWidgetqq* x = mptr->focused_tabWidget(); \
    if ( !x ) { \
        qDebug() << "GET_FOCUSED_VIEW_OR_ERROR =>" << "ERROR"; \
        context()->throwError("editor error"); \
    }

#define GET_EDITOR_AT_OR_ERROR(x,y,z) \
    QsciScintillaqq* y = x->QSciScintillaqqAt(z); \
    if ( !y ) { \
        qDebug() << "GET_EDITOR_AT_OR_ERROR =>" << "ERROR"; \
        context()->throwError(QString("editor not found. %1").arg(z)); \
    }

#define NULL_VALUE \
    QScriptValue(context()->engine(), QScriptValue::NullValue)

namespace api {
    NqqQtScript::NqqQtScript(MainWindow *ptr)
        : QObject(ptr), mptr(ptr)
    {

    }

    static QString get_plugin_name(QScriptContext* ctx) {
        if ( !ctx ) return QString::null;
        return ctx->engine()->property("script_name").toString();
    }

    static qint64 get_plugin_id(QScriptContext* ctx) {
        if ( !ctx ) return -1;
        return ctx->engine()->property("script_id").toLongLong();
    }

    void NqqQtScript::addMenuItem(const QString &text, QScriptValue callback)
    {
        if ( !callback.isFunction() ) {
            context()->throwError("callback must be a function");
        } else {

            QAction* act = mptr->addPluginMenuItem(
                               get_plugin_id  (context()),
                               get_plugin_name(context()),
                               text);

            qScriptConnect(act, SIGNAL(triggered()), context()->activationObject(), callback);
        }
    }

    void NqqQtScript::activateFile(const QString & fileName)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,fileName);
        if ( !qw ) return;
        qw->setFocus();
    }

    void NqqQtScript::activateIndex(int index)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,index);
        if ( !qw ) return;
        qw->setFocus();
    }

    void NqqQtScript::close(int index)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,index);
        if ( !qw ) return;
        qw->close();
    }

    void NqqQtScript::closeAll()
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        for ( int i = 0; i < tw->count(); ++i ) {
            QsciScintillaqq* qw = tw->QSciScintillaqqAt(i);
            if ( qw ) qw->close();
        }
    }

    void NqqQtScript::closeAllButCurrent()
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        for ( int i = 0; i < tw->count(); ++i ) {
            QsciScintillaqq* qw = tw->QSciScintillaqqAt(i);
            if ( qw && qw != tw->focusQSciScintillaqq() ) qw->close();
        }
    }

    int NqqQtScript::currentDocIndex()
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return -1;
        return tw->currentIndex();
    }

    QString NqqQtScript::currentFileName()
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return QString::null;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,-1);
        if ( !qw ) return QString::null;
        return qw->fileName();
    }

    QScriptValue NqqQtScript::getEditor(int index)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return NULL_VALUE;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,index);
        if ( !qw ) return NULL_VALUE;
        return context()->engine()->newQObject(qw);
    }

    QStringList NqqQtScript::getFiles()
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return QStringList();
        QStringList list;
        for ( int i = 0; i < tw->count(); ++i ) {
            QsciScintillaqq* qw = tw->QSciScintillaqqAt(i);
            if ( qw ) list << qw->fileName();
        }
        return list;
    }

    QString NqqQtScript::getVersion()
    {
        return VERSION;
    }

    QScriptValue NqqQtScript::create()
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return NULL_VALUE;
        int index = tw->addNewDocument();
        return this->getEditor(index);
    }

    QScriptValue NqqQtScript::open (QString fileName)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return NULL_VALUE;
        QStringList docs;
        docs << fileName;
        if ( !mptr->getDocumentEngine()->loadDocuments(docs, tw, false) )
            return NULL_VALUE;
        return this->getEditor();
    }

    QString NqqQtScript::prompt(const QString & prompt, const QString & title, const QString & defValue)
    {
        bool    ok   = false;
        QString text = QInputDialog::getText(mptr,
                                             title,
                                             prompt, QLineEdit::Normal,
                                             defValue, &ok);
        if ( ok ) return text;
        return QString::null;
    }

    void NqqQtScript::reload (int index)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,index);
        if ( !qw ) return;
        mptr->getDocumentEngine()->loadDocuments(QStringList(qw->fileName()),qw->tabWidget(),true);
    }

    void NqqQtScript::save       (int index)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,index);
        if ( !qw ) return;
        mptr->save(qw);
    }

    void NqqQtScript::saveAll    ()
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        for ( int i = 0; i < tw->count(); ++i ) {
            QsciScintillaqq* qw = tw->QSciScintillaqqAt(i);
            if ( qw ) mptr->save(qw);
        }
    }

    void NqqQtScript::saveAs(const QString & fileName, int index)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,index);
        if ( !qw ) return;
        mptr->getDocumentEngine()->saveDocument(qw,fileName,false);
    }

    void NqqQtScript::saveAsCopy (const QString & fileName, int index)
    {
        GET_FOCUSED_VIEW_OR_ERROR(tw);
        if ( !tw ) return;
        GET_EDITOR_AT_OR_ERROR   (tw,qw,index);
        if ( !qw ) return;
        mptr->getDocumentEngine()->saveDocument(qw,fileName,true);
    }

    void NqqQtScript::debug(const QString & text)
    {
        qDebug() << "[PLUGIN: "
                 << get_plugin_id(context())
                 << ":" << get_plugin_name(context()) << "] "
                 << text;
    }
}
