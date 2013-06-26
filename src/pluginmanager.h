#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <QScriptEngine>
#include <QScriptProgram>
#include <QStringList>
#include <QFileInfo>
#include <QSharedPointer>

#include "api/nqqscript.h"

class PluginSandbox
{
public:
    PluginSandbox(QString path, QScriptProgram code);

    const QScriptProgram & code   () const { return code_;   }
          QScriptEngine  & engine ()       { return engine_; }
    const QString        & name   () const { return name_;   }
    const QString        & path   () const { return path_;   }
    const QString        & version() const { return version_;}

    void setName   (QString val)           { name_ = val;    }
    void setVersion(QString val)           { version_ = val; }

private:
    QString        name_;
    QString        path_;
    QString        version_;
    QScriptProgram code_;
    QScriptEngine  engine_;
};
typedef QSharedPointer<PluginSandbox> ShrPtrPluginSandbox;


class PluginManager
{
public:
    PluginManager ();
    ~PluginManager();

    const QStringList & scriptLoadPath() const;
    void setScriptLoadPath(const QStringList & paths);

    void init(MainWindow* ptr);
    // this method search and build scripts in the load paths
    void scan();
    // start file system watcher
    bool startWatcher();
    // stop file system watcher
    bool stopWatcher ();

private:
    ShrPtrPluginSandbox new_plugin_sandbox(const QFileInfo & entry);

    QStringList path_list_;
    QList<ShrPtrPluginSandbox> plugin_list_;
    api::NqqQtScript* nqq_script_api;
};

#endif
