#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include "myfunctions.h"
#include "qcompressor.h"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QBuffer>
#include <QFileInfo>
#ifdef QT_DEBUG
	#include <QDebug>
#endif

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->templateComboBox->clear();



	m_pAbout			= new AboutDialog( this );


	connect( ui->pageHtmlSelectB, &QPushButton::clicked, this, [this](){
//		QString path = QFileDialog::getExistingDirectory( this, tr( "Open page directory" ), ui->pagePathLE->text());
		QString openPath = ( ui->pageHtmlLE->text().isEmpty() ) ? QDir::homePath() : ui->pageHtmlLE->text();
		QString path = QFileDialog::getOpenFileName( this, tr( "Open file" ), openPath, tr( "Html Files (*.html)" ));
		if( path != "" ){
			ui->pageHtmlLE->setText( path );
			checkFields( path );
		}
	} );
	connect( ui->pageJsPB, &QPushButton::clicked, this, [this](){
		QString openPath = ( ui->pageJsLE->text().isEmpty() ) ? QDir::homePath() : ui->pageJsLE->text();
		QString path = QFileDialog::getOpenFileName( this, tr( "Open file" ), openPath, tr( "JS Files (*.js)" ));
		if( path != "" ){
			ui->pageJsLE->setText( path );
			checkFields( path );
		}
	} );
	connect( ui->pageCssPB, &QPushButton::clicked, this, [this](){
		QString openPath = ( ui->pageCssLE->text().isEmpty() ) ? QDir::homePath() : ui->pageCssLE->text();
		QString path = QFileDialog::getOpenFileName( this, tr( "Open file" ), openPath, tr( "CSS Files (*.css)" ));
		if( path != "" ){
			ui->pageCssLE->setText( path );
			checkFields( path );
		}
	} );
	connect( ui->generateB, &QPushButton::clicked, this, &MainWindow::slot_generate );
	connect( ui->actionAbout, &QAction::triggered, m_pAbout, &AboutDialog::show );
	connect( ui->templateComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
		app::conf.profile = ui->templateComboBox->itemText( index );
		app::conf.saveSettings = true;
	});
}

MainWindow::~MainWindow()
{
	if( app::conf.saveSettings ) app::saveSettings();
	delete ui;
}

void MainWindow::init()
{
	for( const auto &elem:app::conf.profiles ){
		ui->templateComboBox->addItem( elem.name );
	}

	ui->templateComboBox->setCurrentIndex( 0 );
}

void MainWindow::slot_generate()
{
	QFile outputFile;
	QFile outputFileGZ;
	outputFile.setFileName( "index.html" );
	outputFileGZ.setFileName( "index.html.gz" );
	if( !outputFile.open( QIODevice::WriteOnly ) ) return;

	QString pageHTML = ui->pageHtmlLE->text();
	QString pageJS = ui->pageJsLE->text();
	QString pageCSS = ui->pageCssLE->text();

	for( const auto &elem:app::conf.profiles ){
		if( elem.name == app::conf.profile ){
			if( mf::checkFile( elem.logoFile ) ){
				QImage image( elem.logoFile );
				m_logo.clear();
				m_logo.append( "<img src=\"data:image/png;base64," );
				QByteArray ba;
				QBuffer bu(&ba);
				image.save( &bu, "PNG" );
				m_logo.append( ba.toBase64() );
				m_logo.append( "\"/>" );
			}
			appendFile( outputFile, elem.topPage, ContentType::html );
			if( mf::checkFile( pageHTML ) ) appendFile( outputFile, pageHTML, ContentType::html );
			outputFile.write( "\n<style>\n" );
			appendFile( outputFile, elem.cssFile, ContentType::css );
			if( mf::checkFile( pageCSS ) ) appendFile( outputFile, pageCSS, ContentType::css );
			outputFile.write( "\n</style>\n" );
			outputFile.write( "<script type=\"text/javascript\">\n" );
			appendFile( outputFile, elem.jsFile, ContentType::js );
			if( mf::checkFile( pageJS ) ) appendFile( outputFile, pageJS, ContentType::js );
			outputFile.write( "\n</script>\n" );

			appendFile( outputFile, elem.bottomPage, ContentType::html );
			break;
		}
	}

	outputFile.close();

	//Zipping...
	if( !outputFile.open( QIODevice::ReadOnly ) ) return;
	if( !outputFileGZ.open( QIODevice::WriteOnly ) ) return;

	while( !outputFile.atEnd() ){
		QByteArray buffer = outputFile.read( 1024 );
		QByteArray compressed;
		QCompressor::gzipCompress( buffer, compressed, 9 );
		outputFileGZ.write( compressed );
	}

	outputFile.close();
	outputFileGZ.close();

	QMessageBox::information( this, app::conf.appName, tr( "Generation complete." ) );
}

void MainWindow::appendFile(QFile &outputFile, const QString &filePath, const uint8_t type)
{
	if( !outputFile.isOpen() || !outputFile.isWritable() ) return;

	if( mf::checkFile( filePath ) ){
		QFile file( filePath );
		if( file.open( QIODevice::ReadOnly ) ){
			while( !file.atEnd() ){
				QByteArray buffer = file.readLine( 4096 );
				replace( buffer, type );
				outputFile.write( buffer );
			}
			file.close();
		}
	}
}

void MainWindow::replace(QByteArray &data, const uint8_t type)
{
	auto pageTitle = ui->pageTitleLE->text();

	data.replace( QString( "%TITLE%" ), pageTitle.toUtf8().data() );
	if( m_logo.size() > 35 ){
		data.replace( QString( "%LOGO%" ), m_logo.data() );
	}

	switch( type ){
		case ContentType::html:
			data.replace( "\r", "" );
			data.replace( "\n", "" );
			data.replace( "\t", "" );
		break;
		case ContentType::css:
			data.replace( "\r", "" );
			data.replace( "\n", "" );
			data.replace( "\t", "" );
			data.replace( "( ", "(" );
			data.replace( " )", ")" );
			data.replace( ", ", "," );
		break;
		case ContentType::js:
			data.replace( "\r", "" );
			while( data.indexOf( "//--" ) != -1 ) data.replace( "//--", "//-" );
			data.replace( "//-\n", "" );
			data.replace( "\n", "" );
			data.replace( "\t", "" );
			data.replace( " =", "=" );
			data.replace( "= ", "=" );
			data.replace( "( ", "(" );
			data.replace( " )", ")" );
			data.replace( ", ", "," );
		break;
	}
}

QString MainWindow::getBaseDir(const QString &filePath)
{
	QFileInfo info;
	info.setFile( filePath );
	return info.absoluteDir().absolutePath();
}

void MainWindow::checkFields(const QString &filePath)
{
	if( ui->pageHtmlLE->text().isEmpty() ) ui->pageHtmlLE->setText( getBaseDir( filePath ) );
	if( ui->pageJsLE->text().isEmpty() ) ui->pageJsLE->setText( getBaseDir( filePath ) );
	if( ui->pageCssLE->text().isEmpty() ) ui->pageCssLE->setText( getBaseDir( filePath ) );
}
