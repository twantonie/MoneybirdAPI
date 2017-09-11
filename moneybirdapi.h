#ifndef MONEYBIRDAPI_H
#define MONEYBIRDAPI_H

#include "moneybirdapi_global.h"

#include <QWidget>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QUrl>


class MoneybirdAPI : public QWidget
{
	Q_OBJECT
public:
	static MoneybirdAPI * create(bool showDebugger);
	static void closeWindow();

	void GET(QString url);
	void POST(QString url, QJsonDocument data);
	void PATCH(QString url, QJsonDocument data);
	void DEL(QString url);

private:
	explicit MoneybirdAPI(QString authorizationID, QString administrationID);
	~MoneybirdAPI();
	void addDisplayText(QString text);
	static bool constructed;
	static MoneybirdAPI *moneybird;

	QString authorizationID;
	QString administrationID;
	QNetworkAccessManager qnam;
	QTextEdit *displayJSON;

	QNetworkRequest ParseRequest(QString url);

signals:
	void ParsedReply(QJsonDocument);

private slots:
	void replyFinished(QNetworkReply*);
};

#endif // MONEYBIRDAPI_H
