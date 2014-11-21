#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#define DBPATH  "./offline_db"
#define CFGPATH "./qpsn.cfg"

#define REMOTEDB    "/db"
#define REMOTENEWS  "/news"

#define CONTENT_COLUMNS 4

#define COLUMN_GAMEID   0
#define COLUMN_TITLE    1
#define COLUMN_TYPE     2
#define COLUMN_REGION   3
#define COLUMN_LINK     4
#define COLUMN_RAPNAME  5
#define COLUMN_RAPDATA  6
#define COLUMN_DESC     7
#define COLUMN_UPLOADBY 8

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

private:
    Ui::MainWindow *ui;
    QStringList currentContent;
    QFile * database;
    QTextStream * dbStream;
    QStandardItemModel * model;
    int rapNameCheck(char *r);
    char *dbUrl, *newsUrl;
};

#endif // MAINWINDOW_H
