#include "moneybirdapi.h"
#include <QVBoxLayout>
#include <QBuffer>

bool MoneybirdAPI::constructed = false;
MoneybirdAPI *MoneybirdAPI::moneybird;

MoneybirdAPI * MoneybirdAPI::create(bool showDebugger)
{
	if(!constructed)
	{
		moneybird = new MoneybirdAPI("Bearer 3abf50296c4a19c2b4498aa09270fe45e019ed8a0958fbc7f0b078864ef90a9c"
									 , "157733986296137360");
		constructed = true;

		if (showDebugger)
			moneybird->showMaximized();
	}

	return moneybird;
}

void MoneybirdAPI::closeWindow()
{
	moneybird->close();
	moneybird->deleteLater();
}


MoneybirdAPI::MoneybirdAPI(QString mAuthorizationID, QString mAdministrationID) :
	QWidget(nullptr),
	authorizationID(mAuthorizationID),
	administrationID(mAdministrationID)
{
	connect(&qnam, &QNetworkAccessManager::finished,
			this, &MoneybirdAPI::replyFinished);

	displayJSON = new QTextEdit;
	displayJSON->setReadOnly(true);
	displayJSON->setPlainText("MoneybirdAPI constructed");

	QVBoxLayout *mainlayout = new QVBoxLayout;
	mainlayout->addWidget(displayJSON);
	setLayout(mainlayout);

	setWindowTitle("Debug log");
}

MoneybirdAPI::~MoneybirdAPI()
{

}

void MoneybirdAPI::replyFinished(QNetworkReply *reply)
{
	QByteArray receivedData = reply->readAll();
	QJsonDocument parsedReceivedData(QJsonDocument::fromJson(receivedData));

	addDisplayText(parsedReceivedData.toJson());

	if(reply->error() == reply->NetworkError::NoError)
	{
		emit ParsedReply(parsedReceivedData);
	} else {
		QJsonDocument null;

		addDisplayText(reply->errorString());

		emit ParsedReply(null);
	}
}

void MoneybirdAPI::GET(QString specificUrl)
{
	addDisplayText("GET " + specificUrl);
	qnam.get(ParseRequest(specificUrl));
}

void MoneybirdAPI::POST(QString specificUrl, QJsonDocument data)
{
	addDisplayText("Post " + specificUrl + "\n " + data.toJson());
	qnam.post(ParseRequest(specificUrl), data.toJson(QJsonDocument::Compact));
}

void MoneybirdAPI::PATCH(QString specificUrl, QJsonDocument data)
{
	addDisplayText("Patch " + specificUrl + "\n " + data.toJson());

	QBuffer *buffer=new QBuffer();
	buffer->open((QBuffer::ReadWrite));
	buffer->write(data.toJson(QJsonDocument::Compact));
	buffer->seek(0);

	qnam.sendCustomRequest(ParseRequest(specificUrl), "PATCH", buffer);
}

void MoneybirdAPI::DEL(QString specificUrl)
{
	addDisplayText("Delete " + specificUrl + "\n ");
	qnam.deleteResource(ParseRequest(specificUrl));
}

QNetworkRequest MoneybirdAPI::ParseRequest(QString specificUrl)
{
	QString url = "https://moneybird.com/api/v2/";
	url.append(administrationID);
	url.append("/");
	url.append(specificUrl);



	const QUrl newUrl = QUrl::fromUserInput(url);
	if (!newUrl.isValid()) {
		QNetworkRequest null;
		return null;
	}

	QNetworkRequest request;
	request.setUrl(newUrl);
	request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
	request.setRawHeader("Authorization", authorizationID.toUtf8());

	return request;
}

void MoneybirdAPI::addDisplayText(QString text)
{
	QString previousText = displayJSON->toPlainText();

	displayJSON->setPlainText(previousText + "\n\n" + text);
}

