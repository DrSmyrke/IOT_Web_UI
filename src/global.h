#ifndef GLOBAL_H
#define GLOBAL_H

#include <QDir>
#include <QSettings>
#include <QString>
#include <list>

struct Profile{
	QString name;
	QString topPage;
	QString bottomPage;
	QString cssFile;
	QString jsFile;
	QString logoFile;
};

struct Config{
	QString version;
	QString appName					= "Generator WEB UI";
	QString profile					= "default";
	std::list<Profile> profiles;
	bool saveSettings				= false;
};

namespace app {
	extern Config conf;

	bool parsArgs(int argc, char *argv[]);
	void setLog(const uint8_t logLevel, const QString &mess);
	void loadSettings();
	void saveSettings();
}

#endif // GLOBAL_H
