#include "ViewImage.h"
using namespace autopilot;


ViewImage * ViewImage::createFromFile(QString _path, int _ID)
{
	ViewImage* vi = new ViewImage();
	vi->image = cv::imread(_path.toStdString());
	return vi;
}

ViewImage * autopilot::ViewImage::createFromMat(int _ID, cv::Mat _img)
{
	ViewImage* vi = new ViewImage();
	vi->image = _img;
	return vi;
}

QString autopilot::ViewImage::saveAsCache()
{
	saveFileName = QString::number(Utils::getNewID()) + ".jpg";
	if (cv::imwrite((Utils::getCacheFolder() + saveFileName).toStdString(), image) == true) {
		return saveFileName;
	}
	return QString();
}

void autopilot::ViewImage::setImage(cv::Mat img)
{
}

cv::Mat autopilot::ViewImage::getImage()
{
	return cv::Mat();
}

void autopilot::ViewImage::showImage()
{
}

void autopilot::ViewImage::init(int imageID, int nodeID, float rotation)
{
	this->imageID = imageID;
	this->nodeID = nodeID;
	this->rotation = rotation;
}





void ViewImage::setStatus(ImageStatus sta)
{
	this->status = sta;
	switch (sta)
	{
	case OutOfRange:
		this->setPixmap(QPixmap(Utils::getUIFolder() + "photoPosGrey.png"));
		break;
	case Detected:
		this->setPixmap(QPixmap(Utils::getUIFolder() + "photoPosBlue.png"));
		break;
	case ErrorPos:
		this->setPixmap(QPixmap(Utils::getUIFolder() + "photoPosRed.png"));
		break;
	default:
		break;
	}
}

ImageStatus autopilot::ViewImage::getStatus()
{
	return ImageStatus();
}

void autopilot::ViewImage::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
}


