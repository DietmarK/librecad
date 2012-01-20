/*****************************************************************************/
/*  list.h - List selected entities                                          */
/*                                                                           */
/*  Copyright (C) 2011 Rallaz, rallazz@gmail.com                             */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 2 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#ifndef CAM1_H
#define CAM1_H

#include <QDialog>
#include "qc_plugininterface.h"
#include <QTextEdit>

//class QTextEdit;
class Plug_Entity;

class LC_cam1 : public QObject, QC_PluginInterface
{
    Q_OBJECT
     Q_INTERFACES(QC_PluginInterface)

 public:
    virtual PluginCapabilities getCapabilities() const;
    virtual QString name() const;
    virtual void execComm(Document_Interface *doc,
                                       QWidget *parent, QString cmd);

private:
    QString getStrData(Plug_Entity *ent, int);

};

class lc_cam1dlg : public QDialog
{
    Q_OBJECT

public:
    explicit lc_cam1dlg(QWidget *parent = 0);
    ~lc_cam1dlg();
    void setText(QString text);

public slots:
//    void procesAction(QStringList *commandList);
//    void procesAction(Document_Interface *doc);
//    void checkAccept();

protected:
//    void closeEvent(QCloseEvent *event);

private:
//    void readSettings();
//    void writeSettings();
//    bool failGUI(QString *msg);

private:
//    QString errmsg;
//    QLineEdit *startxedit;
//    QLineEdit *startyedit;
//    QLineEdit *endxedit;
    QTextEdit edit;
};

#endif // CAM1_H
