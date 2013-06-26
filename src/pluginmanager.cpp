#include "pluginmanager.h"
#include "mainwindow.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

PluginSandbox::PluginSandbox(QString path, QScriptProgram code)
    : path_(path), code_(code)
{
}

PluginManager::PluginManager()
    : nqq_script_api(NULL)
{

}

PluginManager::~PluginManager()
{
    delete nqq_script_api;
}

void PluginManager::init(MainWindow* ptr)
{
    if ( nqq_script_api == NULL ) {
        nqq_script_api = new api::NqqQtScript(ptr);
    }
}

const QStringList & PluginManager::scriptLoadPath() const
{
    return path_list_;
}

void PluginManager::setScriptLoadPath(const QStringList & paths)
{
    path_list_ = paths;
}

static bool is_script_directory(QFileInfo entry)
{
    if ( !entry.isDir() ) return false;
    return entry.dir().entryList().contains("main.qs");
}

ShrPtrPluginSandbox PluginManager::new_plugin_sandbox(const QFileInfo &entry)
{
    QString mainModule = entry.absoluteFilePath();
    if ( is_script_directory(entry) )
        mainModule += "/main.qs";

    QFile       file_obj(mainModule);
    QTextStream stream  (&file_obj);

    if (file_obj.open(QFile::ReadOnly)) {
        QString code             = stream.readAll();
        QScriptProgram byte_code(code);

        if ( byte_code.isNull() ) {
            return ShrPtrPluginSandbox();
        }

        ShrPtrPluginSandbox ptr(
            new PluginSandbox(entry.absolutePath(), byte_code) );

        // set global object
        ptr->engine().globalObject().setProperty(
                    "nqq",
                    ptr->engine().newQObject(nqq_script_api));

        QScriptValue script_obj = ptr->engine().evaluate(ptr->code());
        ptr->setName   ( script_obj.property("name"   ).toString() );
        ptr->setVersion( script_obj.property("version").toString() );

        ptr->engine().setProperty("script_name", ptr->name());
        ptr->engine().setProperty("script_id"  , script_obj.objectId());

        // CALL INIT FUNCTION
        QScriptValue init_func = script_obj.property("init");
        if ( init_func.isFunction() ) {
            init_func.call(script_obj, ptr->engine().newArray());
            if ( !ptr->engine().hasUncaughtException() )
                return ptr;
            else {
                qDebug() << "plugin init error: "
                         << ptr->engine().uncaughtException().toString()
                         << " @ " << ptr->engine().uncaughtExceptionLineNumber()
                         << " BT "<< ptr->engine().uncaughtExceptionBacktrace();
            }
        } else {
            qDebug() << "plugin init error: "
                     << "init function not found";
        }

        return ShrPtrPluginSandbox();
    }

    return ShrPtrPluginSandbox();
}

void PluginManager::scan()
{
    // the routine will scan for .qs files
    // or folders
    // with a main.qs file inside

    plugin_list_.clear();

    QStringList flt;
    flt << "*.qs";

    foreach( QString path, path_list_ ) {
        qDebug() << "scan " << path;
        foreach( QFileInfo entry, QDir(path).entryInfoList(flt) ) {
            if ( entry.isFile() || is_script_directory(entry) ) {
                ShrPtrPluginSandbox ptr = new_plugin_sandbox(entry);
                if ( !ptr.isNull() ) {
                    plugin_list_.append( ptr );
                    qDebug() << "loaded "
                             << ptr->path()
                             << " name: "
                             << ptr->name()
                             << " version: "
                             << ptr->version();
                }
            }
        }
    }
}

bool PluginManager::startWatcher()
{
    return true;
}

bool PluginManager::stopWatcher ()
{
    return true;
}
