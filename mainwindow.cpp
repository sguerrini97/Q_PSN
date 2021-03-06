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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // var
    unsigned int lines = 0;
    QStandardItem * riga;
    QString s;
    QStringList sList;
    bool usingOfflineDB = false;
    strcpy( REMOTEDB, REMOTEDB_DEF );
    strcpy( REMOTESUBMIT, REMOTESUBMIT_DEF );

    if( QApplication::arguments().count() > 1 )
    {
        printf("Using custom database: %s\n", QApplication::arguments().at(1).toStdString().c_str() );
        strcpy( REMOTEDB, QApplication::arguments().at(1).toStdString().c_str() );
        if( QApplication::arguments().count() > 2 )
        {
            printf("Using custom sumbission: %s\n", QApplication::arguments().at(2).toStdString().c_str() );
            strcpy( REMOTESUBMIT, QApplication::arguments().at(2).toStdString().c_str() );
        }
    }

    strcat( REMOTESUBMIT, REMOTESUBMITARGS_DEF );

    // Window setup
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint);
    ui->labelUploadedBy->setText("");
    ui->labelSearchResult->setText("");
    ui->labelLogo->setText("");
    this->setFixedSize( size() );
    this->setWindowTitle( "Q_PSN - " VERSION );

    // Database
    database = new QFile( DBPATH );
    if( database->exists() )
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Database", "Do you want to use the offline database?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            usingOfflineDB = true;
        }
        else
        {
            database->remove();
        }
    }
    if( !database->open(QFile::ReadWrite | QFile::Text ))
    {
        QMessageBox::information(this, "Error", "Error reading database file.");
        exit(3);
    }

    dbStream = new QTextStream(database);
    // fetch online db
    if(!usingOfflineDB)
    {
        QNetworkAccessManager managerdb;
        QNetworkRequest requestdb(QUrl( QString::fromUtf8( REMOTEDB ) ));
        QNetworkReply *replydb(managerdb.get(requestdb));
        QEventLoop loopdb;
        QObject::connect(replydb, SIGNAL(finished()), &loopdb, SLOT(quit()));
        loopdb.exec();
        *dbStream << replydb->readAll();
    }

    // db loading
    dbStream->seek(0);
    while( !dbStream->atEnd() )
    {
        s = dbStream->readLine();
        lines++;
    }
    dbStream->seek(0);

    proxy_model_title = new QSortFilterProxyModel();
    proxy_model_type = new QSortFilterProxyModel();

    model = new QStandardItemModel( lines, COLUMN_MAX, this);
    model->setHorizontalHeaderItem(0, new QStandardItem( QString("ID") ));
    model->setHorizontalHeaderItem(1, new QStandardItem( QString("Title") ));
    model->setHorizontalHeaderItem(2, new QStandardItem( QString("Type") ));
    model->setHorizontalHeaderItem(3, new QStandardItem( QString("Region") ));

    for( unsigned int i = 0; i < lines; i++ )
    {
        s = dbStream->readLine();
        sList = s.split(";");
        for( unsigned int j = 0; j < COLUMN_MAX; j++ )
        {
            riga = new QStandardItem( sList[j] );
            riga->setEditable(false);
            model->setItem( i, j, riga );
        }
    }
    ui->tableViewContent->setModel( proxy_model_title );
    proxy_model_title->setSourceModel( proxy_model_type );
    proxy_model_type->setSourceModel( model );

    proxy_model_type->setFilterKeyColumn( COLUMN_TYPE );
    proxy_model_title->setFilterCaseSensitivity( Qt::CaseInsensitive );

    s = "Total entries: ";
    s.append( QString::number( lines, 10 ) );

    ui->labelTotalEntries->setText( s );

    // hide "service" columns
    for( int i = 4; i < COLUMN_MAX; i++ )
    {
        ui->tableViewContent->hideColumn( i );
    }

    // get news
    QNetworkAccessManager managernews;
    QNetworkRequest requestnews(QUrl( REMOTENEWS ));
    QNetworkReply *replynews(managernews.get(requestnews));
    QEventLoop loopnews;
    QObject::connect(replynews, SIGNAL(finished()), &loopnews, SLOT(quit()));
    loopnews.exec();
    ui->pteNews->setPlainText( replynews->readAll() );
    ui->pteNews->setReadOnly( true );


}

void MainWindow::on_leSearch_textChanged(const QString &arg1)
{
    proxy_model_title->setFilterKeyColumn( COLUMN_TITLE );
    proxy_model_title->setFilterRegExp( arg1 );

    if( proxy_model_title->rowCount() == 0 )
    {
        proxy_model_title->setFilterKeyColumn( COLUMN_GAMEID );
        proxy_model_title->setFilterRegExp( arg1 );
    }
}

MainWindow::~MainWindow()
{
    if( database->isOpen() )
        database->close();

    if(model)
        delete model;
    if( proxy_model_title )
        delete proxy_model_title;
    if( proxy_model_type )
        delete proxy_model_type;

    delete ui;
}

void MainWindow::on_tableViewContent_clicked(const QModelIndex &index)
{
    QString s;

    if( index.isValid() )
    {
        ui->btnDownload->setEnabled( true );
        ui->btnLinkCopy->setEnabled( true );
    }

    ui->pteDescription->setPlainText( ui->tableViewContent->model()->data( proxy_model_title->index( index.row(), COLUMN_DESC )).toString() );

    s = ui->tableViewContent->model()->data( proxy_model_title->index( index.row(), COLUMN_UPLOADBY )).toString();
    if( !( s.length() == 0 ) )
        ui->labelUploadedBy->setText( s );
    else
        ui->labelUploadedBy->setText("");

    s = ui->tableViewContent->model()->data( proxy_model_title->index( index.row(), COLUMN_RAPDATA )).toString();
    if( s.length() == 0 )
        ui->btnRap->setEnabled(false);
    else
        ui->btnRap->setEnabled(true);
}

void MainWindow::on_btnRap_clicked()
{
    char * rapPath = NULL, * hexstring = NULL, * pos = NULL;
    unsigned char hexvalues[16];
    FILE * rapFile;
    size_t count = 0;
    QDir rapfolder("exdata");

    QString s;

    if( !rapfolder.exists() )
    {
        rapfolder.mkpath(".");
        if( !rapfolder.exists() )
        {
            QMessageBox::information(this, "Error", "Cannot create \"exdata\" directory.");
            return;
        }
    }

    rapPath = (char*) malloc( sizeof(char)*256 );
    strcpy(rapPath, "exdata/");
    s = ui->tableViewContent->model()->data( proxy_model_title->index( ui->tableViewContent->selectionModel()->selectedIndexes()[0].row(), COLUMN_RAPNAME ) ).toString();
    strcat(rapPath, s.toStdString().c_str() );

    rapFile = fopen( rapPath, "wb" );
    if(!rapFile)
    {
        QMessageBox::information(this, "Error", "Error saving RAP file.");
        free(rapPath);
        return;
    }

    hexstring = (char*) malloc( sizeof(char)*33 );
    s = ui->tableViewContent->model()->data( proxy_model_title->index( ui->tableViewContent->selectionModel()->selectedIndexes()[0].row(), COLUMN_RAPDATA ) ).toString();
    strcpy(hexstring, s.toStdString().c_str() );

    pos = hexstring;

    for(count = 0; count < sizeof(hexvalues)/sizeof(hexvalues[0]); count++)
    {
        sscanf(pos, "%2hhx", &hexvalues[count]);
        pos += 2 * sizeof(char);
    }

    for(count = 0; count < sizeof(hexvalues)/sizeof(hexvalues[0]); count++)
        fputc(hexvalues[count], rapFile);

    QMessageBox::information(this, "Success", "Rap successfully saved!");

    fclose(rapFile);
    free(hexstring);
    free(rapPath);
}

void MainWindow::on_btnDownload_clicked()
{
    QString s = ui->tableViewContent->model()->data( proxy_model_title->index( ui->tableViewContent->selectionModel()->selectedIndexes()[0].row(), COLUMN_LINK ) ).toString();
    QDesktopServices::openUrl(QUrl( s, QUrl::TolerantMode));
}

void MainWindow::on_btnLinkCopy_clicked()
{
    QLineEdit * linkToCopy;
    QString s = ui->tableViewContent->model()->data( proxy_model_title->index( ui->tableViewContent->selectionModel()->selectedIndexes()[0].row(), COLUMN_LINK ) ).toString();

    linkToCopy = new QLineEdit();
    linkToCopy->setText( s );
    linkToCopy->selectAll();
    linkToCopy->copy();
    delete linkToCopy;
}

void MainWindow::on_btnSubSubmit_clicked()
{
    int error = 0;
    char *c;

    // chesks
    if( ui->leSubGameID->text().length() < 9 )
        error = 1;
    else if( !( ui->leSubLink->text().contains("http://") && ui->leSubLink->text().contains(".pkg") ) ) // more checks ?
        error = 4;

    if( !error )
    {
        // AAAA 00000 gameid check
        c = (char*) malloc( sizeof(char)*10 );
        strcpy( c, ui->leSubGameID->text().toStdString().c_str() );
        for( int i = 0; i < 9; i++ )
        {
            if( i < 4 && ( c[i] < 'A' || c[i] > 'Z' ) )
            {
                error = 1;
                break;
            }
            else if( i >=4 && ( c[i] < '0' || c[i] > '9' ) )
            {
                error = 1;
                break;
            }
        }
        free(c);
    }

    if( !error && !(ui->leSubRapData->text().isEmpty()) )
    {
        // rap data (hex) check
        if( ui->leSubRapData->text().length() < 32 )
        {
            error = 3;
        }
        else
        {
            c = (char*) malloc( sizeof(char)*33 );
            strcpy( c, ui->leSubRapData->text().toStdString().c_str() );
            for( int i = 0; i < 32; i++ )
            {
                if( (c[i] < 'A' || c[i] > 'F') && (c[i] < '0' || c[i] > '9') )
                {
                    error = 3;
                    break;
                }
            }
            free(c);
        }
    }

    if( !error && !(ui->leSubRapName->text().isEmpty()) )
    {
        // rap name (cid) check
        if( ui->leSubRapName->text().length() < 36 )
        {
            error = 2;
        }
        else
        {
            c = (char*)malloc( sizeof(char)*37 );
            strcpy( c, ui->leSubRapName->text().toStdString().c_str() );
            error = rapNameCheck(c);
            free(c);
        }
    }

    switch( error )
    {
    case 1:
        QMessageBox::information(this, "Error", "Invalid Game ID.");
        break;
    case 2:
        QMessageBox::information(this, "Error", "Invalid RAP name.");
        break;
    case 3:
        QMessageBox::information(this, "Error", "Invalid RAP data.");
        break;
    case 4:
        QMessageBox::information(this, "Error", "Invalid URL.");
        break;
    default:
        QUrl url = QUrl(
                    QString( REMOTESUBMIT )
                    .arg( ui->leSubGameID->text(), ui->leSubTitle->text(), ui->cbSubType->currentText(),
                          ui->cbSubRegion->currentText(), ui->leSubLink->text(), ui->leSubRapName->text(),
                          ui->leSubRapData->text(), ui->pteSubDescription->toPlainText(), ui->leUploaderName->text() ) );
        QNetworkAccessManager * nam = new QNetworkAccessManager(this);
        nam->get(QNetworkRequest(url));

        QMessageBox::information(this, "Thank you", "Thank you for your help!\nThe content will be available after moderation.");
        delete nam;
        break;
    }
}

int MainWindow::rapNameCheck( const char *r )
{
    int i;

    // 0         1         2         3
    // 012345678901234567890123456789012345
    // UP0006-NPUB30058_00-3ON3NHLARCADE002

    if( r[6] != '-' || r[16] != '_' || r[19] != '-' || r[17] != '0' || r[18] != '0' )
        return 2;

    for( i = 0; i < 6; i++ )
        if( ( r[i] < 'A' || r[i] > 'Z' ) && ( r[i] < '0' || r[i] > '9' ) )
            return 2;

    for( i = 7; i < 16; i++ )
        if( ( r[i] < 'A' || r[i] > 'Z' ) && ( r[i] < '0' || r[i] > '9' ) )
            return 2;

    for( i = 20; i < 36; i++ )
        if( ( r[i] < 'A' || r[i] > 'Z' ) && ( r[i] < '0' || r[i] > '9' ) )
            return 2;

    return 0;
}

void MainWindow::on_leSubGameID_textChanged(const QString &arg1)
{
    ui->leSubGameID->setText( arg1.toUpper() );
}

void MainWindow::on_leSubRapData_textChanged(const QString &arg1)
{
    ui->leSubRapData->setText( arg1.toUpper() );
}

void MainWindow::on_leSubRapName_textChanged(const QString &arg1)
{
    ui->leSubRapName->setText( arg1.toUpper() );
}

void MainWindow::on_btnSrc_clicked()
{
    QDesktopServices::openUrl(QUrl( "https://github.com/sguerrini97/Q_PSN", QUrl::TolerantMode));
}

void MainWindow::on_cbFilter_currentTextChanged(const QString &arg1)
{
    if( arg1 == "All" )
    {
        proxy_model_type->setFilterRegExp("");
    }
    else
    {
        proxy_model_type->setFilterRegExp( arg1 );
    }
}
