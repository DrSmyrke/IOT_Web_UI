#include "global.h"
#ifdef QT_DEBUG
	#include <QDebug>
#endif
namespace app {
	Config conf;

	bool parsArgs(int argc, char *argv[])
	{
		bool ret = true;
		for(int i=0;i<argc;i++){
			if(QString(argv[i]).indexOf("-")==0){
				if(QString(argv[i]) == "--help" or QString(argv[1]) == "-h"){
					printf("Usage: %s [OPTIONS]\n"
							"  -t <PATH>	target dir\n"
						   "  -r <URL>	repository url\n"
						   "  -k <KEY>	repository\n"
							"\n", argv[0]);
					ret = false;
				}
//				if(QString(argv[i]) == "-t") app::conf.targetDir = QString(argv[++i]);
//				if(QString(argv[i]) == "-r") app::conf.repository = QString(argv[++i]);
//				if(QString(argv[i]) == "-k") app::conf.key = QString(argv[++i]);
			}
		}
		return ret;
	}

	void setLog(const uint8_t logLevel, const QString &mess)
	{
		Q_UNUSED( logLevel )
		Q_UNUSED( mess )
		return;
	}

	void loadSettings()
	{
		QSettings settings( "MySoft", app::conf.appName );

		app::conf.profiles.clear();
		Profile defaultProfile;
		defaultProfile.name				= "default";
		defaultProfile.topPage			= ":/assets/top.html";
		defaultProfile.bottomPage		= ":/assets/bottom.html";
		defaultProfile.cssFile			= ":/assets/index.css";
		defaultProfile.jsFile			= ":/assets/index.js";
		defaultProfile.logoFile			= ":/assets/logo.png";
		app::conf.profiles.push_front( defaultProfile );

		app::conf.profile = settings.value("MAIN/profile", app::conf.profile).toString();

		for( auto &group:settings.childGroups() ){
			if( group == "MAIN" ) continue;
#ifdef QT_DEBUG
			qDebug() << group;
#endif
		}
	}

	void saveSettings()
	{
		QSettings settings( "MySoft", app::conf.appName );
		settings.clear();

		settings.setValue( "MAIN/profile", app::conf.profile );

		for( auto &elem:app::conf.profiles ){
			if( elem.name == "default" ) continue;
			settings.beginGroup( elem.name );
			settings.setValue( "topPage", elem.topPage );
			settings.setValue( "bottomPage", elem.bottomPage );
			settings.setValue( "cssFile", elem.cssFile );
			settings.setValue( "jsFile", elem.jsFile );
			settings.setValue( "logoFile", elem.logoFile );
			settings.endGroup();
		}
	}
}
