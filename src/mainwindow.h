#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include "aboutdialog.h"

struct ContentType{
	enum{
		unknown,
		html,
		css,
		js,
	};
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	void init();
private slots:
	void slot_generate();
private:
	Ui::MainWindow *ui;
	AboutDialog* m_pAbout;
	QByteArray m_logo;

	void appendFile(QFile &outputFile, const QString &filePath, const uint8_t type = ContentType::unknown);
	void replace(QByteArray &data, const uint8_t type = ContentType::unknown);
	QString getBaseDir(const QString &filePath);
	void checkFields(const QString &filePath);
};
#endif // MAINWINDOW_H
