/*
    Copyright (C) 2014,2015 - sguerrini97

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cstdio>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtSql/QtSql>

#define DBPATH  "./offline_db"

#define VERSION "0.3"

#define REMOTEDB    "http://qpsn.besaba.com/db.php"
#define REMOTENEWS  "http://qpsn.besaba.com/news"

#define COLUMN_GAMEID   0
#define COLUMN_TITLE    1
#define COLUMN_TYPE     2
#define COLUMN_REGION   3
#define COLUMN_LINK     4
#define COLUMN_RAPNAME  5
#define COLUMN_RAPDATA  6
#define COLUMN_DESC     7
#define COLUMN_UPLOADBY 8
#define COLUMN_MAX      9

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_tableViewContent_clicked(const QModelIndex &index);

    void on_btnRap_clicked();

    void on_btnDownload_clicked();

    void on_btnLinkCopy_clicked();

    void on_btnSubSubmit_clicked();

    void on_leSubGameID_textChanged(const QString &arg1);

    void on_leSubRapData_textChanged(const QString &arg1);

    void on_leSubRapName_textChanged(const QString &arg1);

    void on_leSearch_textChanged(const QString &arg1);

    void on_btnSrc_clicked();

    void on_cbFilter_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QFile * database;
    QTextStream * dbStream;
    QStandardItemModel * model;
    QSortFilterProxyModel *proxy_model_title, *proxy_model_type;
    int rapNameCheck(char *r);
};

#endif // MAINWINDOW_H
